/*
 * qwidgetdrawtrail.cpp
 *
 * 轨迹绘图Widget，支持反转坐标轴，拖动，缩放
 *
 * Created on: 2019年 01月 23日 星期三 15:30:46 CST
 * Author: lixingcong
 */

#include "qwidgetdrawtrail.h"
#include <QLabel>
#include <QPainter>
#include <QMouseEvent>
#include <QPen>
#include <qmath.h>
#include <QDebug>
#include "data/uistyles.h"
#include "data/utils.h"
#include "data/globaldata.h"
#include "data/workfile.h"
#include "data/adapters.h"
#include <QTimer>
#define Pi 3.1415926
#define  SCALE (360/g_Sysparam.AxisParam[GlobalData::selectedAxisNum[1]].ScrewPitch)
QWidgetDrawTrail::QWidgetDrawTrail(QWidget* parent) : QWidget(parent),
    totalPoint(0),
    drawStage(Stage_Idle),
    scaleRatio(1),
    widgetAspectRatio(1),
    highlightIndex(-1),
    zoomStep(1.2),
    timer(new QTimer(this)),
    presstimer(new QTimer(this)),
    lastcolor(Qt::NoBrush),
    prescurrindex(EMPTYINDEX),
    circlesize(4.0)
{
    scaleDirection[0] = 1;
    scaleDirection[1] = 1;

    createPixmap(rect());

    timer->setSingleShot(true);
    timer->setInterval(300); // 双击间隔

    //返回按下的坐标index
    connect(presstimer, SIGNAL(timeout()), //按压时间
            this, SLOT(getPressPoint()));
    presstimer->setInterval(500); // 更新间隔
    recoverScaleCenter();
}

void QWidgetDrawTrail::init(float (*pos)[2], int* colorArray,float size)
{
    this->pos = pos;
    this->totalPoint = 0;
    this->colorArray = colorArray;
    this->circlesize = size;
}

void QWidgetDrawTrail::setTotalPoint(int totalPoint)
{
    realpoint.clear();
    for(int i=0;i<totalPoint;i++)
    {//保存在链表中
        realpoint.insert(i,QPointF(*(*(pos + i) + 0),*(*(pos + i) + 1)));
        //        qDebug()<<i<<realpoint.at(i)<<*(*(pos + i) + 0)<<*(*(pos + i) + 1);
    }
    //qDebug()<<"realpoint"<<realpoint.size();//链表大小
    this->totalPoint = totalPoint;
    if(Stage_Idle == drawStage)
    {
        drawStage = Stage_DrawPoints;
        update();
    }
}

void QWidgetDrawTrail::recoverScaleCenter()
{
    posevent = QPoint(EMPTYINDEX,EMPTYINDEX);
}

void QWidgetDrawTrail::setScalesize(float size)
{
    zoomStep = size;
}

void QWidgetDrawTrail::setScale(bool isZoomIn, bool isForceSetOrigin)
{
    auto scaleRatioLast = scaleRatio;

    if(true == isZoomIn)
        scaleRatio *= zoomStep;
    else
        scaleRatio /= zoomStep;

    // 强制设置原点
    if(true == isForceSetOrigin)
    {
        //设置缩放中心
        QPoint widgetCenterPoint;
        if(posevent == QPoint(EMPTYINDEX,EMPTYINDEX))
            widgetCenterPoint = rect().center();
        else
            widgetCenterPoint = posevent;

        auto deltaX = originAfterMoving.x() - widgetCenterPoint.x();
        auto deltaY = originAfterMoving.y() - widgetCenterPoint.y();

        auto newDeltaY = scaleRatio * deltaY / scaleRatioLast;
        auto newDeltaX = scaleRatio * deltaX / scaleRatioLast;

        originAfterMoving.setY(originAfterMoving.y() + newDeltaY - deltaY);
        originAfterMoving.setX(originAfterMoving.x() + newDeltaX - deltaX);

        originBeforeMoving = originAfterMoving;
    }

    //qDebug("ratio=%lf",scaleRatio);
    drawStage = Stage_DrawPoints;
    update();
}

void QWidgetDrawTrail::setCoordText(const QString& textH, const QString& textV)
{
    coordText[0] = textH;
    coordText[1] = textV;
    update();
}

void QWidgetDrawTrail::clearcurr()
{
    prescurrindex = EMPTYINDEX;
}

