/*
 * qdialogparams_patterns.h
 *
 * 花样图片显示，参数显示
 *
 * Created on: 2019年 01月 26日 星期六 17:05:50 CST
 * Author: lixingcong
 */

#ifndef QDIALOGPARAMS_PATTERNS_H
#define QDIALOGPARAMS_PATTERNS_H

#include <QDialog>
#include "data/adapters.h"
#include "data/patterns.h"
#include <QKeyEvent>
#include <QPaintEvent>


class QPaintEvent;
class QPixmap;

class QDialogParams_Patterns : public QDialog
{
    Q_OBJECT

public:
    explicit QDialogParams_Patterns(Adapters::DataAdapter_Patterns* adapter, int offset, QWidget* parent = nullptr,
                                    Qt::WindowFlags f = 0);

    static QString getStringFromData(const QVariant& val);

protected:
    Adapters::PatternItem pattern;
    Adapters::DataAdapter_Patterns* adapter;
    int offset;
    QPixmap pixmap;
    qreal widgetAspectRatio;
    qreal pixmapAspectRatio;
    QList<QWidget*> createdWidget;


protected:
    void keyPressEvent(QKeyEvent* evt)override
    {
        if(evt->key() == Qt::Key_Enter || evt->key() == Qt::Key_Return)
            return;

        QDialog::keyPressEvent(evt);
    }

    void paintEvent(QPaintEvent* event)override;

public slots:
    void accept();
};

#endif // QDIALOGPARAMS_PATTERNS_H
