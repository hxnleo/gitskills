/*
 * qdialogmainspeed.cpp
 *
 * 设置班次对话框
 *
 * Created on: 2020年 2月 20日 星期一 15:25:40 CST
 * Author: caojing
 */
#include "qdialogspeed.h"
#include "data/globaldata.h"
#include "data/uistyles.h"
#include "data/utils.h"
#include "modbus/modbusaddress.h"
#include "modbus/modbusqueue.h"
#include<QDebug>
#include<QDialogButtonBox>
#include "qlineeditstandard.h"

QDialogSpeed::QDialogSpeed(QWidget* parent, QString str, Qt::WindowFlags f):
                       QDialog(parent, f),line(new QLineEditStandard(this))
{

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    QHBoxLayout *hlayout = new QHBoxLayout();
    QLabel *labe = new QLabel(QString("速度设置"));
    auto buttonBox = Utils::createOkCancelButtons(this);
    line->setText(str);
    line->setAlignment(Qt::AlignHCenter);
    line->setStyleSheet(UIStyles::css_maincolorconf_u11);
    hlayout->addWidget(labe);
    hlayout->addWidget(line);
    vlayout->addLayout(hlayout);
    vlayout->addWidget(buttonBox);
    buttonBox->setFocus();
}

void QDialogSpeed::accept()
{
    emit changeSpeed(line->text().toInt());
    QDialog::accept();
}

void QDialogSpeed::reject()
{
    QDialog::reject();
}

