/*
 * qstyleditemdelegateteachspeed.cpp
 *
 * 教导代理：速度，限制输入0-100
 *
 * Created on: 2019年 01月 26日 星期六 17:05:50 CST
 * Author: lixingcong
 */

#include "qstyleditemdelegateteachspeed.h"
#include <QLineEdit>
#include <QAbstractItemModel>
#include <QPainter>
#include "data/utils.h"
#include "widgets/myinputpanelcontext.h"
#include "QApplication"

QStyledItemDelegateTeachSpeed::QStyledItemDelegateTeachSpeed(int rowCount,
                                                             QObject* parent) : QStyledItemDelegateTeachBase(rowCount, parent)
{

}

void QStyledItemDelegateTeachSpeed::paint(QPainter* painter, const QStyleOptionViewItem& option,
                                          const QModelIndex& index) const
{
    _drawBorders(painter, option, index);

    auto value = index.model()->data(index, Qt::DisplayRole);

    if(false == value.isValid())
        return;

    double pos = value.toDouble();
    QString text = Utils::numberToString(pos);
    QTextOption o(Qt::AlignHCenter | Qt::AlignVCenter);//修改对齐 方向
#ifdef Arm_Kaiteli
    QFont font(QFont("/root/app/x_simsun.ttc"));
    font.setBold(true);
    painter->setFont(font);
#endif

    QRect innerRect(option.rect);
    innerRect.setLeft(innerRect.left() + 2);
    innerRect.setRight(innerRect.right() - 2);
    painter->drawText(innerRect, text, o);
}

QWidget* QStyledItemDelegateTeachSpeed::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                                                     const QModelIndex& index) const
{
    const QMargins defaultMargins(2, 0, 2, 0);

    auto value = index.model()->data(index, Qt::DisplayRole);

    if(false == value.isValid())
        return nullptr;

    auto editor = new QLineEdit(parent);
    connect(editor, SIGNAL(editingFinished()),
            this, SLOT(onCommitAndCloseEditor()));

    // 如需直接编辑坐标数字，将宏激活即可
#if 1
    auto ic = qobject_cast<MyInputPanelContext*>(qApp->inputContext());
    auto panel = ic->getInputPanel();

    // 注意：lixingcong@20181112
    // InputPanel如下的两个槽必须同时使用，否则出现不可预料的Segment Fault
    connect(editor, SIGNAL(selectionChanged()),
            panel, SLOT(onSetNextFocusWidget()));
    connect(editor, SIGNAL(destroyed()), panel, SLOT(onDestroyedFocusedWidget()));
#endif

//    auto validator = new QDoubleValidator(0, 100, 0, editor);
//    validator->setNotation(QDoubleValidator::StandardNotation);
//    editor->setValidator(validator);
    editor->setContentsMargins(defaultMargins);
    return editor;
}

void QStyledItemDelegateTeachSpeed::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    int pos = index.model()->data(index, Qt::DisplayRole).toInt();
    QString text = Utils::numberToString(pos);
    qobject_cast<QLineEdit*>(editor)->setText(text);
}

void QStyledItemDelegateTeachSpeed::setModelData(QWidget* editor, QAbstractItemModel* model,
                                                 const QModelIndex& index) const
{
    QString text = qobject_cast<QLineEdit*>(editor)->text();
    double pos = text.toInt();
    if(pos>100)//速度范围是 0 ～ 100
        return ;
    model->setData(index, pos);

}
