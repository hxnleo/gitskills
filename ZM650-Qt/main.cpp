/*
 * main.cpp
 *
 * 主函数
 *
 * Created on: 2019年 01月 21日 星期一 15:52:14 CST
 * Author: lixingcong
 */

#include <QApplication>
#include <QTextCodec>
#include "widgets/myinputpanelcontext.h"
#include "windows/mainwindow.h"
#include "windows/startupdialog.h"
#include "data/globaldata.h"
#include "data/utils.h"
#include "modbus/modbusaddress.h"
#include "platform/common.h"

// /home/mcgs/work/font
static quint16 language=0;

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

#if QT_VERSION < 0x050000
    // Qt5已经默认为UTF-8编码。这里仅针对Qt4的设置编码
    QTextCodec* utf8 = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(utf8);
    QTextCodec::setCodecForLocale(utf8);
    QTextCodec::setCodecForCStrings(utf8);
#endif

#ifdef _MCGS
    /*font 设置系统字体 simhei.ttf黑体   msyh.ttf 软黑
x_simsun.ttc 新宋体 ADT_Font.ttf 方块数字*/
#ifdef Arm_Kaiteli
    int index = QFontDatabase::addApplicationFont("/root/app/x_simsun.ttc");
#else
    int index = QFontDatabase::addApplicationFont("/root/app/msyh.ttf");
#endif
    GlobalData::fontindex= QFontDatabase::addApplicationFont("/root/app/ADT_Font.ttf");
    qDebug()<<"index"<<index<<GlobalData::fontindex;
    if(index != -1) {
        QStringList fontList(QFontDatabase::applicationFontFamilies(index));

        //qDebug()<<"设置字体";
        /* font -- LiHei Pro */
        if(fontList.count() > 0) {
            QFont font(fontList.at(0));
            font.setPointSize(10);
            font.setBold(false);
            QApplication::setFont(font);
        }
    }
    else
        qDebug()<<"font load false!";
#else
    int nfontId = QFontDatabase::addApplicationFont("/home/mcgs/work/font/msyh.ttf");
    qDebug()<<"fontId = "<< nfontId;//等于-1加载失败
    if(nfontId>=0)
    {
        QString msyh = QFontDatabase::applicationFontFamilies(nfontId).at(0);
        qDebug() <<"Font = " <<msyh;
        QFont font(msyh);
        font.setPointSize(12);
        QApplication::setFont(font);
    }
#endif

    //set mcgs ip
#ifdef _MCGS
    QString ipStr = "192.168.0.12";
    QString netMaskStr  = "255.255.255.0";
    QString gatewayStr = "192.168.0.1";
    platform_sys_info_c::set_sys_ip_addr(ipStr, netMaskStr, gatewayStr);

    int nRet = hal_set_bee_time(30);
    if(0 != nRet)
    {
        qDebug( "beep failed: %d!", nRet);
    }
    QApplication::setOverrideCursor(Qt::BlankCursor);//隐藏鼠标

#endif

    // Modbus must be created before creating MainWindow
    GlobalData::modbusQueue = new ModbusQueue;
    GlobalData::modbusQueue->init("192.168.0.120", 1200, 3);
//    GlobalData::modbusQueue->init("192.168.9.120", 1200, 3);
    //读取一些需要处理的参数
    Utils::sleep(3000);//延时3s,等待下位机参数初始化好以及modbus等任务启动
    {
        quint16 u16,axis = 0;
        ModbusQueue::element_t element;
        element.data=&u16;
        element.len=sizeof(u16);
        element.operation=ModbusQueue::OP_READ_DATA;
        element.startAddress=ModbusAddress::language;//系统语言

        if(GlobalData::modbusQueue->sendQuery(&element,false))
        {
            GlobalData::language=u16;
        }
        //系统轴参数
        element.data=&axis;
        element.len=sizeof(axis);
        element.operation=ModbusQueue::OP_READ_DATA;
        element.startAddress=ModbusAddress::SyetemAxis;//系统轴数

        if(GlobalData::modbusQueue->sendQuery(&element,false))
        {
            GlobalData::systemaxis = axis;
            GlobalData::maxAxisNum = axis;
            qDebug()<<"axis"<<axis;
        }
    }

    //读取刹车状态 ZM450 冉隆平
    Utils::sendOffest(&GlobalData::closebtn,ModbusAddress::CLoseBtn,sizeof(GlobalData::closebtn));

    //读取32位比特数据到 GlobalData::parambite
    Utils::sendOffest(&GlobalData::parambite,ModbusAddress::ParamUse,sizeof(GlobalData::parambite));

    GlobalData::slectetype=READ_BIT(GlobalData::parambite,3);//读取花样样式



    //    int parmstatus= READ_BIT(GlobalData::parambite,2);//去读对应的位数
    //    GlobalData::parambite = SET_BIT(GlobalData::parambite,2);
    //language
    QTranslator translator;
    QFont fon;
    if(GlobalData::language==0)//ch
    {
        translator.load("");
        a.installTranslator(&translator);
    }
    else if(GlobalData::language==1)//En
    {
        fon.setPointSize(10);
        a.setFont(fon);
        translator.load(":/languege/language/ZM650_tr_en.qm");
        a.installTranslator(&translator);
    }
    else//其余语言往下扩展，加载相应语言包即可
    {
        translator.load("");
        a.installTranslator(&translator);
    }


    //keyboard
    MyInputPanelContext* ic = new MyInputPanelContext(&a);
    a.setInputContext(ic);


    // MainWindow
    MainWindow w;

    a.installEventFilter(&w);
    // Modbus and globalData will be deleted after deleting MainWindow
    GlobalData::modbusQueue->setParent(&w);
    GlobalData::getInstance()->setParent(&w);

    // Startup Dialog
    auto startUpDialog=new StartUpDialog;
    startUpDialog->setAttribute(Qt::WA_DeleteOnClose);
    startUpDialog->setWindowIcon(QIcon(":/images/images/NULL.ico"));

    QObject::connect(startUpDialog, SIGNAL(accepted()),
                     &w, SLOT(show()));
    QObject::connect(startUpDialog, SIGNAL(rejected()),
                     &a, SLOT(quit()));
//    QSharedMemory shared("startUpDialog");
//    if(shared.attach())//共享内存被占用则直接返回
//    {//防止程序多开
//        Utils::MyMessageBox(QString("Erro"), QString("只能打开一个应用"),QString("确定"));
//        return 0;
//    }
//    shared.create(1);//共享内存没有被占用则创建UI
    startUpDialog->open();

    return a.exec();
}
