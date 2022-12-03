/*
 * subwindowparams.h
 *
 * 参数界面
 *
 * Created on: 2019年 01月 21日 星期一 15:00:16 CST
 * Author: lixingcong
 */
#ifndef SUBWINDOWPARAMS_H
#define SUBWINDOWPARAMS_H

#include <QWidget>
#include <QVBoxLayout>

class QWidgetShowParamsBase;

class SubWindowParams : public QWidget
{
    Q_OBJECT

public:
    explicit SubWindowParams(QWidget* parent = nullptr);
    ~SubWindowParams();

protected slots:
    void onChangedSection(int index);

protected:
    QVBoxLayout* layout_v;
    QWidgetShowParamsBase* widget;

signals:
    void openOffset(int);
    void moveAxis(int axisNum, int direction, int pressType, double distance);

public slots:
    void onMoveAxis(int axisNum, int direction, int pressType, double distance);
    void onOpenOffset(int);
};
#endif // SUBWINDOWPARAMS_H
