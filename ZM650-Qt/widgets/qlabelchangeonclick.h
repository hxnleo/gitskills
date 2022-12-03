/*
 * qlabelchangeonclick.h
 *
 * 带点击事件的QLabel
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QLABELCHANGEONCLICK_H
#define QLABELCHANGEONCLICK_H

#include "qlabelclick.h"
#include <QStringList>

class QLabelChangeOnClick : public QLabelClick
{
    Q_OBJECT
public:
    QLabelChangeOnClick(QWidget* parent = 0, Qt::WindowFlags f = 0);
    void init(const QStringList& stringList);

    int getIndex()const
    {
        return index;
    }
    void setIndex(int index);

private slots:
    void onClicked();

protected:
    QStringList stringList;
    int index;

protected:
    void _setTextByIndex();
};

#endif // QLABELCHANGEONCLICK_H
