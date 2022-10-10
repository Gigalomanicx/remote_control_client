#include "widget.h"
#include "ui_widget.h"
#include "thread.h"
#include "ControlCAN.h"
#include <QStandardItemModel>

#define TEXT_COLOR_RED(STRING) "<font color=red>" STRING "</font>" "<font color=black> </font>"
#define TEXT_COLOR_BLUE(STRING) "<font color=blue>" STRING "</font>" "<font color=black> </font>"
#define TEXT_COLOR_GREEN(STRING) "<font color=green>" STRING "</font>" "<font color=black> </font>"

QStandardItemModel *model;
bool DEBUG = false;
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowTitle("RemoteControl学生端");//设置窗口名
    /**CAN 相关参数初始化**/
    qRegisterMetaType<VCI_CAN_OBJ>("VCI_CAN_OBJ");//注册can结构体
    qRegisterMetaType<PVCI_CAN_OBJ>("PVCI_CAN_OBJ");//注册can结构体
    m_connect = 0;
    m_sendTimeout = 0;
    /**CAN数据初始化**/
    Steer_EN = 0;
    Brake_EN = 0;
    Gear_EN = 0;
    Gas_EN = 0;
    Park_EN = 0;
    Light_EN = 0;

    m_gear = 0;
    m_park = 1;
    m_gas = 0;
    m_brake = 0;
    m_steer = 0;
    m_gradiant = 0;

    m_turnlightleft = 0;
    m_turnlightright = 0;
    m_farheadlight = 0;
    m_nearheadlight = 0;
    m_flashlight = 0;
    m_backlight = 0;
    m_brakelight = 0;
    m_trumpet = 0;
    m_wiper = 0;



    /**定时器线程相关**/
    Timer1 = new QTimer(this);
    connect(Timer1, &QTimer::timeout, this, &Widget::dealTimer1);

    /**数据接收处理相关**/
    CANTH = new CANThread(this);
    CANTH->CAN_Para_Init();
    connect(CANTH,&CANThread::DataReceiveDone,this, &Widget::dealReceiveData);

    init();




}

Widget::~Widget()
{
    delete ui;
}

