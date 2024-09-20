#ifndef MYTHREAD_H
#define MYTHREAD_H

#include "hidapi.h"
#include "widget.h"
#include "ui_widget.h"
#include <QWidget>
#include <QtCore/QCoreApplication>
#include <QThread>
#include <QMutex>
#include <QDebug>


class Widget;

class MyThread : public QThread
{
public:
    MyThread(Widget *myWidget);
    ~MyThread();
    
    void run();//线程体函数
    
    bool getAdcData(std::string adcStr);//处理数据
    
    Widget *widget = NULL;
    Ui::Widget *ui = NULL;
    HidDevice *hidDev = NULL;//HID设备
    
    volatile uint8_t watchDog = 0;//看门狗计数
    
    volatile uint8_t sampleFlag = 0;//由主线程提供心跳
    volatile uint8_t sampleStart = 0;//采样启动
    volatile uint8_t cusbState = 0;//端口状态
    volatile double adcValueV = 0;//电压
    volatile double adcValueA = 0;//电流
    volatile double adcValueW = 0;//功率
    volatile double adcValueJ = 0;//能量
    
    
signals:
    //void requestStop();
};

#endif // MYTHREAD_H
