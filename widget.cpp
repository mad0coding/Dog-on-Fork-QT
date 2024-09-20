#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    
    memset(para, 0, sizeof(para));
    memset(keys_kv, 0, sizeof(keys_kv));
    memset(keys_func, 0, sizeof(keys_func));

    hidApi = new HidApi();
    hidDev = new HidDevice();//HID设备
    
    adcThread = new MyThread(this);//创建采样线程
    adcThread->start();//线程启动
    
    connect(&timer,SIGNAL(timeout()),this,SLOT(timeout_slot()));//定时器中断信号
    timer.start(5);//定时器设置(单位ms)
    
    ui->label_ctrl->setStyleSheet("color: gray");
    ui->label_shift->setStyleSheet("color: gray");
    ui->label_alt->setStyleSheet("color: gray");
    ui->label_win->setStyleSheet("color: gray");
    
    QString style_prefix = "QPushButton{background-color:rgb(220, 220, 220);";
    QString style_suffix = "QPushButton:hover{background-color:rgb(210, 210, 210);}"
                           "QPushButton:pressed{background-color:rgb(200, 200, 200);}";
    
    style_big_black = style_prefix + "color: black;border-radius: 20px;}" + style_suffix;
    style_big_gray = style_prefix + "color: gray;border-radius: 20px;}" + style_suffix;
    style_mid_black = style_prefix + "color: black;border-radius: 12px;}" + style_suffix;
    style_mid_gray = style_prefix + "color: gray;border-radius: 12px;}" + style_suffix;
    style_small_black = style_prefix + "color: black;border-radius: 8px;}" + style_suffix;
    style_small_gray = style_prefix + "color: gray;border-radius: 8px;}" + style_suffix;
    
    cbox_leds[0] = ui->cBox_led_L;  cbox_leds[1] = ui->cBox_led_R;
    cbox_leds[2] = ui->cBox_led_G;  cbox_leds[3] = ui->cBox_led_B;
    for(int i = 0; i < 4; i++){//设置下拉框内容
        cbox_leds[i]->clear();
        cbox_leds[i]->addItem("无",0);
        cbox_leds[i]->addItem("数字锁定",1);
        cbox_leds[i]->addItem("大写锁定",2);
        cbox_leds[i]->addItem("滚动锁定",3);
        cbox_leds[i]->addItem("CUSB开关",4);
        cbox_leds[i]->addItem("HUB复位",5);
        cbox_leds[i]->addItem("配置切换",6);
        cbox_leds[i]->addItem("无线状态",7);
        cbox_leds[i]->addItem("自定义1",8);
        cbox_leds[i]->addItem("自定义2",8);
    }
    sbox_leds[0] = ui->spinBox_brt_L;   sbox_leds[1] = ui->spinBox_brt_R;
    sbox_leds[2] = ui->spinBox_brt_G;   sbox_leds[3] = ui->spinBox_brt_B;
    cb_leds[0] = ui->cB_led_dir_L;  cb_leds[1] = ui->cB_led_dir_R;
    cb_leds[2] = ui->cB_led_dir_G;  cb_leds[3] = ui->cB_led_dir_B;

    cbox_keys[0] = NULL;                cbox_keys[1] = NULL;
    cbox_keys[2] = ui->cBox_ec_key_1;   cbox_keys[3] = ui->cBox_key_1;
    cbox_keys[4] = NULL;                cbox_keys[5] = NULL;
    cbox_keys[6] = ui->cBox_ec_key_2;   cbox_keys[7] = ui->cBox_key_2;
    for(int i = 0; i < 8; i++){//设置下拉框内容
        if(cbox_keys[i]){//若不为空指针
            cbox_keys[i]->clear();
            cbox_keys[i]->addItem("无",0);
            cbox_keys[i]->addItem("按键",1);
            cbox_keys[i]->addItem("CUSB开关",2);
            cbox_keys[i]->addItem("CUSB掉电",3);
            cbox_keys[i]->addItem("HUB复位",4);
            cbox_keys[i]->addItem("HID配置切换",5);
            cbox_keys[i]->addItem("USB模式切换",6);
            cbox_keys[i]->addItem("自定义1",7);
            cbox_keys[i]->addItem("自定义2",8);
        }
    }
    lb_keys[0] = ui->lb_ec_key_11;  lb_keys[1] = ui->lb_ec_key_12;
    lb_keys[2] = ui->lb_ec_key_1;   lb_keys[3] = ui->lb_key_1;
    lb_keys[4] = ui->lb_ec_key_21;  lb_keys[5] = ui->lb_ec_key_22;
    lb_keys[6] = ui->lb_ec_key_2;   lb_keys[7] = ui->lb_key_2;
    bt_keys[0] = ui->key_ec_key_11; bt_keys[1] = ui->key_ec_key_12;
    bt_keys[2] = ui->key_ec_key_1;  bt_keys[3] = ui->key_key_1;
    bt_keys[4] = ui->key_ec_key_21; bt_keys[5] = ui->key_ec_key_22;
    bt_keys[6] = ui->key_ec_key_2;  bt_keys[7] = ui->key_key_2;
    for(int i = 0; i < 8; i++) bt_keys[i]->setStyleSheet(style_big_black);//按键样式
    
    ui->Bt_write->setStyleSheet(style_big_black);
    ui->Bt_open->setStyleSheet(style_big_black);
    ui->Bt_save->setStyleSheet(style_big_black);
    ui->Bt_saveas->setStyleSheet(style_big_black);
    
    ui->key_clear->setStyleSheet(style_mid_black);
    
    ui->Bt_clear_J->setStyleSheet(style_small_black);
    ui->Bt_hold->setStyleSheet(style_small_black);
    ui->Bt_empty->setStyleSheet(style_small_black);
    ui->Bt_vol_down->setStyleSheet(style_small_black);
    ui->Bt_vol_up->setStyleSheet(style_small_black);
    ui->Bt_vol_prev->setStyleSheet(style_small_black);
    ui->Bt_vol_next->setStyleSheet(style_small_black);
    ui->Bt_vol_mute->setStyleSheet(style_small_black);
    ui->Bt_vol_stop->setStyleSheet(style_small_black);
    ui->Bt_wheel_down->setStyleSheet(style_small_black);
    ui->Bt_wheel_up->setStyleSheet(style_small_black);
    ui->Bt_mouse_L->setStyleSheet(style_small_black);
    ui->Bt_mouse_M->setStyleSheet(style_small_black);
    ui->Bt_mouse_R->setStyleSheet(style_small_black);
    
