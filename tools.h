#ifndef TOOLS_H
#define TOOLS_H

//#include "widget.h"
#include <QCoreApplication>
#include <QTime>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QString>
#include <QDebug>
#include <QWidget>
#include <QPixmap>
#include <QDesktopWidget>

#define PARA_LEN    62//参数数据长度
#define hidCs       0

#define CFG_LIGHT			para[0]//灯效配置
#define CFG_BRIGHT			para[1]//灯效亮度
#define CFG_LED_BRT(i)		para[2 + (i)]//灯亮度
#define CFG_LED(i)			para[6 + (i)]//灯配置
#define CFG_LED_MAP(i)		(0x0F & CFG_LED(i))//灯映射
#define CFG_LED_DIR(i)		(CFG_LED(i) >> 7)//灯反向
#define CFG_LED_DISK		para[10]//U盘读写指示灯

#define CFG_USB_LIMIT		para[12]//CUSB限流值
#define CFG_USB_DELAY		para[13]//CUSB关断延迟
#define CFG_USB_REST		para[14]//CUSB重开延迟
#define CFG_USB_DEFAULT		para[15]//CUSB不受控时的默认设置
#define CFG_BOOT_DISK		para[16]//启动时按按键可进入U盘
#define CFG_G24_MODE		para[17]//2.4G模式
#define CFG_SW_DIR			para[18]//开关方向
#define CFG_SW_MODE			para[19]//开关模式

#define CFG_KEY_MODE(i)		para[20 + 4*hidCs + (i)]//按键模式
#define CFG_KEY_FUNC(i)		para[28 + 4*hidCs + (i)]//按键功能键
#define CFG_KEY_KV(i)		para[36 + 4*hidCs + (i)]//按键键值

#define CFG_EC_DIR1			para[50]//旋钮方向
#define CFG_EC_DIR2			para[51]//旋钮方向

#define kv_report       234
#define kv_loop         235

#define kv_point		237
#define kv_vol_next     238
#define kv_vol_prev     239
#define kv_wheel_up     240
#define kv_wheel_down   241
#define kv_vol_up       242
#define kv_vol_down     243
#define kv_vol_mute     244
#define kv_vol_stop     245
#define kv_mouse_l      246
#define kv_mouse_m      247
#define kv_mouse_r      248
#define kv_ctrl         249
#define kv_shift        250
#define kv_alt          251
#define kv_win          252
#define kv_shortcut     253
#define kv_delay		254

#define LIMIT(x,min,max)	(((x)<=(min))?(min):(((x)>=(max))?(max):(x)))//上下限
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
//#define colorAngle 600//色环的1/6


bool read_asciiArray_file(QString path, uint8_t *buf, uint16_t len);//读明文数组文件
bool read_cfg_file(QString path, uint8_t *buf, uint16_t len);//读配置文件
bool write_cfg_file(QString path, uint8_t *buf, uint16_t len);//写配置文件

//bool read_key_file(QString path, uint8_t *buf);//读key文件
//bool write_key_file(QString path, uint8_t *buf);//写key文件

void mySleep(int ms);//毫秒延时

uint8_t key_to_USB(int key, int Vkey);//QT键值转USB键值
QString USB_to_str(uint8_t key, bool shift);//USB键值转按键名

void rgbToHsv(uint8_t vR, uint8_t vG, uint8_t vB, uint16_t* pH, uint16_t* pS, uint16_t* pV);
void hsvToRgb(uint16_t vH, uint16_t vS, uint16_t vV, uint8_t* pR, uint8_t* pG, uint8_t* pB);

QPixmap rgbToPix(uint8_t r, uint8_t g, uint8_t b, uint16_t w, uint16_t h, uint8_t mode);

uint16_t EndianConvert16(uint16_t num);//16位大小端转换


#endif // TOOLS_H




