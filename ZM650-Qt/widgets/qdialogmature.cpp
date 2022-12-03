/*
 * qdialogmature.cpp
 *
 * 启动时加密狗解密窗口
 *
 * Created on: 2019年 10月 22日 星期一 17:02:40 CST
 * Author: caojing
 */

#include<QDebug>
#include"data/utils.h"
#include<QLabel>
#include<QPushButton>
#include<QIcon>
#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QGridLayout>
#include "qdialogmature.h"
#include "data/globaldata.h"

QDialogMature::QDialogMature(QWidget* parent,Qt::WindowFlags f):
    QDialog(parent, f),edit_Password(new QLineEditStandard(tr("")))
{
    setWindowIcon(QIcon(":/images/images/NULL.ico"));
    this->setWindowTitle(tr("加密狗解密"));

    QVBoxLayout *VLayout = new QVBoxLayout(this);

    QLabel *label0 = new QLabel(tr("随机索引"));
    QLabel *label1 = new QLabel(tr("厂商序列号"));
    QLabel *label2 = new QLabel(tr("解锁密码"));
    QLineEdit *edit_Random = new QLineEditStandard(QString("%1").arg(QDog::getInstance()->m_dog.RandomNum));
    QLineEdit *edit_MFserial = new QLineEditStandard(QString("%1").arg(QDog::getInstance()->m_dog.Factory_SerialNo));
    edit_Random->setEnabled(false);
    edit_MFserial->setEnabled(false);

    QPushButton *close = new QPushButton(tr("取消"));
    QPushButton *enter = new QPushButton(tr("确认"));

    connect(close, SIGNAL(clicked()), this, SLOT(reject()));
    connect(enter, SIGNAL(clicked()), this, SLOT(accept()));

    QGridLayout *glayout = new QGridLayout();
    glayout->addWidget(label0,0,0,1,1);
    glayout->addWidget(edit_Random,0,1,1,1);
    glayout->addWidget(label1,1,0,1,1);
    glayout->addWidget(edit_MFserial,1,1,1,1);
    glayout->addWidget(label2,2,0,1,1);
    glayout->addWidget(edit_Password,2,1,1,1);
    VLayout->addLayout(glayout);
    VLayout->addStretch();
    VLayout->addWidget(enter);

}

void QDialogMature::accept()
{
    qDebug()<<"come";
    QString pwd;
    int pwd_num;
    if(false == QDog::getInstance()->isUnlocked())
    {
        pwd = edit_Password->text();
        pwd_num = pwd.toInt();
        if(1 == QDog::getInstance()->getDogType())
        {//多期
            //qDebug()<<" dog1 ";
            QString str;
            unsigned short level;
            if(false == QDog::getInstance()->validUserPassword(pwd_num,&level))
            {//识别密码是否有效
                Utils::MyMessageBox(QString("Error"), tr("无效密码!"),QObject::tr("确定"),this);
                edit_Password->clear();
                if(QDog::getInstance()->getRemainDays()>=0)
                {
                    if(QMessageBox::Retry==Utils::MyMessageBox(QString("Warning"),tr("解密失败,还可以使用")+QString("%1").
                                                            arg(QDog::getInstance()->getRemainDays())
                                                            +tr("天,请尽快联系商家解密!"),QObject::tr("确定"),this))

                        emit success();
                }
                return;
            }
            QDog::getInstance()->setUserLevel(level+1);

            if(QDog::getInstance()->writeDog())
            {
                //                if(QDog::getInstance()->readDog())
                //                {
                if(QDog::getInstance()->isUnlocked())
                {

                    QDog::getInstance()->writeDogProgress(0);
                    Utils::MyMessageBox(QString("OK"), tr("您已经解锁所有时长,感谢使用"),QObject::tr("确定"),this);
                    emit success();
                }
                else
                {
                    QString str1 = tr("已解锁期数");
                    QString str2 = tr("可使用天数");
                    str = QString("%1: %2: %3: %4")
                            .arg(str1).arg(level).arg(str2).arg(QDog::getInstance()->getRemainDays());
                    Utils::MyMessageBox(QString("OK"), str,QObject::tr("确定"),this);
                    //qDebug()<<"day33"<<QDog::getInstance()->getRemainDays();
                    emit success();
                }
                //                }
            }
            else
            {
                Utils::MyMessageBox(QString("Error"), str,QObject::tr("写入加密狗失败"),this);
                emit success();
            }

        }
        else
        {//单期
            qDebug()<<"单期";
            QDog::getInstance()->writeUserPassword(pwd_num);
            emit fail();
        }
    }
    else
    {
        emit success();
    }
    return;
}

void QDialogMature::reject()
{//不适用
    QDialog::reject();
}