//    QFont font = this->font();
//    printf("%d\n",font.pointSize());
//    font.setPointSize(7);//默认为9
//    this->setFont(font);
}

Widget::~Widget(){
    adcThread->terminate();//强行结束线程
    adcThread->wait();
    delete ui;
}

void Widget::key_handle(uint8_t keyValue, bool ifPress = true)//按键处理
{
    if(ifPress && button_choose != 0xFF && (keyValue < 249 || keyValue > 252)){//若有选择的按键且键值不为功能键
        setKeyValue(button_choose, func, keyValue);//设置按键
    }
    //printf("kv:%d,%d,%d\n",keyValue,ifPress,func);
}

void Widget::keyPressEvent(QKeyEvent *event)//按键按下
{
    if(event->isAutoRepeat() || ifSending){ return; }//若为自动重复触发或正在发送数据则返回
    int key1 = event->key();//读取第一种键值
    int key2 = event->nativeVirtualKey();//读取第二种键值
//    int key3 = event->nativeScanCode();//读取第三种键值(该值能区分左右功能键)
//    printf("key:%d  %d  %d\n",key1,key2,key3);//打印键值
    
    uint8_t keyValue = key_to_USB(key1,key2);//映射到USB键值
    
    if(keyValue >= 249 && keyValue <= 252) func |= 0x01 << (keyValue - 249);
    else{
        ui->label_keyValue->setText("USB键值:" + QString::number(keyValue));
        ui->label_keyName->setText("按键:" + USB_to_str(keyValue,0));
        key_num++;
    }
    
    if(keyValue == 249) ui->label_ctrl->setStyleSheet("color: black");
    else if(keyValue == 250) ui->label_shift->setStyleSheet("color: black");
    else if(keyValue == 251) ui->label_alt->setStyleSheet("color: black");
    else if(keyValue == 252) ui->label_win->setStyleSheet("color: black");
    
    key_handle(keyValue);
    //printf("key:%d\n",keyValue);//打印键值
    //printf("funcP:%d",func);//打印当前功能键
}