void QWidgetDrawTrail::setCoordDirectoin(bool isPositiveH, bool isPositiveV)
{
    scaleDirection[0] = isPositiveH ? 1 : -1;
    scaleDirection[1] = isPositiveV ? 1 : -1;
    drawStage = Stage_DrawPoints;
    update();
}

void QWidgetDrawTrail::resetToFit()
{
    //qDebug()<<"rect"<<rect();
    if(totalPoint <= 1)
        return;

    float X[2] = {LARGE_NUMBER_10, -LARGE_NUMBER_10}; // 数组下标0保存最小值，下标1保存最大值
    float Y[2] = {LARGE_NUMBER_10, -LARGE_NUMBER_10};

    auto compareAndSave = [](const float inValue, float * outValue, bool isGetLargerOne)
    {
        if(isGetLargerOne)
        {
            // 取较大值
            if(*outValue < inValue)
                *outValue = inValue;
        }
        else
        {
            // 取较小值
            if(*outValue > inValue)
                *outValue = inValue;
        }
    };

    for(auto i = 0; i < totalPoint; ++i)
    {
        // 取较小值
        compareAndSave(*(*(pos + i) + 0), &X[0], false);
        compareAndSave(*(*(pos + i) + 1), &Y[0], false);

        // 取较大值
        compareAndSave(*(*(pos + i) + 0), &X[1], true);
        compareAndSave(*(*(pos + i) + 1), &Y[1], true);
        //        qDebug()<<"坐标"<<totalPoint<<*(*(this->pos + i))<<*(*(this->pos + i)+1);
    }
    //    获得图像边界
    //    qDebug("X0=%f, X1=%f",X[0],X[1]);
    //    qDebug("Y0=%f, Y1=%f",Y[0],Y[1]);

    qreal rectWidth = X[1] - X[0];
    qreal rectHeight = Y[1] - Y[0];

    // 实际宽高比
    qreal rectAspectRatio = rectWidth / rectHeight;
    qreal newScaleRatio = 1;

    //qDebug("rectAspectRatio=%f",rectAspectRatio);
    //Debug("widget w=%d, h=%d",width(),height());
#define FILLEMTPY_RATE 0.9

    //计算实际距离
    QPointF distance;
    distance.setX((X[1]+X[0])/2);
    distance.setY((Y[1]+Y[0])/2);
    //图像比例
    if(rectAspectRatio > widgetAspectRatio)
    {
        // 图像内容更宽
        newScaleRatio = width() / rectWidth * FILLEMTPY_RATE;
    }
    else
    {
        // 图像内容更高
        newScaleRatio = height() / rectHeight * FILLEMTPY_RATE;

    }
    //偏移距离= 坐标中心×图像比例
    distance.setX(distance.x()*newScaleRatio);
    distance.setY(distance.y()*newScaleRatio);
    //qDebug()<<"distance"<<distance;
    // 计算新原点
#define FILLEMTPY_ORIGIN_OFFSET 0
    // 偏移
    qreal offsetX = 0;
    qreal offsetY = 0;
    if(scaleDirection[0] > 0)
    {
        offsetX = distance.x();
    }
    else
    {
        offsetX = -distance.x();
    }

    if(scaleDirection[1] > 0)
    {
        offsetY = distance.y();
    }
    else
    {
        offsetY = -distance.y();
    }
    qDebug()<<offsetX<<offsetY<<newScaleRatio;
    //rect中心原点+实际图片的坐标中心偏移
    if(distance.x()>0)
    {//X+
        originAfterMoving.setX(width()/2-offsetX);
    }
    else
    {//X-
        originAfterMoving.setX(width()/2-offsetX);
    }
    if(distance.y()>0)
    {//Y+
        originAfterMoving.setY(height()/2+offsetY);
    }
    else
    {//Y-
        originAfterMoving.setY(height()/2+offsetY);
    }
    originBeforeMoving = originAfterMoving;
    //qDebug()<<"中心坐标位置"<<distance<<width()/2<<height()/2<<originAfterMoving<<newScaleRatio;
    //qDebug("newScaleRatio=%lf, zoomStep=%lf",newScaleRatio,zoomStep);
    scaleRatio = newScaleRatio;
    drawStage = Stage_DrawPoints;
    lastmove.setX(0);
    lastmove.setY(0);
    update();
}