//控件初始化
void Widget::init(){
    logger(1,"欢迎使用RemoteControl客户端");
    logger(1,"1.请连接车辆CAN总线");
    logger(1,"2.然后开启CAN总线");
    logger(1,"3.获取车辆控制授权");
    //参照天隼自动驾驶汽车CAN通信规范
    model = new QStandardItemModel(ui->treeView);//创建模型指定父类
    ui->treeView->setModel(model);

    model->setHorizontalHeaderLabels(QStringList()<<QStringLiteral("C-CAN数据帧")<<QStringLiteral("帧参数")<<QStringLiteral("描述"));
//    //1
//    model->setItem(0,new QStandardItem("车辆授权"));
//        model->item(0)->setChild(0,0,new QStandardItem("授权命令"));
//        model->item(0)->setChild(0,1,new QStandardItem(""));
//        model->item(0)->setChild(0,2,new QStandardItem("0x00-本地授权 0x01-取消本地授权 0x40-查询授权状态 0x41-获取车辆编号"));

//        model->item(0)->setChild(1,0,new QStandardItem("授权码"));
//        model->item(0)->setChild(1,1,new QStandardItem(""));
//        model->item(0)->setChild(1,2,new QStandardItem("6字节授权码"));
//    //2
//    model->setItem(1,new QStandardItem("车辆配置"));
//        model->item(1)->setChild(0,0,new QStandardItem("配置项目"));
//        model->item(1)->setChild(0,1,new QStandardItem(""));
//        model->item(1)->setChild(0,2,new QStandardItem("0x01-转向总成配置 其他值无效"));

//        model->item(1)->setChild(1,0,new QStandardItem("配置命令"));
//        model->item(1)->setChild(1,1,new QStandardItem(""));
//        model->item(1)->setChild(1,2,new QStandardItem("0x01-设置方向盘当前角度为零度角 其他值无效"));
//    //3
//    model->setItem(2,new QStandardItem("车辆运动控制"));
//    model->item(2)->setChild(0,0,new QStandardItem("车辆控制模式"));
//    model->item(2)->setChild(0,1,new QStandardItem(""));
//    model->item(2)->setChild(0,2,new QStandardItem("0x0-手动模式 0x1-自动模式-部分 0x3-自动模式-完全"));

//    model->item(2)->setChild(1,0,new QStandardItem("转向控制模式"));
//    model->item(2)->setChild(1,1,new QStandardItem(""));
//    model->item(2)->setChild(1,2,new QStandardItem("0x0-手动模式 0x1-自动模式"));

//    model->item(2)->setChild(2,0,new QStandardItem("制动控制模式"));
//    model->item(2)->setChild(2,1,new QStandardItem(""));
//    model->item(2)->setChild(2,2,new QStandardItem("0x0-手动模式 0x1-自动模式"));

//    model->item(2)->setChild(3,0,new QStandardItem("档位控制模式"));
//    model->item(2)->setChild(3,1,new QStandardItem(""));
//    model->item(2)->setChild(3,2,new QStandardItem("0x0-手动模式 0x1-自动模式"));

//    model->item(2)->setChild(4,0,new QStandardItem("油门控制模式"));
//    model->item(2)->setChild(4,1,new QStandardItem(""));
//    model->item(2)->setChild(4,2,new QStandardItem("0x0-手动模式 0x1-自动模式"));

//    model->item(2)->setChild(5,0,new QStandardItem("驻车控制模式"));
//    model->item(2)->setChild(5,1,new QStandardItem(""));
//    model->item(2)->setChild(5,2,new QStandardItem("0x0-手动模式 0x1-自动模式"));
//    //4
//    model->setItem(3,new QStandardItem("电源管理"));

//    //5
//    model->setItem(4,new QStandardItem("灯光管理"));

//    //6
//    model->setItem(5,new QStandardItem("车辆授权回复"));

//    //7
//    model->setItem(6,new QStandardItem("车辆状态 1#"));

//    //8
//    model->setItem(7,new QStandardItem("车辆状态 2#"));

//    //9
//    model->setItem(8,new QStandardItem("车辆状态 3#"));

//    //10
//    model->setItem(9,new QStandardItem("车辆里程信息"));

//    //11
//    model->setItem(10,new QStandardItem("车辆灯光状态"));

//    //12
//    model->setItem(11,new QStandardItem("车辆电源状态"));

//    //13
//    model->setItem(12,new QStandardItem("电池状态 1#"));

//    //14
//    model->setItem(13,new QStandardItem("电池状态 2#"));

//    //15
//    model->setItem(14,new QStandardItem("超声波雷达探测效果"));




    model->setItem(0,new QStandardItem("重要参数"));
    model->item(0)->setChild(0,new QStandardItem("车辆名称"));
    model->item(0)->setChild(0,1,new QStandardItem("X牌电动接驳车"));
    model->item(0,0)->setChild(1,0,new QStandardItem("行驶状态"));
    model->item(0,0)->setChild(1,1,new QStandardItem(" "));
    model->item(0,0)->setChild(2,0,new QStandardItem("实时速度"));
    model->item(0,0)->setChild(2,1,new QStandardItem(" Km/h"));
    model->item(0,0)->setChild(3,0,new QStandardItem("剩余电量"));
    model->item(0,0)->setChild(3,1,new QStandardItem(" %"));


    model->setItem(1,0,new QStandardItem("转向"));
    model->item(1,0)->setCheckable(true);
    model->item(1,0)->setChild(0,0,new QStandardItem("目标转角"));
    model->item(1,0)->setChild(0,1,new QStandardItem(" deg"));
    model->item(1,0)->setChild(1,0,new QStandardItem("实时转角"));
    model->item(1,0)->setChild(1,1,new QStandardItem(" deg"));

    model->setItem(2,0,new QStandardItem("档位"));
    model->item(2,0)->setCheckable(true);
    model->item(2,0)->setChild(0,0,new QStandardItem("目标挡位"));
    model->item(2,0)->setChild(0,1,new QStandardItem(" 挡"));

    model->setItem(3,0,new QStandardItem("油门"));
    model->item(3,0)->setCheckable(true);
    model->item(3,0)->setChild(0,0,new QStandardItem("目标油门"));
    model->item(3,0)->setChild(0,1,new QStandardItem(" %"));
    model->item(3,0)->setChild(1,0,new QStandardItem("实时油门"));
    model->item(3,0)->setChild(1,1,new QStandardItem(" %"));



    model->setItem(4,0,new QStandardItem("制动"));
    model->item(4,0)->setCheckable(true);
    model->item(4,0)->setChild(0,0,new QStandardItem("目标制动"));
    model->item(4,0)->setChild(0,1,new QStandardItem(" bar"));
    model->item(4,0)->setChild(1,0,new QStandardItem("实时制动"));
    model->item(4,0)->setChild(1,1,new QStandardItem(" bar"));

    model->setItem(5,0,new QStandardItem("驻车"));
    model->item(5,0)->setCheckable(true);

    model->setItem(6,0,new QStandardItem("灯光"));
    model->item(6,0)->setCheckable(true);
    model->item(6,0)->setChild(0,0,new QStandardItem("前大灯"));
    model->item(6,0)->setChild(0,1,new QStandardItem("关"));
    model->item(6,0)->setChild(1,0,new QStandardItem("左转向灯"));
    model->item(6,0)->setChild(1,1,new QStandardItem("关"));
    model->item(6,0)->setChild(2,0,new QStandardItem("右转向灯"));
    model->item(6,0)->setChild(2,1,new QStandardItem("关"));
    model->item(6,0)->setChild(3,0,new QStandardItem("双闪灯"));
    model->item(6,0)->setChild(3,1,new QStandardItem("关"));
    model->item(6,0)->setChild(4,0,new QStandardItem("远灯"));
    model->item(6,0)->setChild(4,1,new QStandardItem("关"));
    model->item(6,0)->setChild(5,0,new QStandardItem("近灯"));
    model->item(6,0)->setChild(5,1,new QStandardItem("关"));
    model->item(6,0)->setChild(6,0,new QStandardItem("雨刷器"));
    model->item(6,0)->setChild(6,1,new QStandardItem("关"));
    //车辆授权之后 禁用的按钮都会解除 即车辆全授权，但在此之前，关于车辆控制的控件都是禁用状态

    ui->pushButtonRemote->setEnabled(false);
    ui->pushButtonNear->setEnabled(false);
    ui->pushButtonDF->setEnabled(false);
    ui->pushButtonLF->setEnabled(false);
    ui->pushButtonLL->setEnabled(false);
    ui->pushButtonLR->setEnabled(false);
    ui->pushButtonRI->setEnabled(false);
    ui->pushButtonTZ->setEnabled(false);
    ui->pushButtonWP->setEnabled(false);
    ui->pushButtonBEN->setEnabled(false);
    ui->pushButtonYEN->setEnabled(false);
    ui->pushButtonBack->setEnabled(false);
    ui->pushButtonSound->setEnabled(false);
    ui->horizontalSlider->setEnabled(false);
    ui->verticalSlider->setEnabled(false);

}
void Widget::logger(uint level,QString message){
    switch (level)
    {
        case 0://DEBUG Level
            if(DEBUG)
            {
                ui->textBrowserMessage->append((QString)"<font color=\"#40E0D0\">[DEBUG][" + QDateTime::currentDateTime().toString("hh:mm:ss.zzz]:") + message + "\n</font>");
            }
            break;
        case 1://INFO level
            ui->textBrowserMessage->append((QString)"<font color=\"#000000\">[INFO][" + QDateTime::currentDateTime().toString("hh:mm:ss.zzz]:") + message + "\n</font>");
            break;
        case 2://WARN level
            ui->textBrowserMessage->append((QString)"<font color=\"#FF8C00\">[WARN][" + QDateTime::currentDateTime().toString("hh:mm:ss.zzz]:") + message + "\n</font>");
            break;
        case 3://ERROR level
            ui->textBrowserMessage->append((QString)"<font color=\"#FF0000\">[ERROR][" + QDateTime::currentDateTime().toString("hh:mm:ss.zzz]:") + message + "\n</font>");
            break;
        case 4://FATAL level
            ui->textBrowserMessage->append((QString)"<font color=\"#FF0000\">[FATAL][" + QDateTime::currentDateTime().toString("hh:mm:ss.zzz]:") + message + "\n</font>");
            break;
        default://UNKNOWN
            ui->textBrowserMessage->append((QString)"<font color=\"#000000\">[UNKNOWN][" + QDateTime::currentDateTime().toString("hh:mm:ss.zzz]:") + message + "\n</font>");
            break;
    }

}

void Widget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

