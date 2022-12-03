/*
 * qdialogparams_writeanything.cpp
 *
 * 参数界面的写入任意参数弹出对话框
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qdialogparams_writeanything.h"
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QIcon>
#include "data/utils.h"
#include "QObject"

QDialogParams_WriteAnyThing::QDialogParams_WriteAnyThing(void* /*adapter*/, int /*offset*/, QWidget* parent,
        Qt::WindowFlags f): QDialog(parent, f)
{
    setWindowIcon(QIcon(":/images/images/NULL.ico"));
    auto layout_v = new QVBoxLayout(this);
    layout_v->addWidget(new QLabel(tr("确定执行此操作吗?"), this));

    auto buttonBox = Utils::createOkCancelButtons(this);
    layout_v->addWidget(buttonBox);
}

QString QDialogParams_WriteAnyThing::getStringFromData(const QVariant& val)
{
    Q_UNUSED(val);

    //return QString("执行");
    return tr("执行");
    //return QT_TR_NOOP("执行");
}
