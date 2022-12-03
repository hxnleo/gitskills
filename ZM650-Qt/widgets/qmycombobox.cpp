#include "qmycombobox.h"
#include <QListView>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QPoint>
#include <QPainter>

ComboBoxDelegate::ComboBoxDelegate(QObject * parent)
    : QStyledItemDelegate(parent)
{

}

void ComboBoxDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{

    QString  str = index.data().toString();
    QStyleOptionViewItem myOption = option;

    myOption.displayAlignment = Qt::AlignCenter;
    QStyledItemDelegate::paint(painter, myOption, index);
}

PopupLineEdit::PopupLineEdit(QWidget* parent) : QLineEdit(parent)
{

}

PopupLineEdit::PopupLineEdit(const QString &str, QWidget* parent)
 : QLineEdit(parent)
{

}

PopupLineEdit::~PopupLineEdit()
{

}

void PopupLineEdit::mouseReleaseEvent(QMouseEvent *event)
{
    emit clicked();
    QWidget::mouseReleaseEvent(event);
}


void PopupLineEdit::mouseDoubleClickEvent(QMouseEvent *)
{

}

void PopupLineEdit::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
}

qMyComboBox::qMyComboBox(QWidget *parent)
    : QComboBox(parent)
{
    PopupLineEdit* lineEdit = new PopupLineEdit(this);
    lineEdit->setReadOnly(true);
    lineEdit->setAlignment(Qt::AlignCenter);
    setLineEdit(lineEdit);
    connect(lineEdit, SIGNAL(clicked()), this, SLOT(popupTop()));
    ComboBoxDelegate *delegate = new ComboBoxDelegate(this);
    setItemDelegate(delegate);
    setMaxVisibleItems(20);
}

qMyComboBox::~qMyComboBox()
{
}

void qMyComboBox::popupTop()
{
    if (count() <= 1)
        return;
    showPopup();

    QWidget *popup = this->findChild<QFrame*>();
    //将当前坐标转换为显示器坐标

    //QPoint pos = mapToGlobal(QPoint(0,180));
    popup->move(popup->x(),popup->y());
    //popup->move(popup->x(), pos.y() - popup->height());
}
