/*
 * myinputpanel.h
 *
 * 一个修改版的输入面板(by
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef MYINPUTPANEL_H
#define MYINPUTPANEL_H

#include <QtGui>
#include <QtCore>

#include "ui_myinputpanelform.h"
#include "data/globaldata.h"

class MyInputPanel : public QWidget
{
    Q_OBJECT

public:
    MyInputPanel();

public slots:
    void show();
    void hide();
    void onSetNextFocusWidget();
    void onDestroyedFocusedWidget();

signals:
    void characterGenerated(quint32 character);
    void enterPressed();
    void closePressed();
    void composeStatusChanged(bool isComposing);
    void requestForShowPanel();
    void activateFocusedWidget();

protected:
    bool event(QEvent* e);
    QChar getMappedCharClicked(QWidget* w, int index) const;
    void paintEvent(QPaintEvent* e);

protected slots:
    void saveFocusWidget(QWidget* oldFocus, QWidget* newFocus);
    void buttonClicked(QWidget* w);
    void buttonClickedEn(QWidget* w);
    void moveWindow(QPoint pos);
    void timeoutInputEn();
    void toggleShift();
    void onActivateFocusedWidget();

private:
    Ui::MyInputPanelForm form;
    QWidget* lastFocusedWidget;
    QWidget* nextFocusWidget;
    QWidget* destroyedWidget;
    QSignalMapper signalmapperKey;
    QSignalMapper signalmapperEn;
    QTimer timerEnDelay;

    QWidget* lastClickedEn;
    int lastClickedCharIndex;
    bool isUpperCase;
    QMap<QWidget*, QString> mapWidgetToStringEn;
    void initEditNum();
};


#endif // MYINPUTPANEL_H

