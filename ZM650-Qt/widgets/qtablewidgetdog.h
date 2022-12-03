/*
 * qtablewidgetdog.h
 * 加密狗列表
 * Created on: 2019年 11月 7日 星期一 14:57:40 CST
 * Author: caojing
 */
#ifndef QTABLEWIDGETDOG_H
#define QTABLEWIDGETDOG_H
#include <QTableView>
#include <QList>
#include "data/dog.h"
#include "data/globaldata.h"
#include "qitemmodeldog.h"

class QTableWidgetDog : public QTableView
{
    Q_OBJECT
public:
    explicit QTableWidgetDog(QWidget* parent = nullptr);

    void init(DOG* dog,int fontSize = 7);
    void showPassword(int line, int page = 0);
    QFont font;
signals:

public slots:

protected slots:
    void onItemChanged(QStandardItem *item);

protected: // members
    QItemModelDog* model;
    DOG* m_dog;
};


#endif // QTABLEWIDGETDOG_H
