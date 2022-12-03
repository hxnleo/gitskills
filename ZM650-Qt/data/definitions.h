/*
 * definitions.h
 *
 * 全局数据类型定义
 *
 * Created on: 2019年 01月 21日 星期一 14:31:00 CST
 * Author: lixingcong
 */
#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <QMetaType>
#include <QMap>
#include <QList>
#include <QSignalMapper>
#include"data/adt_datatype_qt.h"
namespace Definitions
{

enum ButtonIDs
{
    // 系统
    BTN_SYS_RUN,
    BTN_SYS_STOP,
    BTN_SYS_BACKZERO,
    BTN_SYS_FINDZERO,
    BTN_SYS_BUKONG,
    BTN_SYS_LOCATE,
    BTN_SYS_PREVHOLE,
    BTN_SYS_NEXTHOLE,
    BTN_SYS_TRACEPATH,
    BTN_SYS_TRACEPATH_PREVIEW,
    BTN_SYS_TRACEPATH_FOLLOW,
    BTN_SYS_TRACEPATH_AXIS,
    BTN_SYS_TRACEPATH_ZOOM_IN,
    BTN_SYS_TRACEPATH_ZOOM_OUT,
    BTN_SYS_TRACEPATH_ZOOM_RESET,
    BTN_TEACH_SIZE,
	BTN_SYS_CONTROL,
    BTN_SYS_SETTING,
    BTN_SYS_START,

    // 教导
    BTN_TEACH_CONTROL,
    BTN_TEACH_ADD,
    BTN_TEACH_INSERT,
    BTN_TEACH_MODIFY,
    BTN_TEACH_DELETE,
    BTN_TEACH_DELETE_MULTIPLE,
    BTN_TEACH_SPINDLE_MULTIPLE,
    BTN_TEACH_VERIFY,
    BTN_TEACH_PREVHOLE,
    BTN_TEACH_NEXTHOLE,
    BTN_TEACH_LOCATE,
    BTN_TEACH_DIVIDE,
    BTN_TEACH_COPY,
    BTN_TEACH_SHIFT,
    BTN_TEACH_PAGEUP,
    BTN_TEACH_PAGEDOWN,
    BTN_TEACH_PATTERN,
    BTN_TEACH_ANGLE,
    BTN_TEACH_COLOR_MULTIPLE,
    BTN_TEACH_DELETERECT,
    BTN_TEACH_AXISOFFSET,

    // 参数
    BTN_PARAM_SYSTEXT_BEGIN,
    BTN_PARAM_SYSTEXT_END = BTN_PARAM_SYSTEXT_BEGIN + 10,
    BTN_PARAM_INCONFIG,
    BTN_PARAM_OUTCONFIG,

    // 文件
    BTN_FILE_NEW,
    BTN_FILE_COPY,
    BTN_FILE_PASTE,
    BTN_FILE_DELETE,
    BTN_FILE_OPEN,
    BTN_FILE_UPGRADE,
    BTN_FILE_COPYALL,
    BTN_UDisk_COPYALL,
    BTN_FILE_NEWFOLDER,
    BTN_FLIE_CHANG,//蓝思数据解析
    BTN_FLIE_GENERATE,//生成蓝思格式
    BTN_FLIE_GenerateCSV,
    BTN_FLIE_AnalyzeCSV,

    // 诊断
    BTN_DIAGNOSE_INPUT,
    BTN_DIAGNOSE_OUTPUT,
    BTN_DIAGNOSE_DA,
    BTN_DIAGNOSE_INFO,
    BTN_PARAM_AXISENABLE,
    BTN_DIAGNOSE_OTHERS,
    BTN_DIAGNOSE_CONNECTPC,
    BTN_DIAGNOSE_REBOOT,
    BTN_DIAGNOSE_SCREENOFF,
    BTN_DIAGNOSE_SCREENCONFIG
};

template <typename T_WIDGET, typename T_ITEM>
struct WidgetMapList
{
    QList<T_WIDGET*> widgets;
    QList<T_ITEM> items;
    QSignalMapper signalMapper;
#if 0
    // For special
    QMap<int, T_WIDGET*> mapIdToWidget;
    QMap<T_WIDGET*, int> mapWidgetToId;
#endif
};

union MixDataType
{
    unsigned char u8;
    unsigned short u16;
    unsigned int u32;

    char s8;
    short s16;
    int s32;

    float f32;
    double f64;

    void* otherData;
};

struct ModbusQueryItem
{
    int queryAction;
    MixDataType mixData;
};
// 在下位机ZYNQ工程中的rtc.h日期格式定义
typedef struct tagDATE
{
    unsigned short year;
    unsigned short month;
    unsigned short week;//for padding
    unsigned short day;
}DATE_T;

typedef struct tagTIME
{
    unsigned short hour;
    unsigned short minute;
    unsigned short second;
}TIME_T;

}


Q_DECLARE_METATYPE(Definitions::ModbusQueryItem)

#endif // DEFINITIONS_H