void QWidgetDrawTrail::setHighlightIndex(int index)
{
    static int lastHighlightIndex = -9999;

    if(index<0 || index>=totalPoint)
        return;

    highlightIndex = index;

    if(lastHighlightIndex != index)
    {
        lastHighlightIndex = highlightIndex;

        if(Stage_Idle == drawStage)
        {
            drawStage = Stage_DrawPoints;
            update();
        }
    }
}

void QWidgetDrawTrail::zoomIn()
{
    setScale(true);
}

void QWidgetDrawTrail::zoomOut()
{
    setScale(false);
}

void QWidgetDrawTrail::paintEvent(QPaintEvent*)
{
    if(false == mutexPixmap.tryLock())
        return;

    QPainter painter(this);
    QPixmap* pPixmap = nullptr;

    switch(drawStage)
    {
    case Stage_Idle:
    {
        pPixmap = &pixmap;
        break;
    }

    case Stage_DrawPoints:
    {//初始
        pPixmap = &pixmap;
        pPixmap->fill(Qt::white);

        QPainter pp(pPixmap);
        // coords 坐标系
        drawCoordinates(pp, originFixed, originVerticalEnd, originHorizontalEnd);
        drawStage = Stage_Idle;
        pp.save();
        if(GlobalData::slectetype)//修改此处解决 平移回归原位问题
            transformPainter(pp, rect().center()); // 缩放与平移
        else
            transformPainter(pp, originAfterMoving); // 缩放与平移
        //qDebug()<<"originAfterMoving_起始状态"<<originAfterMoving;
        // points
        drawAllPoints(pp, validDrawRect,originAfterMoving,prescurrindex);//size为0.8 出现卡死问题
        pp.restore();


        break;
    }

    case Stage_Dragging:
    {//平移
        pPixmap = &pixmap;
        pPixmap->fill(Qt::white);

        auto moveRect = QRect(lastPoint, endPoint);
        //移动后的
        if(GlobalData::slectetype)
            originAfterMoving = QPointF(rect().center().x() + moveRect.width()+lastmove.width(), rect().center().y() + moveRect.height()+lastmove.height());
        else
            originAfterMoving = QPointF(originBeforeMoving.x() + moveRect.width(), originBeforeMoving.y() + moveRect.height());
        //qDebug()<<"originAfterMoving_鼠标移动后"<<originAfterMoving;
        QPainter pp(pPixmap);

        // coords
        drawCoordinates(pp, originFixed, originVerticalEnd, originHorizontalEnd);
        pp.save();
        transformPainter(pp, originAfterMoving); // 缩放与平移

        // points
        drawAllPoints(pp, validDrawRect, originAfterMoving,prescurrindex);
        pp.restore();
        lastmove = QRect(lastPoint, endPoint);
        break;
    }
    case Stage_DraggingEnd:
    {
        originBeforeMoving = originAfterMoving;
        drawStage = Stage_Idle;
        update();
        break;
    }

    default:
        break;
    }

    if(pPixmap)
        painter.drawPixmap(0, 0, pPixmap->width(), pPixmap->height(), *pPixmap);

    mutexPixmap.unlock();
}

void QWidgetDrawTrail::mouseMoveEvent(QMouseEvent* event)
{
    if(event->buttons()&Qt::LeftButton) //鼠标左键按下的同时移动鼠标
    {
        //        GlobalData::Debugtext=QString("移动了");
        qDebug()<<endPoint;
        endPoint = event->pos();
        update();
    }
}

void QWidgetDrawTrail::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton) //鼠标左键按下
    {
        posevent.setX(event->posF().x());
        posevent.setY(event->posF().y());
        presstimer->start();//开始按压
        lastPoint = event->pos();
        drawStage = Stage_Dragging; //正在绘选择框

        if(timer->isActive())
        {
            // 双击
            timer->stop();
            emit doubleClicked();
        }else{
            timer->start();
        }
    }
    QWidget::mousePressEvent(event);
}

void QWidgetDrawTrail::mouseReleaseEvent(QMouseEvent* event)
{
    presstimer->stop();//开始停止
    if(event->button() == Qt::LeftButton) //鼠标左键释放
    {
        if(prescurrindex==EMPTYINDEX)
        {
            endPoint = event->pos();
            drawStage = Stage_DraggingEnd;  //结束绘选择框
            update();
        }
    }
    QWidget::mouseReleaseEvent(event);
}

