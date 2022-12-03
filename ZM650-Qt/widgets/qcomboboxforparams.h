/*
 * qcomboboxforparams.h
 *
 * 参数界面的单选下拉框
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QCOMBOBOXFORPARAMS_H
#define QCOMBOBOXFORPARAMS_H

#include <QComboBox>
#include "data/adapters.h"

class QComboBoxForParams : public QComboBox
{
    Q_OBJECT
public:
    explicit QComboBoxForParams(QWidget* parent = nullptr);
    void init(void* data, Adapters::DataType dataType, const QStringList& strs, double minVal, double maxVal);

signals:
    void wroteValueFromUI();

public slots:
    void readValueToUI();

protected slots:
    void onCurrentIndexChanged(int index);

protected:
    Adapters::DataType _dataType;
    int _minIndex, _maxIndex;
    void* _data;
};

#endif // QCOMBOBOXFORPARAMS_H
