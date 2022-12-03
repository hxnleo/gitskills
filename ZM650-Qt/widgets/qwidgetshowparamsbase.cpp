/*
 * qwidgetshowparamsbase.cpp
 *
 * 参数界面的基类
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qwidgetshowparamsbase.h"
#include <qmath.h>
#include <QSpacerItem>
#include "data/utils.h"
#include "data/adapters.h"
#include "data/globaldata.h"
#include <tr1/functional>
#include <QMutexLocker>
#include "data/uistyles.h"
#include <QLineEdit>
#include <QProcess>
#include "Parameter.h"
#include "Teach.h"

#define PATAMSTART 9006
#define PATAMEND   9014
const char QWidgetShowParamsBase::PROP_PARAMTABLE_INDEX[] = "offset";

QWidgetShowParamsBase::QWidgetShowParamsBase(QWidget* parent) : QWidget(parent),
    lastWidget(-1),
    isInitialized(false),
    pendingQueryCount(0),
    labelPageNumber(new QLabel(this))
{
    auto mainLayout = new QHBoxLayout(this);
    auto VLayout = new QVBoxLayout();
    gridLayout = new QGridLayout;
    hlayout = new QHBoxLayout;
    //gridLayout->setHorizontalSpacing(50);
    VLayout->addLayout(gridLayout);
    VLayout->addLayout(hlayout);
    VLayout->addStretch();//防止布局乱飞
    mainLayout->addLayout(VLayout);
    mainLayout->addStretch();
    auto naviBtnLayout = new QVBoxLayout;
    mainLayout->addLayout(naviBtnLayout);

    auto btn_next = new QPushButton(tr("下页"), this);
    auto btn_prev = new QPushButton(tr("上页"), this);
    auto btn_space = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    connect(btn_next, SIGNAL(clicked()),
            this, SLOT(onButtonClickedNext()));
    connect(btn_prev, SIGNAL(clicked()),
            this, SLOT(onButtonClickedPrev()));

    labelPageNumber->setAlignment(Qt::AlignCenter);
    naviBtnLayout->addItem(btn_space);
    naviBtnLayout->addWidget(labelPageNumber);
    naviBtnLayout->addWidget(btn_prev);
    naviBtnLayout->addWidget(btn_next);
    auto size = QSize(70, 90);
    btn_next->setFixedSize(size);
    btn_prev->setFixedSize(size);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),
            this, SLOT(onTimeOut()));
    timer->setInterval(50); // 更新间隔
    // signals
    connect(this, SIGNAL(_signal_modbusQueryCallback(bool, void*)),
            this, SLOT(onModbusQueryCallbackEnd(bool, void*)), Qt::QueuedConnection);
    connect(this, SIGNAL(changswith()),this, SLOT(onChangswith()));
}

QWidgetShowParamsBase::~QWidgetShowParamsBase()
{
    onChangswith();//界面切换需要关闭送丝
    Utils::waitForPendingModbus(&pendingQueryCount);
}
int QWidgetShowParamsBase::selectWidget()
{
    int index = SelectWidget::End;
    QString str=metaObject()->className();
    if(str=="QWidgetShowParams_IOStatus")
        index=SelectWidget::Params_IOStatus;
    else if(str=="QWidgetShowParams_Patterns")
        index=SelectWidget::Params_Patterns;
    else if(str=="QWidgetShowParams_DAConfig")
        index=SelectWidget::Params_DAConfig;
    else if(str=="QWidgetShowParams_ParamTable")
        index=SelectWidget::Params_ParamTable;
    else if(str=="QWidgetShowParamsBase")
        index=SelectWidget::ShowParamsBase;
    else if(str=="QWidgetShowParams_DiagnoseUtils")
        index=SelectWidget::Params_DiagnoseUtils;
    else if(str=="QWidgetShowParams_AxisEnable")
        index=SelectWidget::Params_AxisEnable;
    if(lastWidget !=index)
    {
        lastWidget = index;
        emit changswith();
    }
    return index;
}
void QWidgetShowParamsBase::init(int row, int col, int totalParams)
{
    if(true == isInitialized)
        return;

    this->row = row;
    this->col = col;
    this->totalParams = totalParams;
    this->currentPage = 0;
    this->countPerPage = row * col;
    this->maxPage = qCeil(static_cast<double>(totalParams) / countPerPage);
    for(auto c = 0; c < col; ++c)
    {
        for(auto r = 0; r < row; ++r)
        {
            auto label = new QLabel(this);
            label->hide();
            labelsCreated.append(label);
        }
    }
    //auto space = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    //gridLayout->addItem(space, row, 0);
    isInitialized = true;
    refreshPage(0);
}

QLabel* QWidgetShowParamsBase::laberWidget(QLabel* label,int index)//mod by cj
{//设置各个laber 的size
    QFont font;
    switch (index) {
    case Params_ParamTable:
    case Params_Patterns:
    {
        label->setMinimumWidth(GlobalData::widthRate/4+30);
        label->setMaximumWidth(GlobalData::widthRate/4+30);
    }
        break;
    case ShowParamsBase:
    {
        label->setMinimumWidth(GlobalData::widthRate/6+20);
        label->setMaximumWidth(GlobalData::widthRate/6+20);
    }
        break;
    case Params_DAConfig:
    {
        label->setMinimumWidth(GlobalData::widthRate/6-20);
        label->setMaximumWidth(GlobalData::widthRate/6-20);
        label->setMinimumHeight(GlobalData::highRate/8-10);
        label->setMaximumHeight(GlobalData::highRate/8-10);
        font.setPointSize(8); // 设置字号
        label->setFont(font);  // 设置字体
        return label;
    }
        break;
    case Params_DiagnoseUtils:
    {
        label->setMinimumWidth(GlobalData::widthRate/4);
        label->setMaximumWidth(GlobalData::widthRate/4);
    }
        break;
    default:
    {
        label->setMinimumWidth(GlobalData::widthRate/6+20);
        label->setMaximumWidth(GlobalData::widthRate/6+20);
        font.setPointSize(10); // 设置字号
        label->setFont(font);  // 设置字体
        return label;
    }
        break;
    }
    font.setPointSize(8); // 设置字号
    label->setFont(font);  // 设置字体
    return label;
}

void QWidgetShowParamsBase::refreshPage(int pageNum)
{
    if(false == isInitialized)
        return;

    if(pageNum < 0 || pageNum >= maxPage)
        return;

    QMutexLocker locker(&mutexFieldsCreated);

    // delete previous fields
    foreach(QWidget* w, fieldsCreated)
    {
        gridLayout->removeWidget(w);
        w->deleteLater();
    }
    fieldsCreated.clear();
    foreach(QWidget* w, extreCreated)
    {
        hlayout->removeWidget(w);
        w->deleteLater();
    }
    extreCreated.clear();
    if(Stretch1!= nullptr)
    {
        hlayout->removeItem(Stretch1);
        hlayout->removeItem(Stretch2);
    }
    auto lastElementNumber = (1 + pageNum) * countPerPage;
    auto emptyCount = lastElementNumber - totalParams;
    auto nonEmptyCount = countPerPage;
    if(emptyCount > 0)
    {
        // 不可以满屏
        nonEmptyCount = countPerPage - emptyCount;
    }
    for(auto c = 0; c < col; ++c)
    {
        for(auto r = 0; r < row; ++r)
        {
            auto index = c * row + r;
            auto label = labelsCreated.at(index);
            auto paramTableIndex = index + pageNum * countPerPage;

            if(index < nonEmptyCount)
            {
                createWidget(c,r,index,paramTableIndex);
            }
            else
            {
                // label
                gridLayout->removeWidget(label);
                label->hide();
            }
        }
    }
    extraWidget(pageNum);
    this->currentPage = pageNum;
    updateLabelPageNumber();
    onRefreshPageDone();
}

void QWidgetShowParamsBase::onWroteGoodValue()
{
    auto index = sender()->property(PROP_PARAMTABLE_INDEX).toInt();//得到序列号 名 "offest" 号 value
    ModbusQueue::element_t* element = createModbusElement(index);
    qDebug()<<"startAddress"<<element->startAddress;
    //检车刹车变频器
    if(sendFreQuency(element))
    {
        sendModbusElement(element);
    }
    //Utils::printQString(adapter->getParamName(index)+adapter->getData(index).toString());
}

bool QWidgetShowParamsBase::sendFreQuency(ModbusQueue::element_t* element)
{//变频器打开需要在刹车打开之后  关闭则反过来
    unsigned short  * readstatus ,temp;
    readstatus = &temp;
    //    qDebug()<<"in start";
    //如果端口号接在一起不做此判断
    if(GlobalData::brakenum==GlobalData::motornum)
        return true;
    if(element->startAddress == ModbusAddress::IOStatus_Out+GlobalData::brakenum)
    {//检测刹车
        if(!*(static_cast<unsigned short*>(element->data)))
        {//需要先关闭变频器
            Utils::sendOffest(readstatus,ModbusAddress::IOStatus_Out+GlobalData::motornum,sizeof(*readstatus));
            if(*readstatus)
            {
                Utils::MyMessageBox(QString("Erro"), tr("请先关闭变频器!"),tr("确定"),this);
                return false;
            }
        }
    }
    if(element->startAddress == ModbusAddress::IOStatus_Out+GlobalData::motornum)
    {//检测变频器启动
        if(*(static_cast<unsigned short*>(element->data)))
        {//需要先打开刹车
            Utils::sendOffest(readstatus,ModbusAddress::IOStatus_Out+GlobalData::brakenum,sizeof(*readstatus));
            if(!*readstatus)
            {
                Utils::MyMessageBox(QString("Erro"), tr("请先打开刹车!"),tr("确定"),this);
                return false;
            }
        }
    }
    return true;
}

void QWidgetShowParamsBase::onReadIO()
{
    memset(g_Sysparam.InConfig,0,sizeof(g_Sysparam.InConfig));
    memset(g_Sysparam.OutConfig,0,sizeof(g_Sysparam.OutConfig));
    Adapters::readIOConfig(GlobalData::modbusQueue);//读取IO端口
}

void QWidgetShowParamsBase::onModbusQueryCallbackEnd(bool isOk, void* flag)
{
    auto element = reinterpret_cast<ModbusQueue::element_t*>(flag);
    bool isNeedToDelete = true;

    if(isOk)
    {
        retrytime=0;
        onSendModbusOK(element);
        iniParam(element);//管理参数操作
//        refreshParam(element->startAddress);//参数刷新
    }
    else
    {
        if(true == Utils::dialogIsModbusRetry(this, GlobalData::modbusQueue))
        {
            GlobalData::modbusQueue->sendQuery(element, true, false);
            GlobalData::modbusQueue->queryResume();
            isNeedToDelete=false;
        }
        else
        {
            GlobalData::modbusQueue->queryStopThenOffline();
            onSendModbusFail(element);
        }
    }

    if(true == isNeedToDelete)
    {
        --pendingQueryCount;
        Utils::delete_pointer(element);
    }
}
void QWidgetShowParamsBase::extraWidget(int page)//mod by cj
{//设置额外布局
    QString str=metaObject()->className();
    QFont font;
    font.setPointSize(9); // 设置字号
    if(str=="QWidgetShowParams_DAConfig")
    {
        GlobalData::labelTopTab ->setText(tr("DA%1测试").arg(page));
        datitilledit = new QLabel(QString(tr("DA%1测试界面").arg(page)));
        datitilledit->setAlignment(Qt::AlignCenter);
        datitilledit->setFont(font);  // 设置字体
        QLabel* dalaber = new QLabel(tr("电压(0~10V)"));//DA提示框
        dalaber->setFont(font);  // 设置字体
        dalaber->setAlignment(Qt::AlignCenter);
        auto daledit = createFieldWidget(MAX_DA_NUM+page);//DA输入框
        daledit->setProperty(PROP_PARAMTABLE_INDEX, MAX_DA_NUM+page);//名字都为offset 设置不同序号
        daledit->setFont(font);  // 设置字体
        Stretch1 = new QSpacerItem(daledit->width(), 20,QSizePolicy::Expanding);
        datitilledit->setFixedWidth(daledit->width());
        dalaber->setFixedWidth(daledit->width());
        hlayout->addWidget(datitilledit);
        hlayout->addItem(Stretch1);
        hlayout->addWidget(dalaber);
        hlayout->addWidget(daledit);

        extreCreated.append(datitilledit);
        extreCreated.append(dalaber);
        extreCreated.append(daledit);

#ifdef ARM
        Stretch2 = new QSpacerItem(daledit->width()+8, 20);
        hlayout->addItem(Stretch2);
        btn_wiretest = new QPushButton(tr("送丝测试"));//送丝测试btn
        btn_wiretest->setStyleSheet(UIStyles::css_maincolorconf_u8);
        btn_wiretest->setFixedWidth(daledit->width());
        connect(btn_wiretest, SIGNAL(clicked()),this, SLOT(onBtn_WireTest()));
        hlayout->addWidget(btn_wiretest);
        extreCreated.append(btn_wiretest);
        timer->start();//开始刷新
#endif
    }
}
void QWidgetShowParamsBase::onBtn_WireTest()
{
    //    qDebug()<<"onBtn_WireTest";
    ModbusQueue::element_t* element = new ModbusQueue::element_t;
    auto mixData = new Definitions::MixDataType;
    mixData->u16 = FN_SEND_IRON;
    element->data = &mixData->u16;
    element->len = sizeof(mixData->u16);
    element->operation = ModbusQueue::OP_WRITE_DATA;
    element->startAddress = ModbusAddress::ButtonOperate;
    GlobalData::modbusQueue->sendQuery(element,false);
}

void QWidgetShowParamsBase::onChangswith()
{
    if(btn_wiretest!=nullptr)
    {
        if(GlobalData::realTimeInfo.iSendIron)
        {
            //            qDebug()<<"onChangswith";
            timer->stop();
            onBtn_WireTest();
        }
    }
}

void QWidgetShowParamsBase::onTimeOut()
{
    mutexFieldsCreated.lock();// 上锁，直到callback将其解锁
    if(btn_wiretest!=nullptr)
    {
        btn_wiretest->setStyleSheet(
                    GlobalData::realTimeInfo.iSendIron?
                        UIStyles::css_maincolorconf_u13:
                        UIStyles::css_maincolorconf_u8);
    }
    mutexFieldsCreated.unlock();
}

void QWidgetShowParamsBase::createWidget(int c,int r,int index,int paramTableIndex)
{   
    auto label = labelsCreated.at(index);
    int currWidget = selectWidget();
    QFont font;
    font.setPointSize(9); // 设置字号
    switch (currWidget) {
    case SelectWidget::Params_IOStatus:
    case SelectWidget::Params_AxisEnable:
    {
        auto w = createFieldWidget(paramTableIndex);
        w->setFont(font);  // 设置字体
        GlobalData::getInstance()->widgetEnable(w,getDataAuthority(paramTableIndex));
        if(w)
        {
            w->setProperty(PROP_PARAMTABLE_INDEX, paramTableIndex);
            //auto space = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
            if(currWidget==SelectWidget::Params_AxisEnable)
                gridLayout->setSpacing(30);
            else
                gridLayout->setSpacing(10);
            gridLayout->addWidget(w, c, r, 1, 1);
            fieldsCreated.append(w);
        }
    }
        break;
    case SelectWidget::Params_Patterns:
    {
        auto labelText = getParamName(paramTableIndex);
        //qDebug()<<labelText<<QString("Label %1").arg(paramTableIndex);
        // label
        gridLayout->addWidget(label, r*2+1, c, 1, 1);
        //设置参数laber
        label=laberWidget(label,currWidget);
        label->setText(labelText);
        label->setContentsMargins(5,0,0,0);
        label->show();
        label->setAlignment(Qt::AlignHCenter);
        //config color
        label->setStyleSheet(UIStyles::css_paracolorconf_u3);
        auto w = createFieldWidget(paramTableIndex);
        w->setFont(font);  // 设置字体
        if(w)
        {
            w->setProperty(PROP_PARAMTABLE_INDEX, paramTableIndex);
            //auto space = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
            gridLayout->setSpacing(20);
            gridLayout->addWidget(w, r*2, c, 1, 1);
            fieldsCreated.append(w);
        }
    }
        break;
    case SelectWidget::Params_DAConfig:
    {
        auto labelText = getParamName(paramTableIndex);
        //qDebug()<<labelText<<QString("Label %1").arg(paramTableIndex);
        // label
        gridLayout->addWidget(label, c,r*2, 1, 1);
        //设置参数laber
        label=laberWidget(label,currWidget);
        label->setText(labelText);
        label->setContentsMargins(5,0,0,0);
        label->show();
        //config color
        if(c==0 || c==2 || c==4 || c==6)
        {
            label->setStyleSheet(UIStyles::css_paracolorconf_u0);
        }
        else
        {
            label->setStyleSheet(UIStyles::css_paracolorconf_u0);
        }

        auto w = createFieldWidget(paramTableIndex);
        w->setMinimumHeight(GlobalData::highRate/8-10);
        w->setMaximumHeight(GlobalData::highRate/8-10);

        w->setFont(font);  // 设置字体
        GlobalData::getInstance()->widgetEnable(w,getDataAuthority(paramTableIndex));
        if(w)
        {
            w->setProperty(PROP_PARAMTABLE_INDEX, paramTableIndex);
            //auto space = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
            gridLayout->setSpacing(7);
            gridLayout->addWidget(w, c, r*2+1, 1, 1);
            fieldsCreated.append(w);
        }
    }
        break;
    default:
    {
        //auto labelText=QString("Label %1").arg(paramTableIndex);

        auto labelText = getParamName(paramTableIndex);
        // label
        gridLayout->addWidget(label, r, c * 2, 1, 1);
        //设置参数laber
        label=laberWidget(label,currWidget);
        label->setText(labelText);
        label->setContentsMargins(5,0,0,0);
        label->show();
        if(GlobalData::screensize == FIFTEEN)
        {
            font.setPointSize(7); // 设置字号
            label->setFont(font);
        }
        //config color
        if(index%8==0 || index%8==2 || index%8==4 || index%8==6)
        {
            label->setStyleSheet(UIStyles::css_paracolorconf_u0);
        }
        else
        {
            label->setStyleSheet(UIStyles::css_paracolorconf_u0);
        }
        // fields
        auto w = createFieldWidget(paramTableIndex);
        font.setPointSize(9); // 设置字号
        w->setFont(font);  // 设置字体
        GlobalData::getInstance()->widgetEnable(w,getDataAuthority(paramTableIndex));
        if(w)
        {
            w->setProperty(PROP_PARAMTABLE_INDEX, paramTableIndex);
            //auto space = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
            gridLayout->setSpacing(7);
            gridLayout->addWidget(w, r, c * 2 + 1, 1, 1);
            fieldsCreated.append(w);
        }
    }
        break;
    }
    //    qDebug()<<"currWidget"<<currWidget;
}


void QWidgetShowParamsBase::iniParam(ModbusQueue::element_t* element)
{//对管理参数部分参数的额外处理
    int addrstart,addrend;
    addrstart = Utils::getParaAdress(&g_Sysparam.OperationPassw)+2;
    addrend = addrstart+8;
    //qDebug()<<Utils::getParaAdress(&g_Sysparam.ProgrammerPassw)<<Utils::getParaAdress(&g_Sysparam.OperationPassw);
    if(element->startAddress >= addrstart&&element->startAddress <= addrend &&element->startAddress != addrend-2)
    {//初始化后重启
        //        qDebug()<<element->startAddress<<addr;
        if(element->startAddress==addrend||element->startAddress==addrend-4||element->startAddress==addrstart)
        {//参数初始化的时候重新读取参数
            Adapters::readParamTable(GlobalData::modbusQueue, false);
            // 整体数据读取
            //            element.data = &teachData[startPointIndex];
            //            element.len = sizeof(TEACH_DATA) * static_cast<size_t>(count);
            //            element.mixParam.fifo.code = 2;
            //            element.mixParam.fifo.offset = startPointIndex * static_cast<int>(sizeof(TEACH_DATA));
            //            element.operation = isRead ? ModbusQueue::OP_READ_DATA_FIFO : ModbusQueue::OP_WRITE_DATA_FIFO;
            //            isOk = GlobalData::modbusQueue->sendQuery(&element, false);
        }
        else if(element->startAddress==addrstart+2)
        {//重新读取IO
            Adapters::readIOConfig(GlobalData::modbusQueue);
        }
        Utils::MyMessageBox(QString("Info"), tr("操作完成,请重启控制器"),tr("确定"),this);
    }
}



//void QWidgetShowParamsBase::refreshParam(int addr)
//{
//    int index;
//    Adapters::DataAdapter_ParamTable adapter(ParaRegTab);
//    index = Utils::getParaIndex(addr);
//    ModbusQueue::element_t element;
//    element.data = adapter.getDataPointer(index);
//    element.len = static_cast<unsigned short>(adapter.getDataTypeSize(index));
//    element.startAddress = adapter.getDataModbusAddress(index);
//    element.operation = ModbusQueue::OP_READ_DATA;
//    GlobalData::modbusQueue->sendQuery(&element, false);
//    //    qDebug()<<index<<static_cast<unsigned short>(adapter.getData(index).toUInt());
//}

void QWidgetShowParamsBase::sendModbusElement(ModbusQueue::element_t* element)
{
    if(nullptr != element)
    {
        element->callback = std::tr1::bind(&QWidgetShowParamsBase::_modbusQueryCallback, this, std::tr1::placeholders::_1,
                                           std::tr1::placeholders::_2);
        GlobalData::modbusQueue->sendQuery(element);
        ++pendingQueryCount;
    }

}

#if 0
void QWidgetShowParamsBase::onWroteGoodValue()
{
    auto index = sender()->property(PROP_PARAMTABLE_INDEX).toInt();
    //Utils::printQString(Utils::gbkCharToQString((this->paramTable+index)->pDescText));
    //qDebug("onWroteGoodValue: %d",index);

    // TODO: send signals
}
#endif

void QWidgetShowParamsBase::onButtonClickedNext()
{
    auto p = getCurrentPageNum();
    if(p < getMaxPageNum() - 1)
        refreshPage(p + 1);
}

void QWidgetShowParamsBase::onButtonClickedPrev()
{
    auto p = getCurrentPageNum();

    if(p > 0)
        refreshPage(p - 1);
}

void QWidgetShowParamsBase::updateLabelPageNumber()
{
    labelPageNumber->setText(QString("%1/%2").arg(getCurrentPageNum() + 1).arg(getMaxPageNum()));
}
