/*
 * qcomboboxforparams.cpp
 *
 * 参数界面的单选下拉框
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include <QDebug>
#include "qcomboboxforparams.h"

QComboBoxForParams::QComboBoxForParams(QWidget* parent) : QComboBox(parent)
{

}

void QComboBoxForParams::init(void* data, Adapters::DataType dataType, const QStringList& strs, double minIndex,
                              double maxIndex)
{
    _dataType = dataType;
    _minIndex = static_cast<int>(minIndex);
    _maxIndex = static_cast<int>(maxIndex);
    _data = data;

    // the length of StringList is too short
//    qDebug()<<_maxIndex + 1 - _minIndex<<strs.size();
//    qDebug()<<strs;
//    if(_maxIndex + 1 - _minIndex > strs.size())
//        return;

    addItems(strs);//添加多个

    readValueToUI();

    if(nullptr != _data)
    {
        connect(this, SIGNAL(currentIndexChanged(int)),
                this, SLOT(onCurrentIndexChanged(int)));
    }
}

void QComboBoxForParams::readValueToUI()
{
    if(nullptr == _data)
        return;

    auto val = Adapters::DataToQVariant(_data, _dataType);
    setCurrentIndex(val.toInt() - _minIndex);
}

void QComboBoxForParams::onCurrentIndexChanged(int /*index*/)
{
    //qDebug("ComboBoxChanged: %d",index);
    QVariant val = QVariant(currentIndex() + _minIndex);
    Adapters::QVariantToData(val, _data, _dataType);
    emit wroteValueFromUI();
}
