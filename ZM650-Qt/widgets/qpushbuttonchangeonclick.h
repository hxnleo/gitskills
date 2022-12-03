/*
 * qpushbuttonchangeonclick.h
 *
 * 带点击事件的QPushButton，用于教导界面改变步长
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QPUSHBUTTONCHANGEONCLICK_H
#define QPUSHBUTTONCHANGEONCLICK_H

#include <QPushButton>

class QPushButtonChangeOnClick : public QPushButton
{
    Q_OBJECT
public:
    explicit QPushButtonChangeOnClick(QWidget* parent = 0);

    void init(const QStringList& stringList);

    int getIndex()const
    {
        return index;
    }
    void setIndex(int index);

signals:
    void clickedIndexChanged(int index);

private slots:
    void onClicked();

protected: // members
    QStringList stringList;
    int index;

protected: // functions
    void _setTextByIndex();
};

#endif // QPUSHBUTTONCHANGEONCLICK_H
