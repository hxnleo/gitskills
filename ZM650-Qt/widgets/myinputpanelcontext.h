/*
 * myinputpanelcontext.h
 *
 * 一个修改版的输入面板(by
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef MYINPUTPANELCONTEXT_H
#define MYINPUTPANELCONTEXT_H

#include <QtGui/QInputContext>

#include "myinputpanel.h"

class MyInputPanel;


class MyInputPanelContext : public QInputContext
{
    Q_OBJECT

public:
    MyInputPanelContext(QObject* parent = 0);
    ~MyInputPanelContext();

    bool filterEvent(const QEvent* event);

    QString identifierName();
    QString language();

    bool isComposing() const;

    void reset();
    void sendKey(quint32 chr);
    MyInputPanel* getInputPanel() const
    {
        return inputPanel;
    }

    QPointer<QWidget> w;

private slots:
    void sendCharacter(quint32 character);
    //void ensendCharacter(quint32 character);
    void onEnterPressed();
    void onClosePressed();
    void onComposeStatusChanged(bool isComposing);
    void onRequestShowPanel();

private:
    void updatePosition();

private:
    MyInputPanel* inputPanel;
    bool _isComposing;
};


#endif // MYINPUTPANELCONTEXT_H