void Widget::keyReleaseEvent(QKeyEvent *event)//按键抬起
{
    if(event->isAutoRepeat() || ifSending){ return; }//若为自动重复触发或正在发送数据则返回
    int key1 = event->key();//读取第一种键值
    int key2 = event->nativeVirtualKey();//读取第二种键值
    
    uint8_t keyValue = key_to_USB(key1,key2);//映射到USB键值
    
    if(keyValue >= 249 && keyValue <= 252){
        if(!ifHold) func &= ~(0x01 << (keyValue - 249));
    }
    else if(key_num > 0) key_num--;
    
    if(key_num == 0){
        ui->label_keyValue->setText("USB键值:None");
        ui->label_keyName->setText("按键:None");
    }
    
    if(!ifHold){
        if(keyValue == 249) ui->label_ctrl->setStyleSheet("color: grey");
        else if(keyValue == 250) ui->label_shift->setStyleSheet("color: grey");
        else if(keyValue == 251) ui->label_alt->setStyleSheet("color: grey");
        else if(keyValue == 252) ui->label_win->setStyleSheet("color: grey");
    }
    key_handle(keyValue,false);
    //printf("funcR:%d",func);//打印当前功能键
}

void Widget::on_Bt_mouse_L_clicked(){    key_handle(kv_mouse_l);   }//鼠标左键
void Widget::on_Bt_mouse_M_clicked(){    key_handle(kv_mouse_m);   }//鼠标中键
void Widget::on_Bt_mouse_R_clicked(){    key_handle(kv_mouse_r);   }//鼠标右键

void Widget::on_Bt_wheel_up_clicked(){    key_handle(kv_wheel_up);   }//鼠标滚轮向上
void Widget::on_Bt_wheel_down_clicked(){    key_handle(kv_wheel_down);   }//鼠标滚轮向下

void Widget::on_Bt_vol_up_clicked(){    key_handle(kv_vol_up);   }//媒体音量加
void Widget::on_Bt_vol_down_clicked(){    key_handle(kv_vol_down);   }//媒体音量减
void Widget::on_Bt_vol_mute_clicked(){    key_handle(kv_vol_mute);   }//媒体静音
void Widget::on_Bt_vol_stop_clicked(){    key_handle(kv_vol_stop);   }//媒体停止
void Widget::on_Bt_vol_next_clicked(){    key_handle(kv_vol_next);   }//媒体下一个
void Widget::on_Bt_vol_prev_clicked(){    key_handle(kv_vol_prev);   }//媒体上一个

void Widget::on_Bt_empty_clicked(){    key_handle(0);   }//空按键

void Widget::on_Bt_hold_clicked()//按键保持
{
    if(ifHold){//若之前在按键保持
        ifHold = false;
        func = 0;//清空功能键
        key_num = 0;//清零按键计数
        ui->label_ctrl->setStyleSheet("color: gray");
        ui->label_shift->setStyleSheet("color: gray");
        ui->label_alt->setStyleSheet("color: gray");
        ui->label_win->setStyleSheet("color: gray");
        ui->Bt_hold->setText("按键保持");
    }
    else{
        ifHold = true;
        ui->Bt_hold->setText("取消保持");
    }
}

void Widget::key_click_handle(uint8_t button_cs)//8个按钮按下处理
{
    if(button_choose == button_cs) button_choose = 0xFF;//若此键已被选择则清除
    else button_choose = button_cs;//否则选择之
    for(int i = 0; i < 8; i++){
        if(i == button_choose) bt_keys[i]->setStyleSheet(style_big_gray);
        else bt_keys[i]->setStyleSheet(style_big_black);
    }
    //printf("K:%d,%d\n",button_choose,button_cs);
}

void Widget::on_key_ec_key_11_clicked(){    key_click_handle(0);}//旋钮1逆时针
void Widget::on_key_ec_key_12_clicked(){    key_click_handle(1);}//旋钮1顺时针
void Widget::on_key_ec_key_1_clicked(){     key_click_handle(2);}//旋钮1按键
void Widget::on_key_key_1_clicked(){        key_click_handle(3);}//按键1
void Widget::on_key_ec_key_21_clicked(){    key_click_handle(4);}//旋钮2逆时针
void Widget::on_key_ec_key_22_clicked(){    key_click_handle(5);}//旋钮2顺时针
void Widget::on_key_ec_key_2_clicked(){     key_click_handle(6);}//旋钮2按键
void Widget::on_key_key_2_clicked(){        key_click_handle(7);}//按键2

