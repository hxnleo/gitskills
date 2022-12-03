/*
 * myinputpanel.cpp
 *
 * 一个修改版的输入面板(by
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#include "myinputpanel.h"
#include "data/uistyles.h"
#include "data/utils.h"
#include <QApplication>
const char PROP_BUTTONVAL[] = "BtnVal";

MyInputPanel::MyInputPanel()
    : QWidget(0, Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint),
      lastFocusedWidget(nullptr),
      nextFocusWidget(nullptr),
      destroyedWidget(nullptr),
      lastClickedEn(nullptr),
      lastClickedCharIndex(0),
      isUpperCase(false)
{
    form.setupUi(this);
#ifdef _MCGS
     if(GlobalData::screensize == FIFTEEN)
     {
        QRect rectScreen = QApplication::desktop()->availableGeometry();
        setFixedSize(rectScreen.width()/4.5,rectScreen.height()/2.5);
     }
#endif
    MyInputPanel::initEditNum();
    timerEnDelay.setSingleShot(true);
    timerEnDelay.setInterval(500);
    form.lineEdit->setMaxLength(EDIT_MAXNUM);
    connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)),
            this, SLOT(saveFocusWidget(QWidget*, QWidget*)));

    //    connect(form.closeButton, SIGNAL(clicked()),
    //            this, SLOT(hide()));


    connect(this, SIGNAL(activateFocusedWidget()),
            this, SLOT(onActivateFocusedWidget()), Qt::QueuedConnection);


    // Tips: the focus policy of the QPushButton must be set to "NOFOCUS" !!!!
    form.panelButton_backspace->setProperty(PROP_BUTTONVAL, QVariant(quint32(Qt::Key_Backspace))); //设置退格键
    form.panelButton_up->setProperty(PROP_BUTTONVAL, QVariant(quint32(Qt::Key_Backtab)));
    form.panelButton_down->setProperty(PROP_BUTTONVAL, QVariant(quint32(Qt::Key_Tab)));
    form.panelButton_en_star->setProperty(PROP_BUTTONVAL, QVariant(quint32('*')));
    form.panelButton_en_sharp->setProperty(PROP_BUTTONVAL, QVariant(quint32('#')));
    form.panelButton_en_space->setProperty(PROP_BUTTONVAL, QVariant(quint32(Qt::Key_Space)));
    form.panelButton_enter->setProperty(PROP_BUTTONVAL, QVariant(quint32(Qt::Key_Enter)));
    form.closeButton->setProperty(PROP_BUTTONVAL, QVariant(quint32(Qt::Key_Clear))); //特殊按键 作为取消输入

    form.panelButton_0->setProperty(PROP_BUTTONVAL, QVariant(quint32(Qt::Key_0)));
    form.panelButton_1->setProperty(PROP_BUTTONVAL, QVariant(quint32(Qt::Key_1)));
    form.panelButton_2->setProperty(PROP_BUTTONVAL, QVariant(quint32(Qt::Key_2)));
    form.panelButton_3->setProperty(PROP_BUTTONVAL, QVariant(quint32(Qt::Key_3)));
    form.panelButton_4->setProperty(PROP_BUTTONVAL, QVariant(quint32(Qt::Key_4)));
    form.panelButton_5->setProperty(PROP_BUTTONVAL, QVariant(quint32(Qt::Key_5)));
    form.panelButton_6->setProperty(PROP_BUTTONVAL, QVariant(quint32(Qt::Key_6)));
    form.panelButton_7->setProperty(PROP_BUTTONVAL, QVariant(quint32(Qt::Key_7)));
    form.panelButton_8->setProperty(PROP_BUTTONVAL, QVariant(quint32(Qt::Key_8)));
    form.panelButton_9->setProperty(PROP_BUTTONVAL, QVariant(quint32(Qt::Key_9)));
    form.panelButton_minus->setProperty(PROP_BUTTONVAL, QVariant(quint32(Qt::Key_Minus)));
    form.panelButton_dot->setProperty(PROP_BUTTONVAL, QVariant(quint32(Qt::Key_Period)));

    // NUM
    signalmapperKey.setMapping(form.panelButton_1, form.panelButton_1);
    signalmapperKey.setMapping(form.panelButton_2, form.panelButton_2);
    signalmapperKey.setMapping(form.panelButton_3, form.panelButton_3);
    signalmapperKey.setMapping(form.panelButton_4, form.panelButton_4);
    signalmapperKey.setMapping(form.panelButton_5, form.panelButton_5);
    signalmapperKey.setMapping(form.panelButton_6, form.panelButton_6);
    signalmapperKey.setMapping(form.panelButton_7, form.panelButton_7);
    signalmapperKey.setMapping(form.panelButton_8, form.panelButton_8);
    signalmapperKey.setMapping(form.panelButton_9, form.panelButton_9);
    signalmapperKey.setMapping(form.panelButton_minus, form.panelButton_minus);
    signalmapperKey.setMapping(form.panelButton_0, form.panelButton_0);
    signalmapperKey.setMapping(form.panelButton_dot, form.panelButton_dot);
    signalmapperKey.setMapping(form.panelButton_backspace, form.panelButton_backspace);
    signalmapperKey.setMapping(form.panelButton_up, form.panelButton_up);
    signalmapperKey.setMapping(form.panelButton_down, form.panelButton_down);
    signalmapperKey.setMapping(form.panelButton_en_space, form.panelButton_en_space);
    signalmapperKey.setMapping(form.panelButton_en_star, form.panelButton_en_star);
    signalmapperKey.setMapping(form.panelButton_en_sharp, form.panelButton_en_sharp);
    signalmapperKey.setMapping(form.panelButton_enter, form.panelButton_enter);
    signalmapperKey.setMapping(form.closeButton, form.closeButton);

    connect(form.panelButton_1, SIGNAL(clicked()),
            &signalmapperKey, SLOT(map()));
    connect(form.panelButton_2, SIGNAL(clicked()),
            &signalmapperKey, SLOT(map()));
    connect(form.panelButton_3, SIGNAL(clicked()),
            &signalmapperKey, SLOT(map()));
    connect(form.panelButton_4, SIGNAL(clicked()),
            &signalmapperKey, SLOT(map()));
    connect(form.panelButton_5, SIGNAL(clicked()),
            &signalmapperKey, SLOT(map()));
    connect(form.panelButton_6, SIGNAL(clicked()),
            &signalmapperKey, SLOT(map()));
    connect(form.panelButton_7, SIGNAL(clicked()),
            &signalmapperKey, SLOT(map()));
    connect(form.panelButton_8, SIGNAL(clicked()),
            &signalmapperKey, SLOT(map()));
    connect(form.panelButton_9, SIGNAL(clicked()),
            &signalmapperKey, SLOT(map()));
    connect(form.panelButton_minus, SIGNAL(clicked()),
            &signalmapperKey, SLOT(map()));
    connect(form.panelButton_0, SIGNAL(clicked()),
            &signalmapperKey, SLOT(map()));
    connect(form.panelButton_dot, SIGNAL(clicked()),
            &signalmapperKey, SLOT(map()));
    connect(form.panelButton_backspace, SIGNAL(clicked()),
            &signalmapperKey, SLOT(map()));
    connect(form.panelButton_up, SIGNAL(clicked()),
            &signalmapperKey, SLOT(map()));
    connect(form.panelButton_down, SIGNAL(clicked()),
            &signalmapperKey, SLOT(map()));
    connect(form.panelButton_en_space, SIGNAL(clicked()),
            &signalmapperKey, SLOT(map()));
    connect(form.panelButton_en_star, SIGNAL(clicked()),
            &signalmapperKey, SLOT(map()));
    connect(form.panelButton_en_sharp, SIGNAL(clicked()),
            &signalmapperKey, SLOT(map()));
    connect(form.panelButton_enter, SIGNAL(clicked()),
            &signalmapperKey, SLOT(map()));
    connect(form.closeButton, SIGNAL(clicked()),
            &signalmapperKey, SLOT(map()));

    connect(&signalmapperKey, SIGNAL(mapped(QWidget*)),
            this, SLOT(buttonClicked(QWidget*)));

    // EN
    mapWidgetToStringEn.insert(form.panelButton_en_abc, QString("abc"));
    mapWidgetToStringEn.insert(form.panelButton_en_def, QString("def"));
    mapWidgetToStringEn.insert(form.panelButton_en_ghi, QString("ghi"));
    mapWidgetToStringEn.insert(form.panelButton_en_jkl, QString("jkl"));
    mapWidgetToStringEn.insert(form.panelButton_en_mno, QString("mno"));
    mapWidgetToStringEn.insert(form.panelButton_en_pqrs, QString("pqrs"));
    mapWidgetToStringEn.insert(form.panelButton_en_tuv, QString("tuv"));
    mapWidgetToStringEn.insert(form.panelButton_en_wxyz, QString("wxyz"));

    signalmapperEn.setMapping(form.panelButton_en_abc, form.panelButton_en_abc);
    signalmapperEn.setMapping(form.panelButton_en_def, form.panelButton_en_def);
    signalmapperEn.setMapping(form.panelButton_en_ghi, form.panelButton_en_ghi);
    signalmapperEn.setMapping(form.panelButton_en_jkl, form.panelButton_en_jkl);
    signalmapperEn.setMapping(form.panelButton_en_mno, form.panelButton_en_mno);
    signalmapperEn.setMapping(form.panelButton_en_pqrs, form.panelButton_en_pqrs);
    signalmapperEn.setMapping(form.panelButton_en_tuv, form.panelButton_en_tuv);
    signalmapperEn.setMapping(form.panelButton_en_wxyz, form.panelButton_en_wxyz);

    connect(form.panelButton_en_abc, SIGNAL(clicked()),
            &signalmapperEn, SLOT(map()));
    connect(form.panelButton_en_def, SIGNAL(clicked()),
            &signalmapperEn, SLOT(map()));
    connect(form.panelButton_en_ghi, SIGNAL(clicked()),
            &signalmapperEn, SLOT(map()));
    connect(form.panelButton_en_jkl, SIGNAL(clicked()),
            &signalmapperEn, SLOT(map()));
    connect(form.panelButton_en_mno, SIGNAL(clicked()),
            &signalmapperEn, SLOT(map()));
    connect(form.panelButton_en_pqrs, SIGNAL(clicked()),
            &signalmapperEn, SLOT(map()));
    connect(form.panelButton_en_tuv, SIGNAL(clicked()),
            &signalmapperEn, SLOT(map()));
    connect(form.panelButton_en_wxyz, SIGNAL(clicked()),
            &signalmapperEn, SLOT(map()));

    connect(&signalmapperEn, SIGNAL(mapped(QWidget*)),
            this, SLOT(buttonClickedEn(QWidget*)));

    connect(form.labelMove, SIGNAL(wantToMoveWindow(QPoint)),
            this, SLOT(moveWindow(QPoint)));

    connect(&timerEnDelay, SIGNAL(timeout()),
            this, SLOT(timeoutInputEn()));

    connect(form.panelButton_en_shift, SIGNAL(clicked()),
            this, SLOT(toggleShift()));

    /*
    connect(form.panelButton_en_left, SIGNAL(keyShortPress()),
            this, SLOT(kShort()));
    connect(form.panelButton_en_left, SIGNAL(keyLongPressBegin()),
            this, SLOT(kLong1()));
    connect(form.panelButton_en_left, SIGNAL(keyLongPressEnd()),
            this, SLOT(kLong2()));
    */

    // Auto repeat
    auto setAutoRepeatForButton = [](QPushButton * btn)
    {
        btn->setAutoRepeat(true);
        btn->setAutoRepeatDelay(500);
        btn->setAutoRepeatInterval(100);
    };

    setAutoRepeatForButton(form.panelButton_0);
    setAutoRepeatForButton(form.panelButton_1);
    setAutoRepeatForButton(form.panelButton_2);
    setAutoRepeatForButton(form.panelButton_3);
    setAutoRepeatForButton(form.panelButton_4);
    setAutoRepeatForButton(form.panelButton_5);
    setAutoRepeatForButton(form.panelButton_6);
    setAutoRepeatForButton(form.panelButton_7);
    setAutoRepeatForButton(form.panelButton_8);
    setAutoRepeatForButton(form.panelButton_9);

    setAutoRepeatForButton(form.panelButton_en_space);
    setAutoRepeatForButton(form.panelButton_backspace);
    //setAutoRepeatForButton(form.panelButton_up);
    //setAutoRepeatForButton(form.panelButton_down);

    // CSS
    //form.tabWidget->setStyleSheet("QTabBar::tab { min-height: 6em; min-width: 4em;}");
}

