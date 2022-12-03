/*
 * qdialogparams_writeanything.h
 *
 * 参数界面的写入任意参数弹出对话框
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QDIALOGPARAMS_WRITEANYTHING_H
#define QDIALOGPARAMS_WRITEANYTHING_H

#include <QDialog>
#include "data/adapters.h"

class QDialogParams_WriteAnyThing : public QDialog
{
public:
    explicit QDialogParams_WriteAnyThing(void* /*adapter*/, int /*offset*/, QWidget* parent = nullptr,
                                         Qt::WindowFlags f = 0);

    static QString getStringFromData(const QVariant& val);
};

#endif // QDIALOGPARAMS_WRITEANYTHING_H
