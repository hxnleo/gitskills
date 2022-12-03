#include "qwidgetshowparams_zm662daconfig.h"
#include "ui_qwidgetshowparams_zm662daconfig.h"
#include "qwidgetshowparams_daconfig.h"
#include "data/uistyles.h"
#include "data/utils.h"
#include "data/definitions.h"
#include "modbus/modbusaddress.h"
#include "modbus/modbusqueue.h"
#include "data/globaldata.h"
#include "Teach.h"
#include <QDebug>
#include <QTimer>
QWidgetShowParams_ZM662DAConfig::QWidgetShowParams_ZM662DAConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QWidgetShowParams_ZM662DAConfig)
{
    ui->setupUi(this);
    ui->label_voltage1->setText(tr("DA0输出电压"));
    ui->label_voltage2->setText(tr("DA1输出电压"));
    ui->label_speed1->setText(tr("DA0输出转速"));
    ui->label_speed2->setText(tr("DA1输出转速"));
    ui->label_mainspeed->setText(tr("主轴实际转速"));
    ui->btn_voltage1->setText(tr("输出测试"));
    ui->btn_voltage2->setText(tr("输出测试"));
    ui->btn_speed1->setText(tr("输出测试"));
    ui->btn_speed2->setText(tr("输出测试"));
    ui->btn_speedtest->setText(tr("速度检测"));
    ui->btn_wiretest->setText(tr("送丝检测"));
    ui->groupBox->setTitle(tr("DA测试(0~10V)"));
    ui->groupBox_2->setTitle(tr("DA测试(转 速)"));
    ui->groupBox_3->setTitle(tr(""));
    ui->label_mainspeedvalue->setText(tr("0"));
    ui->line_voltage1->setText("0");
    ui->line_voltage2->setText("0");
    ui->line_speed1->setText("0");
    ui->line_speed2->setText("0");

    ui->line_voltage1->setStyleSheet(UIStyles::css_maincolorconf_u12);
    ui->line_voltage2->setStyleSheet(UIStyles::css_maincolorconf_u12);
    ui->line_speed1->setStyleSheet(UIStyles::css_maincolorconf_u12);
    ui->line_speed2->setStyleSheet(UIStyles::css_maincolorconf_u12);

    ui->btn_voltage1->setStyleSheet(UIStyles::css_maincolorconf_u8);
    ui->btn_voltage2->setStyleSheet(UIStyles::css_maincolorconf_u8);
    ui->btn_speed1->setStyleSheet(UIStyles::css_maincolorconf_u8);
    ui->btn_speed2->setStyleSheet(UIStyles::css_maincolorconf_u8);
    ui->btn_speedtest->setStyleSheet(UIStyles::css_maincolorconf_u8);
    ui->btn_wiretest->setStyleSheet(UIStyles::css_maincolorconf_u8);

    connect(ui->btn_voltage1, SIGNAL(clicked()),
            this, SLOT(onBtn_Voltage1()));
    connect(ui->btn_voltage2, SIGNAL(clicked()),
            this, SLOT(onBtn_Voltage2()));
    connect(ui->btn_speed1, SIGNAL(clicked()),
            this, SLOT(onBtn_Speed1()));
    connect(ui->btn_speed2, SIGNAL(clicked()),
            this, SLOT(onBtn_Speed2()));
    connect(ui->btn_speedtest, SIGNAL(clicked()),
            this, SLOT(onBtn_SpeedTest()));
    connect(ui->btn_wiretest, SIGNAL(clicked()),
            this, SLOT(onBtn_WireTest()));


    QString temppath=":/images/images/TEST_SX.png";
    QImage image,result;
    image.load(temppath);//temppath为图片的路径
    image.rgbSwapped();
    result = image.scaled(ui->label_pixp->width(), ui->label_pixp->width(),Qt::IgnoreAspectRatio, Qt::SmoothTransformation);//放缩图片，以固定大小显示
    ui->label_pixp->setPixmap(QPixmap::fromImage(result));//在Label控件上显示图片

    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),
            this, SLOT(onTimeOut()));
    timer->setInterval(50); // 更新间隔
    timer->start();
}
QWidgetShowParams_ZM662DAConfig::~QWidgetShowParams_ZM662DAConfig()
{
    delete ui;
}

