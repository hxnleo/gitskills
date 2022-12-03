/*
 * adapters.cpp
 *
 * 适配器，用于下位机与上位机之间的数据读取保存
 *
 * Created on: 2019年 01月 21日 星期一 14:31:00 CST
 * Author: lixingcong
 */
#include "adapters.h"
#include <ParaManage.h>
#include <Parameter.h>
#include <public.h>
#include <QStringList>
#include "data/utils.h"
#include "data/globaldata.h"
#include "modbus/modbusqueue.h"
#include "modbus/modbusaddress.h"
TParamTable ParaRegTab_AxisMove[1000];
#define ALARM_NAME_INTERVAL  16
#define IO_NAME_INTERVAL   16
namespace Adapters
{
static QMap<void*, QStringList> mapSelectionTextParams; // 下拉框
static QList<ParamTableSectionType> paramTableSections; // ParaTable的段落
static int mapAlarmNumberToTextIndex[MAX_ALARM_NUM] = {0}; // 报警号映射到报警文本数组的下标

static QStringList  ParaName;//存储参数名称
static QStringList  AlarmName;//存储报警号对应名称
static QStringList  IOFunName_I;//存储输入IO功能号对应名称
static QStringList  IOFunName_O;//存储输出IO功能号对应名称


void initParamAxis()
{//初始化显示表
    int i,j,temp=0;//temp存储 轴配置参数 位数
    TParamTable *p = (TParamTable *)ParaRegTab;
    TParamTable *q = (TParamTable *)ParaRegTab_AxisMove;
    for(i=0,j=0; p[i].DataType!=DT_END; i++)
    {//不处理综合参数 并记录综合参数出现的位置
        if(QString(p[i].pDescText)=="轴配置参数")
        {
            temp = i;
            break;
        }
        q[j] = p[i];
        j++;
    }
    for(i=temp,j=j; p[i].DataType!=DT_END; i++)
    {
        if(axisRemove(GlobalData::systemaxis,QString(p[i].pDescText)))
        {
            continue;
        }
        q[j] = p[i];
        j++;
        //qDebug()<<q[i].pDescText<<q[i].PlcAddr<<q[i].DataType;
    }
    q[j].DataType=DT_END;
    //    for(i=0,j=0; q[i].DataType!=DT_END; i++)
    //    {
    //        qDebug()<<q[i].pDescText<<q[i].PlcAddr<<q[i].DataType;
    //    }
}

void initParamTable()
{
    qDebug()<<"initParamTable";
    ParaManage_Init();//初始化参数地址
    initParamAxis();//显示对应轴初始化
    INT32U* p_REGNULL = &REGNULL;

    QMap<void*, int> mapDataToIndex; // 从data指针到ParaRegTab数组下标映射
    ParamTableSectionType section;
    QString sectionTitle;

    int sectionBeginIndex = 0;

    for(auto i = 0; ParaRegTab_AxisMove[i].DataType != DT_END; ++i)
    {
        //中文翻译指定编码 用于文本显示
        ParaName.append(QCoreApplication::translate("ParaRegTab",ParaRegTab_AxisMove[i].pDescText));
        qDebug()<< ParaRegTab_AxisMove[i].PlcAddr<<ParaRegTab_AxisMove[i].pDescText;
        switch(ParaRegTab_AxisMove[i].DataType)
        {
        case DT_TITLE:
            sectionBeginIndex = i + 1;
            //sectionTitle = Utils::gbkCharToQString(ParaRegTab[i].pDescText);
            //mod by yzg
            //sectionTitle = QObject::tr(ParaRegTab[i].pDescText);
            sectionTitle = QCoreApplication::translate("ParaRegTab",ParaRegTab_AxisMove[i].pDescText);
            break;

        case DT_NONE:
            section.startIndex = sectionBeginIndex;
            section.totalCount = i - section.startIndex;
            section.title = sectionTitle;
            paramTableSections.push_back(section);
            break;

        case DT_INT8U:
        case DT_INT8S:
        case DT_INT16U:
        case DT_INT16S:
        case DT_INT32U:
        case DT_INT32S:
        case DT_FLOAT:
        case DT_DOUBLE:
            if(p_REGNULL == ParaRegTab_AxisMove[i].pData)
                ParaRegTab_AxisMove[i].paramWidgetType = WidgetType_WIRTE_ANYTHING;
            else
                ParaRegTab_AxisMove[i].paramWidgetType = WidgetType_NUMBER;

            mapDataToIndex[ParaRegTab_AxisMove[i].pData] = i;
            break;
        default:
            ParaRegTab_AxisMove[i].paramWidgetType = WidgetType_NULL; // 默认无widget
            break;
        }
    }
    // 特殊化
    // 下拉
    auto addSelectStringToMap = [&](void* pData, const QStringList & strs)
    {
        auto index = mapDataToIndex[pData];
        ParaRegTab_AxisMove[index].paramWidgetType = WidgetType_SELECT;
        mapSelectionTextParams[pData] = strs;
    };
#if 1
    const QStringList strYesNo  = QStringList({QObject::tr("否"), QObject::tr("是")});
    const QStringList strOnOff   = QStringList({QObject::tr("关闭"), QObject::tr("开启")});
    const QStringList strAxis     = QStringList({QObject::tr("X"),QObject::tr("Y"),QObject::tr("Z"),QObject::tr("A"),QObject::tr("B"),QObject::tr("C")});
    const QStringList strOnAxis = QStringList({QObject::tr("关闭"),QObject::tr("X"),QObject::tr("Y"),QObject::tr("Z"),QObject::tr("A"),QObject::tr("B"),QObject::tr("C"),QObject::tr("A7"),QObject::tr("A8"),QObject::tr("A9")});

    addSelectStringToMap(&ATPARA.TouchBeep, strYesNo);
    addSelectStringToMap(&ATPARA.bColor, strYesNo);
    addSelectStringToMap(&g_Sysparam.Home_Mode, QStringList() << QObject::tr("程序归零") << QObject::tr("机械归零"));
    addSelectStringToMap(&ATPARA.bZLimit, strYesNo);
    addSelectStringToMap(&ATPARA.bALimit, strYesNo);
    addSelectStringToMap(&ATPARA.bBLimit, strYesNo);
    addSelectStringToMap(&ATPARA.bRunMode, QStringList() << QObject::tr("离开下位运动") << QObject::tr("到上位运动"));
    addSelectStringToMap(&ATPARA.bPauseMode, QStringList() << QObject::tr("立即停止")<< QObject::tr("暂停时到高位")<< QObject::tr("高速时到高位"));
    addSelectStringToMap(&ATPARA.bTeach, QStringList() << QObject::tr("关闭") << QObject::tr("手摇轮5A") << QObject::tr("手持盒5B") << QObject::tr("输入手盒") << QObject::tr("16轴NEM"));
    addSelectStringToMap(&ATPARA.RunMode, QStringList() << QObject::tr("快移") << QObject::tr("插补"));
    addSelectStringToMap(&g_Sysparam.Inp_AdMode, QStringList() << QObject::tr("S型")<<QObject::tr("梯形")<<QObject::tr("指数函数")<<QObject::tr("三角函数"));
    addSelectStringToMap(&ATPARA.PatMode, QStringList() << QObject::tr("关闭") << QObject::tr("加工拍毛") << QObject::tr("上电拍毛"));
    addSelectStringToMap(&ATPARA.OpenFixture, strYesNo);
    addSelectStringToMap(&ATPARA.ClipDetection, strYesNo);
    addSelectStringToMap(&ATPARA.bColorSpeed, strYesNo);
    addSelectStringToMap(&ATPARA.LastSpeedSw, strYesNo);
    addSelectStringToMap(&ATPARA.BuKongSpeedSw, strYesNo);
    addSelectStringToMap(&ATPARA.bJumpSpeed, strYesNo);
    addSelectStringToMap(&ATPARA.bChangeColor, QStringList() << QObject::tr("运动换色") << QObject::tr("电眼换色") << QObject::tr("关闭"));
    addSelectStringToMap(&ATPARA.bThreeColor, strYesNo);
    addSelectStringToMap(&ATPARA.bColorSpeed, strYesNo);
    //    addSelectStringToMap(&ATPARA.bXEnable, strYesNo);
    addSelectStringToMap(&ATPARA.bYEnable, strYesNo);
    addSelectStringToMap(&ATPARA.bZEnable, strYesNo);
    addSelectStringToMap(&ATPARA.bAEnable, strYesNo);
    addSelectStringToMap(&ATPARA.bBEnable, strYesNo);
    addSelectStringToMap(&ATPARA.iAxisWorkplace, QStringList() << QObject::tr("X")<<QObject::tr("Y")<<QObject::tr("Z")<<QObject::tr("A")<<QObject::tr("B")<<QObject::tr("C"));
    addSelectStringToMap(&ATPARA.bAFirst, QStringList() << QObject::tr("关闭")<<QObject::tr("X")<<QObject::tr("Y")<<QObject::tr("Z")<<QObject::tr("A")<<QObject::tr("B")<<QObject::tr("C"));

    for(int i=0;i<TotalAxis;i++)
    {//根据轴数自动配置链表
//        static QStringList str;
//        if(i==0) str.append(QObject::tr("关闭"));
//        else  str.append(GlobalData::axisName[i-1]);
//        addSelectStringToMap(&ATPARA.iAxisIron, str);

        addSelectStringToMap(&g_Sysparam.AxisParam[i].PulseMode, QStringList() << QObject::tr("正向") << QObject::tr("负向"));
        addSelectStringToMap(&g_Sysparam.AxisParam[i].ZeroForLimit, strOnOff);
        addSelectStringToMap(&g_Sysparam.AxisParam[i].AxisAdMode, QStringList() << QObject::tr("S型")<<QObject::tr("梯形")<<QObject::tr("指数函数")<<QObject::tr("三角函数"));
        addSelectStringToMap(&g_Sysparam.AxisParam[i].ServoAlarmValid, strYesNo);
    }
    addSelectStringToMap(&ATPARA.iAxisIron, strOnAxis);
    addSelectStringToMap(&ATPARA.jzs, QStringList()<<QObject::tr("单工位做两工位")<<QObject::tr("单工位")<<QObject::tr("双工位")<<QObject::tr("三工位"));
    addSelectStringToMap(&g_Sysparam.SysLanguage, QStringList() << QObject::tr("中文") << QObject::tr("英文"));
    addSelectStringToMap(&ATPARA.iFootInStyle, QStringList() << QObject::tr("开启") << QObject::tr("特殊"));
    addSelectStringToMap(&ATPARA.WorkSafePoint, strYesNo);
    addSelectStringToMap(&ATPARA.bZuanEnable, strYesNo);
    addSelectStringToMap(&ATPARA.AFirstMove, strYesNo);
    addSelectStringToMap(&ATPARA.PressHair, strYesNo);
    //addSelectStringToMap(&ATPARA.SelfGoStart, strYesNo);
    addSelectStringToMap(&ATPARA.SelfGoStart, QStringList() << QObject::tr("到第1孔") << QObject::tr("到起点")<< QObject::tr("到最后孔"));

    addSelectStringToMap(&ATPARA.IOhide, strOnOff);
    addSelectStringToMap(&ATPARA.SendWire, strOnOff);
    addSelectStringToMap(&ATPARA.AB_Follow_Spindle, QStringList()  << QObject::tr("否")<< QObject::tr("是")<< QObject::tr("变频器跟随"));
    addSelectStringToMap(&ATPARA.ZA_SYNC, QStringList() << QObject::tr("关闭") << QObject::tr("YZ同步")<< QObject::tr("ZA同步"));

    addSelectStringToMap(&ATPARA.AxisCameMain, strOnAxis);
    addSelectStringToMap(&ATPARA.AxisCameSlave,strOnAxis);
    addSelectStringToMap(&ATPARA.HandBoxSoftLimit, strOnOff);
    addSelectStringToMap(&ATPARA.iAxis2Iron, QStringList()<<QObject::tr("关闭")<<GlobalData::axisName);




    addSelectStringToMap(&ATPARA.SendWire, QStringList()<<QObject::tr("送丝")<<QObject::tr("送革"));

#ifdef Arm_460Y
    addSelectStringToMap(&ATPARA.TouchBeep, strYesNo);
    addSelectStringToMap(&ATPARA.bColor, strYesNo);

    //ATPARA.fRotatePos1;//旋转零点位置
    addSelectStringToMap(&ATPARA.iAxisRotate, QStringList() << QObject::tr("关闭") << QObject::tr("X")<<QObject::tr("Y")<<QObject::tr("Z")<<QObject::tr("A")<<QObject::tr("B")<<QObject::tr("C"));
    addSelectStringToMap(&ATPARA.bRunMode, QStringList() << QObject::tr("离开下位运动") << QObject::tr("到上位运动"));
    addSelectStringToMap(&ATPARA.bThreeColor, QStringList() << QObject::tr("关闭") << QObject::tr("开启"));

    addSelectStringToMap(&ATPARA.bPauseMode, QStringList() << QObject::tr("立即停止")<< QObject::tr("暂停时到高位"));
    addSelectStringToMap(&ATPARA.bTeach, QStringList() << QObject::tr("关闭") << QObject::tr("手摇轮") << QObject::tr("手持盒") << QObject::tr("输入手盒"));
    addSelectStringToMap(&ATPARA.bMetalCheck, QStringList() << QObject::tr("关闭") << QObject::tr("开启"));
    addSelectStringToMap(&ATPARA.LastSpeedSw, QStringList() << QObject::tr("关闭") << QObject::tr("开启"));

    addSelectStringToMap(&ATPARA.RunMode, QStringList() << QObject::tr("快移") << QObject::tr("插补")<< QObject::tr("恒速插补"));
    addSelectStringToMap(&ATPARA.PatMode, QStringList() << QObject::tr("关闭") << QObject::tr("运动开启")<<QObject::tr("上电开启"));

    addSelectStringToMap(&AxParam.ZeroForLimit, strOnOff);
    addSelectStringToMap(&AyParam.ZeroForLimit, strOnOff);
    addSelectStringToMap(&AzParam.ZeroForLimit, strOnOff);
    addSelectStringToMap(&AaParam.ZeroForLimit, strOnOff);
    addSelectStringToMap(&AbParam.ZeroForLimit, strOnOff);
    addSelectStringToMap(&AcParam.ZeroForLimit, strOnOff);//AxParam.AxisAdMode

    addSelectStringToMap(&AxParam.AxisAdMode, QStringList() << QObject::tr("S型")<<QObject::tr("梯形")<<QObject::tr("指数函数")<<QObject::tr("三角函数"));
    addSelectStringToMap(&AyParam.AxisAdMode, QStringList() << QObject::tr("S型")<<QObject::tr("梯形")<<QObject::tr("指数函数")<<QObject::tr("三角函数"));
    addSelectStringToMap(&AzParam.AxisAdMode, QStringList() << QObject::tr("S型")<<QObject::tr("梯形")<<QObject::tr("指数函数")<<QObject::tr("三角函数"));
    addSelectStringToMap(&AaParam.AxisAdMode, QStringList() << QObject::tr("S型")<<QObject::tr("梯形")<<QObject::tr("指数函数")<<QObject::tr("三角函数"));
    addSelectStringToMap(&AbParam.AxisAdMode, QStringList() << QObject::tr("S型")<<QObject::tr("梯形")<<QObject::tr("指数函数")<<QObject::tr("三角函数"));
    addSelectStringToMap(&AcParam.AxisAdMode, QStringList() << QObject::tr("S型")<<QObject::tr("梯形")<<QObject::tr("指数函数")<<QObject::tr("三角函数"));
    addSelectStringToMap(&g_Sysparam.Inp_AdMode, QStringList() << QObject::tr("S型")<<QObject::tr("梯形")<<QObject::tr("指数函数")<<QObject::tr("三角函数"));

    addSelectStringToMap(&AxParam.ServoAlarmValid, strYesNo);
    addSelectStringToMap(&AyParam.ServoAlarmValid, strYesNo);
    addSelectStringToMap(&AzParam.ServoAlarmValid, strYesNo);
    addSelectStringToMap(&AaParam.ServoAlarmValid, strYesNo);
    addSelectStringToMap(&AbParam.ServoAlarmValid, strYesNo);
    addSelectStringToMap(&AcParam.ServoAlarmValid, strYesNo);
    addSelectStringToMap(&g_Sysparam.SysLanguage, QStringList() << QObject::tr("中文") << QObject::tr("英文"));
#endif
#ifdef Arm_662
    addSelectStringToMap(&ATPARA.ClampAutoOpen, strYesNo);
    addSelectStringToMap(&ATPARA.WorkSafePoint, strYesNo);
    addSelectStringToMap(&ATPARA.jzs, QStringList()<<QObject::tr("单工位")<<QObject::tr("双工位")<<QObject::tr("三工位"));
    addSelectStringToMap(&ATPARA.bMetalCheck, QStringList() << QObject::tr("关闭") << QObject::tr("开启"));
    addSelectStringToMap(&ATPARA.bPauseMode, QStringList() << QObject::tr("立即停止")<< QObject::tr("暂停时到高位"));
#endif
#ifdef Arm_Kaiteli
    addSelectStringToMap(&ATPARA.CasTyP, QStringList() << QObject::tr("标准平台") << QObject::tr("气动平台") << QObject::tr("行星齿轮"));
    addSelectStringToMap(&ATPARA.ShakeMode, QStringList() << QObject::tr("关闭") << QObject::tr("上电开启") << QObject::tr("运动开启"));
    addSelectStringToMap(&ATPARA.ColorMode, QStringList()<<QObject::tr("普通换色")<<QObject::tr("电眼换色"));
    addSelectStringToMap(&ATPARA.bColor2, QStringList()<<QObject::tr("换色")<<QObject::tr("抖毛")<<QObject::tr("换色抖毛"));
    addSelectStringToMap(&ATPARA.AutoSaveFile, strYesNo);
    addSelectStringToMap(&ATPARA.HColorStop, strYesNo);
    addSelectStringToMap(&ATPARA.bKDecSpeed, strYesNo);
    addSelectStringToMap(&ATPARA.JmpDecSpeed, strYesNo);
    addSelectStringToMap(&ATPARA.HStopSpeed, strYesNo);
    addSelectStringToMap(&ATPARA.bJianban, strYesNo);
    addSelectStringToMap(&ATPARA.bLastSpeed, strYesNo);
    addSelectStringToMap(&ATPARA.FixtureMode, QStringList()<<QObject::tr("单次")<<QObject::tr("多次"));
    addSelectStringToMap(&ATPARA.FixtureConfig, QStringList()<<QObject::tr("禁止")<<QObject::tr("允许"));
    addSelectStringToMap(&ATPARA.LooseType, QStringList()<<QObject::tr("上电开始")<<QObject::tr("运行开启"));
    addSelectStringToMap(&ATPARA.bTGH, strYesNo);
    addSelectStringToMap(&ATPARA.SpecBrush, strYesNo);
    addSelectStringToMap(&ATPARA.GongWeiSelect, QStringList()<<QObject::tr("单工位")<<QObject::tr("双工位"));
    addSelectStringToMap(&ATPARA.bGoHomeMode, QStringList()<<QObject::tr("同时归零")<<QObject::tr("顺序归零"));
    addSelectStringToMap(&ATPARA.LorR_Work, QStringList()<<QObject::tr("左机头")<<QObject::tr("右机头"));
    addSelectStringToMap(&ATPARA.AfirstBEndSw, strYesNo);
    addSelectStringToMap(&ATPARA.AfirstBEnd, strYesNo);
    addSelectStringToMap(&ATPARA.bMainView, strYesNo);
    addSelectStringToMap(&ATPARA.bCloseClampAuto, strYesNo);
    addSelectStringToMap(&ATPARA.DbEna, strYesNo);
#else
    addSelectStringToMap(&ATPARA.bMetalCheck, QStringList() << QObject::tr("关闭") << QObject::tr("开启"));
#endif

    addSelectStringToMap(&ATPARA.IOBlow, QStringList() << QObject::tr("关闭") << QObject::tr("开启"));
#endif
    // 设定连续几个为特定的WidgetType
    auto setWidgetTypeForItems = [&](void* pData, ParamWidgetType type, int interval, int axisCount)
    {
        auto startIndex = mapDataToIndex[pData];

        for(auto i = 0; i < axisCount; ++i)
        {
            auto index = startIndex + i * interval;
            ParaRegTab_AxisMove[index].paramWidgetType = type;
        }
    };

    // 零点设置
#ifdef Arm_460Y
    setWidgetTypeForItems(&AxParam.HOMEConf, WidgetType_HOME_CONFIG, 1, GlobalData::systemaxis);

    //设置机械偏移
    ParaRegTab_AxisMove[mapDataToIndex[&AxParam.ZPOS_OFFSET]].paramWidgetType = WidgetType_WIRTE_ANYTHING;

    //旋转设置点
    ParaRegTab_AxisMove[mapDataToIndex[&SYS_AXIS[Ay].ZPOS_OFFSET]].paramWidgetType = WidgetType_WIRTE_ANYTHING;

    ParaRegTab_AxisMove[mapDataToIndex[&AzParam.ZPOS_OFFSET]].paramWidgetType = WidgetType_WIRTE_ANYTHING;
#endif

    // 限位设置
    //setWidgetTypeForItems(&AxParam.LIMITConf, WidgetType_LIMIT_CONFIG, 1, PARAM_TABLE_AXIS_NUM);

    // 零点设置
    setWidgetTypeForItems(&AxParam.HOMEConf, WidgetType_HOME_CONFIG, 1, GlobalData::systemaxis);

    // 归零配置
    ParaRegTab_AxisMove[mapDataToIndex[&g_Sysparam.HomeModeBit]].paramWidgetType = WidgetType_ORIGIN_ORDER;
    ParaRegTab_AxisMove[mapDataToIndex[&g_Sysparam.StartModeBit]].paramWidgetType = WidgetType_ORIGIN_ORDER;

    // 密码
    ParaRegTab_AxisMove[mapDataToIndex[&PASSWFLAG]].paramWidgetType = WidgetType_Authority;
    ParaRegTab_AxisMove[mapDataToIndex[&g_Sysparam.SuperPassw]].paramWidgetType = WidgetType_PASSWORD;
    ParaRegTab_AxisMove[mapDataToIndex[&g_Sysparam.ProgrammerPassw]].paramWidgetType = WidgetType_PASSWORD;
    ParaRegTab_AxisMove[mapDataToIndex[&g_Sysparam.OperationPassw]].paramWidgetType = WidgetType_PASSWORD;

    // 系统轴显示
    ParaRegTab_AxisMove[mapDataToIndex[&g_Sysparam.AXISSHOWSMAP]].paramWidgetType = WidgetType_AXIS_SWITCH;
    ParaRegTab_AxisMove[mapDataToIndex[&g_Sysparam.AXISTeachMap]].paramWidgetType = WidgetType_AXIS_SWITCH;
    ParaRegTab_AxisMove[mapDataToIndex[&ATPARA.bAxisMoveZero]].paramWidgetType = WidgetType_AXIS_SWITCH;

    ParaRegTab_AxisMove[mapDataToIndex[&ATPARA.iAxis]].paramWidgetType = WidgetType_QLaber;
}

//轴轴数 参数序号 参数名
bool axisRemove(int axis,QString str)
{//用来限制显示的轴 只显示需要的轴
    switch (axis) {
    case AxisNum::AXIS_2:
        if(str.contains(",Z",Qt::CaseSensitive)||
                str.contains(",A",Qt::CaseSensitive)||
                str.contains(",B",Qt::CaseSensitive)||
                str.contains(",C",Qt::CaseSensitive))
        {
            //qDebug()<<"ParaRegTab"<<num<<str;
            //            qDebug()<<"ParaRegTab"<<num<<ParaRegTab[num].pDescText;
            return true;
        }
        else
            return false;
        break;
    case AxisNum::AXIS_3:
        if(str.contains(",A",Qt::CaseSensitive)||
                str.contains(",B",Qt::CaseSensitive)||
                str.contains(",C",Qt::CaseSensitive))
        {
            //qDebug()<<"ParaRegTab"<<num<<str;
            //            qDebug()<<"ParaRegTab"<<num<<ParaRegTab[num].pDescText;

            return true;
        }
        else
            return false;
        break;
    case AxisNum::AXIS_4:
        if(str.contains(",B",Qt::CaseSensitive)||
                str.contains(",C",Qt::CaseSensitive)||
                str.contains(",A7",Qt::CaseSensitive)||
                str.contains(",A8",Qt::CaseSensitive)||
                str.contains(",A9",Qt::CaseSensitive)||
                str.contains(",A10",Qt::CaseSensitive)||
                str.contains(",A11",Qt::CaseSensitive)||
                str.contains(",A12",Qt::CaseSensitive)||
                str.contains(",A13",Qt::CaseSensitive)||
                str.contains(",A14",Qt::CaseSensitive)||
                str.contains(",A15",Qt::CaseSensitive)||
                str.contains(",A16",Qt::CaseSensitive))
        {
            //qDebug()<<"ParaRegTab"<<num<<str;
            //            qDebug()<<"ParaRegTab"<<num<<ParaRegTab[num].pDescText;
            return true;
        }
        else
            return false;
        break;
    case AxisNum::AXIS_5:
        if(str.contains(",C",Qt::CaseSensitive) ||
                str.contains(",A7",Qt::CaseSensitive)||
                str.contains(",A8",Qt::CaseSensitive)||
                str.contains(",A9",Qt::CaseSensitive)||
                str.contains(",A10",Qt::CaseSensitive)||
                str.contains(",A11",Qt::CaseSensitive)||
                str.contains(",A12",Qt::CaseSensitive)||
                str.contains(",A13",Qt::CaseSensitive)||
                str.contains(",A14",Qt::CaseSensitive)||
                str.contains(",A15",Qt::CaseSensitive)||
                str.contains(",A16",Qt::CaseSensitive))
        {
            //qDebug()<<"ParaRegTab"<<num<<str;
            //            qDebug()<<"ParaRegTab"<<num<<ParaRegTab[num].pDescText;
            return true;
        }
        else
            return false;
        break;
    case AxisNum::AXIS_6:
        if( str.contains(",A7",Qt::CaseSensitive)||
                str.contains(",A8",Qt::CaseSensitive)||
                str.contains(",A9",Qt::CaseSensitive)||
                str.contains(",A10",Qt::CaseSensitive)||
                str.contains(",A11",Qt::CaseSensitive)||
                str.contains(",A12",Qt::CaseSensitive)||
                str.contains(",A13",Qt::CaseSensitive)||
                str.contains(",A14",Qt::CaseSensitive)||
                str.contains(",A15",Qt::CaseSensitive)||
                str.contains(",A16",Qt::CaseSensitive))
        {
            return true;
        }
        else
            return false;
        break;
    case AxisNum::AXIS_7:
        if( str.contains(",A8",Qt::CaseSensitive)||
                str.contains(",A9",Qt::CaseSensitive)||
                str.contains(",A10",Qt::CaseSensitive)||
                str.contains(",A11",Qt::CaseSensitive)||
                str.contains(",A12",Qt::CaseSensitive)||
                str.contains(",A13",Qt::CaseSensitive)||
                str.contains(",A14",Qt::CaseSensitive)||
                str.contains(",A15",Qt::CaseSensitive)||
                str.contains(",A16",Qt::CaseSensitive))
        {
            return true;
        }
        else
            return false;
        break;
    case AxisNum::AXIS_8:
        if( str.contains(",A9",Qt::CaseSensitive)||
                str.contains(",A10",Qt::CaseSensitive)||
                str.contains(",A11",Qt::CaseSensitive)||
                str.contains(",A12",Qt::CaseSensitive)||
                str.contains(",A13",Qt::CaseSensitive)||
                str.contains(",A14",Qt::CaseSensitive)||
                str.contains(",A15",Qt::CaseSensitive)||
                str.contains(",A16",Qt::CaseSensitive))
        {
            return true;
        }
        else
            return false;
        break;
    case AxisNum::AXIS_9 :
    {
        if(  str.contains(",A10",Qt::CaseSensitive)||
             str.contains(",A11",Qt::CaseSensitive)||
             str.contains(",A12",Qt::CaseSensitive)||
             str.contains(",A13",Qt::CaseSensitive)||
             str.contains(",A14",Qt::CaseSensitive)||
             str.contains(",A15",Qt::CaseSensitive)||
             str.contains(",A16",Qt::CaseSensitive))
        {
            return true;
        }
        else
            return false;
        break;
    }
    case AxisNum::AXIS_12 :
    {
        if(  str.contains(",A13",Qt::CaseSensitive)||
             str.contains(",A14",Qt::CaseSensitive)||
             str.contains(",A15",Qt::CaseSensitive)||
             str.contains(",A16",Qt::CaseSensitive))
        {
            return true;
        }
        else
            return false;
        break;
    }
    case AxisNum::AXIS_16 :
    {
        break;
    }
    default:
        break;
    }
    return false;
}

QVariant DataToQVariant(void* data, DataType dataType)
{
    QVariant val;

    switch(dataType)
    {
    case TYPE_U8:
        val = QVariant(*reinterpret_cast<unsigned char*>(data));
        break;

    case TYPE_U16:
        val = QVariant(*reinterpret_cast<unsigned short*>(data));
        break;

    case TYPE_U32:
        val = QVariant(*reinterpret_cast<unsigned int*>(data));
        break;

    case TYPE_S8:
        val = QVariant(*reinterpret_cast<char*>(data));
        break;

    case TYPE_S16:
        val = QVariant(*reinterpret_cast<short*>(data));
        break;

    case TYPE_S32:
        val = QVariant(*reinterpret_cast<int*>(data));
        break;

    case TYPE_F32:
        val = QVariant(*reinterpret_cast<float*>(data));
        break;

    case TYPE_F64:
        val = QVariant(*reinterpret_cast<double*>(data));
        break;

    case TYPE_String:
        val = QVariant(*reinterpret_cast<QString*>(data));
        break;

    case TYPE_StringList:
        val = QVariant(*reinterpret_cast<QStringList*>(data));
        break;

    default:
        break;
    }

    return val;
}

void QVariantToData(const QVariant& val, void* data, DataType dataType)
{
    switch(dataType)
    {
    case TYPE_U8:
        *reinterpret_cast<unsigned char*>(data) = static_cast<unsigned char>(val.toUInt());
        break;

    case TYPE_U16:
        *reinterpret_cast<unsigned short*>(data) = static_cast<unsigned short>(val.toUInt());
        break;

    case TYPE_U32:
        *reinterpret_cast<unsigned int*>(data) = val.toUInt();
        break;

    case TYPE_S8:
        *reinterpret_cast<char*>(data) = static_cast<char>(val.toInt());
        break;

    case TYPE_S16:
        *reinterpret_cast<short*>(data) = static_cast<short>(val.toInt());
        break;

    case TYPE_S32:
        *reinterpret_cast<int*>(data) = val.toInt();
        break;

    case TYPE_F32:
        *reinterpret_cast<float*>(data) = val.toFloat();
        break;

    case TYPE_F64:
        *reinterpret_cast<double*>(data) = val.toDouble();
        break;

    case TYPE_String:
        *reinterpret_cast<QString*>(data) = val.toString();
        break;

    case TYPE_StringList:
        *reinterpret_cast<QStringList*>(data) = val.toStringList();
        break;

    default:
        break;
    }
}

DataAdapter_ParamTable::DataAdapter_ParamTable(TParamTable* baseAddr):
    DataAdapterBase<TParamTable>(baseAddr)
{

}

//add by yzg
DataAdapter_ParamTable::DataAdapter_ParamTable(TParamTable* baseAddr,int startnameindex):
    DataAdapterBase<TParamTable>(baseAddr,startnameindex)
{

}

//mod by yzg
QString DataAdapter_ParamTable::getParamName(int offset) const
{
    //return Utils::gbkCharToQString((_baseAddr + offset)->pDescText);
    //return Utils::utf8CharToQString((_baseAddr + offset)->pDescText);
    return ParaName[nameIndexStart+offset];//add by yzg
}

int DataAdapter_ParamTable::getParamWidgetType(int offset) const
{
    return (_baseAddr + offset)->paramWidgetType;
}

int DataAdapter_ParamTable::getDataType(int offset) const
{
    return (_baseAddr + offset)->DataType;
}

bool DataAdapter_ParamTable::getDataAuthority(int offset) const
{//权限判断函数
    //读取二进制第一位
    bool authority = READ_BIT((_baseAddr + offset)->Attribute,0);
    //qDebug()<<"authority disply"<<QByteArray::number((_baseAddr + offset)->Attribute, 2)<<authority;
    return authority;
}

void DataAdapter_ParamTable::setData(int offset, const QVariant& val)
{
    void* pData = (_baseAddr + offset)->pData;
    auto dataType = static_cast<DataType>((_baseAddr + offset)->DataType);
    QVariantToData(val, pData, dataType);
}

QVariant DataAdapter_ParamTable::getData(int offset)
{
    void* pData = (_baseAddr + offset)->pData;
    auto dataType = static_cast<DataType>((_baseAddr + offset)->DataType);
    return DataToQVariant(pData, dataType);
}

size_t DataAdapter_ParamTable::getDataTypeSize(int offset) const
{
    auto dt = static_cast<size_t>(getDataType(offset));
    return (dt >> 4) & 0xf;
}

void* DataAdapter_ParamTable::getDataPointer(int offset) const
{
    return (_baseAddr + offset)->pData;
}

unsigned short DataAdapter_ParamTable::getDataModbusAddress(int offset) const
{
    return (_baseAddr + offset)->PlcAddr;
}

void initIOName()
{
    qDebug()<<"initIOName";
    //    auto setFunctionName = [&](IO_FUN_NAME * startAddr, const QString & suffix)
    //    {
    //        for(auto i = 0; i < IO_NAME_INTERVAL; ++i)
    //        {
    //            auto fullText = GlobalData::axisName.at(i) + suffix;
    //            auto p = startAddr + i;
    //            Utils::qStringToGbkChar(fullText, p->name, sizeof(p->name));
    //        }
    //    };
    //初始化IO名称数组
    for(auto j = 0; j < MAX_INPUT_FUN; ++j)
    {
        IOFunName_I.append(QString(""));
    }

    for(auto j = 0; j < MAX_OUTPUT_FUN; ++j)
    {
        IOFunName_O.append(QString(""));
    }


    auto setFunctionName = [&](IO_FUN_NAME * startAddr, const QString & suffix,INT16U index,bool io,int num=IO_NAME_INTERVAL)
    {
        if(!io)//输入
        {
            for(auto i = 0; i < num; ++i)
            {
                auto fullText = GlobalData::axisName.at(i) + suffix;
                //auto p = startAddr + i;
                //Utils::qStringToGbkChar(fullText, p->name, sizeof(p->name));
                IOFunName_I[index+i]=fullText;
            }
        }
        else//输出
        {
            for(auto i = 0; i < num; ++i)
            {
                auto fullText = GlobalData::axisName.at(i) + suffix;
                //auto p = startAddr + i;
                //Utils::qStringToGbkChar(fullText, p->name, sizeof(p->name));
                IOFunName_O[index+i]=fullText;
                qDebug()<<"fullText"<<fullText;;
            }
        }
    };
#ifdef Arm_Kaiteli

    // IN
    for(auto i = 0; i < IN_KAITELI_END;i++)
    {
        switch(Input_Fun_Name[i].fun)
        {
        case X_HOME:
            //setFunctionName(&Input_Fun_Name[i], QObject::tr("轴原点"));
            setFunctionName(&Input_Fun_Name[i], QObject::tr("轴原点"),Input_Fun_Name[i].fun,0);
            i += IO_NAME_INTERVAL-1;
            break;

        case X_P_LIMIT:
            //setFunctionName(&Input_Fun_Name[i], QObject::tr("正限位"));
            setFunctionName(&Input_Fun_Name[i], QObject::tr("正限位"),Input_Fun_Name[i].fun,0);
            i += IO_NAME_INTERVAL-1;
            break;

        case X_M_LIMIT:
            //setFunctionName(&Input_Fun_Name[i], QObject::tr("负限位"));
            setFunctionName(&Input_Fun_Name[i], QObject::tr("负限位"),Input_Fun_Name[i].fun,0);
            i += IO_NAME_INTERVAL-1;
            break;

        case X_SERVO_ALARM:
            //setFunctionName(&Input_Fun_Name[i], QObject::tr("伺服报警"));
            setFunctionName(&Input_Fun_Name[i], QObject::tr("伺服报警"),Input_Fun_Name[i].fun,0);
            i += IO_NAME_INTERVAL-1;
            break;

        default:
            IOFunName_I[Input_Fun_Name[i].fun]=QCoreApplication::translate("IO_FUN_NAME",Input_Fun_Name[i].name);
            //qDebug()<<IOFunName_I[Input_Fun_Name[i].fun];
            break;
        }
    }

    // OUT kaiteli
    for(auto i = 0; i < OUT_KAITELI_END;i++)
    {
        switch(Output_Fun_Name[i].fun)
        {
        case OUT_X_SERVO_CLEAR:
            //setFunctionName(&Output_Fun_Name[i], QObject::tr("伺服报警复位"));
            setFunctionName(&Output_Fun_Name[i], QObject::tr("报警复位"),Output_Fun_Name[i].fun,1);
            i += IO_NAME_INTERVAL-1;
            break;

        case OUT_X_SERVO_ON:
            //setFunctionName(&Output_Fun_Name[i], QObject::tr("伺服报警使能位"));
            setFunctionName(&Output_Fun_Name[i], QObject::tr("伺服使能"),Output_Fun_Name[i].fun,1);
            i += IO_NAME_INTERVAL-1;
            break;

        default:
            IOFunName_O[Output_Fun_Name[i].fun]=QCoreApplication::translate("IO_FUN_NAME",Output_Fun_Name[i].name);
            //qDebug()<<IOFunName_I[Output_Fun_Name[i].fun];
            break;
        }
    }
#else
    // IN
    for(auto i = 0; i < MAX_INPUT_FUN;i++)
    {
        switch(Input_Fun_Name[i].fun)
        {
        case X_HOME:
            //setFunctionName(&Input_Fun_Name[i], QObject::tr("轴原点"));
            setFunctionName(&Input_Fun_Name[i], QObject::tr("轴原点"),Input_Fun_Name[i].fun,0);
            i += IO_NAME_INTERVAL-1;
            break;

        case X_P_LIMIT:
            //setFunctionName(&Input_Fun_Name[i], QObject::tr("正限位"));
            setFunctionName(&Input_Fun_Name[i], QObject::tr("正限位"),Input_Fun_Name[i].fun,0);
            i += IO_NAME_INTERVAL-1;
            break;

        case X_M_LIMIT:
            //setFunctionName(&Input_Fun_Name[i], QObject::tr("负限位"));
            setFunctionName(&Input_Fun_Name[i], QObject::tr("负限位"),Input_Fun_Name[i].fun,0);
            i += IO_NAME_INTERVAL-1;
            break;

        case X_SERVO_ALARM:
            //setFunctionName(&Input_Fun_Name[i], QObject::tr("伺服报警"));
            setFunctionName(&Input_Fun_Name[i], QObject::tr("伺服报警"),Input_Fun_Name[i].fun,0);
            i += IO_NAME_INTERVAL-1;
            break;

        default:
            IOFunName_I[Input_Fun_Name[i].fun]=QCoreApplication::translate("IO_FUN_NAME",Input_Fun_Name[i].name);
            //qDebug()<<IOFunName_I[Input_Fun_Name[i].fun];
            break;
        }
    }

    #define MAINAXIS    6
    // OUT
    for(auto i = 0; i < MAX_OUTPUT_FUN;i++)
    {
        switch(Output_Fun_Name[i].fun)
        {
        case OUT_X_SERVO_CLEAR:
            //setFunctionName(&Output_Fun_Name[i], QObject::tr("伺服报警复位"));
            setFunctionName(&Output_Fun_Name[i], QObject::tr("报警复位"),Output_Fun_Name[i].fun,1,MAINAXIS);
            i += MAINAXIS-1;
            break;

        case OUT_X_SERVO_ON:
            //setFunctionName(&Output_Fun_Name[i], QObject::tr("伺服报警使能位"));
            setFunctionName(&Output_Fun_Name[i], QObject::tr("伺服使能"),Output_Fun_Name[i].fun,1,MAINAXIS);
            i += MAINAXIS-1;
            break;

        default:
            IOFunName_O[Output_Fun_Name[i].fun]=QCoreApplication::translate("IO_FUN_NAME",Output_Fun_Name[i].name);
            break;
        }
    }
#endif
}

DataAdapter_IOConfigText::DataAdapter_IOConfigText(IO_FUN_NAME* funcNameBaseAddr, IOConfig* ioConfigBaseAddr, quint16 ioConfigModbusAddr):
    DataAdapterBase<IO_FUN_NAME>(funcNameBaseAddr),
    ioConfigAddr(ioConfigBaseAddr),
    modbusAddr(ioConfigModbusAddr)
{
}

QString DataAdapter_IOConfigText::getParamName(int offset) const
{
    //return Utils::gbkCharToQString((_baseAddr + offset)->name);

    //mod by yzg
    if(_baseAddr==Input_Fun_Name)
    {
        return IOFunName_I[(_baseAddr + offset)->fun];
    }
    else
        if(_baseAddr==Output_Fun_Name)
        {
            return IOFunName_O[(_baseAddr + offset)->fun];
        }
        else
        {
            return QString("");
            //return Utils::gbkCharToQString((_baseAddr + offset)->name);
        }
}

int DataAdapter_IOConfigText::getParamWidgetType(int offset) const
{
    Q_UNUSED(offset)
    return WidgetType_IO_CONFIG;
}

int DataAdapter_IOConfigText::getDataType(int offset) const
{
    Q_UNUSED(offset)
    return TYPE_UNKNOWN;
}

bool DataAdapter_IOConfigText::getDataAuthority(int offset) const
{
    return false;
}

void DataAdapter_IOConfigText::setData(int offset, const QVariant& val)
{
    auto functionCode = (_baseAddr + offset)->fun;
    auto ioConfig = val.value<IOConfig>();
    *(ioConfigAddr + functionCode) = ioConfig;
}

QVariant DataAdapter_IOConfigText::getData(int offset)
{
    auto functionCode = (_baseAddr + offset)->fun;
    QVariant v;
    v.setValue(*(ioConfigAddr + functionCode));
    return v;
}

DataAdapter_IOStatus::DataAdapter_IOStatus(IOConfig* ioConfigBaseAddr, IO_FUN_NAME* funcNameAddr, int funcNameCount):
    DataAdapterBase<IOConfig>(ioConfigBaseAddr),
    funcNameAddr(funcNameAddr),
    funcNameCount(funcNameCount)
{
    for(auto i = 0; i < funcNameCount; ++i)
    {
        auto funcCode = (funcNameAddr + i)->fun; // funcCode是IO_Config下标

        int ioNum = static_cast<int>((ioConfigBaseAddr + funcCode)->IONum);

        if(255 == ioNum) // 未配置该功能
            continue;

        // 实际IONum 映射到 功能文本index
        if(mapIONumToFuncTextIndex.find(ioNum) == mapIONumToFuncTextIndex.end())
        {
            QList<int> list = {i};
            mapIONumToFuncTextIndex.insert(ioNum, list);
        }
        else
        {
            mapIONumToFuncTextIndex[ioNum].append(i);
        }
    }
}

QString DataAdapter_IOStatus::getParamName(int offset) const
{
    return QString("%1").arg(offset);
}

int DataAdapter_IOStatus::getParamWidgetType(int offset) const
{
    Q_UNUSED(offset)
    return WidgetType_IO_STATUS;
}

int DataAdapter_IOStatus::getDataType(int offset) const
{
    Q_UNUSED(offset)
    return TYPE_UNKNOWN;
}

bool DataAdapter_IOStatus::getDataAuthority(int offset) const
{

}

void DataAdapter_IOStatus::setData(int offset, const QVariant& val)
{
    Q_UNUSED(offset)
    Q_UNUSED(val)
}

QVariant DataAdapter_IOStatus::getData(int offset)
{
    Q_UNUSED(offset)
    return QVariant();
}

QString DataAdapter_IOStatus::getFunctionName(int offset) const
{
    const QString noFuncText = QObject::tr("无功能");
    QString str;
    //qDebug()<<g_Sysparam.BootDelay;
    if(GlobalData::iodisplay)//用来调试显示IO端口号
    {
        str = QString("(%1)").arg(offset);
    }
    else
    {
        str = QString("");
    }

    if(mapIONumToFuncTextIndex.find(offset) != mapIONumToFuncTextIndex.end())
    {
        // 将所有功能号追加到QString
        auto funcTextIndexList = mapIONumToFuncTextIndex.value(offset);
        QString funcText = "";
        for(auto i = 0; i < funcTextIndexList.size(); ++i)
        {
            auto funcTextIndex = funcTextIndexList.at(i);
            //funcText += Utils::gbkCharToQString((funcNameAddr + funcTextIndex)->name);
            if(funcNameAddr==Input_Fun_Name)
            {

                funcText += str+IOFunName_I[(funcNameAddr + funcTextIndex)->fun];
            }
            else if(funcNameAddr==Output_Fun_Name)
            {
                funcText += str+IOFunName_O[(funcNameAddr + funcTextIndex)->fun];
            }

            if(i < funcTextIndexList.size() - 1)
                funcText += "/";
        }

        return funcText;
    }

    return noFuncText;
}

const ParamTableSectionType getParamTableSection(int sectionIndex)
{
    return paramTableSections.at(sectionIndex);
}

int getParamTableSectionCount()
{
    return paramTableSections.size();
}

const QStringList getParamTableDropdownList(void* pData)
{
    if(mapSelectionTextParams.find(pData) == mapSelectionTextParams.end())
        return QStringList() << QObject::tr("暂无选项");

    return mapSelectionTextParams[pData];
}

void initAlarmMessage()
{
    qDebug()<<"initAlarmMessage";
    for(auto i = 0; i < MAX_ALARM_NUM; ++i)
    {
        mapAlarmNumberToTextIndex[i] = -1; // default value is an invalid number
        AlarmName.append(QString(""));
    }

    // map the indexes add by yzg
    for(auto i = 0; i < MAX_ALARM_NUM; ++i) // i 为数组Alarm下标
    {
        auto alarmNumber = static_cast<int>(Alarm[i].fun);

        if(-1 == mapAlarmNumberToTextIndex[alarmNumber])
            mapAlarmNumberToTextIndex[alarmNumber] = i;
    }

    //#define ALARM_NAME_INTERVAL 6

    auto setAlarmName = [&](ALARM_FUN_NAME * startAddr, int interval, const QString & suffix)
    {
        for(auto i = 0; i < ALARM_NAME_INTERVAL; ++i)
        {
            auto fullText = GlobalData::axisName.at(i) + suffix;
            auto p = startAddr + i * interval;
            //Utils::qStringToGbkChar(fullText, p->name, sizeof(p->name));
            AlarmName[mapAlarmNumberToTextIndex[p->fun]]=fullText;
        }
    };

    // prefix names
    for(auto i = 0; i < MAX_ALARM_NUM; ++i)
    {//交叉排列
        switch(Alarm[i].fun)
        {
        case X_P_SoftLimit_Alarm:
            setAlarmName(&Alarm[i], 2, QObject::tr("正软件限位报警"));
            break;

        case X_M_SoftLimit_Alarm:
            setAlarmName(&Alarm[i], 2, QObject::tr("负软件限位报警"));
            break;

        case X_P_HardLimit_Alarm:
            setAlarmName(&Alarm[i], 2, QObject::tr("正硬件限位报警"));
            break;

        case X_M_HardLimit_Alarm:
            setAlarmName(&Alarm[i], 2, QObject::tr("负硬件限位报警"));
            break;

        case X_Servo_Alarm:
            setAlarmName(&Alarm[i], 1, QObject::tr("伺服报警"));
            break;

        default://add by yzg
            //解决报警号显示问题
            if(Alarm[i].fun<=C_Servo_Alarm)
                break;
            AlarmName[mapAlarmNumberToTextIndex[Alarm[i].fun]]=QCoreApplication::translate("AlarmTab",Alarm[i].name);
            break;
        }
    }
    //输出报警号
    //    for(auto i = 0; i < MAX_ALARM_NUM; ++i)
    //    {
    //        qDebug()<<"报警号"<<i<<AlarmName[mapAlarmNumberToTextIndex[Alarm[i].fun]];
    //    }
    //    // map the indexes
    //    for(auto i = 0; i < MAX_ALARM_NUM; ++i) // i 为数组Alarm下标
    //    {
    //        auto alarmNumber = static_cast<int>(Alarm[i].fun);

    //        if(-1 == mapAlarmNumberToTextIndex[alarmNumber])
    //            mapAlarmNumberToTextIndex[alarmNumber] = i;
    //    }
}

const QString getAlarmMessage(int alarmNumber)
{
    static const QString wrongMsg = QObject::tr("未知报警号 ");

    if(alarmNumber < 0 || alarmNumber >= MAX_ALARM_NUM)
        return wrongMsg + QString("? %1").arg(alarmNumber) + QString(">= %1").arg(MAX_ALARM_NUM);

    int alramMessageIndex = mapAlarmNumberToTextIndex[alarmNumber];

    if(alramMessageIndex < 0)
        return wrongMsg + QString("?? %1").arg(alarmNumber);

    //return Utils::gbkCharToQString(Alarm[alramMessageIndex].name);//mod by yzg
    return AlarmName[alramMessageIndex];
}

int getAlarmLevel(int alarmNumber)
{
    static const int defaultLevel = 2; // 紧急报警

    if(alarmNumber < 0 || alarmNumber >= MAX_ALARM_NUM)
        return defaultLevel;

    int alramMessageIndex = mapAlarmNumberToTextIndex[alarmNumber];

    if(alramMessageIndex < 0)
        return defaultLevel;

    return static_cast<int>(Alarm[alramMessageIndex].level);
}

bool readParamTable(ModbusQueue* mbq, bool isAllowResume)
{
#ifndef DEBUG_OFFLINE
    static int lastReadIndex = 0; // 断点续传
    bool isOk = true;
    int index = 0;
    ModbusQueue::element_t element;

    DataAdapter_ParamTable adapter(ParaRegTab);

    if(true == isAllowResume)
        index = lastReadIndex;

    // read now!
    for(/*index=0*/; adapter.getDataType(index) != DT_END; ++index)
    {
        //qDebug("reading index=%d",index);

        switch(adapter.getDataType(index))
        {
        case DT_INT8U:
        case DT_INT8S:
        case DT_INT16U:
        case DT_INT16S:
        case DT_INT32U:
        case DT_INT32S:
        case DT_FLOAT:
        case DT_DOUBLE:
            element.data = adapter.getDataPointer(index);
            element.len = static_cast<unsigned short>(adapter.getDataTypeSize(index));
            element.startAddress = adapter.getDataModbusAddress(index);
            element.operation = ModbusQueue::OP_READ_DATA;
            isOk = mbq->sendQuery(&element, false);

            break;

        default:
            break;
        }

        if(false == isOk)
        {
            qDebug()<<"err startAddress"<<element.startAddress;
            break; // for loop
        }
    }

    if(true == isAllowResume)
        lastReadIndex = index;

    return isOk;
#else
    g_Sysparam.AXISSHOWSMAP = 0x3f; // 0x0011 1111
    return true;
#endif // DEBUG_OFFFLINE
}

bool readIOConfig(ModbusQueue* mbq)
{
#ifndef DEBUG_OFFLINE
    bool isOk = true;

    auto readArray = [&](IOConfig * baseAddr, size_t size, unsigned short modbusAddr)
    {
        ModbusQueue::element_t element;

        element.data = baseAddr;
        element.len = static_cast<unsigned short>(size);
        element.operation = ModbusQueue::OP_READ_DATA;
        element.startAddress = modbusAddr;

        return mbq->sendQuery(&element, false);
    };

    isOk = readArray(g_Sysparam.InConfig, sizeof(g_Sysparam.InConfig), ModbusAddress::InConfig) &&
            readArray(g_Sysparam.OutConfig, sizeof(g_Sysparam.OutConfig), ModbusAddress::OutConfig);

    return isOk;
#else

    // IN
    for(auto i = 0; i < MAX_INPUT_NUM; ++i)
        g_Sysparam.InConfig[i].IONum = 255;

    for(auto i = 0; i < MAX_INPUT_FUN; ++i)
    {
        auto funcCode = Input_Fun_Name[i].fun;
        auto IONum = Input_Fun_Name[i].idef;
        g_Sysparam.InConfig[funcCode].IONum = IONum;
    }

    // OUT
    for(auto i = 0; i < MAX_OUTPUT_NUM; ++i)
        g_Sysparam.OutConfig[i].IONum = 255;

    for(auto i = 0; i < MAX_OUTPUT_FUN; ++i)
    {
        auto funcCode = Output_Fun_Name[i].fun;
        auto IONum = Output_Fun_Name[i].idef;
        g_Sysparam.OutConfig[funcCode].IONum = IONum;
    }

    return true;
#endif
}

void initGlobalData()
{
#ifdef DEBUG_OFFLINE
    GlobalData::modbusQueue->queryStopThenOffline();
    GlobalData::realTimeInfo.iCNum = 0;
    GlobalData::realTimeInfo.iMNum = 8;
#endif
    int k=0;
    if(g_Sysparam.AXISSHOWSMAP!=0)
    {
        GlobalData::realaxisName.clear();
    }
    for(auto axisCount = 0; axisCount < TotalAxis; ++axisCount)
    {
        //AXISTeachMap  // AXISSHOWSMAP
        if(g_Sysparam.AXISSHOWSMAP & (0x01 << axisCount))//1左移axisCount位
        {
            GlobalData::realaxisName.insert(k,GlobalData::axisName.at(axisCount));//得到实际轴号
            k++;
            GlobalData::totalAxisNum++; // 实际有效的轴数
            //qDebug()<<axisCount<<k<<GlobalData::axisName.at(axisCount)<<GlobalData::axis[k-1]<<GlobalData::realaxisName.at(k-1);
            if(g_Sysparam.AXISTeachMap & (0x01 << axisCount))
            {
                GlobalData::teachaxisName.insert(GlobalData::teachdisplay,GlobalData::axisName.at(axisCount));//得到实际轴号
                GlobalData::teachaxis[GlobalData::teachdisplay]=axisCount;
                GlobalData::teachdisplay++;
                //qDebug()<<"display in teach"<<k-1<<GlobalData::teachaxisName[k-1];
            }
        }

        //qDebug()<<(1 << axisCount);
    }
    //    GlobalData::maxAxisNum = TotalAxis; // 最大轴数
}

const QString getWorkstatusText(int workstatus)
{
    QString returnStr = QString();

    switch(workstatus)
    {
    case Stop:
        returnStr = QObject::tr("停止");
        break;

    case Run:
        returnStr = QObject::tr("运行");
        break;

    case Pause:
        returnStr = QObject::tr("暂停");
        break;

    case Step_Pause:
        returnStr = QObject::tr("单节暂停");
        break;

    case GoZero:
        returnStr = QObject::tr("回零");
        break;

    case Manual:
        returnStr = QObject::tr("手动");
        break;

    case USB_Link:
        returnStr = QObject::tr("USB连接");
        break;

    case St_GoTo_N:
        returnStr = QObject::tr("定位移动");
        break;

    case St_Move_Table:
        returnStr = QObject::tr("工作台移动");
        break;

    case St_Zero_Table:
        returnStr = QObject::tr("工作台归零");
        break;

    case St_Zero_MainAxis:
        returnStr = QObject::tr("主轴归零");
        break;

    case St_GoStart:
        returnStr = QObject::tr("回起");
        break;

    case St_Bukong:
        returnStr = QObject::tr("补孔");
        break;

    case St_LastStation:
        returnStr = QObject::tr("上工位");
        break;

    case St_NextStation:
        returnStr = QObject::tr("下工位");
        break;

    case St_LastHole:
        returnStr = QObject::tr("上孔");
        break;

    case St_NextHole:
        returnStr = QObject::tr("下孔");
        break;

    case St_FixedHole:
        returnStr = QObject::tr("定位");
        break;

    case St_TestSpindle:
        returnStr = QObject::tr("主轴速度测试");
        break;

    case St_TestSongSi:
        returnStr = QObject::tr("送丝测试");
        break;

    default:
        break;
    }

    return QString() == returnStr ? "??" : returnStr;
}

int getRealIOFromFunctionCode(int func, bool isInput)
{
    if(isInput)
    {
        // 输入
        if(func < 0 || func >= MAX_INPUT_FUN)
            return -1;
        else
            return g_Sysparam.InConfig[func].IONum;
    }

    // 输出
    if(func < 0 || func >= MAX_OUTPUT_FUN)
        return -1;
    else
        return g_Sysparam.OutConfig[func].IONum;
}

}
