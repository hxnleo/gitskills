/*
 * qdialogdataform.cpp
 *
 * 填表对话框，支持多种数据类型(数字和字符串)
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qdialogdataform.h"
#include <QLabel>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QMessageBox>
#include <QApplication>
#include <QKeyEvent>
#include "data/uistyles.h"
#include "data/utils.h"
#include <QDialogButtonBox>
#include "qlineeditstandard.h"

QDialogDataForm::QDialogDataForm(QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f),
    isInitialized(false),
    labelTitle(new QLabelMarquee(this))
{
    // delete on close
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowIcon(QIcon(":/images/images/NULL.ico"));
    // layout
    auto mainLayout = new QVBoxLayout(this);

    this->setFixedSize(300, 350);

    // title
    mainLayout->addWidget(labelTitle);

    auto scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    auto scrollAreaContents = new QWidget;
    formLayout = new QFormLayout(scrollAreaContents);
    UIStyles::setMarginAndSpacing(formLayout, 1, 4);
    scrollArea->setWidget(scrollAreaContents);

    mainLayout->addWidget(scrollArea);

    auto buttonBox = Utils::createOkCancelButtons(this);
    mainLayout->addWidget(buttonBox);

    buttonBox->setFocus();
}

void QDialogDataForm::showDialog(const QString& title, const QList<QDialogDataForm::item_t>& items, bool marqueeEnable,QString stayle)
{
    if(isInitialized)
        return;
    this->items = items;
    QWidget* formWidget = nullptr;
    QVariant val;
    QPalette palette = QApplication::palette();
    palette.setColor(QPalette::Base, Qt::lightGray);

    labelTitle->setText(title);
    labelTitle->setAlignment(Qt::AlignHCenter);
    labelTitle->setMarqueeEnable(marqueeEnable);
    if(!stayle.isEmpty())
    {
        labelTitle->setStyleSheet(stayle);
    }
    foreach(const item_t& item, items)
    {
        // create widgets
        switch(item.type)
        {
        case Adapters::TYPE_U8:
        case Adapters::TYPE_S8:
        case Adapters::TYPE_U16:
        case Adapters::TYPE_S16:
        case Adapters::TYPE_U32:
        case Adapters::TYPE_S32:
        case Adapters::TYPE_F32:
        case Adapters::TYPE_F64:
        case Adapters::TYPE_String:
            formWidget = new QLineEditStandard(this);
            formWidget->setStyleSheet(UIStyles::css_maincolorconf_u11);
            if(Adapters::TYPE_String != item.type)
            {
                auto editor = qobject_cast<QLineEdit*>(formWidget);
                formWidget->setStyleSheet("qproperty-alignment:AlignHCenter");
                // only allowing input numbers
                editor->setPlaceholderText(QString("%1~%2").arg(item.minVal).arg(item.maxVal));

                if(Adapters::TYPE_F32 == item.type || Adapters::TYPE_F64 == item.type)
                    editor->setValidator(new QRegExpValidator(QRegExp("[-+]?(\\d*\\.\\d+|\\d+)"), editor));
                else if(Adapters::TYPE_S8 == item.type || Adapters::TYPE_S16 == item.type || Adapters::TYPE_S32 == item.type)
                    editor->setValidator(new QRegExpValidator(QRegExp("[-+]?\\d+"), editor));
                else
                    editor->setValidator(new QRegExpValidator(QRegExp("\\d+"), editor));
            }
            if(item.isReadOnly)
            {
                qobject_cast<QLineEdit*>(formWidget)->setReadOnly(true);
                qobject_cast<QLineEdit*>(formWidget)->setPalette(palette);
            }
            break;
        case Adapters::TYPE_StringList:
            if(nullptr == item.stringList) // ERROR: null string list
            {
                formWidget = nullptr;
                break;
            }
            if(false == item.isReadOnly)
            {
                //通过自定义类型 qMyComboBox 实现居中
                formWidget = new qMyComboBox(this);
                formWidget->setStyleSheet(UIStyles::css_maincolorconf_u11);
                qobject_cast<QComboBox*>(formWidget)->addItems(*item.stringList);
                qobject_cast<QComboBox*>(formWidget)->setCurrentIndex(*reinterpret_cast<int*>(item.data));
            }
            else
            {
                formWidget = new QLineEditStandard(this);
                formWidget->setStyleSheet("qproperty-alignment:AlignHCenter");
                auto editor = qobject_cast<QLineEdit*>(formWidget);
                auto index = *reinterpret_cast<int*>(item.data);
                editor->setText(item.stringList->at(index));
                editor->setReadOnly(true);
                editor->setPalette(palette);
            }

            break;

        default:
            formWidget = nullptr;
            break;
        }

        // wrong datatype, skip it
        if(nullptr == formWidget)
            continue;
        // add to  layout
        formLayout->addRow(item.name, formWidget);
        this->itemWidgets.append(formWidget);

        // set the default value for each widget
        switch(item.type)
        {
        case Adapters::TYPE_S8:
            val = QVariant(*reinterpret_cast<char*>(item.data));
            break;

        case Adapters::TYPE_S16:
            val = QVariant(*reinterpret_cast<short*>(item.data));
            break;

        case Adapters::TYPE_S32:
            val = QVariant(*reinterpret_cast<int*>(item.data));
            break;

        case Adapters::TYPE_U8:
            val = QVariant(*reinterpret_cast<unsigned char*>(item.data));
            break;

        case Adapters::TYPE_U16:
            val = QVariant(*reinterpret_cast<unsigned short*>(item.data));
            break;

        case Adapters::TYPE_U32:
            val = QVariant(*reinterpret_cast<unsigned int*>(item.data));
            break;

        case Adapters::TYPE_F32:
            val = QVariant(*reinterpret_cast<float*>(item.data));
            break;

        case Adapters::TYPE_F64:
            val = QVariant(*reinterpret_cast<double*>(item.data));
            break;

        case Adapters::TYPE_String:
            val = QVariant(*reinterpret_cast<QString*>(item.data));
            break;

        default:
            val = 0;
            break;
        }

        if(Adapters::TYPE_StringList == item.type)
            continue;

        qobject_cast<QLineEdit*>(formWidget)->setText(val.toString());
    }

    isInitialized = true;

    open();
}

void QDialogDataForm::accept()
{
    bool isOk = true;
    QVariant val;
    QString text; // for QLineEdit only

    for(auto i = 0; i < items.size(); ++i)
    {
        auto item = items.at(i);

        // skip read only
        if(true == item.isReadOnly)
            continue;

        auto widget = itemWidgets.at(i);

        // get value from widget
        switch(item.type)
        {
        case Adapters::TYPE_U8:
        case Adapters::TYPE_U16:
        case Adapters::TYPE_U32:
            text = qobject_cast<QLineEdit*>(widget)->text();
            val = text.toUInt();

            if(0 >= text.length() || val.toDouble() < item.minVal || val.toDouble() > item.maxVal)
                isOk = false;

            break;

        case Adapters::TYPE_S8:
        case Adapters::TYPE_S16:
        case Adapters::TYPE_S32:
            text = qobject_cast<QLineEdit*>(widget)->text();
            val = text.toInt();

            if(0 >= text.length() || val.toDouble() < item.minVal || val.toDouble() > item.maxVal)
                isOk = false;

            break;

        case Adapters::TYPE_F32:
        case Adapters::TYPE_F64:
            text = qobject_cast<QLineEdit*>(widget)->text();
            val = (Adapters::TYPE_F32 == item.type ? text.toFloat() : text.toDouble());

            if(0 >= text.length() || val.toDouble() < item.minVal || val.toDouble() > item.maxVal)
                isOk = false;

            break;

        case Adapters::TYPE_String:
            val = qobject_cast<QLineEdit*>(widget)->text();
            break;

        case Adapters::TYPE_StringList:
            val = qobject_cast<QComboBox*>(widget)->currentIndex();
            break;

        default:
            // do nothing
            break;
        }

        // show error message
        if(false == isOk)
        {
            QString errMsg = QString("%1: %2~%3").arg(item.name).arg(item.minVal).arg(item.maxVal);
            Utils::MyMessageBox(QString("Error Range"), errMsg,QObject::tr("确定"),this);
            break;
        }

        // store good value
        switch(item.type)
        {
        case Adapters::TYPE_U8:
            *reinterpret_cast<unsigned char*>(item.data) = val.toUInt();
            break;

        case Adapters::TYPE_U16:
            *reinterpret_cast<unsigned short*>(item.data) = val.toUInt();
            break;

        case Adapters::TYPE_U32:
            *reinterpret_cast<unsigned int*>(item.data) = val.toUInt();
            break;

        case Adapters::TYPE_S8:
            *reinterpret_cast<char*>(item.data) = val.toInt();
            break;

        case Adapters::TYPE_S16:
            *reinterpret_cast<short*>(item.data) = val.toInt();
            break;

        case Adapters::TYPE_S32:
        case Adapters::TYPE_StringList:
            *reinterpret_cast<int*>(item.data) = val.toInt();
            break;

        case Adapters::TYPE_F32:
            *reinterpret_cast<float*>(item.data) = val.toFloat();
            break;

        case Adapters::TYPE_F64:
            *reinterpret_cast<double*>(item.data) = val.toDouble();
            break;

        case Adapters::TYPE_String:
            *reinterpret_cast<QString*>(item.data) = val.toString();
            break;

        default:
            break;
        }
    }

    if(isOk)
        QDialog::accept();
}

void QDialogDataForm::keyPressEvent(QKeyEvent* evt)
{
    if(evt->key() == Qt::Key_Enter || evt->key() == Qt::Key_Return)
        return;

    QDialog::keyPressEvent(evt);
}
