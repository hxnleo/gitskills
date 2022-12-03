/*
 * qwidgetaxisposition.h
 * 
 * 实时坐标模块
 * 
 * Created on: 2019年 02月 26日 星期二 11:46:59 CST
 * Author: lixingcong
 */

#ifndef QWIDGETAXISPOSITION_H
#define QWIDGETAXISPOSITION_H

#include <QWidget>
#include <QList>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

class QTimer;

class QWidgetAxisPosition: public QWidget
{
    Q_OBJECT
public:
    explicit QWidgetAxisPosition(QWidget* parent=nullptr,bool screenflg= false, Qt::WindowFlags f = 0);

    void init(int axisCount, const QStringList& axisName,Qt::Alignment align1=Qt::AlignCenter, Qt::Alignment align2=Qt::AlignCenter);
    void initScreen(int axisCount, const QStringList& axisName, Qt::Alignment align=Qt::AlignCenter);

    void setLayoutSpacing(int spacingH, int spacingV);
    void setLabelCSS(const QString& css);

signals:
    void doubleClicked();

public slots:
    void updatePostion();

protected: // members
    QList<QLabel*> label0Created;
    QList<QLabel*> label1Created;
    QList<QLabel*> label2Created;
    QVBoxLayout* widgetLayout;
    QGridLayout* mainLayout;
    QStringList axisName;
    int axisCount;
    QTimer* timer;
    QStringList fontList;

protected:
    void paintEvent(QPaintEvent* event)override;
    void mousePressEvent(QMouseEvent* event)override;
    void mouseReleaseEvent(QMouseEvent* event)override;
};

#endif // QWIDGETAXISPOSITION_H