//窗口关闭事件 没什么必要
void Widget::closeEvent(QCloseEvent *event){
    int connect = m_connect;
    m_connect = 0;
    if(connect)
    {
        VCI_CloseDevice(static_cast<DWORD>(CANTH->m_devtype),CANTH->m_devind); //关闭CAN设备
        Timer1->stop();                 //停止发送定时器
        CANTH->setFlag(true);           //设置线程退出标志
        CANTH->quit();                  //退出接收线程
        CANTH->wait();                  //阻塞线程
        /**提示信息显示清除**/
        ui->textBrowserMessage->clear();
        ui->treeView->close();
    }

    //退出提示窗口
    switch( QMessageBox::information( this, tr("CarTools提示"),tr("是否要退出CarTools程序?"),tr("确定"), tr("取消"), nullptr, 1 ) )
     {
        case 0:
            event->accept();
            break;

        case 1:

        default:
            event->ignore();
            break;
    }
}


//点击连接按钮事件
void Widget::on_CANConnect_clicked()
{
    //如果连接已经成功，将要关闭的话
    if(m_connect == 1)
    {
        m_connect = 0;
        ui->CANState->setChecked(false);
        //配置CAN的四个选项设置为可选
        ui->CANDevice->setEnabled(true);
        ui->CANIndex->setEnabled(true);
        ui->CANPort->setEnabled(true);
        ui->CANBaud->setEnabled(true);

        VCI_CloseDevice(static_cast<DWORD>(CANTH->m_devtype),CANTH->m_devind);
        CANTH->setFlag(true);   //设置线程退出标志
        CANTH->quit();          //退出接收线程
        CANTH->wait();
        Timer1->stop();         //停止发送定时器
        /**提示信息显示清除**/
        ui->textBrowserMessage->clear();
        return;
    }
    //如果没连接成功，那么开始连接
    VCI_INIT_CONFIG init_config;
    int index, mode, cannum, baud, DevType;
    /**设备选择,默认USBCAN_2E_U**/
    DevType = ui->CANDevice->currentIndex();
    if(0 == DevType)
    {
        DevType = 20;
    }
    else if (1 == DevType)
    {
        DevType = 21;
    }
    else if (2 == DevType)
    {
        DevType = 19;
    }
    else
    {
        DevType = 22;
    }
    CANTH->m_devtype=DevType;
    /**索引选择,默认为0**/
    index = ui->CANIndex->currentIndex();
    CANTH->m_devind = static_cast<DWORD>(index);
    /**CAN工作模式选择**/
    mode = 0;   //0:正常模式,   1:只听模式
    /**CAN通道选择,默认为0**/
    cannum = ui->CANPort->currentIndex();
    CANTH->m_cannum = cannum;
    /**波特率设置,默认500Kpbs**/
    baud = ui->CANBaud->currentIndex();
    if(baud == 0)
    {
        baud = 0x060003;
    }
    else if(baud == 1)
    {
        baud = 0x060004;
    }
    else if(baud == 2)
    {
        baud = 0x060007; //目前给设置为默认500Kpbs
    }
    else if(baud == 3)
    {
        baud = 0x1C0008;
    }
    else if(baud == 4)
    {
        baud = 0x1C0011;
    }
    else if(baud == 5)
    {
        baud = 0x160023;
    }
    else if(baud == 6)
    {
        baud = 0x1C002C;
    }
    else
    {
        baud = 0x1600B3;
    }

    /**初始化CAN配置**/
    init_config.Mode = static_cast<uchar>(mode);
    /**打开CAN设备**/
    if(VCI_OpenDevice(static_cast<DWORD>(CANTH->m_devtype),static_cast<DWORD>(index),0)!=STATUS_OK)
    {
        logger(3,"打开设备失败!");
        QMessageBox::warning(this,"警告",QStringLiteral("打开设备失败!"));
        return;
    }
    /**设置CAN波特率**/
    if (VCI_SetReference(static_cast<DWORD>(CANTH->m_devtype),static_cast<DWORD>(index), static_cast<DWORD>(cannum), 0, &baud) != STATUS_OK)
    {
        logger(3,"设置波特率错误，打开设备失败!");
        QMessageBox::warning(this,"警告",QStringLiteral("设置波特率错误，打开设备失败!"));
        VCI_CloseDevice(static_cast<DWORD>(CANTH->m_devtype),static_cast<DWORD>(index));
        return;
    }
    /**初始化CAN设备**/
    if(VCI_InitCAN(static_cast<DWORD>(CANTH->m_devtype),static_cast<DWORD>(index), static_cast<DWORD>(cannum), &init_config)!=STATUS_OK)
    {
        logger(3,"初始化CAN失败!");
        QMessageBox::warning(this,"警告",QStringLiteral("初始化CAN失败!"));
        VCI_CloseDevice(static_cast<DWORD>(CANTH->m_devtype),static_cast<DWORD>(index));
        return;
    }
    /**滤波设置目前省略，之后再配置**/

    /**连接成功标志**/
    m_connect = 1;
    logger(1,"CAN设备连接成功！");
    //CAN设备连接成功之后该怎么做
    ui->CANState->setChecked(true);

    ui->CANDevice->setEnabled(false);
    ui->CANIndex->setEnabled(false);
    ui->CANPort->setEnabled(false);
    ui->CANBaud->setEnabled(false);
    /**接收线程启动**/
    CANTH->setFlag(false);
    CANTH->start();
}

//点击启动按钮事件
void Widget::on_CANStart_clicked()
{

    if(m_connect == 0)
        QMessageBox::warning(this,"警告","CAN设备未连接，请先设置CAN连接参数\n连接CAN总线成功后在进行尝试");
        return;
    if(VCI_StartCAN(static_cast<DWORD>(CANTH->m_devtype),CANTH->m_devind,static_cast<DWORD>(CANTH->m_cannum)) == 1)
    {
        logger(1,"CAN设备启动成功！");
          ui->POWERState->setChecked(true);
        /**定时发送启动**/
        if(Timer1->isActive() == false)
            Timer1->start(50);
    }
    else
    {
        logger(3,"CAN设备启动失败！");
        ui->POWERState->setChecked(false);
        QMessageBox::warning(this,"警告","CAN设备启动失败");
    }
}


//提示信息显示最后一行
void Widget::on_textBrowserMessage_textChanged()
{
    ui->textBrowserMessage->moveCursor(QTextCursor::End);
}

