/*
 * qlineeditnumber.cpp
 *
 * 专用于输入数字的QLineEdit
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "widgets/myinputpanelcontext.h"
#include "QApplication"
#include "qlineeditnumber.h"
#include "data/utils.h"
#include <QIntValidator>
#include <QDoubleValidator>
#include <QDebug>

//数据 类型 最小值 最大值 权限
void QLineEditNumber::init(void* data, Adapters::DataType dataType, double minVal, double maxVal)
{
    _dataType = dataType;
    _data = data;
    Single = false;

    // read value from pData
    readValueToUI();

    // regex validator
    switch(_dataType)
    {
    case Adapters::TYPE_U8:
    case Adapters::TYPE_U16:
    case Adapters::TYPE_U32:
    case Adapters::TYPE_S8:
    case Adapters::TYPE_S16:
    case Adapters::TYPE_S32:
        setValidator(new QIntValidator(static_cast<int>(minVal), static_cast<int>(maxVal), this));
        break;

    case Adapters::TYPE_F32:
    case Adapters::TYPE_F64:
        setValidator(new QDoubleValidator(minVal, maxVal, 3, this));
        break;

    default:
        break;
    }
    // hint text
    setPlaceholderText(Utils::numberToString(minVal, 2, 3)+" ~ "+Utils::numberToString(maxVal, 2, 3));
    // 如需直接编辑坐标数字，将宏激活即可
#if _MCGS
    auto ic = qobject_cast<MyInputPanelContext*>(qApp->inputContext());
    auto panel = ic->getInputPanel();
    //qDebug()<<panel;

    // 注意：lixingcong@20181112
    // InputPanel如下的两个槽必须同时使用，否则出现不可预料的Segment Fault
    connect(this, SIGNAL(selectionChanged()),panel, SLOT(onSetNextFocusWidget()));
    connect(this, SIGNAL(destroyed()), panel, SLOT(onDestroyedFocusedWidget()));
#endif
    connect(this, SIGNAL(editingFinished()),
            this, SLOT(onEditingFinished()));

}
void QLineEditNumber::init(void* data, float minVal, float maxVal)
{//初始化DA的编辑框
    float* flo = static_cast<float*>(data);
    QString str = QString::number(*flo, 'f', 2);
    setText(str);//得到后面的字符串
    daDeal(data,Adapters::TYPE_F32);//设置数据
    // hint text 灰色框
    setPlaceholderText(Utils::numberToString(minVal, 2, 3)+" ~ "+Utils::numberToString(maxVal, 2, 3));
    setValidator(new QDoubleValidator(minVal, maxVal, 2, this));//文本约束
    // 如需直接编辑坐标数字，将宏激活即可
#if _MCGS
    auto ic = qobject_cast<MyInputPanelContext*>(qApp->inputContext());
    auto panel = ic->getInputPanel();
    //qDebug()<<panel;

    // 注意：lixingcong@20181112
    // InputPanel如下的两个槽必须同时使用，否则出现不可预料的Segment Fault
    connect(this, SIGNAL(selectionChanged()),panel, SLOT(onSetNextFocusWidget()));
    connect(this, SIGNAL(destroyed()), panel, SLOT(onDestroyedFocusedWidget()));
#endif
    //当按下按下Return或Enter键或线编辑失去焦点时发送信号
    connect(this, SIGNAL(editingFinished()),
            this, SLOT(onEditingFinished()));
}


void QLineEditNumber::setNumber(const QVariant& num)
{
    setText(num.toString());

    if(nullptr != _data)
    {
        Adapters::QVariantToData(num, _data, _dataType);
    }
}

QVariant QLineEditNumber::getNumber() const
{
    return QVariant(text());
}

void QLineEditNumber::readValueToUI()
{
    if(nullptr != _data)
    {
        QVariant val = Adapters::DataToQVariant(_data, _dataType);
        if(_dataType==Adapters::TYPE_F32||_dataType==Adapters::TYPE_F64)
        {//最多显示到0.01
            setText(QString::number(val.toFloat(),'f',2));
        }
        else
            setText(val.toString());
    }
    originalValue = text();
}

void QLineEditNumber::onEditingFinished()
{
    //    QLineEdit* w = qobject_cast<QLineEdit*>(sender());
    //    w->setReadOnly(true);
    qDebug("QLineEditNumber::onEditingFinished()");
    if(!Single)
    {
        //if(nullptr != _data && text().length() > 0 && originalValue != text())
        {
            float num = text().toFloat();
            emit changData(num);
            writeGoodValue(QVariant(text()));
            originalValue = text();
        }
    }
    else
    {//DA时候不需要看是否数据改变
        //if(nullptr != _data && text().length() > 0)
        {
            float num = text().toFloat();
            emit changData(num);
            writeGoodValue(QVariant(text()));
            originalValue = text();
        }
    }
}
void QLineEditNumber::daDeal(void* data,Adapters::DataType dataType)
{//DA定制
    _data = data;
    _dataType = dataType;
    originalValue=text();
}

void QLineEditNumber::setDaSingleData(bool flag)
{//DA按下编辑框发送数据
    Single = flag;
}

void QLineEditNumber::writeGoodValue(const QVariant& val)
{
    //qDebug("QLineEditNumber::writeGoodValue()");
    // store good value
    Adapters::QVariantToData(val, _data, _dataType);
    emit wroteValueFromUI();
}

void QLineEditNumber::focusInEvent(QFocusEvent* e)
{
    // https://forum.qt.io/topic/8389/solved-have-text-in-qlineedit-selected-on-edit-start/3
    QLineEdit::focusInEvent(e);
#if defined(Q_OS_WIN) || defined(_MCGS)
    selectAll();
#endif
//    if(false == _selectOnMousePress)
//    {
//        selectAll();
//        _selectOnMousePress = true;
//    }
}

void QLineEditNumber::mousePressEvent(QMouseEvent* me)
{
    QLineEdit::mousePressEvent(me);
#if defined(Q_OS_WIN) || defined(_MCGS)
    selectAll();
#endif
//    if(true == _selectOnMousePress)
//    {
//        selectAll();
//        _selectOnMousePress = false;
//    }
    if(Single)
    {
        if (me->button() == Qt::LeftButton)
        {//按下编辑框
            //qDebug()<<"press button!";
            emit sendSingleData();
        }
    }
}


void QLineEditNumber::mouseReleaseEvent(QMouseEvent *e)
{
    QLineEdit::mouseReleaseEvent(e);
#if defined(Q_OS_WIN) || defined(_MCGS)
    selectAll();
#endif
}

