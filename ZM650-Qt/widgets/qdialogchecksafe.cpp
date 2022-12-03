/*
 * qdialogchecksafe.cpp
 *
 * 重命名对话框
 *
 * Created on: 2019年 10月 22日 星期一 17:02:40 CST
 * Author: caojing
 */
#include "qdialogchecksafe.h"


QDialogCheckSafe::QDialogCheckSafe(int i, int type, QWidget* parent, Qt::WindowFlags f):
    QDialog(parent, f),mode(i),type(type)
{
    setWindowIcon(QIcon(":/images/images/NULL.ico"));
    this->setWindowTitle(tr(""));

    QVBoxLayout *VLayout = new QVBoxLayout(this);
    QLabel *label = new QLabel(tr("检查是否安全切换模式?"));
    QPushButton *enter = new QPushButton(tr("确认"));
    QPushButton *close = new QPushButton(tr("取消"));

    connect(enter, SIGNAL(clicked()), this, SLOT(accept()));
    connect(close, SIGNAL(clicked()), this, SLOT(reject()));

    QHBoxLayout *HLayout = new QHBoxLayout();
    HLayout->addWidget(enter);
    HLayout->addWidget(close);

    VLayout->addWidget(label);
    VLayout->addLayout(HLayout);
}
void QDialogCheckSafe::deelFixture()
{
    switch(QMessageBox::warning(this, tr("注意"), tr(" 选则起始夹具?"),
                                tr("夹具A"), tr("夹具B")))
    {
    case BTN::BTN_OK:
        qDebug()<<"BTN_OK";

    {
        //需要发送命令
        //emit btn_ok(mode); //跳到主框去执行
    }

        QDialog::accept();
        break;
    case BTN::BTN_NO:
        qDebug()<<"BTN_NO";
        reject();
        break;
    default:
        break;
    }
}

void QDialogCheckSafe::accept()
{
    //this->hide();
    switch (mode) {
    case 0:
    case 2:
        if(type==1)
        {
            deelFixture();
        }
        break;
    case 3:
        if(type==1)
        {
            deelFixture();
        }

    default:
        break;
    }
    return;
}

void QDialogCheckSafe::reject()
{
    emit btn_no(mode);
    QDialog::reject();
}