void MyInputPanel::show()
{
    emit composeStatusChanged(true);
//    form.tabWidget->setWindowModality(Qt::ApplicationModal);
    form.tabWidget->setCurrentIndex(0);
    isUpperCase = false;
    QWidget::show();
}

void MyInputPanel::hide()
{
    emit composeStatusChanged(false);
    QWidget::hide();
}

void MyInputPanel::onSetNextFocusWidget()
{
    QWidget* w = qobject_cast<QWidget*>(sender());

    if(w)
        nextFocusWidget = w;
}

void MyInputPanel::onDestroyedFocusedWidget()
{
    destroyedWidget = qobject_cast<QWidget*>(sender());
}

bool MyInputPanel::event(QEvent* e)
{
    switch(e->type())
    {//单击了某一个窗体会激活窗体
    case QEvent::WindowActivate:

        //qDebug("event = %d, WINDOW ACTIVE",e->type());
        if(nullptr != lastFocusedWidget && lastFocusedWidget != destroyedWidget)
            emit activateFocusedWidget();
        break;
    default:
        break;
    }
    return QWidget::event(e);
}

QChar MyInputPanel::getMappedCharClicked(QWidget* w, int index) const
{
    QChar qchr(mapWidgetToStringEn[w].at(index));

    if(isUpperCase)
        qchr = qchr.toUpper();

    return qchr;
}

