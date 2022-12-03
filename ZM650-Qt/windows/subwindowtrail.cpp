/*
 * subwindowtrail.cpp
 *
 * 轨迹子窗口，支持设置各种参数
 *
 * Created on: 2019年 01月 23日 星期三 22:10:41 CST
 * Author: lixingcong
 */
#include "subwindowtrail.h"
#include "ui_subwindowtrail.h"
#include "data/globaldata.h"
#include "widgets/qpushbuttonspage.h"
#include "data/workfile.h"
#include "widgets/qdialogloadingworkfile.h"
#include "widgets/qdialogdataform.h"
#include "data/utils.h"

SubWindowTrail::SubWindowTrail(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SubWindowTrail),
    isFollowing(false),
    size(2)
{
    ui->setupUi(this);
    //GlobalData::labelTopTab->setText(QString::fromUtf8("轨迹预览"));
    GlobalData::labelTopTab->setText(tr("轨迹预览"));//mod by yzg

    memset(&positions, 0, sizeof(positions[0][0])*MAXTEACHNUM * 2);

    // bottom btns
    QList<QPushButtonsPage::item_t> listButtonItems =
    {
        //mod by yzg
        {tr("恢复"), Definitions::BTN_SYS_TRACEPATH_PREVIEW},
        {tr("跟踪"), Definitions::BTN_SYS_TRACEPATH_FOLLOW},
//        {tr("最佳"), Definitions::BTN_SYS_TRACEPATH_ZOOM_RESET},
        {tr("放大"), Definitions::BTN_SYS_TRACEPATH_ZOOM_IN},
        {tr("缩小"), Definitions::BTN_SYS_TRACEPATH_ZOOM_OUT},
        {tr("比例"), Definitions::BTN_TEACH_SIZE},
        {tr("轴选"), Definitions::BTN_SYS_TRACEPATH_AXIS},
        {tr("花样"), Definitions::BTN_TEACH_PATTERN},
    };

    GlobalData::pushButtonPage->init(listButtonItems);
    connect(GlobalData::pushButtonPage, SIGNAL(buttonClicked(int)),
            this, SLOT(_onClickedBottomButton(int)));

    ui->widgetRunStatus->init(GlobalData::realaxisName, GlobalData::totalAxisNum);
    ui->widgetTrail->init(positions, pointColors,6.0);

    onAcceptSetAxis();
    isLoaded = Workfile::getInstance()->isAlreadyReadAll();

    if(false == isLoaded)
    {
        auto d = new QDialogLoadingWorkfile(this);
        d->setAttribute(Qt::WA_DeleteOnClose);
        connect(d, SIGNAL(accepted()),
                this, SLOT(pickUpPositionData()));
        d->readWrite(true, -1, 0); // read all
    }else{
        pickUpPositionData();
    }

    // global signals
    connect(GlobalData::getInstance(), SIGNAL(updateRealtimeInfo()),
            this, SLOT(onUpdateRealtimeInfo()));

    onUpdateRealtimeInfo(); // update now!
    _onClickedBottomButton(Definitions::BTN_SYS_TRACEPATH_PREVIEW);
}

SubWindowTrail::~SubWindowTrail()
{
    delete ui;
}

void SubWindowTrail::onUpdateRealtimeInfo()
{
    static quint32 lastPointIndex = 0xffffffff;
    static quint32 lastTotalPoint = 0xffffffff;

    ui->widgetRunStatus->onUpdateAxisPosition();

    if(lastPointIndex != GlobalData::realTimeInfo.iCNum)
    {
        auto i = static_cast<int>(GlobalData::realTimeInfo.iCNum);
        emit _setCurrentPoint(i - 1); // 减去起点

        ui->widgetRunStatus->onUpdateCurrentPointIndex(i);

        lastPointIndex = GlobalData::realTimeInfo.iCNum;
    }

    if(lastTotalPoint != GlobalData::realTimeInfo.iMNum)
    {
        auto i = static_cast<int>(GlobalData::realTimeInfo.iMNum);
        ui->widgetRunStatus->onUpdateTotalPoint(i);

        lastTotalPoint = GlobalData::realTimeInfo.iMNum;
    }
}

