/*
 * utils.cpp
 *
 * 公用的工具函数
 *
 * Created on: 2019年 01月 21日 星期一 14:31:00 CST
 * Author: lixingcong
 */
#include "utils.h"
#include <SysText.h>
#include "Parameter.h"
#include <QTextCodec>
#include <QTimer>
#include <QTime>
#include <QDir>
#include <QEventLoop>
#include <QApplication>
#include <QDesktopWidget>
#include "modbus/modbusqueue.h"
#include <QApplication>
#include <QThread>
#include <qmath.h>
#include <QDialog>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QFontDatabase>
#include "data/globaldata.h"
#include <unistd.h>

#ifndef DEBUG_USE_PCSCREEN
#include <QApplication>
#include <QDesktopWidget>
#endif

#ifdef QT_DEBUG
#include <QDebug>
#endif

int retrytime = 0;
namespace Utils
{

static void QStringToChar(const QString& strIn, void* strOut, size_t strOutBufferLength, const char* encode)
{
    QTextCodec* codec = QTextCodec::codecForName(encode);

    if(!codec)
        return;// "Wrong QTextCodec";

    auto byteArray = codec->fromUnicode(strIn);

    if(static_cast<size_t>(byteArray.length()) + 1 > strOutBufferLength)
        return; // "Buffer length to small"

    memcpy(strOut, byteArray.data(), static_cast<size_t>(byteArray.length()));
    *(reinterpret_cast<char*>(strOut) + byteArray.length()) = 0;
}

//Utf8 QString转字符数组
static void QStringToCharUtf8(const QString& strIn, void* strOut, size_t strOutBufferLength)
{

    auto byteArray = strIn.toUtf8();

    qDebug()<<"=========="<<QString(byteArray);

    if(static_cast<size_t>(byteArray.length()) + 1 > strOutBufferLength)
        return; // "Buffer length to small"

    memcpy(strOut, byteArray.data(), static_cast<size_t>(byteArray.length()));
    *(reinterpret_cast<char*>(strOut) + byteArray.length()) = 0;
}

static const QString CharToQString(const void* strIn, const char* encode)
{
    /*  注意Qt4下，需要先使用setCodecForLocale()和setCodecForCStrings()设定默认的QString编码
     *  代码样例:
     *
     *  #if QT_VERSION < 0x050000
     *   QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");
     *   QTextCodec::setCodecForTr(utf8);
     *   QTextCodec::setCodecForLocale(utf8);
     *   QTextCodec::setCodecForCStrings(utf8);
     *  #endif
     */

    // https://blog.csdn.net/hejinjing_tom_com/article/details/77780565
    QTextCodec* codec = QTextCodec::codecForName(encode);

    if(!codec)
        return QString(); // "Wrong QTextCodec";

    return codec->toUnicode(reinterpret_cast<const char*>(strIn));
}

static const QString utfCharToQString(const void* strIn)
{
    return QString(((char*)strIn));
}

void sleep(int milliSecond)
{
    QTimer t;
    QEventLoop e;

    t.setSingleShot(true);
    QObject::connect(&t, SIGNAL(timeout()), &e, SLOT(quit()));

    t.start(milliSecond);
    e.exec();
}

//系统延时函数,非阻塞延时
void sleep1(unsigned int msec)
{

    QTime dieTime = QTime::currentTime().addMSecs(msec);

    while( QTime::currentTime() < dieTime )

        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

}

QString gbkCharToQString(const void* strIn)
{
#ifdef  Arm_GBK
    return CharToQString(strIn, "GB18030");
#else
    return CharToQString(strIn, "UTF-8");
#endif
}

QString utf8CharToQString(const void* strIn)
{
    return utfCharToQString(strIn);
}

QString usigerchartostring(const void* strIn)
{
    return QString((char*)strIn);
}

void setFullScreen(QWidget* w)
{
#ifdef DEBUG_USE_PCSCREEN
#define PC_SCREEN_WIDTH  1024//800
#define PC_SCREEN_HEIGHT 600//480
    w->setFixedSize(PC_SCREEN_WIDTH, PC_SCREEN_HEIGHT);
    GlobalData::widthRate = PC_SCREEN_WIDTH-70;
    GlobalData::highRate = PC_SCREEN_HEIGHT-120;
    GlobalData::screensize = TEN;
    qDebug("Debuging on PC, w=%d, h=%d", PC_SCREEN_WIDTH, PC_SCREEN_HEIGHT);
#else
    // #include <QDesktopWidget>
    QRect rectScreen = QApplication::desktop()->availableGeometry();
    w->setWindowFlags(Qt::FramelessWindowHint);
    w->setGeometry(0, 0, rectScreen.width(), rectScreen.height());
    GlobalData::widthRate = rectScreen.width()-70;
    GlobalData::highRate = rectScreen.height()-120;
    if(rectScreen.height()<=500)
    {
        GlobalData::screensize = SEVEN;
        int index = QFontDatabase::addApplicationFont("/root/app/msyh.ttf");
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
    }
    else if(rectScreen.height()<=700)
        GlobalData::screensize = TEN;
    else
        GlobalData::screensize = FIFTEEN;
    qDebug("Running on board, w=%d, h=%d size=%d", rectScreen.width(), rectScreen.height(),GlobalData::screensize);
#endif
}

bool slotGrabFullScreen()
{//截屏函数
    //设置当前目录:MCGS屏幕 下为U盘 LINUX下为 文件执行目录的上层
    QString filePathName="";
#ifdef _MCGS
    filePathName += "/media/usb0/";
#else
    filePathName += QDir::currentPath();
#endif
    //    filePathName += QDir::currentPath()+"/";
    filePathName += QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss-zzz");
    filePathName += ".bmp";

    if (filePathName.isEmpty()) {

        qDebug()<<"filePathName is isEmpty";
        return false;
    }
    QPixmap Pixmap = QPixmap::grabWindow(QApplication::desktop()->winId());
    if(!Pixmap.save(filePathName, "BMP"))
    {
        qDebug()<<"Pixmap screen fail";
        return false;
    }
    //#ifdef _MCGS
    QImage image;
    image.load(filePathName);
    image.rgbSwapped();//RGB转化
    Pixmap = QPixmap::fromImage(image);
    //qDebug()<<QDir::currentPath()<<filePathName;
    if(!Pixmap.save(filePathName, "JPG"))
    {
        qDebug()<<"image screen fail";
        return false;
    }
#ifdef  Q_OS_LINUX
    sync();
#endif
    //#endif
    return true;
}

void qStringToGbkChar(const QString& strIn, void* strOut, size_t strOutBufferLength)
{
#ifdef  Arm_GBK
    QStringToChar(strIn, strOut, strOutBufferLength, "GB18030");
#else
    QStringToChar(strIn, strOut, strOutBufferLength, "UTF-8");
#endif
}

void qStringToUtf8Char(const QString& strIn, void* strOut, size_t strOutBufferLength)
{
    QStringToCharUtf8(strIn, strOut, strOutBufferLength);
}

void printQString(const QString& str)
{
#ifdef QT_DEBUG
    qDebug() << str;
#endif
}

void printCharStringHex(const void* str, int len, int countPerLine)
{
#ifdef QT_DEBUG

    for(auto i = 0; i < len; ++i)
    {
        if(i % countPerLine == 0)
            qDebug("-");

        qDebug("0x%02x", *(reinterpret_cast<const unsigned char*>(str) + i));
    }

#endif
}

int getParaAdress(void* data)
{//获取参数表中参数对应的地址  注意！！如果获取了没有的参数地址会引起上下位机同事奔溃
    for(int i=0; ParaRegTab[i].DataType!=DT_END; i++)
    {
        if(ParaRegTab[i].pData == data)
        {
            //qDebug()<<"参数地址"<<ParaRegTab[i].PlcAddr;
            return ParaRegTab[i].PlcAddr;
        }
    }
    qDebug()<<"参数错误";
    return 0;
}
int getParaIndex(int addr)
{//获取参数表中参数地址对应的索引
    for(int i=0; ParaRegTab[i].DataType!=DT_END; i++)
    {
        if(ParaRegTab[i].PlcAddr == addr)
        {
            //qDebug()<<"参数地址"<<ParaRegTab[i].PlcAddr;
            return i;
        }
    }
    qDebug()<<"参数错误";
    return 0;
}
//参数：标题 提示信息 左提示 右 提示
QMessageBox::StandardButton  MyMessageBox(QString title, QString message,QString text, QWidget *parent)
{//封装 显示中文提示框
    QMessageBox megBox(parent);
    megBox.addButton(text,QMessageBox::AcceptRole);
    megBox.setText(message);
    megBox.setToolTip(title);
    int ret= megBox.exec();
    //其实就是从0开始，只不过AcceptRole在枚举中为0，此处用可以增加可读性
    switch(ret){
    case QMessageBox::AcceptRole:
        return QMessageBox::Retry;
        break;
    default:
        return QMessageBox::Cancel;
        break;
    }
    return QMessageBox::Cancel;
}
//参数：标题 提示信息 左提示 右 提示
QMessageBox::StandardButton  MyMessageBox(QString title, QString message,QString text, QString text1, QWidget *parent)
{//封装 显示中文提示框
    QMessageBox megBox(parent);
    megBox.addButton(text,QMessageBox::AcceptRole);
//    if(text1!="")
    megBox.addButton(text1,QMessageBox::RejectRole);
    megBox.setText(message);
    megBox.setToolTip(title);
    int ret= megBox.exec();
    //其实就是从0开始，只不过AcceptRole在枚举中为0，此处用可以增加可读性
    switch(ret){
    case QMessageBox::AcceptRole:
        return QMessageBox::Retry;
        break;
    default:
        return QMessageBox::Cancel;
        break;
    }
    return QMessageBox::Cancel;
}

bool dialogIsModbusRetry(QWidget* parent, ModbusQueue* modbusQueue)
{
    const QString title = QObject::tr("通讯故障");
    const QString retryMsg = QObject::tr("通讯失败，是否重试？");

    if(nullptr != modbusQueue)
    {
        if(false == modbusQueue->isOffline())
            modbusQueue->queryPause(); // 暂停
        else
            return false;
    }

    if(retrytime<MAX_RETRY)
    {
        retrytime++;
        qDebug()<<retrytime;
        Utils::sleep1(1);
        return QMessageBox::Retry == QMessageBox::Retry;
    }
    else
    {
        //        QMessageBox::StandardButton reply =
        //                QMessageBox::question(parent, title, retryMsg, QMessageBox::Retry | QMessageBox::Cancel);//其实就是从0开始，只不过AcceptRole在枚举中为0，此处用可以增加可读性
        //        return QMessageBox::Retry == reply;
        QMessageBox::StandardButton reply =
                MyMessageBox(title, retryMsg,QObject::tr("重试"),QObject::tr("取消"),parent);

        return QMessageBox::Retry == reply;
    }
}

void waitForPendingModbus(int* modbusPendingCount, int sleepTime)
{
    while(*modbusPendingCount > 0)
    {
        //debug_printf("waitForPendingModbus()...");
        Utils::sleep(sleepTime);
    }
}

void waitForThreadFinished(QThread* thread, int sleepTime)
{
    while(thread->isRunning())
    {
        //debug_printf("waitForThreadFinished()...");
        Utils::sleep(sleepTime);
    }
}

const QString getHumanReadableSize(int bytes)
{
    qreal num = 0.0;
    int unit = 0;

    const QStringList units = {"B", "KB", "MB", "GB", "TB"};

    if(bytes > 1)
    {
        unit = static_cast<int>(qFloor(qLn(bytes)) / qLn(1024));
        num = static_cast<qreal>(bytes) / qPow(2, unit * 10);
    }

    return numberToString(num, 1) + units.at(unit);
}

QDialogButtonBox* createOkCancelButtons(QDialog* dialog)
{
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, dialog);
    QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));
    buttonBox->button(QDialogButtonBox::Ok)->setText(QObject::tr("确定"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(QObject::tr("取消"));

    return buttonBox;
}
void sendOffest(void* data,void* addr,int size,bool write)
{//数据读写
    ModbusQueue::element_t element;
    element.data=data;
    element.len=size;
    element.operation=write?ModbusQueue::OP_WRITE_DATA:ModbusQueue::OP_READ_DATA;
    element.startAddress=Utils::getParaAdress(addr);//旋转零点位置X
    //qDebug()<<Utils::getParaAdress(data);
    GlobalData::modbusQueue->sendQuery(&element,false);
}
void sendOffest(void* data,int addr,int size,bool write)
{//数据读写
    ModbusQueue::element_t element;
    element.data=data;
    element.len=size;
    element.operation=write?ModbusQueue::OP_WRITE_DATA:ModbusQueue::OP_READ_DATA;
    element.startAddress=addr;//旋转零点位置X
    //qDebug()<<Utils::getParaAdress(data);
    GlobalData::modbusQueue->sendQuery(&element,false);
}
// 计算平方的函数;
double square(const double num){return num * num;}

// 计算屏幕上面两个点之间的直线距离的函数，需要与计算平方函数同时使用;
double TwoPtDistance(const QPointF& pt1, const QPointF& pt2)
{return sqrt(double(square(pt2.x() - pt1.x()) + square(pt2.y() - pt1.y())));}



QString loadFontFamilyFromFiles(const QString &fontFileName)
{//字体加载
    static QHash<QString, QString> tmd;
    if (tmd.contains(fontFileName)) {
        return tmd.value(fontFileName);
    }
    QString font = "";
    QFile fontFile(fontFileName);
    if(!fontFile.open(QIODevice::ReadOnly)) {
        qDebug()<<"Open font file error";
        return font;
    }

    int loadedFontID = QFontDatabase::addApplicationFontFromData(fontFile.readAll());
    QStringList loadedFontFamilies = QFontDatabase::applicationFontFamilies(loadedFontID);
    if(!loadedFontFamilies.empty()) {
        font = loadedFontFamilies.at(0);
    }
    fontFile.close();

    if (!(font.isEmpty()))
        tmd.insert(fontFileName, font);
    return font;
}

bool isDirExist(QString fullPath)
{//判断文件是否存在
#ifdef _MCGS
    QDir dir(fullPath);
    if(dir.exists())
    {
      return true;
    }

    return false;
#else
    return true;
#endif
}

} // namespace Utils