QString str;
//定时器线程函数
void Widget::dealTimer1()
{

    if(m_connect == 0)
        return;
    VCI_CAN_OBJ frameinforight;
    frameinforight.ID = 0x0C00D1D0;
    frameinforight.SendType = 0;    //发送格式：0:正常发送 1:单次正常发送 2:自发自收 3.单次自发自收
    frameinforight.RemoteFlag = 0;  //帧格式：0：数据帧 1：远程帧
    frameinforight.ExternFlag = 1;  //帧类型：0：标准帧 1为扩展帧，29位ID
    frameinforight.DataLen = 8;
    frameinforight.Data[0] = 0x00;
    frameinforight.Data[1] = 0x00;
    frameinforight.Data[2] = 0x24;
    frameinforight.Data[3] = 0x92;
    frameinforight.Data[4] = 0xAB;
    frameinforight.Data[5] = 0x41;
    frameinforight.Data[6] = 0x79;
    frameinforight.Data[7] = 0x5F;
    m_sendTimeout = 1000;
    VCI_SetReference(static_cast<DWORD>(CANTH->m_devtype),CANTH->m_devind,static_cast<DWORD>(CANTH->m_cannum),4,&m_sendTimeout);//设置发送超时
    uint ret2 = VCI_Transmit(static_cast<DWORD>(CANTH->m_devtype),CANTH->m_devind,static_cast<DWORD>(CANTH->m_cannum),&frameinforight,1);
    if(ret2==1)
    {
        ui->textBrowserMessage->insertPlainText("授权发送成功！\n");
    }
    else
    {
        ui->textBrowserMessage->insertPlainText("授权发送失败！\n");
    }

    VCI_CAN_OBJ frameinfo;
    frameinfo.ID = 0x0C08D1D0;
    frameinfo.SendType = 0;    //发送格式：0:正常发送 1:单次正常发送 2:自发自收 3.单次自发自收
    frameinfo.RemoteFlag = 0;  //帧格式：0：数据帧 1：远程帧
    frameinfo.ExternFlag = 1;  //帧类型：0：标准帧 1为扩展帧，29位ID
    frameinfo.DataLen = 8;
    //车辆行驶模式
    if(ui->radioButtonAA->isChecked()) Run_Mode = 3;
    if(ui->radioButtonPA->isChecked()) Run_Mode = 1;
    if(ui->radioButtonM->isChecked()) Run_Mode = 0;
    Data08.TxData.runMode = Run_Mode;
    //模块使能
    Data08.TxData.steerEnable = Steer_EN;
    Data08.TxData.brakeEnable = Brake_EN;
    Data08.TxData.gearEnable  = Gear_EN;
    Data08.TxData.gasEnable   = Gas_EN;
    Data08.TxData.parkEnable  = Park_EN;
    Data08.TxData.lightEnable = Light_EN;
    //挡位以及驻车
    Data08.TxData.targetGear = m_gear;
    Data08.TxData.targetPark = m_park;
    //油门数据
    Data08.TxData.targetGasValue = m_gas;
    //转向数据
    m_steer = ui->horizontalSlider->value()*10;
    if(m_steer<0) m_steer = ~(-m_steer)+1;
    Data08.TxData.target_steer_angle_value_H = m_steer>>8&0xff;
    Data08.TxData.target_steer_angle_value_L = m_steer&0xff;
    //转速设置
    m_gradiant = ui->spinBoxTS->value()&0xff;
    Data08.TxData.target_steer_angle_gradiant = m_gradiant;
    // 制动数据
    Data08.TxData.targetBrakePressure = m_brake/5;
    // 循环计数
    rolling_counter++;
    if(rolling_counter>15)
        rolling_counter = 0;
    Data08.TxData.rolling_counter = rolling_counter;
    Data08.TxData.res1 = 0;

    for(uint i=0 ;i<8;i++)
    {
       frameinfo.Data[i] = Data08.UCData[i];
       str += QString::number(frameinfo.Data[i],16)+" ";
    }
//    qDebug() << hex << frameinfo.ID;
//    qDebug() << "数据" << str;
    str = nullptr;

    m_sendTimeout = 1000;
    VCI_SetReference(static_cast<DWORD>(CANTH->m_devtype),CANTH->m_devind,static_cast<DWORD>(CANTH->m_cannum),4,&m_sendTimeout);//设置发送超时
    uint ret = VCI_Transmit(static_cast<DWORD>(CANTH->m_devtype),CANTH->m_devind,static_cast<DWORD>(CANTH->m_cannum),&frameinfo,1);
    if(ret==1){
        logger(0,"CAN总线报文通信正常");
    }else{
        logger(0,"CAN总线报文通信错误！");
    }

    VCI_CAN_OBJ frameinfolight;
    frameinfolight.ID = 0x0C0AD1D0;
    frameinfolight.SendType = 0;    //发送格式：0:正常发送 1:单次正常发送 2:自发自收 3.单次自发自收
    frameinfolight.RemoteFlag = 0;  //帧格式：0：数据帧 1：远程帧
    frameinfolight.ExternFlag = 1;  //帧类型：0：标准帧 1为扩展帧，29位ID
    frameinfolight.DataLen = 8;
    Data0C.TxData.turnLightLeft = m_turnlightleft;
    Data0C.TxData.turnLightRight = m_turnlightright;
    Data0C.TxData.farHeadLight = m_farheadlight;
    Data0C.TxData.nearHeadLight = m_nearheadlight;
    Data0C.TxData.flashLight = m_flashlight;
    Data0C.TxData.backLight = m_backlight;
    Data0C.TxData.brakeLight = m_brakelight;
    Data0C.TxData.trumpet = m_trumpet;
    Data0C.TxData.wiper = m_wiper;
    Data0C.TxData.res1 = 0x00;
    Data0C.TxData.res2 = 0x00;
    Data0C.TxData.res3 = 0x00;
    Data0C.TxData.res4 = 0x00;
    Data0C.TxData.res5 = 0x00;
    Data0C.TxData.res6 = 0x00;
    for(uint i=0 ;i<8;i++)
    {
       frameinfolight.Data[i] = Data0C.UCData[i];

    }
    m_sendTimeout = 1000;

    VCI_SetReference(static_cast<DWORD>(CANTH->m_devtype),CANTH->m_devind,static_cast<DWORD>(CANTH->m_cannum),4,&m_sendTimeout);//设置发送超时
    uint ret1 = VCI_Transmit(static_cast<DWORD>(CANTH->m_devtype),CANTH->m_devind,static_cast<DWORD>(CANTH->m_cannum),&frameinfolight,1);
//    if(ret1==1){
//    }else{
//        ui->textBrowserMessage->insertPlainText("灯光发送失败！\n");
//    }


}