void SubWindowTrail::_onClickedBottomButton(int btn_id)
{
    switch(btn_id)
    {
    case Definitions::BTN_SYS_TRACEPATH_PREVIEW:
        //进来直接显示图像 不再需要按钮
        if(true == isFollowing)
        {
            disconnect(this, SIGNAL(_setCurrentPoint(int)),
                       ui->widgetTrail, SLOT(setHighlightIndex(int)));
            isFollowing = false;
        }

        if(true == isLoaded && GlobalData::realTimeInfo.iMNum > 1)
        {
            ui->widgetTrail->clearcurr();
            ui->widgetTrail->recoverScaleCenter();
            ui->widgetTrail->setHighlightIndex(-1);
            ui->widgetTrail->setTotalPoint(GlobalData::realTimeInfo.iMNum - 1);
            ui->widgetTrail->resetToFit();
        }
        break;

    case Definitions::BTN_SYS_TRACEPATH_FOLLOW:
        if(false == isFollowing)
        {
            ui->widgetTrail->setTotalPoint(GlobalData::realTimeInfo.iMNum - 1);
            ui->widgetTrail->resetToFit();

            connect(this, SIGNAL(_setCurrentPoint(int)),
                    ui->widgetTrail, SLOT(setHighlightIndex(int)));

            isFollowing = true;
        }
        break;

    case Definitions::BTN_SYS_TRACEPATH_ZOOM_RESET:
        ui->widgetTrail->resetToFit();
        break;

    case Definitions::BTN_SYS_TRACEPATH_ZOOM_IN:
        ui->widgetTrail->zoomIn();
        break;

    case Definitions::BTN_SYS_TRACEPATH_ZOOM_OUT:
        ui->widgetTrail->zoomOut();
        break;

    case Definitions::BTN_SYS_TRACEPATH_AXIS:
    {
        //QStringList dirText = {QString::fromUtf8("负方向"), QString::fromUtf8("正方向")};
        QStringList dirText = {tr("负方向"), tr("正方向")};//mod by yzg
        QStringList typeText = {tr("平板"), tr("圆盘")};
        qDebug()<<"realaxisName"<<GlobalData::realaxisName;
        QList<QDialogDataForm::item_t> items =
        {
            {
                //QString::fromUtf8("横坐标轴"), Adapters::TYPE_StringList, &GlobalData::selectedAxisNum[0], false, 0, GlobalData::totalAxisNum - 1.0,
                tr("图形样式"), Adapters::TYPE_StringList, &GlobalData::slectetype, false, 0, 4,
                &typeText

            },
            {
                //QString::fromUtf8("横坐标轴"), Adapters::TYPE_StringList, &GlobalData::selectedAxisNum[0], false, 0, GlobalData::totalAxisNum - 1.0,
                tr("横坐标轴"), Adapters::TYPE_StringList, &GlobalData::selectedAxisNum[0], false, 0, GlobalData::totalAxisNum - 1.0,
                const_cast<QStringList*>(&GlobalData::realaxisName)

            },
            {
                //QString::fromUtf8("横坐标轴方向"), Adapters::TYPE_StringList, &GlobalData::selectedAxisDirection[0], false, 0, 1,
                tr("横坐标轴方向"), Adapters::TYPE_StringList, &GlobalData::selectedAxisDirection[0], false, 0, 1,
                &dirText
            },
            {
                //QString::fromUtf8("纵坐标轴"), Adapters::TYPE_StringList, &GlobalData::selectedAxisNum[1], false, 0, 1,
                tr("纵坐标轴"), Adapters::TYPE_StringList, &GlobalData::selectedAxisNum[1], false, 0, 1,
                const_cast<QStringList*>(&GlobalData::realaxisName)
            },
            {
                //QString::fromUtf8("纵坐标轴方向"), Adapters::TYPE_StringList, &GlobalData::selectedAxisDirection[1], false, 0, 1,
                tr("纵坐标轴方向"), Adapters::TYPE_StringList, &GlobalData::selectedAxisDirection[1], false, 0, 1,
                &dirText
            },
        };
        auto d = new QDialogDataForm(this);
        connect(d, SIGNAL(accepted()), this, SLOT(onAcceptSetAxis()));
        //d->showDialog("设置坐标方向", items);
        d->showDialog(tr("设置坐标方向"), items);//mod by yzg

        break;
    }
    case  Definitions::BTN_TEACH_SIZE:
    {
        QList<QDialogDataForm::item_t> items =
        {
            {
                //QString::fromUtf8("横坐标轴"), Adapters::TYPE_StringList, &GlobalData::selectedAxisNum[0], false, 0, GlobalData::totalAxisNum - 1.0,
                tr("缩放比例"), Adapters::TYPE_F32, &size, false, 1, 10,
                const_cast<QStringList*>(&GlobalData::realaxisName)
            },
        };
        auto d = new QDialogDataForm(this);
        connect(d, SIGNAL(accepted()), this, SLOT(onSetScale()));
        d->showDialog(tr("设置缩放比例"), items);//mod by yzg
        break;
    }

    default:
        break;
    }
}

void SubWindowTrail::pickUpPositionData()
{
    qDebug("read done");
    auto teachData = Workfile::getInstance()->getTeachData() + 1; // 跳过第一个点（起点）

    for(quint32 i = 0; i < GlobalData::realTimeInfo.iMNum; ++i)
    {
        positions[i][0] = (teachData + i)->fMove_pos[GlobalData::selectedAxisNum[0]];
        positions[i][1] = (teachData + i)->fMove_pos[GlobalData::selectedAxisNum[1]];
        pointColors[i] = static_cast<int>((teachData + i)->icolor);
    }
    isLoaded = true;
}

void SubWindowTrail::onAcceptSetAxis()
{
    if(GlobalData::slectetype)//处理下发数据
        GlobalData::parambite = SET_BIT(GlobalData::parambite,3);
    else
        GlobalData::parambite = CLR_BIT(GlobalData::parambite,3);
    Utils::sendOffest(&GlobalData::parambite,ModbusAddress::ParamUse,sizeof(GlobalData::parambite),true);


    if(true == isLoaded)
        pickUpPositionData();

    ui->widgetTrail->setCoordText(GlobalData::realaxisName.at(GlobalData::selectedAxisNum[0]), GlobalData::realaxisName.at(GlobalData::selectedAxisNum[1]));
    ui->widgetTrail->setCoordDirectoin(GlobalData::selectedAxisDirection[0] > 0, GlobalData::selectedAxisDirection[1] > 0);
    _onClickedBottomButton(Definitions::BTN_SYS_TRACEPATH_PREVIEW);
}

void SubWindowTrail::onSetScale()
{
    ui->widgetTrail->setScalesize(size);
}