void Widget::on_key_clear_clicked()//按键清除键
{
    if(button_choose != 0xFF) setKeyValue(button_choose, 0, 0);
}

void Widget::setKeyValue(uint8_t key_i, uint8_t fv, uint8_t kv)//设置按键
{
    QString keyName = "";
    if(fv & 0x01) keyName += "C";//ctrl
    if(fv & 0x04) keyName += "A";//alt
    if(fv & 0x02) keyName += "S";//shift
    if(fv & 0x08) keyName += "W";//win
    if(keyName.length()) keyName += "+";
    keyName += USB_to_str(kv, 0);//USB键值转按键名
    lb_keys[key_i]->setText(keyName);//设置名称
    bt_keys[key_i]->setStyleSheet(style_big_black);//按键样式
    keys_func[key_i] = fv;//填入功能键值
    keys_kv[key_i] = kv;//填入键值
    button_choose = 0xFF;//选择清空
}

void Widget::on_tabWidget_currentChanged(int index)//翻页
{
    key_click_handle(0xFF);//状态归零
    key_num = 0;
}

void Widget::ui2para()//ui控件转为参数数组
{
    CFG_LIGHT = ui->cBox_light->currentIndex();//灯效配置
    CFG_BRIGHT = ui->spinBox_bright->value();//灯效亮度
    for(int i = 0; i < 4; i++){
        CFG_LED_BRT(i) = sbox_leds[i]->value();//灯亮度
        CFG_LED(i) = (cbox_leds[i]->currentIndex() & 0x0F) | (((bool)cb_leds[i]->isChecked()) << 7);//灯配置
    }
    CFG_LED_DISK = (bool)ui->cB_led_disk->isChecked();//U盘读写指示灯
    CFG_USB_LIMIT = ui->dpinBox_limit->value() * 100;//CUSB限流值
    CFG_USB_DELAY = ui->spinBox_delay->value();//CUSB关断延迟
    CFG_USB_REST = ui->dpinBox_rest->value() * 10;//CUSB重开延迟
    CFG_USB_DEFAULT = (bool)ui->cB_cusb_dir->isChecked();//CUSB不受控时的默认设置
    CFG_BOOT_DISK = (bool)ui->cB_key_to_disk->isChecked();//启动时按按键可进入U盘
    CFG_G24_MODE = ui->cBox_g24->currentIndex();//2.4G接收使能
    CFG_SW_DIR = (bool)ui->cB_sw_dir->isChecked();//开关方向
    CFG_SW_MODE = ui->cBox_sw->currentIndex();//开关模式
    for(int i = 0; i < 8; i++){
        if(cbox_keys[i]) CFG_KEY_MODE(i) = cbox_keys[i]->currentIndex();//按键模式
        else CFG_KEY_MODE(i) = 1;//旋钮的等效按键默认为按键模式
        CFG_KEY_FUNC(i) = keys_func[i];//按键功能键
        CFG_KEY_KV(i) = keys_kv[i];//按键键值
    }
    CFG_EC_DIR1 = (bool)ui->cB_ec_dir_1->isChecked();//旋钮方向
    CFG_EC_DIR2 = (bool)ui->cB_ec_dir_2->isChecked();//旋钮方向
}