//接收数据处理函数 这里是车辆回传过来的实时信息
void Widget::dealReceiveData(PVCI_CAN_OBJ objs, uint length)
{
    for(uint i=0;i<length;i++)
    {
        uint CAN_ID = objs[i].ID;
        if(CAN_ID == 0x1814D0D1)
        {
            if(objs[i].RemoteFlag==0)
            {
                if(objs[i].DataLen>8)
                    objs[i].DataLen=8;
                for(int j=0;j<objs[i].DataLen;j++)
                {
                    Data1814.UCData[j] = objs[i].Data[j];
                }
            }
        }
        if(CAN_ID == 0x18F014D1)
        {
            if(objs[i].RemoteFlag==0)
            {
                if(objs[i].DataLen>8)
                    objs[i].DataLen=8;
                for(int j=0;j<objs[i].DataLen;j++)
                {
                    Data18F0.UCData[j] = objs[i].Data[j];
                }
                //行驶状态解析
                if(Data18F0.RxData.runModeState == 0x00)
                {
                    model->item(0,0)->child(1,1)->setText("断电模式");
                }else if(Data18F0.RxData.runModeState == 0x10)
                {
                    model->item(0,0)->child(1,1)->setText("待机模式");
                }else if(Data18F0.RxData.runModeState == 0x20)
                {
                    model->item(0,0)->child(1,1)->setText("工程模式");
                }else if(Data18F0.RxData.runModeState == 0x30)
                {
                    model->item(0,0)->child(1,1)->setText("手动驾驶模式");
                }else if(Data18F0.RxData.runModeState == 0x40)
                {
                    model->item(0,0)->child(1,1)->setText("自动驾驶模式");
                }else if(Data18F0.RxData.runModeState == 0x50)
                {
                    model->item(0,0)->child(1,1)->setText("紧急停车模式");
                }
                //工作模式解析 使车辆实时信息和treeview一致
                if(Data18F0.RxData.steerState == 1)
                {
                    model->item(1,0)->setCheckState(Qt::Checked);
                }else{
                    model->item(1,0)->setCheckState(Qt::Unchecked);
                }
                if(Data18F0.RxData.gearState == 1)
                {
                    model->item(2,0)->setCheckState(Qt::Checked);
                }else{
                    model->item(2,0)->setCheckState(Qt::Unchecked);
                }
                if(Data18F0.RxData.gasState == 1)
                {
                    model->item(3,0)->setCheckState(Qt::Checked);
                }else{
                    model->item(3,0)->setCheckState(Qt::Unchecked);
                }
                if(Data18F0.RxData.brakeState == 1)
                {
                    model->item(4,0)->setCheckState(Qt::Checked);
                }else{
                    model->item(4,0)->setCheckState(Qt::Unchecked);
                }
                if(Data18F0.RxData.parkState == 1)
                {
                    model->item(5,0)->setCheckState(Qt::Checked);
                }else{
                    model->item(5,0)->setCheckState(Qt::Unchecked);
                }
                if(Data18F0.RxData.lightState == 1)
                {
                    model->item(6,0)->setCheckState(Qt::Checked);
                }else{
                    model->item(6,0)->setCheckState(Qt::Checked);
                }
                //车辆授权信息
                if(Data18F0.RxData.licenseState == 0x01)
                {
//                    logger(2,"车辆已授权！");
                    ui->CARState->setChecked(true);
                }else{
//                    logger(2,"正在获取车辆授权");
                    VCI_CAN_OBJ frameinfo;
                    frameinfo.ID = 0x0C00D1D0;
                    frameinfo.SendType = 0;    //发送格式：0:正常发送 1:单次正常发送 2:自发自收 3.单次自发自收
                    frameinfo.RemoteFlag = 0;  //帧格式：0：数据帧 1：远程帧
                    frameinfo.ExternFlag = 1;  //帧类型：0：标准帧 1为扩展帧，29位ID
                    frameinfo.DataLen = 8;
                    frameinfo.Data[0] = 0x00;
                    frameinfo.Data[1] = 0x00;
                    frameinfo.Data[2] = 0x24;
                    frameinfo.Data[3] = 0x92;
                    frameinfo.Data[4] = 0xAB;
                    frameinfo.Data[5] = 0x41;
                    frameinfo.Data[6] = 0x79;
                    frameinfo.Data[7] = 0x5F;

                    for(uint i=0 ;i<8;i++)
                    {
                       frameinfo.Data[i] = Data08.UCData[i];
                       str += QString::number(frameinfo.Data[i],16)+" ";
                    }
//                    qDebug() << hex << frameinfo.ID;
//                    qDebug() << "数据" << str;
                    str = nullptr;
                    m_sendTimeout = 1000;
                    VCI_SetReference(static_cast<DWORD>(CANTH->m_devtype),CANTH->m_devind,static_cast<DWORD>(CANTH->m_cannum),4,&m_sendTimeout);//设置发送超时
                    uint ret = VCI_Transmit(static_cast<DWORD>(CANTH->m_devtype),CANTH->m_devind,static_cast<DWORD>(CANTH->m_cannum),&frameinfo,1);
                    if(ret==1)
                    {
//                        logger(2,"授权发送成功，车辆已授权");
                        ui->CARState->setChecked(true);
                        ui->labelKey->setStyleSheet("border-image: url(:/icons/resources/icons/key_on.png);");
                    }
                    else
                    {
//                        logger(2,"授权发送失败，请检查车辆状态");
                        ui->CARState->setChecked(false);
                        ui->labelKey->setStyleSheet("border-image: url(:/icons/resources/icons/key_off.png);");
                    }
                }
            }
        }
        if(CAN_ID == 0x18F015D1)
        {
            if(objs[i].RemoteFlag==0)
            {
                if(objs[i].DataLen>8)
                    objs[i].DataLen=8;
                for(int j=0;j<objs[i].DataLen;j++)
                {
                    Data15D1.UCData[j] = objs[i].Data[j];
                }
                //实际挡位
                if(Data15D1.RxData.realGearState == 0)
                {
                    ui->labelGear->setText("车辆挡位：N");
                    model->item(2,0)->child(0,1)->setText("N 档");
                }else if(Data15D1.RxData.realGearState == 1)
                {
                    ui->labelGear->setText("车辆挡位：D");
                    model->item(2,0)->child(0,1)->setText("D 档");
                }else if(Data15D1.RxData.realGearState == 2)
                {
                    ui->labelGear->setText("车辆挡位：R");
                    model->item(2,0)->child(0,1)->setText("R 档");
                }else{
                    ui->labelGear->setText("车辆挡位：P");
                    model->item(2,0)->child(0,1)->setText("P 档");
                }

                int realAngle;
                realAngle = (Data15D1.RxData.RealSteerAngle_H*256+Data15D1.RxData.RealSteerAngle_L)/10;
                if(((Data15D1.RxData.RealSteerAngle_H<<1) & 0x01) == 1)
                {
                    realAngle = 0xffff-realAngle;
                }
                //实际角度 实时转角：0 deg
                ui->labelRTS->setText("实时转角："+QString("%1").arg(realAngle)+" deg");
                model->item(1,0)->child(1,1)->setText(QString("%1").arg(realAngle) + " deg");
                //实际油门 实时油门：0 %
                ui->labelRYS->setText("实时油门："+QString("%1").arg(Data15D1.RxData.realGasState)+" %");
                model->item(3,0)->child(1,1)->setText(QString("%1").arg(Data15D1.RxData.realGasState) + " %");
            }
        }
        //实际制动
        if(CAN_ID == 0x18F016D1)
        {
            if(objs[i].RemoteFlag==0)
            {
                if(objs[i].DataLen>8)
                    objs[i].DataLen=8;
                for(int j=0;j<objs[i].DataLen;j++)
                {
                    Data16D1.UCData[j] = objs[i].Data[j];
                }
                //实际制动 实时制动：0 bar
                ui->labelRBS->setText("实时制动："+QString("%1").arg(Data16D1.RxData.realBrakeState) + " bar");
                model->item(4,0)->child(1,1)->setText(QString("%1").arg(Data16D1.RxData.realBrakeState) + " bar");
            }
        }
          //剩余电量
        if(CAN_ID == 0x18F020D1)
        {
            if(objs[i].RemoteFlag==0)
            {
                if(objs[i].DataLen>8)
                    objs[i].DataLen=8;
                for(int j=0;j<objs[i].DataLen;j++)
                {
                    Data20D1.UCData[j] = objs[i].Data[j];
                }
                //剩余电量 电池电量：100 %
                ui->labelBTS->setText("电池电量：" + QString("%1").arg(Data20D1.RxData.batteryValue)+" %");
                model->item(0,0)->child(3,1)->setText(QString("%1").arg(Data20D1.RxData.batteryValue) + " %");

            }
        }
        //车辆实时速度
        if(CAN_ID == 0x18F01AD1)
        {
            if(objs[i].RemoteFlag==0)
            {
                if(objs[i].DataLen>8)
                    objs[i].DataLen=8;
                for(int j=0;j<objs[i].DataLen;j++)
                {
                    Data1AD1.UCData[j] = objs[i].Data[j];
                }
                int carSpeed;
                carSpeed = Data1AD1.RxData.carSpeed_H*256+Data1AD1.RxData.carSpeed_L/10;
                if(((Data1AD1.RxData.carSpeed_H<<1) & 0x01) == 1)
                {
                    carSpeed = 0xffff-carSpeed;
                }
                //车辆实时速度 车辆速度：0 Km/h
                ui->labelSPS->setText("车辆速度：" + QString("%1").arg(carSpeed) + " Km/h");
                model->item(0,0)->child(2,1)->setText(QString("%1").arg(carSpeed) + " Km/h");
            }
        }


    }
}