void MyInputPanel::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);
    painter.drawRoundedRect(0, 5, width() - 5, height() - 7, 3, 3);
    QWidget::paintEvent(e);
}

void MyInputPanel::toggleShift()
{
    isUpperCase = !isUpperCase;

    auto setTextUpperOrLower = [](QPushButton * btn, bool isUpper)
    {
        QString t(btn->text());

        if(isUpper)
            btn->setText(t.toUpper());
        else
            btn->setText(t.toLower());
    };

    // Make letter upper case
    setTextUpperOrLower(form.panelButton_en_abc, isUpperCase);
    setTextUpperOrLower(form.panelButton_en_def, isUpperCase);
    setTextUpperOrLower(form.panelButton_en_ghi, isUpperCase);
    setTextUpperOrLower(form.panelButton_en_jkl, isUpperCase);
    setTextUpperOrLower(form.panelButton_en_mno, isUpperCase);
    setTextUpperOrLower(form.panelButton_en_pqrs, isUpperCase);
    setTextUpperOrLower(form.panelButton_en_tuv, isUpperCase);
    setTextUpperOrLower(form.panelButton_en_wxyz, isUpperCase);

    // Color
    if(isUpperCase)
    {
        form.panelButton_en_shift->setStyleSheet(UIStyles::CSS_INPUTPANEL_SHIFT);
    }
    else
    {
        form.panelButton_en_shift->setStyleSheet(QString());
    }
}