void QWidgetShowParams_ZM662DAConfig::onBtn_Voltage1()
{
    ModbusQueue::element_t element;
    auto teachsend = new TEACH_SEND;//数据结构体
    memset(teachsend, 0, sizeof(TEACH_SEND));
    teachsend->Type = DA_OUT_VOLTAGE_0;//数据种类
    teachsend->SendData.DA_Test.type = DATypeData::One;//type种类
    teachsend->SendData.DA_Test.fVolt = ui->line_voltage1->text().toFloat();//得到数据 fVolt 修改为下面的那个参数
    //qDebug()<<teachsend->Type<<getData;
    element.data = teachsend;
    element.len = sizeof(TEACH_SEND);
    element.operation = ModbusQueue::OP_WRITE_DATA;
    element.startAddress = ModbusAddress::TeachWorkfileHead;
    GlobalData::modbusQueue->sendQuery(&element,false);

}

void QWidgetShowParams_ZM662DAConfig::onBtn_Voltage2()
{
    ModbusQueue::element_t element;
    auto teachsend = new TEACH_SEND;//数据结构体
    memset(teachsend, 0, sizeof(TEACH_SEND));
    teachsend->Type = DA_OUT_VOLTAGE_1;//数据种类
    teachsend->SendData.DA_Test.type = DATypeData::One;//type种类
    teachsend->SendData.DA_Test.fVolt = ui->line_voltage2->text().toFloat();//得到数据 fVolt 修改为下面的那个参数
    qDebug()<<teachsend->Type<<ui->line_voltage2->text().toFloat();
    element.data = teachsend;
    element.len = sizeof(TEACH_SEND);
    element.operation = ModbusQueue::OP_WRITE_DATA;
    element.startAddress = ModbusAddress::TeachWorkfileHead;
    GlobalData::modbusQueue->sendQuery(&element,false);
}

void QWidgetShowParams_ZM662DAConfig::onBtn_Speed1()
{
    ModbusQueue::element_t element;
    auto teachsend = new TEACH_SEND;//数据结构体
    memset(teachsend, 0, sizeof(TEACH_SEND));
    teachsend->Type = DA_SPEED_VOLTAGE_0;//数据种类
    teachsend->SendData.DA_Test.iSpeed = ui->line_speed1->text().toFloat();//得到数据 fVolt 修改为下面的那个参数
    //qDebug()<<teachsend->Type<<getData;
    element.data = teachsend;
    element.len = sizeof(TEACH_SEND);
    element.operation = ModbusQueue::OP_WRITE_DATA;
    element.startAddress = ModbusAddress::TeachWorkfileHead;
    GlobalData::modbusQueue->sendQuery(&element,false);
}

void QWidgetShowParams_ZM662DAConfig::onBtn_Speed2()
{
    ModbusQueue::element_t element;
    auto teachsend = new TEACH_SEND;//数据结构体
    memset(teachsend, 0, sizeof(TEACH_SEND));
    teachsend->Type = DA_SPEED_VOLTAGE_1;//数据种类
    teachsend->SendData.DA_Test.iSpeed = ui->line_speed2->text().toFloat();//得到数据 fVolt 修改为下面的那个参数
    //qDebug()<<teachsend->Type<<getData;
    element.data = teachsend;
    element.len = sizeof(TEACH_SEND);
    element.operation = ModbusQueue::OP_WRITE_DATA;
    element.startAddress = ModbusAddress::TeachWorkfileHead;
    GlobalData::modbusQueue->sendQuery(&element,false);
}

void QWidgetShowParams_ZM662DAConfig::onBtn_SpeedTest()
{
    ModbusQueue::element_t* element = new ModbusQueue::element_t;
    auto mixData = new Definitions::MixDataType;
    mixData->u16 = FN_MAIN_SPEED;
    element->data = &mixData->u16;
    element->len = sizeof(mixData->u16);
    element->operation = ModbusQueue::OP_WRITE_DATA;
    element->startAddress = ModbusAddress::ButtonOperate;
    GlobalData::modbusQueue->sendQuery(element,false);
}

void QWidgetShowParams_ZM662DAConfig::onBtn_WireTest()
{
    ModbusQueue::element_t* element = new ModbusQueue::element_t;
    auto mixData = new Definitions::MixDataType;
    mixData->u16 = FN_SEND_IRON;
    element->data = &mixData->u16;
    element->len = sizeof(mixData->u16);
    element->operation = ModbusQueue::OP_WRITE_DATA;
    element->startAddress = ModbusAddress::ButtonOperate;
    GlobalData::modbusQueue->sendQuery(element,false);
}

void QWidgetShowParams_ZM662DAConfig::onTimeOut()
{
    ui->label_mainspeedvalue->setText(Utils::numberToString(GlobalData::realTimeInfo.iSpd));
    ui->btn_wiretest->setStyleSheet(GlobalData::realTimeInfo.iSendIron ? UIStyles::css_maincolorconf_u13 : UIStyles::css_maincolorconf_u8);
}
