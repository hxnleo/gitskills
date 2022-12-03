/*
 * subwindowdiagnose.h
 *
 * 诊断界面
 *
 * Created on: 2019年 01月 21日 星期一 15:00:16 CST
 * Author: lixingcong
 */
#ifndef SUBWINDOWDIAGNOSE_H
#define SUBWINDOWDIAGNOSE_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class QWidgetShowParams_IOStatus;

class SubWindowDiagnose : public QWidget
{
    Q_OBJECT
public:
    explicit SubWindowDiagnose(QWidget* parent = nullptr);
    ~SubWindowDiagnose();

protected slots:
    void onChangedSection(int index);

protected:
    QVBoxLayout* layout_v;
    QWidget* widget;
};

#endif // SUBWINDOWDIAGNOSE_H
