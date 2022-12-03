/*
 * qdialogparams_patterns.cpp
 *
 * 花样图片显示，参数显示
 *
 * Created on: 2019年 01月 26日 星期六 17:05:50 CST
 * Author: lixingcong
 */

#include "qdialogparams_patterns.h"
#include <QPainter>
#include <QPixmap>
#include <QLineEdit>
#include <QLabel>
#include "widgets/qwidgetfieldlayout.h"
#include <QDoubleValidator>
#include <QComboBox>
#include <QDialogButtonBox>
#include "data/utils.h"
#include "data/globaldata.h"
#include "qlineeditstandard.h"

QDialogParams_Patterns::QDialogParams_Patterns(Adapters::DataAdapter_Patterns* adapter, int offset, QWidget* parent,
        Qt::WindowFlags f): QDialog(parent, f),
    adapter(adapter),
    offset(offset)
{
    setWindowIcon(QIcon(":/images/images/NULL.ico"));
    setFixedSize(727, 433);

    pattern = adapter->getData(offset).value<Adapters::PatternItem>();
    pixmap = QPixmap(pattern.resourceName[1]);

    widgetAspectRatio = static_cast<qreal>(rect().width()) / static_cast<qreal>(rect().height());
    pixmapAspectRatio = static_cast<qreal>(pixmap.width()) / static_cast<qreal>(pixmap.height());

    for(auto i = 0; i < pattern.params.size(); ++i)
    {
//        qDebug()<<"HUAYANG"<<GlobalData::patternarry[offset][i];
        auto param = pattern.params.at(i);
        QWidget* label = new QLabel(param.name, this);
        QWidget* field = nullptr;
        switch(param.dataType)
        {
        case Adapters::TYPE_U8:
        case Adapters::TYPE_S8:
        case Adapters::TYPE_U16:
        case Adapters::TYPE_S16:
        case Adapters::TYPE_U32:
        case Adapters::TYPE_S32:
        {
            auto w = new QLineEditStandard(this);

            auto validator = new QDoubleValidator(param.range[0], param.range[1], 0, this);
            validator->setNotation(QDoubleValidator::StandardNotation);
            w->setText(GlobalData::patternarry[offset][i]);
            w->setValidator(validator);
            w->setFixedWidth(param.width);
            field = w;
            break;
        }
        case Adapters::TYPE_F32:
        case Adapters::TYPE_F64:
        {
            auto w = new QLineEditStandard(this);

            auto validator = new QDoubleValidator(param.range[0], param.range[1], 3, this);
            validator->setNotation(QDoubleValidator::StandardNotation);
            w->setText(GlobalData::patternarry[offset][i]);
            w->setValidator(validator);
            w->setFixedWidth(param.width);
            field = w;
            break;
        }

        case Adapters::TYPE_StringList:
        {
            auto w = new QComboBox(this);
            w->insertItems(0, param.stringList);
            w->setCurrentIndex(GlobalData::patternarry[offset][i].toInt());
            field = w;
            break;
        }

        default:
            break;
        }
        if(GlobalData::screensize == FIFTEEN)
        {
            QFont fon;
            fon.setPointSize(7);
            label->setFont(fon);
            field->setFont(fon);
        }

        auto layoutWidget = new QWidgetFieldLayout(param.isHorizontal, label, field, this);
        layoutWidget->setMinimumWidth(param.width);
        layoutWidget->move(param.position);

        // 添加参数
        createdWidget.append(field);
    }

    // 确定 取消键
    auto buttonBox = Utils::createOkCancelButtons(this);
    auto buttonBoxPosition = rect().bottomRight();
    buttonBoxPosition.rx() -= 200;
    buttonBoxPosition.ry() -= 50;
    buttonBox->move(buttonBoxPosition);
    buttonBox->setFocus();
}

QString QDialogParams_Patterns::getStringFromData(const QVariant& val)
{
    return val.value<Adapters::PatternItem>().name;
}

void QDialogParams_Patterns::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    int w = 0;
    int h = 0;

    if(pixmapAspectRatio > widgetAspectRatio)
    {
        // 图像内容更宽
        w = rect().width();
        h = w / pixmapAspectRatio;
    }
    else
    {
        // 图像内容更高
        h = rect().height();
        w = h * pixmapAspectRatio;
    }

    painter.drawPixmap(0, 0,
                       w, h,
                       pixmap);

    QDialog::paintEvent(event);
}

void QDialogParams_Patterns::accept()
{
    QList<float> valueList;

    for(auto i = 0; i < createdWidget.size(); ++i)
    {
        auto fieldWidget = createdWidget.at(i);

        if(nullptr == fieldWidget)
        {
            valueList.append(0);
            continue;
        }

        switch(pattern.params.at(i).dataType)
        {
        case Adapters::TYPE_U8:
        case Adapters::TYPE_S8:
        case Adapters::TYPE_U16:
        case Adapters::TYPE_S16:
        case Adapters::TYPE_U32:
        case Adapters::TYPE_S32:
        case Adapters::TYPE_F32:
        case Adapters::TYPE_F64:
        {
            auto lineEdit = qobject_cast<QLineEdit*>(fieldWidget);
            valueList.append(lineEdit->text().toFloat());
            break;
        }

        case Adapters::TYPE_StringList:
        {
            auto comboBox = qobject_cast<QComboBox*>(fieldWidget);
            valueList.append(static_cast<float>(comboBox->currentIndex()));
            break;
        }

        default:
            break;
        }
    }

    QVariant v;
    v.setValue<QList<float>>(valueList);
    adapter->setData(offset, v);

    QDialog::accept();
}
