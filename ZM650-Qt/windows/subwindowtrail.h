/*
 * subwindowtrail.h
 *
 * 轨迹子窗口，支持设置各种参数
 *
 * Created on: 2019年 01月 23日 星期三 22:10:41 CST
 * Author: lixingcong
 */

#ifndef SUBWINDOWTRAIL_H
#define SUBWINDOWTRAIL_H

#include <QWidget>
#include <Teach.h>

namespace Ui
{
class SubWindowTrail;
}

class SubWindowTrail : public QWidget
{
    Q_OBJECT

public:
    explicit SubWindowTrail(QWidget* parent = nullptr);
    ~SubWindowTrail();

public slots:
    void onUpdateRealtimeInfo();

protected slots:
    void _onClickedBottomButton(int btn_id);
    void pickUpPositionData();
    void onAcceptSetAxis();
    void onSetScale();

signals: // 私有信号
    void _setCurrentPoint(int pointIndex);

private:
    Ui::SubWindowTrail* ui;
    float positions[MAXTEACHNUM][2]; // 加工点坐标
    int pointColors[MAXTEACHNUM]; // 加工点颜色
    bool isLoaded;
    bool isFollowing;
    float size;//缩放倍数
};

#endif // SUBWINDOWTRAIL_H