void QWidgetDrawTrail::getPressPoint()
{//得到区域内的点  如果达到丝的级别识别不了
    int tempindex = prescurrindex;//纪录上一次数据
    prescurrindex = EMPTYINDEX;//清除上一次数据
    presstimer->stop();//开始停止
    //映射到实际坐标点的点位
    presscoord = QPoint(posevent.x()-originAfterMoving.x(),originAfterMoving.y()-posevent.y())/scaleRatio;
    //得到鼠标默认矩形大小 按比例映射到实际坐标矩形大小
    //范围 = （鼠标点-坐标原点）*比例 ->对点取方框 QRECTSIZE
    pointrect = QRectF(QPoint(posevent.x(),-posevent.y())/scaleRatio
                       -QPointF(QRECTSIZE/2,QRECTSIZE/2)
                       ,QSizeF(QRECTSIZE,QRECTSIZE));
    QVector<double> distance;
    int temp = 0;
    float tempx,tempy;
    QRectF temppoint(pointrect);
    for(int i = 0; i < totalPoint; i++)
    {
        if(GlobalData::slectetype)
        {
            tempx = realpoint.at(i).x()*SCALE*cos(realpoint.at(i).y()*SCALE*Pi/180);
            tempy = realpoint.at(i).x()*SCALE*sin(realpoint.at(i).y()*SCALE*Pi/180);
            temppoint.setX(pointrect.x()*SCALE*cos(realpoint.at(i).y()*SCALE*Pi/180));
            temppoint.setY(pointrect.x()*SCALE*sin(realpoint.at(i).y()*SCALE*Pi/180));
            temppoint.setSize(QSizeF(1,1));
            //qDebug()<<tempx<<tempy<<pointrect<<temppoint<<cos(realpoint.at(i).y()*Pi/180);
        }
        else
        {
            tempx = realpoint.at(i).x();
            tempy = realpoint.at(i).y();
        }
        //qDebug()<<i<<realpoint.at(i).x()*cos(realpoint.at(i).y()*Pi/180)<<realpoint.at(i).x()*sin(realpoint.at(i).y()*Pi/180)<<realpoint.at(i).x()<<realpoint.at(i).y()<<pointrect;
        if(tempx>=temppoint.left()&&tempx<=temppoint.right()
                &&tempy<=temppoint.bottom()&&tempy>=temppoint.top())
        {
            distance.insert(temp,Utils::TwoPtDistance(presscoord,realpoint.at(i)));
            temp++;

            //qDebug()<<"index"<<i+1<<"个数"<<temp<<realpoint.size()<<realpoint.at(i)<<tempx<<pointrect;
            GlobalData::Debugtext=QString();
        }
    }
    if(distance.size()==0)
    {
        prescurrindex = tempindex;
        return;
    }
    qSort(distance.begin(),distance.end());//从小到达排序
    if(temp>0)
    {
        for(int k=0;k<totalPoint;k++)
        {
            //qDebug()<<k<<Utils::absolute(Utils::TwoPtDistance(presscoord,realpoint.at(k)),distance.first());
            if(Utils::absolute(Utils::TwoPtDistance(presscoord,realpoint.at(k)),distance.first())<=1)
            {
                prescurrindex = k;
                //qDebug()<<"presscoord"<<prescurrindex<<posevent<<originAfterMoving<<presscoord<<pointrect;
                break;
            }
        }
    }
    drawStage = Stage_DrawPoints;
    update();
}

void QWidgetDrawTrail::resizeEvent(QResizeEvent* event)
{
    createPixmap(rect());
    drawStage = Stage_DrawPoints;
    update();
    resetToFit();
}

void QWidgetDrawTrail::wheelEvent(QWheelEvent* event)
{
#if 1
    // https://stackoverflow.com/questions/6650219/zooming-function-on-a-qwidget
    setScale(event->delta() > 0);
#else
    QWidget::wheelEvent(event);
#endif
}

void QWidgetDrawTrail::transformPainter(QPainter& painter, const QPointF& translatePoint)
{
    //    qDebug()<<"transformPainter"<<translatePoint;
    painter.translate(translatePoint);//设置逻辑坐标
    painter.scale(scaleDirection[0]*scaleRatio, scaleDirection[1]*scaleRatio);
}

