#include "subwindowlocalconfig.h"
#include "data/utils.h"
#include "ui_subwindowlocalconfig.h"

#include <QMessageBox>
#include <QTimer>

SubWindowLocalConfig::SubWindowLocalConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SubWindowLocalConfig)
{
    ui->setupUi(this);

    InitOther();

    //connect(ui->beep,SIGNAL(clicked()),this,SLOT(on_beep_clicked1()));
    //connect(ui->backlight,SIGNAL(clicked()),this,SLOT(on_backlight_clicked1()));

    connect(ui->setip,SIGNAL(clicked()),this,SLOT(on_setIp_clicked1()));

    connect(ui->getip,SIGNAL(clicked()),this,SLOT(on_getIp_clicked1()));

    //connect(ui->setsystime,SIGNAL(clicked()),this,SLOT(on_systime_clicked1()));

    connect(ui->setrtc,SIGNAL(clicked()),this,SLOT(on_setrtc_clicked1()));
    connect(ui->getrtc,SIGNAL(clicked()),this,SLOT(on_getrtc_clicked1()));


    QTimer *timer = new QTimer(this);
    //新建定时器
    connect(timer,SIGNAL(timeout()),this,SLOT(timerUpDate()));
    //关联定时器计满信号和相应的槽函数
    timer->start(1000);
    //定时器开始计时，其中1000表示1000ms即1秒

}

SubWindowLocalConfig::~SubWindowLocalConfig()
{
    delete ui;
}


/*
** 蜂鸣器
*/
void SubWindowLocalConfig::on_beep_clicked1()
{
#ifdef _MCGS
//    int nRet = hal_set_bee_time((size_t)ui->spinBox_beeptime->value());
//    if(0 != nRet)
//    {
//        qDebug( "beep failed: %d!", nRet);
//    }
#else
    qDebug("beep clicked");
#endif
}

/*
** 背光
*/
void SubWindowLocalConfig::on_backlight_clicked1()
{
#ifdef _MCGS
//    //set
//    int index = ui->comboBox_light->currentIndex();
//    int nCbStatus = ui->comboBox_light->itemData(index).toInt();
//    int nRet = hal_set_lcd_status(nCbStatus);
//    if(0 != nRet)
//    {
//        qDebug("set LCD light failed: %d!", nRet);
//    }


//    //get
//    int lightStatus = hal_get_lcd_status();

//    ui->label_light_status->clear();
//    QMap<int, QString>::iterator findIter = m_lightItems.find(lightStatus);
//    if(findIter != m_lightItems.end())
//    {
//        ui->label_light_status->setText(findIter.value());
//    }
//    else
//    {
//        qDebug("get LCD light status failed: %d!", lightStatus);
//    }
#else
    qDebug("on_backlight_clicked");
#endif
}

/*
** 设置ip
*/
void SubWindowLocalConfig::on_setIp_clicked1()
{

    QMessageBox::StandardButton reply;

    reply =  Utils::MyMessageBox(QString("QMessageBox::question()"), "set ip",tr("确定"),tr("取消"),this);
    if (reply == QMessageBox::Retry)
    {
        #ifdef _MCGS
             QString ipStr = ui->lineEdit_setip->text();
             QString netMaskStr  = ui->lineEdit_setmask->text();
             QString gatewayStr = ui->lineEdit_setgateway->text();

             platform_sys_info_c::set_sys_ip_addr(ipStr, netMaskStr, gatewayStr);
        #else
            qDebug("on_setIp_clicked Yes");
        #endif

    }
    else if (reply == QMessageBox::Cancel)
    {
        qDebug("on_setIp_clicked No");
    }
}

