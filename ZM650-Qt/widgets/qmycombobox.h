#ifndef QMYCOMMBOX_H
#define QMYCOMMBOX_H

#include <QStyledItemDelegate>
#include <QLineEdit>
#include <QComboBox>
#include <QMouseEvent>


class ComboBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ComboBoxDelegate(QObject * parent = 0);
    void paint(QPainter * painter, const QStyleOptionViewItem & option,
               const QModelIndex & index) const;
};


class PopupLineEdit : public QLineEdit
{
    Q_OBJECT

public:

    explicit PopupLineEdit(QWidget* parent = 0);
    explicit PopupLineEdit(const QString &str, QWidget* parent = 0);
    ~PopupLineEdit();
    Q_SIGNAL void clicked();
protected:
    void mouseReleaseEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *event);
};

class qMyComboBox : public QComboBox
{
    Q_OBJECT

    public:
    explicit qMyComboBox(QWidget *parent = 0);
    ~qMyComboBox();
    Q_SLOT void popupTop();
};

#endif // QMYCOMMBOX_H
