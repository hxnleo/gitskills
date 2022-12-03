/*
 * qwidgetteachstatus.h
 *
 * 教导界面左侧的状态显示
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QWIDGETTEACHSTATUS_H
#define QWIDGETTEACHSTATUS_H

#include <QWidget>
#include <QList>
#include "data/globaldata.h"
class QLabel;
class QPushButton;
class QStringList;
class QBoxLayout;

class QWidgetTeachStatus : public QWidget
{
    Q_OBJECT

public:
    explicit QWidgetTeachStatus(QWidget* parent = 0);
    ~QWidgetTeachStatus();

    void init(const QStringList& axisName, int axisCount);
    double getMoveStep()const
    {
        return TEACH_STEPS[stepIndex];
    }

signals:
    //void updateTeachStep(double step);
    void updateTeachStep(unsigned int step);
    void updatePositionWidget();
    void updateQuickteach();

public slots:
    void onUpdateAxisPosition();
    void onUpdateCurrentPointIndex(int index);
    void onUpdateTotalPoint(int totalPoint);
    void onSetTeachStepText();
    void onLinkage();
    void onUpdataLinkBtn();

protected slots:
    void _onClickedTeachStep();
    void _onQuickteaching();
    void _onUpatarQuick(bool);

protected:
    void paintEvent(QPaintEvent *e);
    bool _isInitialized;
    QStringList _axisName;
    int _axisCount;

    QBoxLayout* _layout;

    QLabel* _labelCurrentPointIndex;
    QLabel* _labelTotalPoint;
    QLabel* _labelMaixAngle;
    QPushButton* _btnTeachStep;
    QPushButton* _btnQuickteaching;
    QPushButton* pushButton_linkage;

    size_t stepIndex;
    static const double TEACH_STEPS[];

};

#endif // QWIDGETTEACHSTATUS_H