void MyInputPanel::onActivateFocusedWidget()
{
    lastFocusedWidget->activateWindow();
}

void MyInputPanel::saveFocusWidget(QWidget* oldFocus, QWidget* newFocus)
{
    Q_UNUSED(oldFocus)

    if(newFocus != 0 && !this->isAncestorOf(newFocus))
    {
        if(newFocus == nextFocusWidget)
        {
            emit requestForShowPanel();
        }

        lastFocusedWidget = newFocus;
        nextFocusWidget = nullptr;

        //qDebug("inputPanel: old=%x, new=%x",oldFocus,newFocus);
    }
}

void MyInputPanel::initEditNum()
{
    //另外一个储存值在发送完成后清除
    form.lineEdit->setText("");//清除预输入
    GlobalData::editnum.clear();//清除累计的qstring
    GlobalData::num=0;
}

void MyInputPanel::buttonClicked(QWidget* w)
{
    quint32 chr = qvariant_cast<quint32>(w->property(PROP_BUTTONVAL));
    //qDebug()<<chr;
    //设置lineEdit
    QString str = "";
    if(GlobalData::num==EDIT_MAXNUM)
    {
        if(Qt::Key_Backspace == chr)
        {//退格操作
            GlobalData::num-=1;
            GlobalData::editnum.chop(1);//删除最后一位
            //GlobalData::editnum = GlobalData::editnum.left(GlobalData::editnum.length() - 1);
        }
    }
    else if(GlobalData::num<EDIT_MAXNUM)
    {
        if(chr==45)
        {
            str= "-";
            GlobalData::num+=1;
            GlobalData::editnum=GlobalData::editnum+str;
        }
        else if(chr==46)
        {
            str = ".";
            GlobalData::num+=1;
            GlobalData::editnum=GlobalData::editnum+str;
        }
        else if(chr>=48&&chr<=57)
        {
            str = QString::number(chr-48);
            GlobalData::num+=1;
            GlobalData::editnum=GlobalData::editnum+str;
        }
        else if(Qt::Key_Backspace != chr)
        {
            emit characterGenerated(chr);
        }
    }
    if(Qt::Key_Enter == chr)
    {
        //qDebug("Enter pressed!");
        if(form.lineEdit->text().isEmpty())
        {
            this->hide();
            return;
        }
        initEditNum();
        GlobalData::storenum.append(chr);//把原始数据传进去就好
        form.lineEdit->setText(GlobalData::editnum);
        emit enterPressed();//en界面
        return;
    }
    else if(Qt::Key_Clear == chr)
    {
        //qDebug("Clear pressed!");
        initEditNum();
        emit closePressed();
    }
    else if(Qt::Key_Backspace == chr)
    {//退格操作
        if(!GlobalData::infile)
        {
            if(form.tabWidget->currentIndex())
            {//en

                emit characterGenerated(chr);
            }
            else
            {
                if(GlobalData::num!=0)
                {
                    GlobalData::num-=1;
                    GlobalData::editnum.chop(1);//删除最后一位
                }

            }
        }
        else
        {
            GlobalData::num-=1;
            GlobalData::editnum.chop(1);//删除最后一位
        }
    }

    //qDebug()<<form.tabWidget->currentIndex();//获取活动界面
    GlobalData::storenum.append(chr);//把原始数据传进去就好
    form.lineEdit->setText(GlobalData::editnum);
}

