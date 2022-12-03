/*
 * qwidgetdrawtrail.h
 *
 * 轨迹绘图Widget，支持反转坐标轴，拖动，缩放
 *
 * Created on: 2019年 01月 23日 星期三 15:30:46 CST
 * Author: lixingcong
 */

#ifndef QWIDGETDRAWTRAIL_H
#define QWIDGETDRAWTRAIL_H

#include <QWidget>
#include <QMutex>

class QPainter;
class QPoint;
class QMouseEvent;
class QTimer;

#define LARGE_NUMBER_10 10E10
#define QRECTSIZE 10//矩形范围
#define EMPTYINDEX -1

class QWidgetDrawTrail : public QWidget
{
protected:
    enum
    {
        Stage_Idle,
        Stage_Dragging,
        Stage_DraggingEnd,
        Stage_DrawPoints,
    };
    enum
    {
        No_Color,
        One_Color,
        Two_Color,
        Three_Color,
    };
    Q_OBJECT
public:
    explicit QWidgetDrawTrail(QWidget* parent = nullptr);
    void init(float (*pos)[2], int* colorArray, float size);

    // H: horizontal水平的 V: vertical垂直的
    void setCoordText(const QString& textH, const QString& textV);
    void setCoordDirectoin(bool isPositiveH, bool isPositiveV);
    void clearcurr();
    void setScalesize(float size);
    void recoverScaleCenter();

signals:
    void doubleClicked();

public slots: // 外部槽
    void resetToFit(); // 适应缩放
    void setHighlightIndex(int index); // 设置当前高亮点
    void setTotalPoint(int totalPoint); // 设置总点数
    void zoomIn(); // 放大
    void zoomOut(); // 缩小
    void getPressPoint();

protected: // override
    void paintEvent(QPaintEvent*)override;
    void mousePressEvent(QMouseEvent* event)override;
    void mouseMoveEvent(QMouseEvent*)override;
    void mouseReleaseEvent(QMouseEvent*)override;
    void resizeEvent(QResizeEvent*)override;
    void wheelEvent(QWheelEvent*)override;

protected: // draw
    void transformPainter(QPainter& painter, const QPointF& translatePoint);
    void drawCoordinates(QPainter& painter, const QPointF& origin, const QPointF& originVEnd, const QPointF& originHEnd);
    void drawAllPoints(QPainter& painter, const QRect& widgetRect, const QPointF& origin, int currindex = EMPTYINDEX);
    void createPixmap(const QRect& validRect);
    void setScale(bool isZoomIn, bool isForceSetOrigin = true);


protected:
    QRect lastmove;
    QPixmap pixmap;
    QPoint lastPoint;
    QPoint endPoint;
    int totalPoint;
    float (*pos)[2];
    int* colorArray;
    int drawStage;
    qreal scaleRatio;
    qreal scaleDirection[2];

    QPointF originBeforeMoving;
    QPointF originAfterMoving;
    QPointF originFixed;
    QPointF originVerticalEnd;
    QPointF originHorizontalEnd;

    QMutex mutexPixmap;
    qreal widgetAspectRatio; // 图像宽高比，例如(16:9)
    int highlightIndex;
    QString coordText[2];

    qreal zoomStep; // 放大倍数|缩小倍数。

    QRect validDrawRect; // 有效的绘图区域，只绘制在此区域有效的点
    QTimer* timer;
    QTimer* presstimer;
    QColor lastcolor; //保存颜色
    QPointF presscoord; //按压实际坐标
    QRectF pointrect; //默认鼠标点击为矩形范围
    QVector<QPointF> realpoint; //保存数据点位
//    QVector<QPointF> addpoint; //保存矩形区域内的点(假如一个区域内N个点)
    QPoint posevent;//获取鼠标press坐标
    int prescurrindex; //获取当前返回数据索引
//    bool moving; //鼠标是否处于滑动状态
    qreal circlesize; //绘圈大小
};

#endif // QWIDGETDRAWTRAIL_H
