/*
 * qitemmodelteach.cpp
 *
 * 教导列表的数据model，调用Adapter对接下位机
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qitemmodelteach.h"
#include "Teach.h"
#include <QBitArray>
#include "data/utils.h"
#include "data/globaldata.h"

QItemModelTeach::QItemModelTeach(int axisCount, QStringList& axisName, int countEachPage, TEACH_DATA* teachData,
                                 int totalPointNum, int maxPoint, QObject* parent):
#ifdef Arm_662
	QStandardItemModel(axisCount + 4, countEachPage, parent),
#else
    QStandardItemModel(axisCount + 3, countEachPage, parent),
#endif
    axisCount(axisCount),
    currentPage(0),
    countEachPage(countEachPage),
    maxPoint(maxPoint),
    totalPointNum(totalPointNum),
    axisName(axisName),
    teachData(teachData)
{

}

int QItemModelTeach::rowCount(const QModelIndex& /*parent*/) const
{//行设置
#ifdef Arm_662
	return axisCount + 4; // 轴数+颜色+拍毛+速度+停车
#else
    return axisCount + 3; // 轴数+颜色+速度+气缸
#endif
    
}

int QItemModelTeach::columnCount(const QModelIndex& /*parent*/) const
{//列设置
    return countEachPage;
}

bool QItemModelTeach::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(!index.isValid())
        return false;


    if(Qt::EditRole == role)
    {//编辑
        bool isEqual = false;
        auto row = index.row();
        auto col = index.column();
        auto pointIndex = getPointIndexFromColumn(col);

#ifdef Arm_662 
		if(pointIndex >= totalPointNum||pointIndex==0) //大机器起点不能修改
#else
    	if(pointIndex >= totalPointNum)
#endif            
		return false;

        if(row < axisCount)
        {
            // axis
            compareAndSetValue((this->teachData + pointIndex)->fMove_pos[GlobalData::teachaxis[row]], value.toDouble(), &isEqual);
        }
        else if(row == axisCount)
        {
            // color
            compareAndSetValue((this->teachData + pointIndex)->icolor, value.toUInt(), &isEqual);
        }
        else if(row == axisCount + 1)
        {
            // speed
#ifdef Arm_662 
		compareAndSetValue((this->teachData + pointIndex)->bPatHair, value.toUInt(), &isEqual);
#else
    	compareAndSetValue((this->teachData + pointIndex)->iMainSpeed, value.toUInt(), &isEqual);
#endif            
            
        }
        else if(row == axisCount + 2)
        {
            
#ifdef Arm_662 
		compareAndSetValue((this->teachData + pointIndex)->iMainSpeed, value.toUInt(), &isEqual);
#else
    	// cylinder
            QBitArray bitArray = value.toBitArray();

            for(auto c = 0; c < TEACH_OUT; ++c)
                (this->teachData + pointIndex)->iOut[c] = bitArray.at(c) ? 1 : 0;

            isEqual = false;
#endif  
        }
		#ifdef Arm_662
		else if(row == axisCount + 3)
        {
            // stop
            compareAndSetValue((this->teachData + pointIndex)->iSkid, value.toUInt(), &isEqual);
        }
		#endif
        else
        {
            return false;
        }

        // send signals(required)
        if(false == isEqual)
            emit dataChanged(index, index);

        return true;
    }

    return false;
}

QVariant QItemModelTeach::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(Qt::DisplayRole == role)
    {//参数显示
        auto row = index.row();
        auto col = index.column();
        auto pointIndex = getPointIndexFromColumn(col);

        if(pointIndex >= totalPointNum)
            return QVariant();

        if(row < axisCount)
        {
            // axis
            return QVariant((this->teachData + pointIndex)->fMove_pos[GlobalData::teachaxis[row]]);
        }
        else if(row == axisCount)
        {
            // color
            return QVariant((this->teachData + pointIndex)->icolor);
        }
        else if(row == axisCount + 1)
        {
            // speed
			#ifdef Arm_662
			return QVariant((this->teachData + pointIndex)->bPatHair);
			#else
            return QVariant((this->teachData + pointIndex)->iMainSpeed);
			#endif
        }
        else if(row == axisCount + 2)
        {
            // cylinder
			#ifdef Arm_662
			return QVariant((this->teachData + pointIndex)->iMainSpeed);
			#else
            QBitArray bitArray(TEACH_OUT);

            for(auto c = 0; c < TEACH_OUT; ++c)
                bitArray.setBit(c, (this->teachData + pointIndex)->iOut[c] > 0);

            return QVariant(bitArray);
			#endif
        }
		#ifdef Arm_662
		 else if(row == axisCount + 3)
        {
            // stop
            return QVariant((this->teachData + pointIndex)->iSkid);
        }
		#endif
    }
    else if(Qt::TextAlignmentRole == role)
    {//参数位置
        return QVariant(Qt::AlignLeft);
    }
//    else if(Qt::BackgroundColorRole == role )
//    {
//        if(index.row()%5 == 0)
//                    return QColor(Qt::red);
//        else if(index.row()%5 == 1)
//                    return QColor(Qt::green);
//        else if(index.row()%5 == 2)
//                    return QColor(Qt::blue);
//        else if(index.row()%5 == 3)
//                    return QColor(Qt::yellow);
//        else if(index.row()%5 == 4)
//                    return QColor(Qt::gray);
//        else
//        return QVariant();
//    }

    // invalid
    return QVariant();
}

QVariant QItemModelTeach::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();

    QString text;

    if(Qt::Horizontal == orientation)
    {//水平头
        auto pointIndex = getPointIndexFromColumn(section);;

        if(pointIndex >= totalPointNum)
            return QVariant();

        if(pointIndex > 0)
            text = Utils::numberToString(pointIndex);
        else
            //text = QString::fromUtf8("起点");
            text = tr("起点");
    }
    else
    {//垂直头
        if(section < axisCount)
        {
            // axis
            text = axisName.at(section);
        }
        else if(section == axisCount)
        {
            //text = QString::fromUtf8("颜色");
            text = tr("颜色");
        }
        else if(section == axisCount + 1)
        {
			#ifdef Arm_662
				text = tr("拍毛");
			#else
				text = tr("速度");
			#endif

            
        }
        else if(section == axisCount + 2)
        {
            #ifdef Arm_662
				text = tr("速度");
			#else
				text = tr("输出");
			#endif
            
        }
		#ifdef Arm_662
		else if(section == axisCount + 3)
        {
            //text = QString::fromUtf8("停车");
            text = tr("停车");
        }
		#endif
    }

    return QVariant(text);
}

void QItemModelTeach::onCurrentPageChanged(int pageNum)
{
    if(pageNum * countEachPage >= maxPoint)
    {
        debug_printf("QItemModelTeach::setCurrentPage: out of range");
        return;
    }

    currentPage = pageNum;
    refreshUI();
}

void QItemModelTeach::onTotalPointChanged(int totalPoint)
{
    this->totalPointNum = totalPointNum;
    refreshUI();
}

void QItemModelTeach::refreshUI()
{
    reset();
}