//绘画对象 原点 V结束点 H结束点
void QWidgetDrawTrail::drawCoordinates(QPainter& painter, const QPointF& origin, const QPointF& originVEnd,
                                       const QPointF& originHEnd)
{
    // Coordinate axis line
    painter.setPen(QPen(Qt::black, 2));
    painter.drawLine(origin, originVEnd);
    painter.drawLine(origin, originHEnd);

    // Coordinate triangle
#define COORD_TRIANGLE_HEIGHT 12
#define COORD_TRIANGLE_WIDTH 12
    QPainterPath path;
    path.moveTo(originVEnd);
    path.lineTo(originVEnd.x() - COORD_TRIANGLE_WIDTH / 2, originVEnd.y() + COORD_TRIANGLE_HEIGHT);
    path.lineTo(originVEnd.x() + COORD_TRIANGLE_WIDTH / 2, originVEnd.y() + COORD_TRIANGLE_HEIGHT);
    path.lineTo(originVEnd);
    painter.fillPath(path, QBrush(Qt::black));

    path.moveTo(originHEnd);
    path.lineTo(originHEnd.x() - COORD_TRIANGLE_HEIGHT, originHEnd.y() - COORD_TRIANGLE_WIDTH / 2);
    path.lineTo(originHEnd.x() - COORD_TRIANGLE_HEIGHT, originHEnd.y() + COORD_TRIANGLE_WIDTH / 2);
    path.lineTo(originHEnd);
    painter.fillPath(path, QBrush(Qt::black));

    // Coordinate text
#define COORD_TEXT_MARGIN 8
    const QString directionText[2] = {"+", "-"};

    painter.drawText(originHEnd.x() + COORD_TEXT_MARGIN, originHEnd.y(), coordText[0]
            + (scaleDirection[0] > 0.0 ? directionText[0] : directionText[1]));

    painter.drawText(originVEnd.x(), originVEnd.y() - COORD_TEXT_MARGIN, coordText[1]
            + (scaleDirection[1] > 0.0 ? directionText[0] : directionText[1]));
}