//获取车辆控制权触发函数
void Widget::on_pushButtonCarLicense_clicked()
{
    if(m_connect == 0)
        QMessageBox::warning(this,"警告","CAN设备未获得车辆授权，请设置正确的CAN连接参数\n或检查车辆状态后进行尝试");
        return;
     VCI_CAN_OBJ frameinfo;
     frameinfo.ID = 0x0C00D1D0;
     frameinfo.SendType = 0;    //发送格式：0:正常发送 1:单次正常发送 2:自发自收 3.单次自发自收
     frameinfo.RemoteFlag = 0;  //帧格式：0：数据帧 1：远程帧
     frameinfo.ExternFlag = 1;  //帧类型：0：标准帧 1为扩展帧，29位ID
     frameinfo.DataLen = 8;
     frameinfo.Data[0] = 0x00;
     frameinfo.Data[1] = 0x00;
     frameinfo.Data[2] = 0xBF;
     frameinfo.Data[3] = 0x95;
     frameinfo.Data[4] = 0xE6;
     frameinfo.Data[5] = 0xCB;
     frameinfo.Data[6] = 0x49;
     frameinfo.Data[7] = 0x47;

     for(uint i=0 ;i<8;i++)
     {
        frameinfo.Data[i] = Data08.UCData[i];
        str += QString::number(frameinfo.Data[i],16)+" ";
     }
//     qDebug() << hex << frameinfo.ID;
//     qDebug() << "数据" << str;
     str = nullptr;

     m_sendTimeout = 1000;
     VCI_SetReference(static_cast<DWORD>(CANTH->m_devtype),CANTH->m_devind,static_cast<DWORD>(CANTH->m_cannum),4,&m_sendTimeout);//设置发送超时
     uint ret = VCI_Transmit(static_cast<DWORD>(CANTH->m_devtype),CANTH->m_devind,static_cast<DWORD>(CANTH->m_cannum),&frameinfo,1);
     if(ret==1)
     {
         ui->CARState->setChecked(true);
         logger(1,"授权发送成功，车辆已授权");
         ui->textBrowserMessage->insertPlainText("授权发送成功！\n");
         ui->labelKey->setStyleSheet("border-image: url(:/icons/resources/icons/key_on.png);");
     }
     else
     {
         ui->CARState->setChecked(false);
         logger(3,"授权发送失败，请检查车辆状态");
         ui->textBrowserMessage->insertPlainText("授权发送失败！\n");
         ui->labelKey->setStyleSheet("border-image: url(:/icons/resources/icons/key_off.png);");
     }
}