void MyInputPanel::buttonClickedEn(QWidget* w)
{
    if(w != lastClickedEn)
    {
        if(nullptr != lastClickedEn && timerEnDelay.isActive())
        {
            timerEnDelay.stop();
            //qDebug()<<"immm char:"<<mapWidgetToStringEn[lastClickedEn][lastClickedCharIndex];
            QChar qchr = getMappedCharClicked(lastClickedEn, lastClickedCharIndex);
            emit characterGenerated(qvariant_cast<quint32>(qchr));

        }

        lastClickedCharIndex = -1;
        lastClickedEn = w;
    }

    if(mapWidgetToStringEn.contains(w))
    {
        if(lastClickedCharIndex >= mapWidgetToStringEn[w].size() - 1)
            lastClickedCharIndex = 0;
        else
            ++lastClickedCharIndex;

        //qDebug()<<"new char:"<<mapWidgetToStringEn[w][lastClickedCharIndex];
        QChar qchr = getMappedCharClicked(w, lastClickedCharIndex);
        form.labelChar->setText(QString(qchr));
        timerEnDelay.start();
    }
    else
    {
        //qDebug("MyInputPanel::buttonClickedEn: Not such Key!");
    }
}

void MyInputPanel::moveWindow(QPoint pos)
{
    this->move(mapToParent(pos));
}

void MyInputPanel::timeoutInputEn()
{
    QChar qchr = getMappedCharClicked(lastClickedEn, lastClickedCharIndex);
    if(GlobalData::infile)
    {//用于数字英文混合输入
        if(GlobalData::num<EDIT_MAXNUM)
        {
            GlobalData::editnum=GlobalData::editnum+qchr;
            GlobalData::storenum.append(qvariant_cast<quint32>(qchr));//把原始数据传进去就好
            form.lineEdit->setText(GlobalData::editnum);
            GlobalData::num+=1;
        }
    }
    else
    {
        emit characterGenerated(qvariant_cast<quint32>(qchr));
        lastClickedEn = nullptr;
        form.labelChar->setText("");
    }
    //qDebug()<<GlobalData::infile;
}

