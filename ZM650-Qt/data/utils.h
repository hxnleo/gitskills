/*
 * utils.h
 *
 * 公用的工具函数
 *
 * Created on: 2019年 01月 21日 星期一 14:31:00 CST
 * Author: lixingcong
 */
#ifndef UTILS_H
#define UTILS_H
#include <QMessageBox>
#include <QObject>
#include <QWidget>
#include <QDebug>
#include <math.h>

class ModbusQueue;
class QThread;
class QDialogButtonBox;
class QDialog;

#ifdef QT_DEBUG
    #define debug_printf(fmt,...) do{qDebug(fmt,##__VA_ARGS__);}while(0)
#else
    #define debug_printf(fmt,...)
#endif

#define MAX_RETRY 2
#define SET_BIT(x,y)      x|(1<<y)    //设置x的第y位比特位
#define CLR_BIT(x,y)      x&(~(1<<y)) //清除x的第y位比特位
#define READ_BIT(x,y)     (x>>y)&0x01 //读取x的第y位比特位

extern int retrytime;
namespace Utils
{
// 回收内存模板函数，使用指针的引用来修改指针为NULL
// https://stackoverflow.com/questions/41225738/c-template-function-to-delete-and-reset-pointers
template<class T>
void delete_array(T*& ptr)
{
    if(nullptr != ptr)
    {
        delete[] ptr;
        ptr = nullptr;
    }
}

template<class T>
void delete_pointer(T*& ptr)
{
    if(nullptr != ptr)
    {
        delete ptr;
        ptr = nullptr;
    }
}

template<typename T>
const QString numberToString(T number, int decimal = 0, int fieldWidth = 0)
{
    if(decimal > 0)
    {
        double v = static_cast<double>(number);
        return QString("%1").arg(v, fieldWidth, 'f', decimal, '0');
    }

    return QString("%1").arg(number, fieldWidth);
}

template<typename T>
const QString dataToString(T number,int Digit = 0)
{//保留几位整数
    QString str = QString::number(number);
    for(int i=0;i<Digit-1;i++)
    {

        if(number*pow(10,i)< pow(10,Digit-1))
        {
            //qDebug()<<number*pow(10,i)<<pow(10,Digit-1);
            str ="0"+str;
        }
        else
            break;
    }
    return str;
}

template<typename T_FROM, typename T_TO>
void copyValues(const T_FROM* from, T_TO* to, int count)
{
    for(auto i = 0; i < count; ++i)
        *(to + i) = static_cast<T_TO>(*(from + i));
}

void sleep(int milliSecond);
void sleep1(unsigned int msec);

QString gbkCharToQString(const void* strIn);
QString utf8CharToQString(const void* strIn);
QString usigerchartostring(const void *strIn);
void qStringToGbkChar(const QString& strIn, void* strOut, size_t strOutBufferLength);
void qStringToUtf8Char(const QString& strIn, void* strOut, size_t strOutBufferLength);

void setFullScreen(QWidget* w);
bool slotGrabFullScreen();//截屏函数

void printQString(const QString& str);
void printCharStringHex(const void* str, int len, int countPerLine = 5);

int getParaAdress(void* data);
int getParaIndex(int addr);
bool dialogIsModbusRetry(QWidget* parent, ModbusQueue *modbusQueue = nullptr);


QMessageBox::StandardButton  MyMessageBox(QString title, QString message, QString text, QWidget *parent= nullptr);
QMessageBox::StandardButton  MyMessageBox(QString title, QString message, QString text, QString text1, QWidget *parent= nullptr);
int  dialogIsModbusRetry(QString title, QString message);
bool dialogIsModbusRetry(QWidget* parent, ModbusQueue* modbusQueue);
void waitForPendingModbus(int* modbusPendingCount, int sleepTime = 50);
void waitForThreadFinished(QThread* thread, int sleepTime = 50);

const QString getHumanReadableSize(int bytes);

QDialogButtonBox* createOkCancelButtons(QDialog* dialog);
void sendOffest(void* data, void *addr, int size, bool write = false);
void sendOffest(void* data,int addr,int size,bool write = false);
double square(const double num);
double TwoPtDistance(const QPointF& pt1, const QPointF& pt2);
QString loadFontFamilyFromFiles(const QString &fontFileName);
bool isDirExist(QString fullPath);
template<typename T_FLOAT> //浮点数比较 绝对值
int absolute(T_FLOAT num1,T_FLOAT num2){return fabs(num1 - num2)*pow(10,5);}
} // namespace Utils

#endif // UTILS_H
