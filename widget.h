#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "windef.h"         //WIN下定义
#include "qdatetime.h"      //系统时间头文件
#include <QCloseEvent>      //关闭按钮事件头文件
#include <QMessageBox>      //提示框头文件
#include <QDebug>           //调试用头文件
#include <QTimer>           //定时器头文件
#include <QThread>          //线程用头文件
#include <QMetaType>        //注册自定义信号与槽
#include "ControlCAN.h"     //添加CAN卡库
#include "thread.h"         //数据接收线程头文件
#include <QPainter>         //引入绘制头文件



QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE


/**CAN发送数据联合体**/

//5.1.3车辆运动控制
typedef union
{
    struct
    {
        uint8_t runMode                     :2; //自动驾驶模式使能
        uint8_t steerEnable                 :1; //转向使能
        uint8_t brakeEnable                 :1; //制动使能
        uint8_t gearEnable                  :1; //档位使能
        uint8_t gasEnable                   :1; //油门使能
        uint8_t parkEnable                  :1; //驻车使能
        uint8_t lightEnable                 :1; //声光使能
        uint8_t targetGear                  :4; //目标档位
        uint8_t targetPark                  :4; //目标驻车
        uint8_t targetGasValue              :8; //目标油门百分比
        uint8_t target_steer_angle_value_H  :8; //目标转角高字节
        uint8_t target_steer_angle_value_L  :8; //目标转角低字节
        uint8_t target_steer_angle_gradiant :8; //转角速度
        uint8_t targetBrakePressure         :8; //目标制动压力
        uint8_t res1                        :4;
        uint8_t rolling_counter             :4; //循环计数
    }TxData;
    uint8_t UCData[8];
} CANSEND08;

//5.1.5灯光管理
typedef union
{
    struct
    {
        uint8_t turnLightLeft               :2; //左转灯
        uint8_t turnLightRight              :2; //右转灯
        uint8_t farHeadLight                :2; //远光灯
        uint8_t nearHeadLight               :2; //近光灯
        uint8_t flashLight                  :2; //双闪灯
        uint8_t backLight                   :2; //倒车灯
        uint8_t brakeLight                  :2; //刹车灯
        uint8_t trumpet                     :2; //喇叭
        uint8_t wiper                       :2; //雨刷
        uint8_t res1                        :6; //目标制动压力
        uint8_t res2                        :8;
        uint8_t res3                        :8;
        uint8_t res4                        :8;
        uint8_t res5                        :8;
        uint8_t res6                        :8;
    }TxData;
    uint8_t UCData[8];
} CANSEND0C;

/**CAN接收数据联合体**/

//5.1.6车辆授权回复
typedef union
{
    struct
    {
        uint8_t respondCMD      :8; //回复命令
        uint8_t CMDState        :8; //命令状态
        uint8_t carNum1_H       :8; //车辆编号1
        uint8_t carNum1_L       :8; //车辆编号1
        uint8_t carNum2_H       :8; //车辆编号2
        uint8_t carNum2_L       :8; //车辆编号2
        uint8_t carNum3_H       :8; //车辆编号3
        uint8_t carNum3_L       :8; //车辆编号3
    }RxData;
    uint8_t UCData[8];
} CANRECEIVE1814;

//5.1.7车辆状态1#
typedef union
{
    struct
    {
        uint8_t runModeState    :8; //车辆模式
        uint8_t steerState      :1; //授权状态
        uint8_t brakeState      :1; //授权状态
        uint8_t gearState       :1; //授权状态
        uint8_t gasState        :1; //授权状态
        uint8_t parkState       :1; //授权状态
        uint8_t lightState      :1; //授权状态
        uint8_t res1            :2;
        uint8_t errorCode_H     :8; //故障码
        uint8_t errorCode_L     :8; //故障码
        uint8_t res2            :8;
        uint8_t licenseState    :8; //授权状态
        uint8_t node            :8; //操作提示
        uint8_t res3            :4;
        uint8_t rolling_counter :4; //循环计数

    }RxData;
    uint8_t UCData[8];
} CANRECEIVE18F0;

//5.1.8车辆状态2#
typedef union
{
    struct
    {
        uint8_t GearSWState     :4; //
        uint8_t realGearState   :4; //
        uint8_t GasSWState      :8; //
        uint8_t realGasState    :8; //
        uint8_t RealSteerAngle_H:8; //
        uint8_t RealSteerAngle_L:8; //
        uint8_t SteerGradiant   :8; //
        uint8_t SteerTorque     :8; //
        uint8_t res1            :4;
        uint8_t rolling_counter :4; //循环计数

    }RxData;
    uint8_t UCData[8];
} CANRECEIVE15D1;

//5.1.9车辆状态3#
typedef union
{
    struct
    {
        uint8_t BrakeSWState    :8; //
        uint8_t realBrakeState  :8; //
        uint8_t res1            :8; //
        uint8_t res2            :8; //
        uint8_t res3            :8; //
        uint8_t res4            :8; //
        uint8_t res5            :8; //
        uint8_t res6            :4;
        uint8_t rolling_counter :4; //循环计数

    }RxData;
    uint8_t UCData[8];
} CANRECEIVE16D1;

