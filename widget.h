#ifndef WIDGET_H
#define WIDGET_H

#include "tools.h"
#include "mythread.h"

#include <QWidget>
#include "windows.h"
#include "hidapi.h"
#include <iostream>

#include <QSpinBox>
#include <QComboBox>
#include <QInputDialog>
#include <QPlainTextEdit>
#include <QTimer>

class MyThread;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    
    HidApi *hidApi = NULL;//HID
    HidDevice *hidDev = NULL;//HID设备
    
    MyThread *adcThread = NULL;//采样线程
    QTimer timer;//定义计时器
    
    QString fileNow = "";//现在打开的文件
    
    volatile bool ifSending = false;//是否正在发送
    
    volatile bool ifHold = false;//功能键是否按键保持
    volatile uint8_t func = 0;//功能键按下情况
    volatile uint8_t button_choose = 0xFF;//按钮选择
    volatile uint8_t key_num = 0;//按下按键数量
    
    QString style_big_black, style_big_gray;//样式设定
    QString style_mid_black, style_mid_gray;//样式设定
    QString style_small_black, style_small_gray;//样式设定
    
    QComboBox *cbox_leds[4];
    QSpinBox *sbox_leds[4];
    QCheckBox *cb_leds[4];
    
    QComboBox *cbox_keys[8];
    QLabel *lb_keys[8];
    QPushButton *bt_keys[8];
    uint8_t keys_kv[8], keys_func[8];
    
    uint8_t para[PARA_LEN];//参数数组
    
public:
    bool openHID(uint16_t vid, uint16_t pid);//打开HID设备
    uint32_t connectHID(uint8_t cmd);//以自定义HID连接设备
    
    void openCfgFile();//打开配置文件
    void saveCfgFile();//保存配置文件
    void openKeyFile();//打开key文件
    void saveKeyFile();//保存key文件
    
    void setKeyValue(uint8_t key_i, uint8_t fv, uint8_t kv);//设置按键
    
private slots:
    void key_handle(uint8_t keyValue, bool ifPress);//按键处理
    void keyPressEvent(QKeyEvent *event);//按键按下
    void keyReleaseEvent(QKeyEvent *event);//按键抬起
    
    void on_Bt_mouse_L_clicked();//鼠标左键
    void on_Bt_mouse_M_clicked();//鼠标中键
    void on_Bt_mouse_R_clicked();//鼠标右键
    void on_Bt_wheel_up_clicked();//鼠标滚轮向上
    void on_Bt_wheel_down_clicked();//鼠标滚轮向下
    void on_Bt_vol_stop_clicked();//媒体停止
    void on_Bt_vol_mute_clicked();//媒体静音
    void on_Bt_vol_up_clicked();//媒体音量加
    void on_Bt_vol_down_clicked();//媒体音量减
    void on_Bt_vol_next_clicked();//媒体下一个
    void on_Bt_vol_prev_clicked();//媒体上一个
    void on_Bt_empty_clicked();//空按键
    
    void on_Bt_hold_clicked();//按键保持
    
    void key_click_handle(uint8_t button_cs);//8个按钮按下处理
    void on_key_ec_key_11_clicked();
    void on_key_ec_key_12_clicked();
    void on_key_ec_key_1_clicked();
    void on_key_key_1_clicked();
    void on_key_ec_key_21_clicked();
    void on_key_ec_key_22_clicked();
    void on_key_ec_key_2_clicked();
    void on_key_key_2_clicked();
    
    void on_key_clear_clicked();//按键清除键
    void on_tabWidget_currentChanged(int index);//翻页
    
    void ui2para();//ui控件转为参数数组
    void para2ui();//参数数组转为ui控件
    
    void on_Bt_write_clicked();//写入设备按钮
    void on_Bt_open_clicked();//打开配置文件按钮
    void on_Bt_save_clicked();//保存配置文件按钮
    void on_Bt_saveas_clicked();//另存为配置文件按钮
    
    void on_Bt_clear_J_clicked();//能量清空
    void on_cB_get_VA_stateChanged(int arg1);//开始/停止采样
    void timeout_slot();//定时器处理
    
public://private:
    Ui::Widget *ui;
};

#endif // WIDGET_H