void Widget::para2ui()//参数数组转为ui控件
{
    ui->cBox_light->setCurrentIndex(CFG_LIGHT);//灯效配置
    ui->spinBox_bright->setValue(CFG_BRIGHT);//灯效亮度
    for(int i = 0; i < 4; i++){
        sbox_leds[i]->setValue(CFG_LED_BRT(i));//灯亮度
        cbox_leds[i]->setCurrentIndex(CFG_LED(i) & 0x0F);//灯配置 灯映射
        cb_leds[i]->setChecked(CFG_LED(i) >> 7);//灯配置 灯反向
    }
    ui->cB_led_disk->setChecked(CFG_LED_DISK);//U盘读写指示灯
    ui->dpinBox_limit->setValue((double)CFG_USB_LIMIT / 100);//CUSB限流值
    ui->spinBox_delay->setValue(CFG_USB_DELAY);//CUSB关断延迟
    ui->dpinBox_rest->setValue((double)CFG_USB_REST / 10);//CUSB重开延迟
    ui->cB_cusb_dir->setChecked(CFG_USB_DEFAULT);//CUSB不受控时的默认设置
    ui->cB_key_to_disk->setChecked(CFG_BOOT_DISK);//启动时按按键可进入U盘
    ui->cBox_g24->setCurrentIndex(CFG_G24_MODE);//2.4G接收使能
    ui->cB_sw_dir->setChecked(CFG_SW_DIR);//开关方向
    ui->cBox_sw->setCurrentIndex(CFG_SW_MODE);//开关模式
    for(int i = 0; i < 8; i++){
        if(cbox_keys[i]) cbox_keys[i]->setCurrentIndex(CFG_KEY_MODE(i));//按键模式
        if(/*CFG_KEY_MODE(i) == */1) setKeyValue(i, CFG_KEY_FUNC(i), CFG_KEY_KV(i));//设置按键
    }
    ui->cB_ec_dir_1->setChecked(CFG_EC_DIR1);//旋钮方向
    ui->cB_ec_dir_2->setChecked(CFG_EC_DIR2);//旋钮方向
}


bool Widget::openHID(uint16_t vid, uint16_t pid)//打开HID设备
{
//    hidApi = new HidApi(); hidDev = new HidDevice();//HID设备
    HidDeviceList devList;//HID设备列表
    
    if(!hidApi->isInitialized()) return false;//HidApi未成功初始化
    
    if(hidDev && hidDev->isOpened()) hidDev->close();//若已打开则先关闭
    
    devList = hidApi->scanDevices(vid, pid);//按VID和PID扫描设备
    
    for(size_t i = 0; i < devList.size(); i++){//可打印扫描到的设备的信息
        if(devList[i].getUsagePage() == 0xFF00){//在devList中找到要通信的设备
            *hidDev = devList[i];//选定该设备
            if(!hidDev->isInitialized()) return false;//HID设备未初始化则退出
            if(!hidDev->open()) return false;//HID设备打开失败则退出
            return true;//成功
        }
    }
    return false;//无可用HID或无设备响应
}

uint32_t Widget::connectHID(uint8_t cmd)//以自定义HID连接设备
{
    if(!openHID(ui->spinBox_vid->value(), ui->spinBox_pid->value())) return false;//若打开HID设备失败则退出

    std::string hidWriteStr;//创建写入缓存
    hidWriteStr.resize(65, 0);//重置为65字节
    if(cmd == 0){//配置命令及数据
        hidWriteStr[0] = 0;
        if(cmd == 0) hidWriteStr[1] = 0x01, hidWriteStr[2] = 64;//包头填入配置连接命令
        for(int i = 0; i < PARA_LEN; i++) hidWriteStr[3 + i] = para[i];//填入配置数据
        
        while(hidDev->readAvailable()) hidDev->read(1);//若已有数据则先读出
        if(hidDev->write(hidWriteStr) != 65) return false;//若写入失败则退出
        
        std::string hidReadStr = hidDev->read(500);//等待读取响应数据最多500ms
        hidDev->close();//关闭HID设备
        if(hidReadStr.length() == 65 && hidReadStr[1] == 0x01 && hidReadStr[2] == ~hidWriteStr[2]){//若正确响应
            return true;
        }
    }
//    else if(cmd == 1){//采样命令
//        hidWriteStr[0] = 0;
//        hidWriteStr[1] = 'M'; hidWriteStr[2] = 'B';//填入采样命令
//    }
    return false;//无可用HID或无设备响应
}

void Widget::openCfgFile()//打开配置文件
{
    if(!read_cfg_file(fileNow, para, PARA_LEN)){
        QMessageBox::critical(this,"提示","无路径");
        return;
    }
    para2ui();//参数数组转为ui控件
    if(/*cfgUnit->read_cfg_data()*/1){
        QFileInfo fileinfo = QFileInfo(fileNow);//文件信息
        ui->lineEdit_fileName->setText(fileinfo.fileName());//显示文件名
    }
    else QMessageBox::critical(this,"提示","配置读取失败");
}

