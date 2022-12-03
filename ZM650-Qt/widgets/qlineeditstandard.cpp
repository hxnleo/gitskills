/*
 * qlineeditnumber.cpp
 *
 * 封装标准的 QLineEdit 类(实现单击触发按键框)
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: caojing
 */
#include "widgets/myinputpanelcontext.h"
#include "QApplication"
#include "qlineeditstandard.h"

QLineEditStandard::QLineEditStandard(QWidget* parent): QLineEdit(parent)
{
#if _MCGS
    //返回定义的软键盘
    auto ic = qobject_cast<MyInputPanelContext*>(qApp->inputContext());
    auto panel = ic->getInputPanel();//得到键盘
    // 注意：lixingcong@20181112
    // InputPanel如下的两个槽必须同时使用，否则出现不可预料的Segment Fault
    //焦点改变时候  在编辑框弹出对应键盘
    connect(this, SIGNAL(selectionChanged()),
            panel, SLOT(onSetNextFocusWidget()));
    connect(this, SIGNAL(destroyed()), panel, SLOT(onDestroyedFocusedWidget()));
#endif
}
void QLineEditStandard::focusInEvent(QFocusEvent* e)
{
    // https://forum.qt.io/topic/8389/solved-have-text-in-qlineedit-selected-on-edit-start/3
    QLineEdit::focusInEvent(e);
    selectAll();

}

void QLineEditStandard::mousePressEvent(QMouseEvent* e)
{
    QLineEdit::mousePressEvent(e);
    selectAll();

}

void QLineEditStandard::mouseReleaseEvent(QMouseEvent *e)
{
    QLineEdit::mouseReleaseEvent(e);
    selectAll();
}

