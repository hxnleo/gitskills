/*
 * qitemmodelteach.h
 *
 * 教导列表的数据model，调用Adapter对接下位机
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QITEMMODELTEACH_H
#define QITEMMODELTEACH_H

#include <QStandardItemModel>
#include "Teach.h"

class QItemModelTeach : public QStandardItemModel
{
    Q_OBJECT
public:
    QItemModelTeach(int axisCount, QStringList& axisName, int countEachPage, TEACH_DATA* teachData, int totalPointNum,
                    int maxPoint, QObject* parent = 0);

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;

    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    int getPointIndexFromColumn(int column)const
    {
        return column + countEachPage * currentPage;
    }

public slots:
    void onCurrentPageChanged(int pageNum);
    void onTotalPointChanged(int totalPoint);

    // DataOperate.c
public slots:
    void refreshUI();

protected:
    int axisCount;

    int currentPage; // 当前页 0开始
    int countEachPage; // 每页个数
    int maxPoint; // 最大孔数
    int totalPointNum; // 总孔数

    QStringList axisName;

    TEACH_DATA* teachData;

    template<typename T1, typename T2>
    void compareAndSetValue(T1& oldVaule, const T2& newValue, bool* isEqual)
    {
        auto newValueOfT1 = static_cast<T1>(newValue);
        *isEqual = (oldVaule == newValueOfT1);

        if(false == *isEqual) // set old to new value
            oldVaule = newValueOfT1;
    }
};

#endif // QITEMMODELTEACH_H
