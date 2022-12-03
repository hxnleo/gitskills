#ifndef QITEMMODELDOG_H
#define QITEMMODELDOG_H

#include<QStandardItemModel>

class QItemModelDog : public QStandardItemModel
{
public:
    QItemModelDog(QObject* parent = 0);

    //QVariant data(const QModelIndex& index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QStringList m_strHeaderList;
};

#endif // QITEMMODELDOG_H
