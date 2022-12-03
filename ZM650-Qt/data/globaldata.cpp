/*
 * globaldata.cpp
 *
 * 全局变量
 *
 * Created on: 2019年 01月 21日 星期一 14:31:00 CST
 * Author: lixingcong
 */
#include "globaldata.h"
#include <QStringList>

#ifndef LIXINGCONG_DEMO_UI
    #include "widgets/qpushbuttonspage.h"
#endif

#include "modbus/modbusqueue.h"


#ifndef LIXINGCONG_DEMO_UI
    QPushButtonsPage* GlobalData::pushButtonPage = nullptr;
#endif
const QStringList GlobalData::axisName = {("X"), ("Y"), ("Z"), ("A"), ("B"), ("C"),("A7"), ("A8"), ("A9"), ("A10"), ("A11"), ("A12"),("A13"), ("A14"), ("A15"), ("A16")};
QStringList GlobalData::realaxisName = {("X"), ("Y"), ("Z"), ("A"), ("B"), ("C"),("A7"), ("A8"), ("A9"), ("A10"), ("A11"), ("A12"),("A13"), ("A14"), ("A15"), ("A16")};
QStringList GlobalData::teachaxisName = {("X"), ("Y"), ("Z"), ("A"), ("B"), ("C"),("A7"), ("A8"), ("A9"), ("A10"), ("A11"), ("A12"),("A13"), ("A14"), ("A15"), ("A16")};

double GlobalData::TEACH_STEPS[] = {0.010, 0.100, 1.000};//mod by yzg 190529
size_t GlobalData::stepIndex = 0;
int GlobalData::axis[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
int GlobalData::teachaxis[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
int GlobalData::totalAxisNum = 0;
int GlobalData::teachdisplay = 0;
int GlobalData::maxAxisNum = TotalAxis;
quint16 GlobalData::lastAlarmNumber=0xffff;
ModbusQueue* GlobalData::modbusQueue = nullptr;
STRANSLATE_T GlobalData::realTimeInfo = {0};
GlobalData* GlobalData::instance = nullptr;
QString GlobalData::workfileName = "";
QLabel* GlobalData::labelTopTab = nullptr;
QString GlobalData::editnum = "";
QList<quint32> GlobalData::storenum;//显示输入的值
bool GlobalData::infile = false;
int GlobalData::num = 0;//显示输入的值的个数
QString GlobalData::renewEdit = "";
bool GlobalData::online = true;
int GlobalData::widthRate;
int GlobalData::highRate;
int GlobalData::dogflag = false;//快到期标志
const QString GlobalData::filenameProgramA9 = QString("A9Rom.bin");
const QString GlobalData::filenameProgramQt = QString("adtrom");
const QString GlobalData::filenameProgramMotion = QString("Motion.bin");
int GlobalData::screensize = 0;
bool GlobalData::ScreenCapture=false;
bool GlobalData::backZeroIcon = false;
bool GlobalData::backZeroFlag = true;
quint16 GlobalData::closebtn = false;
int GlobalData::brakenum = 0;
int GlobalData::motornum = 0;
int GlobalData::iostatus = 0;
int GlobalData::systemaxis = TotalAxis;
bool GlobalData::iodisplay = false;
float GlobalData::Color1 = 0;
float GlobalData::Color2 = 0;
float GlobalData::Color3 = 0;
QString GlobalData::PicturePath = "/root/app/logo.bmp";
QString GlobalData::FontPath = "/root/app/";
QString GlobalData::Debugtext = "";
int GlobalData::fontindex = -1;
INT32U GlobalData::parambite= 0;
int GlobalData::slectetype=0;
int GlobalData::selectedAxisNum[2]={0,1}; // X Y
int GlobalData::selectedAxisDirection[2]={1,1}; // 正向 正向
quint32 GlobalData::doubleClickNextWidget=0;
int GlobalData::paramauthority= GlobalData::USER_LEVEL_H;
QString GlobalData::patternarry[MAX_PATTERN_ROW][MAX_PATTERN_COL]={0};
quint16 GlobalData::language=0;
bool GlobalData::DISABLETOUCH=false;
int GlobalData::swithcolumn=-1;
bool GlobalData::parampassword = false;

bool GlobalData::DataAuthority(bool type)
{//权限判断
    switch (GlobalData::paramauthority) {
    case GlobalData::USER_LEVEL_L://最低权限不能操作参数
            return false;
        break;
    case GlobalData::USER_LEVEL_M://用户权限操作部分参数
        if(type)
            return true;
        else
            return false;
        break;
    case GlobalData::USER_LEVEL_H://最高权限开放所有
        return true;
        break;
    default:
        return false;
        break;
    }
    return false;
}
void GlobalData::widgetEnable(QWidget* w,bool authority)
{//控件操作使能
    if(Run == GlobalData::realTimeInfo.iWSta)
        w->setDisabled(true);
    else
    {
        if(authority)
            w->setDisabled(false);
        else
            w->setDisabled(true);
    }

}

GlobalData* GlobalData::getInstance()
{
    if(nullptr == instance)
        instance = new GlobalData;
    return instance;
}