//转向使能按键触发函数
void Widget::on_checkBoxTEN_stateChanged(int arg1)
{
    if(arg1 ==Qt::Checked)
    {
        logger(0,"转向使能开启");
        model->item(1,0)->setCheckState(Qt::Checked);
        ui->horizontalSlider->setEnabled(true);
        ui->pushButtonTZ->setEnabled(true);
        ui->pushButtonRI->setEnabled(true);
        ui->pushButtonLF->setEnabled(true);
        Steer_EN = 1;
    }
    else
    {
        logger(0,"转向使能关闭");
        model->item(1,0)->setCheckState(Qt::Unchecked);
        ui->horizontalSlider->setEnabled(false);
        ui->pushButtonTZ->setEnabled(false);
        ui->pushButtonRI->setEnabled(false);
        ui->pushButtonLF->setEnabled(false);
        Steer_EN = 0;
    }
}
//制动使能按键触发函数
void Widget::on_checkBoxBEN_stateChanged(int arg1)
{
    if(arg1 ==Qt::Checked)
    {
        logger(0,"制动使能开启");
        model->item(4,0)->setCheckState(Qt::Checked);
        ui->pushButtonBEN->setEnabled(true);
        Brake_EN = 1;
    }
    else
    {
        logger(0,"制动使能关闭");
        model->item(4,0)->setCheckState(Qt::Unchecked);
        ui->pushButtonBEN->setEnabled(false);
        Brake_EN = 0;
    }
}
//挡位使能按键触发函数
void Widget::on_checkBoxGEN_stateChanged(int arg1)
{
    if(arg1 ==Qt::Checked)
    {
        logger(0,"挡位使能开启");
        model->item(2,0)->setCheckState(Qt::Checked);
        ui->verticalSlider->setEnabled(true);
        Gear_EN = 1;
    }
    else
    {
        logger(0,"挡位使能关闭");
        model->item(2,0)->setCheckState(Qt::Unchecked);
        ui->verticalSlider->setEnabled(false);
        Gear_EN = 0;
    }
}
//油门使能按键触发函数
void Widget::on_checkBoxYEN_stateChanged(int arg1)
{
    if(arg1 ==Qt::Checked)
    {
        logger(0,"油门使能开启");
        model->item(3,0)->setCheckState(Qt::Checked);
        ui->pushButtonYEN->setEnabled(true);
        ui->spinBoxTS->setEnabled(true);
        ui->spinBoxST->setEnabled(true);
        Gas_EN = 1;
    }
    else
    {
        logger(0,"油门使能关闭");
        model->item(3,0)->setCheckState(Qt::Unchecked);
        ui->pushButtonYEN->setEnabled(false);
        ui->spinBoxTS->setEnabled(false);
        ui->spinBoxST->setEnabled(false);
        Gas_EN = 0;
    }
}
//驻车使能按键触发函数
void Widget::on_checkBoxPEN_stateChanged(int arg1)
{
    if(arg1 ==Qt::Checked)
    {
        logger(0,"驻车使能开启");
        model->item(5,0)->setCheckState(Qt::Checked);
        Park_EN = 1;
    }
    else
    {
        logger(0,"驻车使能关闭");
        model->item(5,0)->setCheckState(Qt::Unchecked);
        Park_EN = 0;
    }
}
//灯光使能按键触发函数
void Widget::on_checkBoxLEN_stateChanged(int arg1)
{
    if(arg1 ==Qt::Checked)
    {
        logger(0,"灯光使能开启");
        model->item(6,0)->setCheckState(Qt::Checked);
        ui->pushButtonRemote->setEnabled(true);
        ui->pushButtonNear->setEnabled(true);
        ui->pushButtonDF->setEnabled(true);
        ui->pushButtonLR->setEnabled(true);
        ui->pushButtonLL->setEnabled(true);
        ui->pushButtonWP->setEnabled(true);
        ui->pushButtonBack->setEnabled(true);
        Light_EN = 1;
    }
    else
    {
        logger(0,"灯光使能关闭");
        model->item(6,0)->setCheckState(Qt::Unchecked);
        ui->pushButtonRemote->setEnabled(false);
        ui->pushButtonNear->setEnabled(false);
        ui->pushButtonDF->setEnabled(false);
        ui->pushButtonLR->setEnabled(false);
        ui->pushButtonLL->setEnabled(false);
        ui->pushButtonWP->setEnabled(false);
        ui->pushButtonBack->setEnabled(false);
        Light_EN = 0;
    }
}
//挡位选择按键触发函数
void Widget::on_verticalSlider_sliderReleased()
{
    int value = ui->verticalSlider->sliderPosition();
    switch (value) {
        case 1:     //P档位
            logger(1,"切换为P档位");
            ui->labelGear->setText("车辆挡位：P");
            m_park = 1;
            break;

        case 2:     //R档位
            logger(1,"切换为R档位");
            ui->labelGear->setText("车辆挡位：R");
            m_park = 0;
            m_gear = 2;
            break;

        case 3:     //N档位
            logger(1,"切换为N档位");
            ui->labelGear->setText("车辆挡位：N");
            m_park = 0;
            m_gear = 0;
            break;

        case 4:     //D档位
            logger(1,"切换为D档位");
            ui->labelGear->setText("车辆挡位：D");
            m_park = 0;
            m_gear = 1;
            break;

        default:
            logger(2,"切换为未知档位");
            ui->labelGear->setText("车辆挡位：UN");
            break;

    }
}

//转向控制相关
void Widget::on_horizontalSlider_valueChanged(int value)
{
    //转向控制的m参数在线程里读取。

    model->item(1,0)->child(0,1)->setText(QString("%1").arg(value) + " deg");
}

void Widget::on_pushButtonTZ_clicked()
{
    ui->horizontalSlider->setValue(0);
}
//左右转键盘逻辑
void Widget::on_pushButtonLF_pressed()
{
    ui->horizontalSlider->setValue(ui->horizontalSlider->value()+10);
}

