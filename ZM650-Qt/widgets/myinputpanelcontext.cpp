/*
 * myinputpanelcontext.cpp
 *
 * 一个修改版的输入面板(by
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include <QtCore>

#include "myinputpanelcontext.h"



MyInputPanelContext::MyInputPanelContext(QObject* parent): QInputContext(parent),
    w(nullptr),inputPanel(new MyInputPanel),
    _isComposing(false)
{
#ifdef _MCGS
    inputPanel->setAttribute(Qt::WA_ShowModal, true);//按键弹出后不能触发外部窗口事件
#endif
    connect(inputPanel, SIGNAL(characterGenerated(quint32)), SLOT(sendCharacter(quint32)));
    connect(inputPanel, SIGNAL(enterPressed()), this, SLOT(onEnterPressed()));
    connect(inputPanel, SIGNAL(closePressed()), this, SLOT(onClosePressed()));
    connect(inputPanel, SIGNAL(composeStatusChanged(bool)), this, SLOT(onComposeStatusChanged(bool)));
    connect(inputPanel, SIGNAL(requestForShowPanel()), this, SLOT(onRequestShowPanel()));
}

MyInputPanelContext::~MyInputPanelContext()
{
    // Generally, for safety purposes, do not use 'delete'
    // This is the exception. Because the time to delete this object is when the evenloop quited...
    // So we need to delete forcelly
    delete inputPanel;
}


bool MyInputPanelContext::filterEvent(const QEvent* event)
{
    if(event->type() == QEvent::RequestSoftwareInputPanel)
    {
        updatePosition();
        inputPanel->show();
        return true;
    }
    else if(event->type() == QEvent::CloseSoftwareInputPanel)
    {
        inputPanel->hide();
        return true;
    }

    return false;
}


QString MyInputPanelContext::identifierName()
{
    return "MyInputPanelContext";
}

void MyInputPanelContext::reset()
{
}

bool MyInputPanelContext::isComposing() const
{
    return _isComposing;
}

QString MyInputPanelContext::language()
{
    return "en_US";
}

void MyInputPanelContext::sendKey(quint32 chr)
{
    //QPointer<QWidget> w = focusWidget();//可以判断指针是否为空
    if(!w)
    {
        //QMessageBox::information(NULL, tr("新建"), tr("无焦点"));
        return;
    }
    QKeyEvent keyPress(QEvent::KeyPress, chr, Qt::NoModifier, QString(chr));
    QApplication::sendEvent(w, &keyPress);

    QKeyEvent keyRelease(QEvent::KeyRelease, chr, Qt::NoModifier, QString());
    QApplication::sendEvent(w, &keyRelease);
    //QMessageBox::information(NULL, tr("新建"), tr("焦点"));
}
void MyInputPanelContext::sendCharacter(quint32 character)
{
    //qDebug()<<character;
    sendKey(character);
}

void MyInputPanelContext::onEnterPressed()
{
    //获取进来的焦点控件 分配给每一个字符
    w = focusWidget();
//    qDebug()<<"focusWidget"<<w;
//清除原有内容
//    QLineEdit *wiget = qobject_cast<QLineEdit*>(focusWidget());
//    wiget->clear();
    QList<quint32>::iterator k;
    for(k=GlobalData::storenum.begin(); k!=GlobalData::storenum.end(); ++k)
    {
        sendKey(*k);
    }
    //清空焦点
    w = nullptr;
    GlobalData::storenum.clear();
    inputPanel->hide();
}
void MyInputPanelContext::onClosePressed()
{
    GlobalData::storenum.clear();
    inputPanel->hide();

}

void MyInputPanelContext::onComposeStatusChanged(bool isComposing)
{
    _isComposing = isComposing;
}

void MyInputPanelContext::onRequestShowPanel()
{
    if(false == _isComposing)
    {
        updatePosition();
        inputPanel->show();
    }
}

void MyInputPanelContext::updatePosition()
{
    //const int HEIGHT_INTERVAL=2;
    QWidget* widget = focusWidget();
    if(!widget)
        return;

    QRect rectScreenGeometry = QApplication::desktop()->geometry(); // 屏幕坐标

    QRect rectPanel = inputPanel->rect(); // 输入面板大小
    QRect rectWidget = widget->rect(); // TextEdit大小

    // -----------------------
    QPoint pointWidgetLeftTop(rectWidget.left(), rectWidget.top());
    QPoint pointWidgetRightTop(rectWidget.right(), rectWidget.top());
    QPoint pointWidgetLeftBottom(rectWidget.left(), rectWidget.bottom());
    //QPoint pointWidgetRightBottom(rectWidget.right(), rectWidget.bottom());

    auto mapPoint = [&widget](QPoint * p)
    {
        *p = widget->mapToGlobal(*p);
    };

    mapPoint(&pointWidgetLeftTop);
    mapPoint(&pointWidgetRightTop);
    mapPoint(&pointWidgetLeftBottom);
    //mapPoint(&pointWidgetRightBottom);

    // 起始点
    QPoint* pointOkToMove = nullptr;
    QPoint pointPanelLeft = QPoint(pointWidgetLeftTop.x() - rectPanel.width(), pointWidgetLeftTop.y());
    QPoint pointPanelRight = QPoint(pointWidgetRightTop.x(), pointWidgetRightTop.y());
    QPoint pointPanelTop = QPoint(pointWidgetLeftTop.x(), pointWidgetLeftTop.y() - rectPanel.height());
    QPoint pointPanelBottom = QPoint(pointWidgetLeftBottom.x(), pointWidgetLeftBottom.y());

    // 区
    QSize sizePanel = rectPanel.size();
    QRect rectPanelLeft = QRect(pointPanelLeft, sizePanel);
    QRect rectPanelRight = QRect(pointPanelRight, sizePanel);
    QRect rectPanelTop = QRect(pointPanelTop, sizePanel);
    QRect rectPanelBottom = QRect(pointPanelBottom, sizePanel);

    // 确定哪个区比较合适
    if((rectScreenGeometry & rectPanelRight).size() == sizePanel)
    {
        //qDebug("Fit right");
        pointOkToMove = &pointPanelRight;
    }
    else if((rectScreenGeometry & rectPanelBottom).size() == sizePanel)
    {
        //qDebug("Fit bottom");
        pointOkToMove = &pointPanelBottom;
    }
    else if((rectScreenGeometry & rectPanelLeft).size() == sizePanel)
    {
        //qDebug("Fit left");
        pointOkToMove = &pointPanelLeft;
    }
    else if((rectScreenGeometry & rectPanelTop).size() == sizePanel)
    {
        //qDebug("Fit top");
        pointOkToMove = &pointPanelTop;
    }
    else
    {
        // default
        //qDebug("Fit left or right");
        if(pointWidgetLeftTop.x() > rectScreenGeometry.width() / 2)
            pointOkToMove = &pointPanelLeft;
        else
            pointOkToMove = &pointPanelRight;
    }

    // 强制移动到屏幕内
    // 上下
    if(pointOkToMove->y() < rectScreenGeometry.top())
    {
        pointOkToMove->setY(rectScreenGeometry.top());
    }
    else if(pointOkToMove->y() > rectScreenGeometry.bottom() - rectPanel.height())
    {
        pointOkToMove->setY(rectScreenGeometry.bottom() - rectPanel.height());
    }

    // 左右
    if(pointOkToMove->x() < rectScreenGeometry.left())
    {
        pointOkToMove->setX(rectScreenGeometry.left());
    }
    else if(pointOkToMove->x() > rectScreenGeometry.right() - rectPanel.width())
    {
        pointOkToMove->setX(rectScreenGeometry.right() - rectPanel.width());
    }

    inputPanel->move(*pointOkToMove);

}

