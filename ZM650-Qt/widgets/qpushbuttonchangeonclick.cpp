/*
 * qpushbuttonchangeonclick.cpp
 *
 * 带点击事件的QPushButton，用于教导界面改变步长
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qpushbuttonchangeonclick.h"
#include "data/utils.h"

QPushButtonChangeOnClick::QPushButtonChangeOnClick(QWidget* parent)
    : QPushButton(parent)
{
    this->index = 0;

    connect(this, SIGNAL(clicked()),
            this, SLOT(onClicked()));
}

void QPushButtonChangeOnClick::init(const QStringList& stringList)
{
    this->stringList = stringList;
    this->index = 0;

    _setTextByIndex();
}

void QPushButtonChangeOnClick::setIndex(int index)
{
    this->index = index;
    _setTextByIndex();
}

void QPushButtonChangeOnClick::onClicked()
{
    //debug_printf("old index=%d",this->index);

    if(this->index < this->stringList.size() - 1)
        ++this->index;
    else
        this->index = 0;

    emit clickedIndexChanged(this->index);

    //debug_printf("new index=%d\n",this->index);

    _setTextByIndex();
}

void QPushButtonChangeOnClick::_setTextByIndex()
{

    if(this->index >= 0 && this->index < stringList.size())
    {
        setText(stringList.at(this->index));
    }
}
