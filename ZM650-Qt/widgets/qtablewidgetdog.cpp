/*
 * qtablewidgetdog.cpp
 * 加密狗列表
 * Created on: 2019年 11月 7日 星期一 14:57:40 CST
 * Author: caojing
 */
#include "qtablewidgetdog.h"
#include <QHeaderView>
#define MAX_ROW 24
QTableWidgetDog::QTableWidgetDog(QWidget* parent): QTableView(parent)
{
    model= new QItemModelDog(this);
    connect(model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(onItemChanged(QStandardItem*)));
    horizontalHeader()->setVisible(true);
    verticalHeader()->setVisible(false);
}

void QTableWidgetDog::init(DOG* dog,int fontSize)
{
    m_dog = dog;
    font.setPointSize(fontSize);
    this->setFont(font);
    font.setPointSize(7);
    verticalHeader()->setFont(font);
    //horizontalHeader()->setFont(font);

    this->setModel(model);
    model->setColumnCount(3);
    horizontalHeader()->setResizeMode(QHeaderView::Stretch);//列宽设置
    verticalHeader()->setResizeMode(QHeaderView::Stretch);//列宽设置
//    verticalHeader()->setDefaultSectionSize(22);
}

void QTableWidgetDog::onItemChanged(QStandardItem *item)
{//数据改变
    int col = item->column();
    int row = item->row();
    switch (col%3)
    {
    case 0:
        break;
    case 1:
        QDog::getInstance()->m_dog.User_Password[row]=item->text().toInt();
        //        qDebug()<<item->text().toInt();
        break;
    case 2:
        QDog::getInstance()->m_dog.User_Days[row]=item->text().toInt();
        break;
    default:
        break;
    }
    //qDebug()<<"item"<<item->text()<<item->index();

}

void QTableWidgetDog::showPassword(int line,int page)
{//密码显示
    model->clear();

    int temp=0;
    if(page)
    {
       temp = 12;
    }
    for(int i=0;i<line;i++)
    {
        model->setItem(i, 0, new QStandardItem(QString("%1").arg(i+temp+1)));
        model->setItem(i, 1, new QStandardItem(QString("%1").arg(m_dog->User_Password[i+temp])));
        model->setItem(i, 2, new QStandardItem(QString("%1").arg(m_dog->User_Days[i+temp])));
        model->item(i, 0)->setTextAlignment(Qt::AlignCenter);
        model->item(i, 1)->setTextAlignment(Qt::AlignCenter);
        model->item(i, 2)->setTextAlignment(Qt::AlignCenter);
    }
}
