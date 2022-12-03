/*
 * qlabelchangeonclick.cpp
 *
 * 带点击事件的QLabel
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qlabelchangeonclick.h"
#include "data/utils.h"

QLabelChangeOnClick::QLabelChangeOnClick(QWidget* parent, Qt::WindowFlags f)
    : QLabelClick(parent, f)
{
    this->index = 0;

    connect(this, SIGNAL(clicked()),
            this, SLOT(onClicked()));
}

void QLabelChangeOnClick::init(const QStringList& stringList)
{
    this->stringList = stringList;
    this->index = 0;

    _setTextByIndex();
}

void QLabelChangeOnClick::setIndex(int index)
{
    this->index = index;
    _setTextByIndex();
}

void QLabelChangeOnClick::onClicked()
{
    debug_printf("old index=%d", this->index);

    if(this->index < this->stringList.size() - 1)
        ++this->index;
    else
        this->index = 0;

    debug_printf("new index=%d\n", this->index);

    _setTextByIndex();
}

void QLabelChangeOnClick::_setTextByIndex()
{

    if(this->index >= 0 && this->index < stringList.size())
    {
        setText(stringList.at(this->index));
    }
}
