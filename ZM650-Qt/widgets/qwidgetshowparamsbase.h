/*
 * qwidgetshowparamsbase.h
 *
 * 参数界面的基类
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QWIDGETSHOWPARAMSBASE_H
#define QWIDGETSHOWPARAMSBASE_H

#include <QWidget>
#include <QGridLayout>
#include "modbus/modbusqueue.h"
#include <QMutex>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#define MAX_DA_NUM 42
class QLabel;

class QWidgetShowParamsBase : public QWidget
{
    Q_OBJECT
public:
    explicit QWidgetShowParamsBase(QWidget* parent = nullptr);
    ~QWidgetShowParamsBase();

    enum SelectWidget
    {//用来去区别上面还是下面
        Params_IOStatus,//IO诊断界面
        Params_Patterns,//花样界面
        Params_DAConfig,//DA界面
        Params_ParamTable,//参数界面
        ShowParamsBase,
        Params_DiagnoseUtils,//工具界面
        Params_AxisEnable,//工具界面
        End,//其他界面往前面加
    };
    void init(int row, int col, int totalParams);
    QLabel* laberWidget(QLabel* label,int index);
    int getCurrentPageNum() const
    {
        return currentPage;
    }
    int getMaxPageNum() const
    {
        return maxPage;
    }

protected:
    virtual QWidget* createFieldWidget(int index) = 0;
    virtual QString getParamName(int offset)const = 0;
    virtual bool getDataAuthority(int offset) const = 0;
    virtual ModbusQueue::element_t* createModbusElement(int offset) const = 0;
    virtual void onSendModbusOK(ModbusQueue::element_t* element)=0;
    virtual void onSendModbusFail(ModbusQueue::element_t* element)=0;
    virtual void onRefreshPageDone() {}

protected:
    void _modbusQueryCallback(bool isOk, void* flag)
    {
        emit _signal_modbusQueryCallback(isOk, flag);
    }
    void extraWidget(int page = 0);
    int selectWidget();
    void createWidget(int c,int r,int index,int paramTableIndex);
    int lastWidget;//上一个widget

signals:
    void _signal_modbusQueryCallback(bool isOk, void* flag);
    void changswith();

protected slots:
    void onButtonClickedNext();
    void onButtonClickedPrev();
    void onWroteGoodValue();
    void onReadIO();
    void onModbusQueryCallbackEnd(bool isOk, void* flag);
    void onBtn_WireTest();
    void onChangswith();
    void onTimeOut();

protected:
    QGridLayout* gridLayout;
    QHBoxLayout* hlayout;
    int row, col; // 参数的个数（行列）
    int totalParams;
    int currentPage;
    int countPerPage;//当前页的部件数
    int maxPage;
    QList<QLabel*> labelsCreated;
    QList<QWidget*> fieldsCreated;//存储部件
    QList<QWidget*> extreCreated;//存储额外部件
    bool isInitialized;
    int pendingQueryCount;
    QLabel* labelPageNumber;
    QLabel* datitilledit;
    QMutex mutexFieldsCreated;
    QPushButton* btn_wiretest;//送丝按钮
    QTimer* timer;//刷新送丝状态
    QSpacerItem *Stretch1;//用于外加弹簧
    QSpacerItem *Stretch2;//用于外加弹簧
    static const char PROP_PARAMTABLE_INDEX[];

protected: // functions
    void updateLabelPageNumber();
    void refreshPage(int pageNum);
    void iniParam(ModbusQueue::element_t* element);
//    void refreshParam(int addr);
    void sendModbusElement(ModbusQueue::element_t* element);
    bool sendFreQuency(ModbusQueue::element_t *element);
};

#endif // QWIDGETSHOWPARAMSBASE_H
