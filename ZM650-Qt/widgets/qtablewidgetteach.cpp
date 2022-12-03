/*
 * qtablewidgetteach.cpp
 *
 * 教导列表
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qtablewidgetteach.h"
#include <QHeaderView>
#include <QStringList>
#include <QMouseEvent>
#include <QStandardItemModel>
#include "qstyleditemdelegateteachbase.h"
#include "qstyleditemdelegateteachaxis.h"
#include "qstyleditemdelegateteachcombobox.h"
#include "qstyleditemdelegateteachcylinder.h"
#include "qstyleditemdelegateteachspeed.h"
#include "qstyleditemdelegateteachpushbutton.h"
#include <QBitArray>
#include "Teach.h"
#include "Parameter.h"
#include "qitemmodelteach.h"
#include <QMessageBox>
#include "data/globaldata.h"
#include "data/utils.h"
#include "data/utilsforcontroller.h"
#include "data/workfile.h"
#include "data/definitions.h"
#include <QDebug>

//#include <qmath.h>

QTableWidgetTeach::QTableWidgetTeach(QWidget* parent): QTableView(parent),
    itemModel(nullptr)
{

}

void QTableWidgetTeach::init(int countEachPage, int totalPointNum, int maxPoint, int axisCount,
                             QStringList& axisName, int fontSize)
{
    //样式设置
    _setWidgetStyle(fontSize);
    // variables
    this->pointIndex = 0;
    this->currentPage = 0;
    this->maxPoint = maxPoint;
    this->totalPointNum = totalPointNum;

    if(countEachPage <= 0)
        this->countEachPage = 5;
    else
        this->countEachPage = countEachPage;

    this->maxPage = maxPoint / this->countEachPage; //qCeil(static_cast<qreal>(maxPoint)/this->countEachPage);

    // TODO: destroy previous layout
    this->axisCount = axisCount;
    this->axisName = axisName;

    reloadItemModel();

    QStyledItemDelegateTeachBase* delegate = nullptr;

    // set delegates for rows [AXIS]
    for(auto row = 0; row < axisCount; ++row)
    {
        delegate = new QStyledItemDelegateTeachAxis(itemModel->rowCount(), this);
        setItemDelegateForRow(row, delegate);//行代理
        connect(this, SIGNAL(_selectedColumnChanged(int)),
                delegate, SLOT(onSelectedColumnChanged(int)));
    }

    // set delegates for rows [COLOR] [SPEED] [OUTPUT]
#ifdef Arm_Kaiteli
    auto colorList=QStringList({tr("关"),tr("开")});
    delegate = new QStyledItemDelegateTeachPushButton(itemModel->rowCount(),
                                                      colorList, this);
#else
    QStringList colorList;
	if(SYS_ZM.bThreeColor)
        colorList=QStringList({tr("不变"),tr("毛1"), tr("毛2"), tr("皮")});
    else
        colorList=QStringList({tr("不变"),tr("毛"), tr("皮")});
   /* if(SYS_ZM.bThreeColor)
        colorList<<tr("颜色3");*/
    delegate = new QStyledItemDelegateTeachComboBox(itemModel->rowCount(),
                                                    colorList, this);
#endif
#ifdef Arm_662
    //颜色
    setItemDelegateForRow(itemModel->rowCount() -4, delegate);
    connect(this, SIGNAL(_selectedColumnChanged(int)),
            delegate, SLOT(onSelectedColumnChanged(int)));

    auto fluffList=QStringList({tr("关"), tr("开")});
    //拍毛
    delegate = new QStyledItemDelegateTeachComboBox(itemModel->rowCount(),
                                                    fluffList, this);
    setItemDelegateForRow(itemModel->rowCount() -3, delegate);
    connect(this, SIGNAL(_selectedColumnChanged(int)),
            delegate, SLOT(onSelectedColumnChanged(int)));

    //速度
    delegate = new QStyledItemDelegateTeachSpeed(itemModel->rowCount(), this);
    setItemDelegateForRow(itemModel->rowCount() -2, delegate);
    connect(this, SIGNAL(_selectedColumnChanged(int)),
            delegate, SLOT(onSelectedColumnChanged(int)));

    auto stopList=QStringList({tr("关"), tr("开")});
    //停车

    delegate = new QStyledItemDelegateTeachComboBox(itemModel->rowCount(),
                                                    stopList, this);
    setItemDelegateForRow(itemModel->rowCount() -1, delegate);
    connect(this, SIGNAL(_selectedColumnChanged(int)),
            delegate, SLOT(onSelectedColumnChanged(int)));
