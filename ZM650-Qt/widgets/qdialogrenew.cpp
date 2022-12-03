/*
 * qdialogrenew.cpp
 *
 * 重命名对话框
 *
 * Created on: 2019年 10月 22日 星期一 17:02:40 CST
 * Author: caojing
 */
#include "qdialogrenew.h"
#include "data/globaldata.h"
#include<QDebug>

QDialogReNew::QDialogReNew(QWidget* parent,Qt::WindowFlags f):
                       QDialog(parent, f),
                       edit(new QLineEdit(this))
{
    setWindowIcon(QIcon(":/images/images/NULL.ico"));
    this->setWindowTitle(tr("请重新命名"));

    QVBoxLayout *VLayout = new QVBoxLayout(this);
    QLabel *label = new QLabel(tr("文件名已经存在,请填入新文件名"));

    QPushButton *close = new QPushButton(tr("否"));
    QPushButton *enter = new QPushButton(tr("是"));

    connect(close, SIGNAL(clicked()), this, SLOT(reject()));
    connect(enter, SIGNAL(clicked()), this, SLOT(accept()));

    QHBoxLayout *HLayout = new QHBoxLayout();
    HLayout->addWidget(enter);
    HLayout->addWidget(close);


    VLayout->addWidget(label);
    VLayout->addWidget(edit);
    VLayout->addLayout(HLayout);
}

void QDialogReNew::accept()
{
    //qDebug()<<edit->text();
    GlobalData::renewEdit = edit->text();
    QDialog::accept();
}

void QDialogReNew::reject()
{
    QDialog::reject();
}
