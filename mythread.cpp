#include "mythread.h"

MyThread::MyThread(Widget *myWidget)
{
    widget = myWidget;
    ui = myWidget->ui;
    hidDev = myWidget->hidDev;
}

MyThread::~MyThread()
{
    
}

void MyThread::run()
{
    static const uint16_t sampleTime[] = {1,2,3,4,5,10,20,30,50,100};
    static uint8_t sampleTick = -1, sampleFailed = 0;//采样节拍 采样失败计数
    static uint8_t sampleState = 0;
    while(1){
        while(!sampleFlag) QThread::usleep(500);//等待心跳
        sampleFlag = 0;
        watchDog = 0;//喂狗
        
        //if(ui->cB_cusb_drop->isChecked()) while(1);//模拟卡死 测试代码
        
        if(sampleStart == 0) continue;//未启动状态
        else if(sampleStart == 1){//预启动状态
            if(!widget->openHID(ui->spinBox_vid->value(), ui->spinBox_pid->value())){//若打开HID设备失败则退出
                //QMessageBox::critical(this,"启动失败","HID设备打开失败");
                ui->cB_get_VA->setChecked(false);//取消启动勾选
                sampleStart = 0;//置为未启动状态
                continue;
            }
            sampleStart = 2;//置为启动状态
            sampleState = 0;//置为需要发送
            sampleFailed = 0;//失败次数清零
            continue;
        }
        else if(sampleStart == 3){//预停止状态
            if(hidDev->isOpened()) hidDev->close();//关闭HID设备
            sampleStart = 0;//置为未启动状态
            continue;
        }
        
        if(!hidDev->isOpened()){//若未打开
            ui->cB_get_VA->setChecked(false);//取消启动勾选
            sampleStart = 0;//置为未启动状态
            continue;
        }
        
        sampleTick = (sampleTick + 1) % (sampleTime[ui->cBox_sample->currentIndex()] * 2);
        if(sampleTick == 0){//时间到
            if(sampleState != 2) sampleFailed++;//若非空闲态则视为一次失败
            if(sampleFailed > 3){
                ui->cB_get_VA->setChecked(false);//若失败次数超过限值则取消启动勾选
            }
            sampleState = 0;//强制开始新周期
        }
        
        //采样状态:0需要发送 1等待回复 2空闲状态
        if(sampleState == 0){//需要发送
            std::string hidWriteStr;//创建写入缓存
            hidWriteStr.resize(65, 0);//重置为65字节
            hidWriteStr[0] = 0;
            hidWriteStr[1] = 'M'; hidWriteStr[2] = 'B';//填入采样命令
            
            while(hidDev->readAvailable()) hidDev->read(0);//若已有数据则先读出
            
            if(hidDev->write(hidWriteStr) != 65) continue;//若写入失败则退出
            //printf("t:03,%d,%d\n",sampleTick,hidDev->isOpened());
            usleep(500);//无此处延时容易导致通信失败次数增加
            sampleState = 1;
        }
        else if(sampleState == 1){//等待回复
            if(!hidDev->readAvailable()) continue;//无回复则跳过本轮
            
            std::string hidReadStr = hidDev->read(1);//等待读取响应数据最多1ms
            sampleFailed = 0;//失败次数清零
            if(getAdcData(hidReadStr)){//处理数据
                sampleFailed = 0;//失败次数清零
                sampleState = 2;//进入空闲状态
            }
        }
        else if(sampleState == 2){//空闲状态
            
        }
    }
}

bool MyThread::getAdcData(std::string adcStr)//处理数据
{
    static const uint16_t sampleTime[] = {1,2,3,4,5,10,20,30,50,100};
    
    if(!(adcStr.length() == 65 && adcStr[1] == 'M' && adcStr[2] == ~'B')){//若响应错误
        return false;
    }//否则若正确响应
    
    uint16_t adcData[4];//获取ADC数据
    adcData[0] = ((uint8_t)adcStr[3] << 8) | (uint8_t)adcStr[4];//电压
    adcData[1] = ((uint8_t)adcStr[5] << 8) | (uint8_t)adcStr[6];//电流
    adcData[2] = ((uint8_t)adcStr[7] << 8) | (uint8_t)adcStr[8];//1.19V参考测量值
    adcData[3] = ((uint8_t)adcStr[9] << 8) | (uint8_t)adcStr[10];//1.19V参考硬件值
    cusbState = (uint8_t)adcStr[12];//端口状态
    
    double adcValueWork = (double)adcData[3] * 4096 / 1000 / (double)adcData[2];//用参考源反推工作电压
    //if(ui->cB_cusb_dir->isChecked()) adcValueWork = 3.3;//测试代码
    
    adcValueV = (double)adcData[0] * adcValueWork * 2 / 4096;   //电压
    adcValueA = (double)adcData[1] * adcValueWork / 2 / 4096;   //电流
    adcValueA = adcValueA > 0.0009 ? (adcValueA - 0.0009) : 0;  //对电流添加死区
    if(cusbState != 0x01) adcValueA = 0;                        //若关断则置零
    adcValueV *= 0.99;                                          //对电压引入校正系数
    if(ui->cB_cusb_drop->isChecked()) adcValueV -= adcValueA * (0.02 + 0.03);   //对电压引入压降
    adcValueW = adcValueV * adcValueA;                          //功率
    adcValueJ += adcValueW * sampleTime[ui->cBox_sample->currentIndex()] / 100; //能量
    
    return true;
}