void Widget::saveCfgFile()//保存配置文件
{
    QFileInfo fileinfo = QFileInfo(fileNow);//文件信息
    if(fileinfo.fileName().isEmpty()) return;
    if(fileinfo.suffix().isEmpty()){
        fileNow += ".dofcfg";//若无后缀则添加
        fileinfo = QFileInfo(fileNow);
    }
    ui2para();//ui控件转为参数数组
    if(/*cfgUnit->write_cfg_data()*/1){
        write_cfg_file(fileNow, para, PARA_LEN);
        ui->lineEdit_fileName->setText(fileinfo.fileName());//显示文件名
    }
    else QMessageBox::critical(this,"提示","配置超长");
}

void Widget::on_Bt_write_clicked()//写入设备按钮
{
    if(ui->cB_get_VA->isChecked()){//若正在采样
        QMessageBox::critical(this,"禁止写入","请先停止CUSB采样");
        return;
    }
    ui->Bt_write->setStyleSheet(style_big_gray);
    ui->Bt_write->setEnabled(false);//暂时禁用按钮
    ui2para();//ui控件转为参数数组
    if(/*cfgUnit->write_cfg_data()*/1){
        if(connectHID(0)) ;//QMessageBox::information(this,"提示","写入成功");
        else QMessageBox::critical(this,"HID通信","写入失败");
    }
    ui->Bt_write->setEnabled(true);//恢复启用按钮
    ui->Bt_write->setStyleSheet(style_big_black);
}

void Widget::on_Bt_open_clicked()//打开文件按钮
{
    QString fileNew = QFileDialog::getOpenFileName(this,QStringLiteral("打开配置文件"),
                                                   "./configFile",QStringLiteral("配置文件(*dofcfg)"));
    if(fileNew.isEmpty()) return;
    fileNow = fileNew;
    openCfgFile();
}

void Widget::on_Bt_save_clicked()//保存文件按钮
{
    if(fileNow.isEmpty()){
        fileNow = QFileDialog::getSaveFileName(this,QStringLiteral("保存配置文件"),
                                               "./configFile",QStringLiteral("配置文件(*dofcfg)"));
    }
    saveCfgFile();//保存配置文件
}

void Widget::on_Bt_saveas_clicked()//另存为文件按钮
{
    fileNow = QFileDialog::getSaveFileName(this,QStringLiteral("另存为配置文件"),
                                           "./configFile",QStringLiteral("配置文件(*dofcfg)"));
    saveCfgFile();//保存配置文件
}


void Widget::on_Bt_clear_J_clicked()//能量清空
{
    adcThread->adcValueJ = 0;
    //QApplication::beep();//发出系统提示音
}

void Widget::on_cB_get_VA_stateChanged(int arg1)//启动/停止采样
{
    ui->cBox_sample->setEnabled(!arg1);//启动采样则禁用更改周期
    if(arg1){
        adcThread->sampleStart = 1;//置为预启动状态
    }
    else{
        adcThread->sampleStart = 3;//置为预停止状态
    }
}

void Widget::timeout_slot()//定时器处理函数
{
    adcThread->sampleFlag = 1;//为线程提供心跳
    adcThread->watchDog++;//看门狗计数
    if(adcThread->watchDog > 100){//看门狗超时认为线程卡死
        //ui->cB_cusb_drop->setChecked(false);//取消模拟卡死 测试代码
        //printf("watchDogOut\n");
        //QApplication::beep();//发出系统提示音
        adcThread->watchDog = 0;//喂狗
        
        adcThread->terminate();//强行结束线程
        adcThread->wait();//等待完成
        adcThread->start();//重启线程
    }
    QString numStr = "";//用来显示各个数值
    numStr = QString::number(adcThread->adcValueV, 'f', 4);
    ui->lineEdit_V->setText(numStr);
    numStr = QString::number(adcThread->adcValueA, 'f', 4);
    ui->lineEdit_A->setText(numStr);
    numStr = QString::number(adcThread->adcValueW, 'f', 4);
    ui->lineEdit_W->setText(numStr);
    numStr = QString::number(adcThread->adcValueJ, 'f', 3);
    ui->lineEdit_J->setText(numStr);
    numStr = "状态:";
    if(adcThread->cusbState & 0x0F) numStr += "ON";
    else numStr += "OFF";
    if(adcThread->cusbState & 0xF0) numStr += "-CUT";
    ui->lb_cusb_state->setText(numStr);
}










