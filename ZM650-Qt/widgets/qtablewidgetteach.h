/*
 * qtablewidgetteach.h
 *
 * 教导列表
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QTABLEWIDGETTEACH_H
#define QTABLEWIDGETTEACH_H

#include <QTableView>
#include <QList>

class QMouseEvent;
class QItemModelTeach;

class QTableWidgetTeach : public QTableView
{
    Q_OBJECT
public:
    explicit QTableWidgetTeach(QWidget* parent = nullptr);

    void init(int countEachPage, int totalPointNum, int maxPoint, int axisCount, QStringList& axisName,
              int fontSize = 7);
    int getPointIndex() const
    {
        return pointIndex;
    }
    int getTotalPointNum() const
    {
        return totalPointNum;
    }

    int getCurrentPageNum() const
    {
        return currentPage;
    }
    int getMaxPage() const
    {
        return maxPage;
    }

    void getStartIndexFromPage(int pageNum, int* startIndex, int* pointCount);

signals: // 内部信号
    void _selectedColumnChanged(int selectedColumn);
    void _currentPageChanged(int pageNum);


signals: // 外部信号
    void pmoveToPoint(int pointIndex);
    void pointDataChangedByUserInput(int pointIndex);
    void reloadPageTeachData();
    void notifyTowriteCylinder(int cylinderIndex, int status);
    void addNewPiont();

public slots: // 外部槽
    void onNewPointIndexChanged(int pointIndex);
    void onTotalPointNumChanged(int totalPointNum); // 总孔数发生改变
    void onPageChanged(int newPageNum);
    void onClickHeader(int headerIndex);
    void reloadItemModel();

protected slots: // 内部槽
    void _onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
    void _onNotifyTowriteCylinder(int cylinderIndex, int status);

protected: // members
    int axisCount;
    QStringList axisName;

    int pointIndex; // 当前孔 0开始
    int currentPage; // 当前页 0开始
    int maxPage; // 最大页数=最大孔数/每页个数
    int maxPoint; // 最大孔数
    int countEachPage; // 每页个数
    int totalPointNum; // 总孔数

    QItemModelTeach* itemModel;

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void _setWidgetStyle(int fontSize);
    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
};

#endif // QTABLEWIDGETTEACH_H