/*
** 获取ip
*/
void SubWindowLocalConfig::on_getIp_clicked1()
{
#ifdef _MCGS
    QString getTmpip = "";
    QString getTmpmask = "";
    QString getTmpgateway = "";

    if(platform_sys_info_c::get_sys_ip_addr(getTmpip, getTmpmask, getTmpgateway))
    {
        ui->lineEdit_getip->setText(getTmpip);
        ui->lineEdit_getmask->setText(getTmpmask);
        ui->lineEdit_getgateway->setText(getTmpgateway);
    }
#else
    qDebug("on_getIp_clicked");
#endif
}
/*
**系統時間
*/
void SubWindowLocalConfig::on_systime_clicked1()
{
    qDebug("on_systime_clicked");
}
/*
**RTC時間
*/
void SubWindowLocalConfig::on_setrtc_clicked1()
{
    QMessageBox::StandardButton reply;

    reply =  Utils::MyMessageBox(QString("QMessageBox::question()"), "RTC ip",tr("确定"),tr("取消"),this);

    if (reply == QMessageBox::Retry)
    {
         #ifdef _MCGS
        bool bRet = platform_sys_info_c::rtc_check();
        if(!bRet)
        {
            qDebug("check rtc status error");
        }

        //QDateTime tmptime = ui->dateEdit_setrtc->dateTime();
        QDateTime tmptime;
        tmptime.setDate(ui->dateEdit_setrtc->date());
        tmptime.setTime(ui->timeEdit_set->time());


        bRet = platform_sys_info_c::set_rtc_time(tmptime);
        if(!bRet)
        {
            qDebug("set rtc time failed");
        }
        #else
            qDebug("on_setrtc_clicked");
        #endif
    }
    else
    {
            qDebug("on_setrtc_clicked No");
    }
}
/*
**獲取RTC時間
*/
void SubWindowLocalConfig::on_getrtc_clicked1()
{
#ifdef _MCGS
    bool bRet = platform_sys_info_c::rtc_check();
    if(!bRet)
    {
        qDebug("check rtc status error");
    }

    QDateTime currTime = platform_sys_info_c::get_rtc_time();
    if(currTime.isValid())
    {
        ui->dateEdit_getrtc->setDateTime(currTime);
        ui->timeEdit_get->setDateTime(currTime);
    }
    else
    {
        qDebug("get rtc time invalid");
    }
#else
    qDebug("on_getrtc_clicked");
#endif
}

void SubWindowLocalConfig::InitOther()
{
    //setip
    QRegExp  setipRegExp("((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])[//.]){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])");
    ui->lineEdit_setip->setValidator(new QRegExpValidator(setipRegExp, ui->lineEdit_setip));
    ui->lineEdit_setmask->setValidator(new QRegExpValidator(setipRegExp, ui->lineEdit_setmask));
    ui->lineEdit_setgateway->setValidator(new QRegExpValidator(setipRegExp, ui->lineEdit_setgateway));


    ui->lineEdit_setip->setText("200.200.200.191");
    ui->lineEdit_setmask->setText("255.255.255.0");
    ui->lineEdit_setgateway->setText("200.200.200.1");

    #ifdef _MCGS
    //light
//    m_lightItems[HAL_DISPLAY_BACKLIGHT_OPEN] = QString("open");
//    m_lightItems[HAL_DISPLAY_BACKLIGHT_CLOSE] = QString("close");
//    QMap<int, QString>::iterator iter= m_lightItems.begin();
//    for(; iter!= m_lightItems.end(); ++iter)
//    {
//        ui->comboBox_light->addItem(iter.value(), iter.key());
//    }
    #else
        //light
//        m_lightItems[0] = QString("open");
//        m_lightItems[1] = QString("close");
//        QMap<int, QString>::iterator iter= m_lightItems.begin();
//        for(; iter!= m_lightItems.end(); ++iter)
//        {
//            ui->comboBox_light->addItem(iter.value(), iter.key());
//        }
        qDebug("InitOther");
    #endif
}


void SubWindowLocalConfig::timerUpDate()
{

#ifdef _MCGS
QDateTime time = platform_sys_info_c::get_rtc_time();
#else
QDateTime time = QDateTime::currentDateTime();
#endif
//获取系统现在的时间
QString str = time.toString("yyyy-MM-dd hh:mm:ss dddd");
//设置系统时间显示格式
ui->label_datime->setText(str);
//在标签上显示时间
}

