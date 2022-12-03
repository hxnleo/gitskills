/*
 * qdialogteachcylinder.cpp
 *
 * 教导界面的气缸编辑弹出对话框
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qdialogteachcylinder.h"
#include "qpushbuttonchangeonclick.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QVariant>
#include "data/uistyles.h"
#include "data/utils.h"
#include "data/globaldata.h"
#include <QLabel>

const char CYLINDER_INDEX[] = "cylinderIndex";

QDialogTeachCylinder::QDialogTeachCylinder( int cylinderCount, QWidget* parent, Qt::WindowFlags f,bool type) :
    QDialog(parent, f),
    cylinderCount(cylinderCount)
{
    MAINCYLINDER =
    {
        tr("夹具1"),
        tr("夹具2"),
        tr("夹具3"),
        tr("夹具4"),
        tr("夹具5"),
        tr("夹具6"),

    };
    NAMES =
    {
    #ifdef Arm_460Y
        tr("夹子"),
        tr("分毛"),
        tr("编程"),
        tr("编程"),
        tr("编程"),
        tr("编程"),
    #elif Arm_Kaiteli
        tr("二段速度"),
        tr("换色抖毛"),
        tr("备用"),
        tr("备用"),
        tr("备用"),
        tr("备用"),
    #else
        tr("编程"),
        tr("编程"),
        tr("编程"),
        tr("编程"),
        tr("编程"),
        tr("编程"),
    #endif
        //   QObject::trUtf8("急救"),//测试程序
    };
    setWindowIcon(QIcon(":/images/images/NULL.ico"));
    setFixedSize(QSize(0, 220));

    //QPalette palette(this->palette());
    //palette.setColor(QPalette::Background, Qt::darkYellow);//#765951
    //this->setPalette(palette);
    //this->setPalette(QPalette(QColor(118,89,81)));
    //setBackgroundRole(QPalette::Background);

    this->setStyleSheet(UIStyles::css_maincolorconf_cylinder1);

    auto mainLayout = new QVBoxLayout(this);
    auto cylindersLayout = new QGridLayout;

    mainLayout->addLayout(cylindersLayout);

    QStringList textList;
    textList << QString::fromUtf8("关") << QString::fromUtf8("开");

    const QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    for(auto c = 0; c < cylinderCount; ++c)
    {
//        auto label = new QLabel(NAMES.at(c),this);
        auto label = new QLabel(this);
        if(type)
            label->setText(MAINCYLINDER.at(c));
        else
            label->setText(NAMES.at(c));
        cylindersLayout->addWidget(label,0,c,Qt::AlignHCenter);

        auto btn = new QPushButtonChangeOnClick(this);
        btn->setProperty(CYLINDER_INDEX, QVariant(c));
        listButtons.append(btn);
        btn->init(textList);
        btn->setSizePolicy(sizePolicy);

//        btn->setPalette(QPalette(QColor(34,103,118)));
//        btn->setBackgroundRole(QPalette::Background);
        //btn->setStyleSheet(UIStyles::css_maincolorconf_cylinder);

        cylindersLayout->addWidget(btn,1,c);

        connect(btn, SIGNAL(clickedIndexChanged(int)),
                this, SLOT(_onButtonClicked(int)));
    }

    auto buttonsLayout = new QHBoxLayout;
    mainLayout->addLayout(buttonsLayout);

    auto btn_ok = new QPushButton(QString("确定"), this);
    if(GlobalData::screensize == TEN)
    {
        btn_ok->setMinimumHeight(45);
    }
    else
    {
        btn_ok->setMinimumHeight(35);
    }

    //btn_ok->setPalette(QPalette(QColor(34,103,118)));
    //btn_ok->setBackgroundRole(QPalette::Background);

    //stylesheet
    btn_ok->setStyleSheet(UIStyles::css_maincolorconf_cylinder);
    //btn_minus->setStyleSheet(UIStyles::css_teachcolorconf_u0);

    buttonsLayout->addWidget(btn_ok);

    connect(btn_ok, SIGNAL(clicked()),
            this, SLOT(accept()));
}

QDialogTeachCylinder::~QDialogTeachCylinder()
{
}



QBitArray QDialogTeachCylinder::getBitArray() const
{
    QBitArray bitArray(listButtons.size());

    // bit较小的为第一个气缸
    for(auto bit = 0; bit < listButtons.size(); ++bit)
    {
        bitArray.setBit(bit, listButtons.at(bit)->getIndex() ? true : false);
    }

    return bitArray;
}

void QDialogTeachCylinder::setBitArray(const QBitArray& bitArray)
{
    // bit较小的为第一个气缸
    for(auto bit = 0; bit < listButtons.size(); ++bit)
    {
        listButtons.at(bit)->setIndex(bitArray.at(bit) ? 1 : 0);
        listButtons.at(bit)->setStyleSheet(listButtons.at(bit)->getIndex() == 1 ? UIStyles::CSS_IO_HIGHLIGHT : QString());
        //listButtons.at(bit)->setStyleSheet(listButtons.at(bit)->getIndex() == 1 ? UIStyles::css_maincolorconf_cylinder2 : UIStyles::css_maincolorconf_cylinder);
    }
}

void QDialogTeachCylinder::_onButtonClicked(int ioStatus)
{
    auto cylinderIndex = sender()->property(CYLINDER_INDEX).toInt();

    qobject_cast<QWidget*>(sender())->setStyleSheet(ioStatus ? UIStyles::CSS_IO_HIGHLIGHT : QString());

    // 通知其它线程写气缸
    emit writeCylinder(cylinderIndex, ioStatus);
}
