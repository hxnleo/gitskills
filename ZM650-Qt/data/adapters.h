/*
 * adapters.h
 *
 * 适配器，用于下位机与上位机之间的数据读取保存
 *
 * Created on: 2019年 01月 21日 星期一 14:31:00 CST
 * Author: lixingcong
 */
#ifndef ADAPTERS_H
#define ADAPTERS_H

#include <QObject>
#include <SysText.h>
#include <QMap>
#include <QVariant>
#include <TestIO.h>
#include <Parameter.h>
#include <SysAlarm.h>
#include <QPoint>
#include <QSize>
#include <QStringList>

class ModbusQueue;

Q_DECLARE_METATYPE(IOConfig)

extern TParamTable ParaRegTab_AxisMove[1000];//用来保存实际显示参数表
namespace Adapters
{
enum ParamWidgetType
{
    WidgetType_NULL = 0,
    WidgetType_NUMBER, // 纯数字
    WidgetType_SELECT, // 下拉选择
    WidgetType_HOME_CONFIG, // 零点设置
    WidgetType_LIMIT_CONFIG, // 限位设置
    WidgetType_IO_CONFIG, // IO电平配置
    WidgetType_PASSWORD, // 密码
    WidgetType_ORIGIN_ORDER, // 归零设置+回起设置
    WidgetType_AXIS_SWITCH, // 轴开关
    WidgetType_WIRTE_ANYTHING, // 任意写东西
    WidgetType_IO_STATUS, // IO 诊断端口
    WidgetType_PATTERN, // 花样
    WidgetType_QLaber, // 不可编辑文本显示
    WidgetType_Authority, // 权限选择
};

enum DataType
{
    TYPE_UNKNOWN = 0,
    TYPE_U8 = DT_INT8U,
    TYPE_S8 = DT_INT8S,
    TYPE_U16 = DT_INT16U,
    TYPE_S16 = DT_INT16S,
    TYPE_U32 = DT_INT32U,
    TYPE_S32 = DT_INT32S,
    TYPE_F32 = DT_FLOAT,
    TYPE_F64 = DT_DOUBLE,

    TYPE_String = 0x100,
    TYPE_StringList,
};

enum AxisNum
{//轴数
    AXIS_2 = 2,
    AXIS_3,
    AXIS_4,
    AXIS_5,
    AXIS_6,
    AXIS_7,
    AXIS_8,
    AXIS_9   = 9,
    AXIS_12 = 12,
    AXIS_16 = 16,
    AXIS_END,
};

struct ParamTableSectionType
{
    QString title;
    int startIndex;
    int totalCount;
};

// 初始化
void initParamAxis();
void initParamTable();
void initIOName();
void initAlarmMessage();
void initGlobalData(); // 从下位机读完参数后才能执行
bool axisRemove(int axis, QString str);

// 读参数
bool readParamTable(ModbusQueue* mbq, bool isAllowResume = false); // isAllowResume是否断点续传
bool readIOConfig(ModbusQueue* mbq); // isAllowResume是否断点续传

// 对应Systext.c中的参数表
const ParamTableSectionType getParamTableSection(int sectionIndex);
int getParamTableSectionCount();
const QStringList getParamTableDropdownList(void* pData);

// 功能号到IO的映射
int getRealIOFromFunctionCode(int func, bool isInput);

// 数据转换
QVariant DataToQVariant(void* data, DataType dataType);
void QVariantToData(const QVariant& val, void* data, DataType dataType);

// 报警
const QString getAlarmMessage(int alarmNumber);
int getAlarmLevel(int alarmNumber);

// 工作状态
const QString getWorkstatusText(int workstatus);

template<typename T>
class DataAdapterBase
{
public:
    DataAdapterBase(T* baseAddr): _baseAddr(baseAddr) {}
    DataAdapterBase(T* baseAddr,int startnameindex): _baseAddr(baseAddr),nameIndexStart(startnameindex) {}//add by yzg
    virtual ~DataAdapterBase() {}

    virtual QString getParamName(int offset) const = 0;
    virtual int getParamWidgetType(int offset) const = 0;
    virtual int getDataType(int offset) const = 0;
    virtual bool getDataAuthority(int offset) const = 0;

    virtual void setData(int offset, const QVariant& val) = 0;
    virtual QVariant getData(int offset) = 0;

    T* getBaseAddr() const
    {
        return _baseAddr;
    }

    //add by yzg
    int getNameIndexStart() const
    {
        return nameIndexStart;
    }

protected:
    T* _baseAddr;
    int nameIndexStart;//add by yzg
};

class DataAdapter_ParamTable : public DataAdapterBase<TParamTable>
{
public:
    DataAdapter_ParamTable(TParamTable* baseAddr);
    DataAdapter_ParamTable(TParamTable* baseAddr,int startnameindex);//add by yzg
    QString getParamName(int offset) const override;
    int getParamWidgetType(int offset) const override;
    int getDataType(int offset) const override;
    bool getDataAuthority(int offset) const override;

    void setData(int offset, const QVariant& val) override;
    QVariant getData(int offset) override;

    // for special usage
    size_t getDataTypeSize(int offset)const;
    void* getDataPointer(int offset)const;
    unsigned short getDataModbusAddress(int offset)const;
};

class DataAdapter_IOConfigText : public DataAdapterBase<IO_FUN_NAME>
{
public:
    DataAdapter_IOConfigText(IO_FUN_NAME* funcNameBaseAddr, IOConfig* ioConfigBaseAddr, quint16 ioConfigModbusAddr);
    QString getParamName(int offset) const override;
    int getParamWidgetType(int offset) const override;
    int getDataType(int offset) const override;
    bool getDataAuthority(int offset) const override;

    void setData(int offset, const QVariant& val) override;
    QVariant getData(int offset) override;

    IOConfig* getIOConfigBaseAddr()const{return ioConfigAddr;}
    quint16 getIOConfigModbusAddr()const{return modbusAddr;}
protected:
    IOConfig* ioConfigAddr;
    quint16 modbusAddr;
};

class DataAdapter_IOStatus : public DataAdapterBase<IOConfig>
{
public:
    DataAdapter_IOStatus(IOConfig* ioConfigBaseAddr, IO_FUN_NAME* funcNameAddr, int funcNameCount);
    QString getParamName(int offset) const override;// 根据真正的IO端口获取“端口XX”
    int getParamWidgetType(int offset) const override;
    int getDataType(int offset) const override;
    bool getDataAuthority(int offset) const override;
    void setData(int offset, const QVariant& val) override;
    QVariant getData(int offset) override;

    QString getFunctionName(int offset) const; // 根据真正的IO端口获取功能描述
protected:
    IO_FUN_NAME* funcNameAddr;
    int funcNameCount;
    QMap<int, QList<int>> mapIONumToFuncTextIndex; // IOConfig数组下标 -> 文本数组的数组下标
};

}

#endif // ADAPTERS_H