void Widget::on_pushButtonRI_pressed()
{
    ui->horizontalSlider->setValue(ui->horizontalSlider->value()-10);
}
//油门控制
void Widget::on_pushButtonYEN_clicked()
{
    if(m_brake>0){
       m_brake -= ui->spinBoxST->value();
       model->item(3,0)->child(0,1)->setText(QString("%1").arg(m_gas) + " %");
       model->item(4,0)->child(0,1)->setText(QString("%1").arg(m_brake) + " bar");
    }else if(m_brake<=0){//如果制动等于0 如果油门设置小于100，则将转速设置累加到油门上 上限为100
       m_brake = 0;
       if(m_gas<100)
            m_gas += ui->spinBoxST->value();
       else {
            m_gas = 100;
       }

       model->item(3,0)->child(0,1)->setText(QString("%1").arg(m_gas) + " %");
       model->item(4,0)->child(0,1)->setText(QString("%1").arg(m_brake) + " bar");
    }
}
//刹车控制 制动控制
void Widget::on_pushButtonBEN_clicked()
{
    //如果油门大于0则给油门设为0
    if(m_gas>0)
    {
       m_gas = 0;
       logger(0,QString("%1").arg(m_gas));
       model->item(3,0)->child(0,1)->setText(QString("%1").arg(m_gas) + " %");
       model->item(4,0)->child(0,1)->setText(QString("%1").arg(m_brake) + " bar");
    }else if(m_gas<=0){//油门若等于0 如果制动小于75 则累加制动步长到制动上 上限为75
       if(m_brake<75)
       {
            m_brake += ui->spinBoxST->value();
       }else{
            m_brake = 75;
       }
       model->item(3,0)->child(0,1)->setText(QString("%1").arg(m_gas) + " %");
       model->item(4,0)->child(0,1)->setText(QString("%1").arg(m_brake) + " bar");
    }
}
//喇叭控制
void Widget::on_pushButtonSound_pressed()
{
    logger(0,"喇叭开启");
    ui->labelUnvoise->setStyleSheet("border-image: url(:/icons/resources/icons/voise_on.png);");
    m_trumpet = 1;
}
void Widget::on_pushButtonSound_released()
{
    logger(0,"喇叭关闭");
    ui->labelUnvoise->setStyleSheet("border-image: url(:/icons/resources/icons/voise_off.png);");
    m_trumpet = 0;
}
//雨刷控制
void Widget::on_pushButtonWP_clicked()
{
    if( m_wiper == 0)
    {
        logger(0,"雨刷器打开");
        ui->labeRraindeng->setStyleSheet("border-image: url(:/icons/resources/icons/wiper_on.png);");
        m_wiper = 1;
    }else{
        logger(0,"雨刷器关闭");
        ui->labeRraindeng->setStyleSheet("border-image: url(:/icons/resources/icons/wiper_off.png);");
        m_wiper = 0;
    }
}
//灯光控制
//左转向灯监听
void Widget::on_pushButtonLL_clicked()
{
    if( m_turnlightleft == 0)
    {
        logger(0,"左转灯打开");
        ui->label_left->setStyleSheet("border-image: url(:/icons/resources/icons/left_on.png);");
        m_turnlightleft = 1;
    }else{
        logger(0,"左转灯关闭");
        ui->label_left->setStyleSheet("border-image: url(:/icons/resources/icons/left_off.png);");
        m_turnlightleft = 0;
    }
}
//右转向灯监听
void Widget::on_pushButtonLR_clicked()
{
    if( m_turnlightright == 0)
    {
        logger(0,"右转灯打开");
        ui->label_right->setStyleSheet("border-image: url(:/icons/resources/icons/right_on.png);");
        m_turnlightright = 1;
    }else{
        logger(0,"右转灯关闭");
        ui->label_right->setStyleSheet("border-image: url(:/icons/resources/icons/right_off.png);");
        m_turnlightright = 0;
    }
}
//双闪灯控制
void Widget::on_pushButtonDF_clicked()
{
    if( m_flashlight == 0)
    {
        logger(0,"双闪灯打开");
        ui->labelDanger->setStyleSheet("border-image: url(:/icons/resources/icons/doubleflash_on.png);");
        m_flashlight = 1;
    }else{
        logger(0,"双闪灯关闭");
        ui->labelDanger->setStyleSheet("border-image: url(:/icons/resources/icons/doubleflash_off.png);");
        m_flashlight = 0;
        m_turnlightright = 0;
        m_turnlightleft = 0;


    }
}
//近光灯控制
void Widget::on_pushButtonNear_clicked()
{
    if( m_nearheadlight == 0)
    {
        logger(0,"近光灯打开");
        ui->labelJindeng->setStyleSheet("border-image: url(:/icons/resources/icons/jindeng_on.png);");
        m_nearheadlight = 1;
    }else{
        logger(0,"近光灯关闭");
        ui->labelJindeng->setStyleSheet("border-image: url(:/icons/resources/icons/jindeng_off.png);");
        m_nearheadlight = 0;
    }
}
//远光灯控制
void Widget::on_pushButtonRemote_clicked()
{
    if( m_farheadlight == 0)
    {
        logger(0,"远光灯打开");
        ui->labelYuandeng->setStyleSheet("border-image: url(:/icons/resources/icons/yuandeng_on.png);");
        m_farheadlight = 1;
    }else{
        logger(0,"远光灯关闭");
        ui->labelYuandeng->setStyleSheet("border-image: url(:/icons/resources/icons/yuandeng_off.png);");
        m_farheadlight = 0;
    }
}







void Widget::on_checkBoxDEBUG_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked)
    {
        DEBUG = true;
        logger(0,"DEBUG模式已开启！");
        ui->labelKey->setStyleSheet("border-image: url(:/icons/resources/icons/key_on.png);");
                ui->labelPark->setStyleSheet("border-image: url(:/icons/resources/icons/park_on.png);");
//        ui->checkBoxBEN->setChecked(true);
//        ui->checkBoxLEN->setChecked(true);
//        ui->checkBoxGEN->setChecked(true);
//        ui->checkBoxYEN->setChecked(true);
//        ui->checkBoxPEN->setChecked(true);
//        ui->checkBoxTEN->setChecked(true);
//        ui->CANState->setChecked(true);
//        ui->POWERState->setChecked(true);
//        ui->CARState->setChecked(true);
    }else if(arg1 == Qt::Unchecked)
    {
        logger(0,"DEBUG模式已关闭！");
        DEBUG = false;
        ui->labelKey->setStyleSheet("border-image: url(:/icons/resources/icons/key_off.png);");
                ui->labelPark->setStyleSheet("border-image: url(:/icons/resources/icons/park_off.png);");
//        ui->checkBoxBEN->setChecked(false);
//        ui->checkBoxLEN->setChecked(false);
//        ui->checkBoxGEN->setChecked(false);
//        ui->checkBoxYEN->setChecked(false);
//        ui->checkBoxPEN->setChecked(false);
//        ui->checkBoxTEN->setChecked(false);
//        ui->CANState->setChecked(false);
//        ui->POWERState->setChecked(false);
//        ui->CARState->setChecked(false);
    }
}

void Widget::on_pushButtonBack_clicked()
{
    if( m_backlight == 0)
    {
        logger(0,"倒车灯打开");
        ui->labelPark->setStyleSheet("border-image: url(:/icons/resources/icons/park_on.png);");
        m_backlight = 1;
    }else{
        logger(0,"倒车灯关闭");
        ui->labelPark->setStyleSheet("border-image: url(:/icons/resources/icons/park_off.png);");
        m_backlight = 0;
    }
}