//5.1.13电池状态1#
typedef union
{
    struct
    {
        uint8_t batteryState    :8; //电池状态
        uint8_t batteryValue    :8; //电池电量
        uint8_t batteryVol_H    :8; //电池电压
        uint8_t batteryVol_L    :8; //电池电压
        uint8_t batteryCur_H    :8; //电池电流
        uint8_t batteryCur_L    :8; //电池电流
        uint8_t batteryTem      :8; //电池温度
        uint8_t res1            :4;
        uint8_t rolling_counter :4; //循环计数

    }RxData;
    uint8_t UCData[8];
} CANRECEIVE20D1;


//5.1.10车辆里程信息
typedef union
{
    struct
    {
        uint8_t carSpeed_H      :8; //车辆速度
        uint8_t carSpeed_L      :8; //车辆速度
        uint8_t Subtotal_H      :8; //小计里程
        uint8_t Subtotal_L      :8; //小计里程
        uint8_t total_1         :8; //总里程
        uint8_t total_2         :8; //总里程
        uint8_t total_3         :8; //总里程
        uint8_t res1            :4;
        uint8_t rolling_counter :4; //循环计数


    }RxData;
    uint8_t UCData[8];
} CANRECEIVE1AD1;



class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event);
    ~Widget();


private:
    Ui::Widget *ui;


protected:
    virtual void closeEvent(QCloseEvent *event);


public:
    /**CAN 配置相关参数**/
    int m_connect;
    int m_sendTimeout;
    /**车辆指令参数**/
    CANSEND08 Data08;           //发送指令,发送周期50ms
    CANSEND0C Data0C;
    CANRECEIVE1814 Data1814;    //授权帧

    CANRECEIVE18F0 Data18F0;
    CANRECEIVE15D1 Data15D1;
    CANRECEIVE16D1 Data16D1;    //运行帧
    CANRECEIVE20D1 Data20D1;    //电源管理
    CANRECEIVE1AD1 Data1AD1;    //里程管理


public:
    /**控件变量相关**/
    uint8_t Run_Mode;           //行驶模式
    uint8_t Steer_EN;           //转向使能
    uint8_t Brake_EN;           //制动使能
    uint8_t Gear_EN;            //挡位使能
    uint8_t Gas_EN;             //油门使能
    uint8_t Park_EN;            //驻车使能
    uint8_t Light_EN;           //灯光使能

    uint8_t m_gear;             //目标挡位
    uint8_t m_park;             //目标驻车
    uint8_t m_gas;              //目标油门
    int m_steer;                //目标转角
    uint8_t m_gradiant;         //目标转速
    uint8_t m_brake;            //目标制动
    uint8_t rolling_counter;    //循环计数标志

    uint8_t m_turnlightleft;
    uint8_t m_turnlightright;
    uint8_t m_farheadlight;
    uint8_t m_nearheadlight;
    uint8_t m_flashlight;
    uint8_t m_backlight;
    uint8_t m_brakelight;
    uint8_t m_trumpet;
    uint8_t m_wiper;

private:
    /**线程定时器相关**/
    QTimer *Timer1;
    CANThread *CANTH;


private:
    QDateTime current_date_time;
    QString current_date;

private slots:
    /**线程定时器相关**/
    void dealTimer1();                                      //定时器槽函数
//    void dealReceiveData(PVCI_CAN_OBJ objs,uint length);    //数据接收处理函数
    void logger(uint level,QString message);
    void on_CANConnect_clicked();
    void on_CANStart_clicked();
//    void on_pushButtonCarLicense_clicked();
    void on_textBrowserMessage_textChanged();
//    void on_pushButtonTEN_clicked();
//    void on_pushButtonBEN_clicked();
//    void on_pushButtonGEN_clicked();
//    void on_pushButtonYEN_clicked();
//    void on_pushButtonPEN_clicked();
//    void on_pushButtonLEN_clicked();
//    void on_pushButtonD_clicked();
//    void on_pushButtonN_clicked();
//    void on_pushButtonR_clicked();
//    void on_pushButtonPark_clicked();
//    void on_horizontalSlider_valueChanged(int value);
//    void on_pushButtonTZ_clicked();
//    void on_pushButtonLF_clicked();
//    void on_pushButtonRI_clicked();
//    void on_pushButtonG_clicked();
//    void on_pushButtonB_clicked();
//    void on_pushButtonSound_pressed();
//    void on_pushButtonSound_released();
//    void on_pushButtonWP_clicked();
//    void on_pushButtonLL_clicked();
//    void on_pushButtonLR_clicked();
//    void on_pushButtonLEL_clicked();
//    void on_pushButtonLNear_clicked();
//    void on_pushButtonLRemote_clicked();
};
#endif // WIDGET_H
