/*
 * qpushbuttonspage.h
 *
 * 单列PushButton，用于页面底部的操作按钮
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QPUSHBUTTONSPAGE_H
#define QPUSHBUTTONSPAGE_H

#include <QWidget>
#include "data/definitions.h"
#include "widgets/qmypushbutton_u0.h"

class QPushButton;
class QMutex;


class QPushButtonsPage : public QWidget
{
    Q_OBJECT

public:
    struct item_t
    {
        QString text;
        int id;
    };


public:
    explicit QPushButtonsPage(QWidget* parent = nullptr);

    void initLayout(int countEachPage, bool isVertical = false); // 每页个数 是否垂直布局
    void init(const QList<item_t>& items); // 所有按钮，指针指向一个数组，该数组以name=QString()结束


    ~QPushButtonsPage();

signals:
    void buttonClicked(int id);

protected slots:
    void _buttonClicked(QWidget* w);
    void _buttonClickedNext();
    void _buttonClickedPrev();
    void _clearFocus();

protected: // members
    int currentPage;
    int maxPage;
    int countEachPage;

    //QPushButton* btn_next;
    //QPushButton* btn_prev;
    QMyPushButton_U0* btn_next;
    QMyPushButton_U0* btn_prev;
    QFont* font;

    //Definitions::WidgetMapList<QPushButton, QPushButtonsPage::item_t> maplist;
    Definitions::WidgetMapList<QMyPushButton_U0, QPushButtonsPage::item_t> maplist;

protected: // functions
    void refreshPageText();
    void deleteLayout();
    void calculateMaxPage();
};

#endif // QPUSHBUTTONSPAGE_H