#else
    setItemDelegateForRow(itemModel->rowCount() - 3, delegate);//轴 颜色 速度 气缸
    connect(this, SIGNAL(_selectedColumnChanged(int)),
            delegate, SLOT(onSelectedColumnChanged(int)));

    delegate = new QStyledItemDelegateTeachSpeed(itemModel->rowCount(), this);
    setItemDelegateForRow(itemModel->rowCount() - 2, delegate);
    connect(this, SIGNAL(_selectedColumnChanged(int)),
            delegate, SLOT(onSelectedColumnChanged(int)));

    delegate = new QStyledItemDelegateTeachCylinder(itemModel->rowCount(), 6, this);
    setItemDelegateForRow(itemModel->rowCount() - 1, delegate);
    connect(this, SIGNAL(_selectedColumnChanged(int)),
            delegate, SLOT(onSelectedColumnChanged(int)));
    connect(delegate, SIGNAL(notifyTowriteCylinder(int, int)),
            this, SLOT(_onNotifyTowriteCylinder(int, int)));
#endif
    


    // click on headers
    connect(horizontalHeader(), SIGNAL(sectionClicked(int)),
            this, SLOT(onClickHeader(int)));
}

void QTableWidgetTeach::getStartIndexFromPage(int pageNum, int* startIndex, int* pointCount)
{//每次去去当前页的是数据  我们系统默认为5个 最后页为最后页的数据个数
    if(pageNum < 0 || pageNum >= maxPage)
        return;

    if(0 == totalPointNum)
        *startIndex = -1;
    else
        *startIndex = pageNum * countEachPage;

    auto nextPageStartIndex = *startIndex + countEachPage;

    if(nextPageStartIndex >= maxPoint)
        *pointCount = maxPoint - *startIndex;
    else
        *pointCount = countEachPage;
}

void QTableWidgetTeach::onNewPointIndexChanged(int pointIndex)
{//表格item处理
    if(pointIndex >= maxPoint || pointIndex < 0)
        return;

    int col = (pointIndex) % itemModel->columnCount();

    emit _selectedColumnChanged(col);//修改选框

    int newPageNum = (pointIndex) / this->countEachPage;

    if(newPageNum != this->currentPage)
    {
        emit _currentPageChanged(newPageNum);
        this->currentPage = newPageNum;

        //update after setting new pageNum
        emit reloadPageTeachData();
    }

    // update the index
    this->pointIndex = pointIndex;
    // debug_printf("[click] new PointIndex=%d",getPointIndex());

    // repaint the UI
    viewport()->update();
}

void QTableWidgetTeach::onTotalPointNumChanged(int totalPointNum)
{
    this->totalPointNum = totalPointNum;

    if(nullptr != itemModel)
        itemModel->onTotalPointChanged(totalPointNum);
}

void QTableWidgetTeach::onPageChanged(int newPageNum)
{
    if(newPageNum < 0 || newPageNum >= maxPage)
        return;

    auto pageOffset = pointIndex % countEachPage;
    auto newPointIndex = pageOffset + newPageNum * countEachPage;

    if(newPointIndex >= totalPointNum)
        newPointIndex = totalPointNum - 1;

    onNewPointIndexChanged(newPointIndex);
}

void QTableWidgetTeach::onClickHeader(int headerIndex)
{
    auto newIndex = itemModel->getPointIndexFromColumn(headerIndex);

    //    if(newIndex>totalPointNum- 1)
    //        return;
    if(newIndex != this->pointIndex)
    {
        onNewPointIndexChanged(newIndex);
        return;
    }

    //qDebug("click header: %d",headerIndex);
    auto clickedIndex = headerIndex + countEachPage * currentPage;
    QMessageBox::StandardButton reply =
            Utils::MyMessageBox(QString("运动到..."), tr("运动到孔") + QString("%1").arg(clickedIndex),
                                QObject::tr("确定"),QObject::tr("取消"),this);

    if(reply == QMessageBox::Retry)
    {
        emit pmoveToPoint(clickedIndex);
    }
}

