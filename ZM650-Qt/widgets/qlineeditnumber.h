/*
 * qlineeditnumber.h
 *
 * 专用于输入数字的QLineEdit
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QLINEEDITRANGE_H
#define QLINEEDITRANGE_H

#include <QLineEdit>
#include <QMouseEvent>
#include "data/adapters.h"

class QLineEditNumber : public QLineEdit
{
    Q_OBJECT

public:
    explicit QLineEditNumber(QWidget* parent = nullptr)
        : QLineEdit(parent), _selectOnMousePress(false) {}
    explicit QLineEditNumber(const QString& str, QWidget* parent = nullptr)
        : QLineEdit(str, parent), _selectOnMousePress(false) {}

    void init(void* data, Adapters::DataType dataType, double minVal, double maxVal);
    void init(void* data, float minVal, float maxVal);
    void setDaSingleData(bool flag);
    bool DataAuthority(int type);
    void setNumber(const QVariant& num);
    QVariant getNumber()const;
    Adapters::DataType getDataTyper()const
    {
        return _dataType;
    }

signals:
    void wroteValueFromUI(); // 从UI写入到pData
    void sendSingleData(); // 从UI写入到pData
    void changData(float data);

public slots:
    void readValueToUI(); // 从pData中更新到UI

protected slots:
    void onEditingFinished();


protected: //vars
    Adapters::DataType _dataType;
    void* _data;
    bool _selectOnMousePress;
    QString originalValue;
    bool Single;

protected: //functions
    void writeGoodValue(const QVariant& val);
    void daDeal(void* data,Adapters::DataType dataType);
    void focusInEvent(QFocusEvent* e);
    void mousePressEvent(QMouseEvent* me);
    void mouseReleaseEvent(QMouseEvent *e);
};

#endif // QLINEEDITRANGE_H
