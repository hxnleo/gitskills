#include "qitemmodeldog.h"

QItemModelDog::QItemModelDog(QObject *parent):QStandardItemModel(parent)
{
    m_strHeaderList<<"期数"<<"密码"<<"天数";
}

//QVariant QItemModelDog::data(const QModelIndex& index, int role) const
//{
//    if(!index.isValid())
//        return QVariant();

//    switch (role)
//    {
//    case Qt::TextAlignmentRole:
//        return QVariant(Qt::AlignCenter);
//    }
//    return QVariant();
//}

Qt::ItemFlags QItemModelDog::flags(const QModelIndex &index) const
{
    if (index.column() == 0)
        return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable ;
    else
        return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

QVariant QItemModelDog::headerData(int section, Qt::Orientation orientation, int role) const
{//设置表头

    if (orientation == Qt::Horizontal)
    {
        if (role == Qt::DisplayRole)
        {//设置表头显示
            if (section < 0 || section >= m_strHeaderList.size())
                return "";
            else
                return m_strHeaderList[section];
        }
//        if (role == Qt::SizeHintRole)
//        {
//            return QSize(5, 5);
//        }
    }
    else if (orientation == Qt::Vertical) {
        if (role == Qt::DisplayRole)
            return QVariant();
    }

    return QStandardItemModel::headerData(section, orientation, role);
}