void QTableWidgetTeach::reloadItemModel()
{
    // destroy previous model
    if(nullptr != itemModel)
    {
        itemModel->deleteLater();
        itemModel = nullptr;
    }

    // create model
    itemModel = new QItemModelTeach(axisCount, axisName, countEachPage, Workfile::getInstance()->getTeachData(),
                                    totalPointNum, maxPoint, this);
    setModel(itemModel);
    connect(this, SIGNAL(_currentPageChanged(int)),
            itemModel, SLOT(onCurrentPageChanged(int)));

    connect(itemModel, SIGNAL(dataChanged(QModelIndex, QModelIndex)),
            this, SLOT(_onDataChanged(QModelIndex, QModelIndex)));

    // update to latest page
    itemModel->onCurrentPageChanged(getCurrentPageNum());
}

void QTableWidgetTeach::_onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    Q_UNUSED(bottomRight)
    auto p = itemModel->getPointIndexFromColumn(topLeft.column());
    //debug_printf("QTableWidgetTeach::_onDataChanged (%d,%d), point=%d, val=%f",topLeft.row(),topLeft.column(),p ,topLeft.data().toDouble());

    emit pointDataChangedByUserInput(pointIndex);
}

void QTableWidgetTeach::_onNotifyTowriteCylinder(int cylinderIndex, int status)
{
    //debug_printf("write cylinder %d, status=%d",cylinderIndex, status);
    emit notifyTowriteCylinder(cylinderIndex, status);
}

void QTableWidgetTeach::mousePressEvent(QMouseEvent* event)
{
#if 1
    QWidget::mousePressEvent(event);//传递给父类
//    qDebug()<<"QTableWidgetTeach::mousePressEvent"<<event->pos()
    if(event->button() == Qt::LeftButton)
    {
        QModelIndex index = indexAt(event->pos());
        int pointIndex = index.column() + countEachPage * currentPage; //FIXME +1

        if(pointIndex < totalPointNum+1)  // column you want to use for one click
        {
            if(pointIndex == totalPointNum)
            {//制作在最后孔后面添加孔位
                if(GlobalData::swithcolumn == totalPointNum)
                    emit addNewPiont();
                else
                    emit _selectedColumnChanged(totalPointNum);//修改选框

                //qDebug()<<"++"<<pointIndex;

                return;
            }
            if(index.row() != axisCount) // 不是“颜色”的行
            {
                if(pointIndex != this->pointIndex)
                {//除了最后点位都走这里
                    onNewPointIndexChanged(pointIndex);
                    setCurrentIndex(index);
                }
                else
                {
                    if(true == Utils::checkWorkstatusIfStop())
                        edit(index);
                }
            }
            else   // 颜色后教导数据
            {
                onNewPointIndexChanged(pointIndex);
                setCurrentIndex(index);

                if(true == Utils::checkWorkstatusIfStop())
                    edit(index);
            }
        }

        return;
    }

#endif
    QTableView::mousePressEvent(event);

}

void QTableWidgetTeach::mouseReleaseEvent(QMouseEvent* event)
{
    QWidget::mouseReleaseEvent(event);//传递给父类
}

void QTableWidgetTeach::_setWidgetStyle(int fontSize)
{
    // over-write ui settings
    QFont font;
    font.setPointSize(fontSize);

    this->setFont(font);
    verticalHeader()->setFont(font);
    horizontalHeader()->setFont(font);

    horizontalHeader()->setVisible(true);
    verticalHeader()->setVisible(true);

    verticalHeader()->setResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setStyleSheet("QTableView QHeaderView::section{background-color:#B0B4E8;color:white}");
    //verticalHeader()->setStyleSheet("QTableView QHeaderView::section{background-color:#FFFAFA}");
    //this->setStyleSheet("QTableCornerButton::section{background-color:red;}");
    this->setStyleSheet("QTableView{gridline-color:gray;}");



    setSelectionMode(QAbstractItemView::NoSelection);
}

QModelIndex QTableWidgetTeach::moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    auto index = currentIndex();

    switch(cursorAction)
    {
    case MoveNext:
        if(index.row() < axisCount - 1)
            return QTableView::moveCursor(MoveDown, modifiers);
        else
            return index;

        break;

    case MovePrevious:
        if(index.row() > 0)
            return QTableView::moveCursor(MoveUp, modifiers);
        else
            return index;

        break;

    default:
        break;
    }

    return QTableView::moveCursor(cursorAction, modifiers);
}
