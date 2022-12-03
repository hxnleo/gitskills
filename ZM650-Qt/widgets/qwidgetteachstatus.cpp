/*
 * qwidgetteachstatus.cpp
 *
 * 教导界面左侧的状态显示
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "qwidgetteachstatus.h"
#include "data/globaldata.h"
#include "data/utils.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QPainter>
#include <QPushButton>
#include "data/uistyles.h"
#include "qwidgetaxisposition.h"

//const double QWidgetTeachStatus::TEACH_STEPS[] = {0.001, 0.010, 0.100, 1.000, 10.000};
const double QWidgetTeachStatus::TEACH_STEPS[] = {0.010, 0.100, 1.000};//mod by yzg 190529

QWidgetTeachStatus::QWidgetTeachStatus(QWidget* parent) :
    QWidget(parent),
    _isInitialized(false),
    stepIndex(0)
{
    _layout = new QVBoxLayout(this);

    UIStyles::setMarginAndSpacing(_layout, 1, 1);

    // TODO: remove
    this->setAttribute(Qt::WA_DeleteOnClose);
}
void QWidgetTeachStatus::paintEvent(QPaintEvent* event)
{//绘制外框
    QPainter painter(this);
    painter.setPen(Qt::blue);
    painter.drawRect(rect().left(), rect().top(), rect().right(), rect().bottom());
    //qDebug()<<rect();
}

void QWidgetTeachStatus::init(const QStringList& axisName, int axisCount)
{
#define Font_SVE  25
#define Font_TEN  30
    if(_isInitialized)
        return;

    _axisName = axisName;
    _axisCount = axisCount;
    QFont fon,num_font;
    int btnheight;
    if(GlobalData::screensize == TEN)
    {
        fon.setPointSize(9);
        num_font.setPointSize(10);
        btnheight = Font_TEN;
    }
    else
    {
        fon.setPointSize(8);
        btnheight = Font_SVE;
    }

    auto setCSS = [](QWidget * w)
    {
        w->setStyleSheet(UIStyles::CSS_TEACHSTAUS_LABEL);
    };


    // 坐标
    //auto labelHintAxisPositions = new QLabel(QString::fromUtf8(tr("坐标")), this);
    auto labelHintAxisPositions = new QLabel(tr("坐标"), this);
    labelHintAxisPositions->setFixedHeight(btnheight);
    labelHintAxisPositions->setFont(fon);
    setCSS(labelHintAxisPositions);
    labelHintAxisPositions->setContentsMargins(5, 0, 0, 0);
    _layout->addWidget(labelHintAxisPositions);

    auto positionWidget=new QWidgetAxisPosition(this);
    positionWidget->init(axisCount,axisName);
    positionWidget->setStyleSheet(UIStyles::CSS_TEACHSTAUS_EDITE);//设置字体大小

    connect(this, SIGNAL(updatePositionWidget()),
            positionWidget, SLOT(updatePostion()));
    _layout->addWidget(positionWidget);


    // 总孔数
    //auto labelHintTotalPoint = new QLabel(QString::fromUtf8(tr("总孔数")), this);
    auto labelHintTotalPoint = new QLabel(tr("总孔数"), this);
    labelHintTotalPoint->setFixedHeight(btnheight);
    labelHintTotalPoint->setFont(fon);
    setCSS(labelHintTotalPoint);
    labelHintTotalPoint->setContentsMargins(5, 0, 0, 0);
    _labelTotalPoint = new QLabel(Utils::numberToString(GlobalData::realTimeInfo.iMNum), this);
    _labelTotalPoint->setFont(num_font);
    _labelTotalPoint->setAlignment(Qt::AlignCenter);
    _layout->addWidget(labelHintTotalPoint);
    _layout->addWidget(_labelTotalPoint);

    // 当前孔
    //auto labelHintCurrentPointIndex = new QLabel(QString::fromUtf8(tr("当前孔")), this);
    auto labelHintCurrentPointIndex = new QLabel(tr("当前孔"), this);
    labelHintCurrentPointIndex->setFixedHeight(btnheight);
    labelHintCurrentPointIndex->setFont(fon);
    setCSS(labelHintCurrentPointIndex);
    labelHintCurrentPointIndex->setContentsMargins(5, 0, 0, 0);
    _labelCurrentPointIndex = new QLabel(Utils::numberToString(GlobalData::realTimeInfo.iCNum), this);
    _labelCurrentPointIndex->setFont(num_font);
    _labelCurrentPointIndex->setAlignment(Qt::AlignCenter);
    _layout->addWidget(labelHintCurrentPointIndex);
    _layout->addWidget(_labelCurrentPointIndex);
#ifdef Arm_662
    //主轴角度
    auto labelHintMaixAngle = new QLabel(tr("主轴角度"), this);
    labelHintMaixAngle->setFixedHeight(btnheight);
    labelHintMaixAngle->setFont(fon);
    setCSS(labelHintMaixAngle);
    labelHintMaixAngle->setContentsMargins(5, 0, 0, 0);
    _labelMaixAngle = new QLabel(QString::fromUtf8("0"), this);
    _labelMaixAngle->setStyleSheet(UIStyles::CSS_TEACHSTAUS_EDITE);
    _labelMaixAngle->setFont(num_font);
    _labelMaixAngle->setAlignment(Qt::AlignCenter);
    _layout->addWidget(labelHintMaixAngle);
    _layout->addWidget(_labelMaixAngle);
    //当前文件显示
    auto labelCurrFile = new QLabel(tr("当前文件显示"), this);
    labelCurrFile->setFixedHeight(btnheight);
    labelCurrFile->setFont(fon);
    setCSS(labelCurrFile);
    labelCurrFile->setContentsMargins(5, 0, 0, 0);
    auto _labelCurrFile = new QLabel(GlobalData::workfileName, this);
    _labelCurrFile->setStyleSheet(UIStyles::CSS_TEACHSTAUS_EDITE);
    _labelCurrFile->setAlignment(Qt::AlignCenter);
    _labelCurrFile->setFont(num_font);
    _layout->addWidget(labelCurrFile);
    _layout->addWidget(_labelCurrFile);
#endif
    // 教导步长
    //auto labelHintTeachStep = new QLabel(QString::fromUtf8(tr("教导步长")), this);
    auto labelHintTeachStep = new QLabel(tr("教导步长"), this);
    labelHintTeachStep->setFixedHeight(btnheight);
    labelHintTeachStep->setFont(fon);
    setCSS(labelHintTeachStep);
    labelHintTeachStep->setContentsMargins(5, 0, 0, 0);
    _btnTeachStep = new QPushButton(Utils::numberToString(TEACH_STEPS[stepIndex], 3, 5), this);
    _btnTeachStep->setStyleSheet(UIStyles::css_teachcolorconf_u0);
    connect(_btnTeachStep, SIGNAL(clicked()), this, SLOT(_onClickedTeachStep()));
    _layout->addWidget(labelHintTeachStep);
    _layout->addWidget(_btnTeachStep);
#ifdef Arm_Kaiteli
    //快速教导
    _btnQuickteaching = new QPushButton(tr("快速修改"), this);
    //_btnTeachStep->setStyleSheet(UIStyles::CSS_TEACHSTAUS_BTN_STEP);
    _btnQuickteaching->setStyleSheet(UIStyles::css_teachcolorconf_u0);
    connect(_btnQuickteaching, SIGNAL(clicked()), this, SLOT(_onQuickteaching()));
    _layout->addWidget(_btnQuickteaching);
#elif ARM
    //联动
    pushButton_linkage = new QPushButton(tr("联动"), this);
    //_btnTeachStep->setStyleSheet(UIStyles::CSS_TEACHSTAUS_BTN_STEP);
    pushButton_linkage->setStyleSheet(UIStyles::css_teachcolorconf_u0);
    connect(pushButton_linkage, SIGNAL(clicked()), this, SLOT(onLinkage()));
    _layout->addWidget(pushButton_linkage);
#endif
    onSetTeachStepText();

    _isInitialized = true;
}

QWidgetTeachStatus::~QWidgetTeachStatus()
{

}

void QWidgetTeachStatus::onUpdateAxisPosition()
{
    GlobalData::backZeroIcon = false;
    emit updatePositionWidget();
}

void QWidgetTeachStatus::onLinkage()
{
    quint16 u16;
    ModbusQueue::element_t element;
    u16 = !GlobalData::realTimeInfo.iLinkage;
    element.data=&u16;
    element.len=sizeof(u16);
    element.operation=ModbusQueue::OP_WRITE_DATA;
    element.startAddress=ModbusAddress::LinkMove;//联动地址
    GlobalData::modbusQueue->sendQuery(&element,false);


}

void QWidgetTeachStatus::onUpdataLinkBtn()
{
    pushButton_linkage->setStyleSheet(GlobalData::realTimeInfo.iLinkage ? UIStyles::css_teachcolorconf_u1 : UIStyles::css_teachcolorconf_u0);
}


void QWidgetTeachStatus::onUpdateCurrentPointIndex(int index)
{
    _labelCurrentPointIndex->setText(Utils::numberToString(index));
}

void QWidgetTeachStatus::onUpdateTotalPoint(int totalPoint)
{//更新总孔数
    int temp=0;
    if(totalPoint>0)
    {//减去偏移
        temp=totalPoint-1;
    }
    else
    {
        temp=totalPoint;
    }
    //设置实际孔数
    _labelTotalPoint->setText(Utils::numberToString(temp));
}

//void QWidgetTeachStatus::_onClickedTeachStep()
//{
//    if((stepIndex + 1) < sizeof(TEACH_STEPS) / sizeof(TEACH_STEPS[0]))
//        ++stepIndex;
//    else
//        stepIndex = 0;

//    _btnTeachStep->setText(Utils::numberToString(TEACH_STEPS[stepIndex], 3, 5));
//    emit updateTeachStep(TEACH_STEPS[stepIndex]);
//}

//mod by yzg 190529
void QWidgetTeachStatus::_onClickedTeachStep()
{
    stepIndex=GlobalData::realTimeInfo.iHandRange;
    if((stepIndex + 1) < sizeof(TEACH_STEPS) / sizeof(TEACH_STEPS[0]))
        ++stepIndex;
    else
        stepIndex = 0;


    GlobalData::stepIndex = stepIndex;
    //_btnTeachStep->setText(Utils::numberToString(TEACH_STEPS[stepIndex], 3, 5));
    emit updateTeachStep(stepIndex);
}

void QWidgetTeachStatus::_onQuickteaching()
{// Arm_Kaiteli
    emit updateQuickteach();
}

void QWidgetTeachStatus::_onUpatarQuick(bool color)
{// Arm_Kaiteli
    _btnQuickteaching->setStyleSheet(color ? UIStyles::css_teachcolorconf_u1 : UIStyles::css_teachcolorconf_u0);
}

//mod by yzg 190529
void QWidgetTeachStatus::onSetTeachStepText()
{
    _btnTeachStep->setText(Utils::numberToString(TEACH_STEPS[GlobalData::realTimeInfo.iHandRange], 3, 5));
}