// 参数：父类 范围 原点 当前选中索引
void QWidgetDrawTrail::drawAllPoints(QPainter& painter, const QRect& widgetRect, const QPointF& origin,int currindex)
{
    const QBrush brushCur(Qt::red);
    const QPen penLine(Qt::black, 1.0 / scaleRatio);
    const QPen penCoordDash(Qt::green, 1.0 / scaleRatio, Qt::DashLine);
    const qreal circleRadius = circlesize / scaleRatio;
    // 原点坐标轴(虚线)
    painter.setPen(penCoordDash);
#ifdef Q_OS_WIN // Windows
    painter.drawLine(0, -origin.y() / scaleRatio,
                     0, (widgetRect.height() - origin.y()) / scaleRatio); // 竖直线
    painter.drawLine(-origin.x() / scaleRatio, 0,
                     (widgetRect.width() - origin.x()) / scaleRatio, 0); // 水平线
#else // Linux
    //    painter.drawLine(0, -LARGE_NUMBER_10,
    //                     0, LARGE_NUMBER_10); // 竖直线
    //    painter.drawLine(-LARGE_NUMBER_10, 0,
    //                     LARGE_NUMBER_10, 0); // 水平线
#endif
    QPointF lastPoint;

    for(auto i = 0; i < totalPoint; ++i)
    {
        float x = *(*(this->pos + i) + 0);//(*x)[]
        float y = *(*(this->pos + i) + 1);
        //        qDebug()<<"坐标"<<totalPoint<<*(*(this->pos + i))<<*(*(this->pos + i)+1);
        QPointF point;
        if(GlobalData::slectetype)
        {
            point = QPointF(x*cos(y*SCALE*Pi/180), -x*sin(y*SCALE*Pi/180));
            point = point/SCALE*2;
            x = x*cos(y*SCALE*Pi/180);
            y = -x*sin(y*SCALE*Pi/180);
        }
        else
            point = QPointF(x, -y);

//        qDebug()<<"SCALE"<<SCALE<<"point"<<point;
#ifdef Q_OS_WIN // Windows
        auto xRealPixel = x * scaleRatio + origin.x();
        auto yRealPixel = -y * scaleRatio + origin.y();
        auto pointRealPixel = QPoint(xRealPixel, yRealPixel);

        if(i > 0 && true == widgetRect.contains(pointRealPixel))
        {
            auto pointDistance = (point.x() - lastPoint.x()) * (point.x() - lastPoint.x()) +
                    (point.y() - lastPoint.y()) * (point.y() - lastPoint.y());

#define MINIMAL_POINT_DISTANCE (3*3)

            if(pointDistance  > MINIMAL_POINT_DISTANCE)  // 两点间距少于x个像素则不绘制
            {
                painter.setPen(penLine);
                //painter.drawLine(lastPoint, point);
            }
        }

        lastPoint = point;

        //        if(false == widgetRect.contains(pointRealPixel))
        //            continue;

#else // Linux

        if(i > 0)
        {
            painter.setPen(penLine);
            painter.drawLine(lastPoint, point);
        }
        lastPoint = point;
#endif
        //qDebug()<<"color"<<*(colorArray + i);
        painter.setBrush(Qt::NoBrush);
        if(currindex!=-1)
        {
            if(currindex==i)
            {//对点亮色  输出文本信息
                QFont font;
                font.setPointSizeF(circleRadius*1.5);
                painter.setFont(font);
                painter.setBrush(brushCur);
                painter.drawText(point+QPoint(circleRadius*2,0), QString("%1 (%2 , %3)").arg(currindex).arg(x).arg(-y));
            }
        }
        else
        {
            auto color =*(colorArray + i);//溢出将造成死机
            if(Run == GlobalData::realTimeInfo.iWSta)       //2022年4月2日 tyx 运动状态区分
            {
                if(highlightIndex == i) // 当前点：填充为实心圆
                    painter.setBrush(brushCur);
                else if(highlightIndex > i)//前孔之前的
                {
                    if(color!=0)
                    {
                        painter.setBrush(UIStyles::COLOR_TRAIL.at(color));//设置之前的
                        lastcolor = UIStyles::COLOR_TRAIL.at(color);
                    }
                    else
                    {
                        painter.setBrush(lastcolor);
                    }
                }
            }
            else
            {
                if(color!=0)
                {
                    painter.setBrush(UIStyles::COLOR_TRAIL.at(color));//设置之前的
                    lastcolor = UIStyles::COLOR_TRAIL.at(color);
                }
                else
                {
                    painter.setBrush(lastcolor);
                }
            }
        }
        //painter.setBrush(Qt::NoBrush);

        //绘画单个空位显示打小
        if(i==0)
            painter.setPen(Qt::green);
        else
            painter.setPen(Qt::black);

        painter.drawEllipse(point, circleRadius, circleRadius);
        //painter.drawEllipse(point, 2.0, 2.0);
    }
}

void QWidgetDrawTrail::createPixmap(const QRect& validRect)
{
    mutexPixmap.lock();

    validDrawRect = validRect;
    // 画布扩大周边
#define VALID_RECT_PADDING (100)
    validDrawRect.setLeft(validDrawRect.left() - VALID_RECT_PADDING);
    validDrawRect.setRight(validDrawRect.right() + VALID_RECT_PADDING);
    validDrawRect.setTop(validDrawRect.top() - VALID_RECT_PADDING);
    validDrawRect.setBottom(validDrawRect.bottom() + VALID_RECT_PADDING);

    auto pixmapSize = validRect.size();
    pixmap = QPixmap(pixmapSize);
    pixmap.fill(Qt::white);

    widgetAspectRatio = static_cast<qreal>(pixmapSize.width()) / static_cast<qreal>(pixmapSize.height());

#define COORD_PADDING_PIXEL 6
#define COORD_LENGTH 50
    //设置原点

    originFixed = QPointF(COORD_PADDING_PIXEL, pixmapSize.height() - COORD_PADDING_PIXEL);
    //qDebug()<<"originFixed"<<originFixed;
    originBeforeMoving = originFixed;
    originAfterMoving = originFixed;

    originVerticalEnd = originFixed;
    originVerticalEnd.setY(originVerticalEnd.y() - COORD_LENGTH);

    originHorizontalEnd = originFixed;
    originHorizontalEnd.setX(originHorizontalEnd.x() + COORD_LENGTH);

    mutexPixmap.unlock();
}
