/*
 * qstyleditemdelegateteachaxis.cpp
 *
 * 教导列表的代理类：轴数字输入
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qstyleditemdelegateteachaxis.h"
#include <QLineEdit>
#include <QAbstractItemModel>
#include <QPainter>
#include "data/utils.h"
#include "widgets/qlineeditnumber.h"

QStyledItemDelegateTeachAxis::QStyledItemDelegateTeachAxis(int rowCount, QObject* parent)
    : QStyledItemDelegateTeachBase(rowCount, parent)
{
}

void QStyledItemDelegateTeachAxis::paint(QPainter* painter, const QStyleOptionViewItem& option,
                                         const QModelIndex& index) const
{
    _drawBorders(painter, option, index);

    auto value = index.model()->data(index, Qt::DisplayRole);

    if(false == value.isValid())
        return;


    //设置字体
    double pos = value.toDouble();
    QString text = Utils::numberToString(pos, 2, 6);
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

QWidget* QStyledItemDelegateTeachAxis::createEditor(QWidget* parent, const QStyleOptionViewItem& /*option*/,
                                                    const QModelIndex& index) const
{
    const QMargins defaultMargins(2, 0, 2, 0);

    auto value = index.model()->data(index, Qt::DisplayRole);

    if(false == value.isValid())
        return nullptr;

    auto editor = new QLineEditNumber(parent);
    connect(editor, SIGNAL(editingFinished()),
            this, SLOT(onCommitAndCloseEditor()));

    // 如需直接编辑坐标数字，将宏激活即可
#if 1
    auto ic = qobject_cast<MyInputPanelContext*>(qApp->inputContext());
    auto panel = ic->getInputPanel();
    //qDebug()<<panel;

    // 注意：lixingcong@20181112
    // InputPanel如下的两个槽必须同时使用，否则出现不可预料的Segment Fault
    connect(editor, SIGNAL(selectionChanged()),
            panel, SLOT(onSetNextFocusWidget()));
    connect(editor, SIGNAL(destroyed()), panel, SLOT(onDestroyedFocusedWidget()));
#endif

    //设置无效字符
    editor->setValidator(new QRegExpValidator(QRegExp("[-+]?(\\d*\\.\\d+|\\d+)"), editor));
    editor->setContentsMargins(defaultMargins);

    return editor;
}

void QStyledItemDelegateTeachAxis::setEditorData(QWidget* editor, const QModelIndex& index) const
{//设置文本显示
    //qDebug()<<"setEditorData";
    double pos = index.model()->data(index, Qt::DisplayRole).toDouble();
    QString text = Utils::numberToString(pos, 2, 6);

    qobject_cast<QLineEdit*>(editor)->setText(text);

}

void QStyledItemDelegateTeachAxis::setModelData(QWidget* editor, QAbstractItemModel* model,
                                                const QModelIndex& index) const
{//设置内容 只有设置而了model 才能改变数据和文本显示
    //qDebug()<<"setModelData";
    QString text = qobject_cast<QLineEdit*>(editor)->text();
    double pos = text.toDouble();
    model->setData(index, pos);

}
