#ifndef SUBWINDOWLOCALCONFIG_H
#define SUBWINDOWLOCALCONFIG_H

#include <QWidget>
#include "platform/common.h"
#include <QMap>

namespace Ui {
class SubWindowLocalConfig;
}

class SubWindowLocalConfig : public QWidget
{
    Q_OBJECT

public:
    explicit SubWindowLocalConfig(QWidget *parent = nullptr);
    ~SubWindowLocalConfig();

private slots:

    //void on_QtSqlite_test_clicked();

    //void on_CSqlite_clicked();

    void on_beep_clicked1();

    void on_backlight_clicked1();

    void on_setIp_clicked1();

    void on_getIp_clicked1();

    void on_systime_clicked1();

    void on_setrtc_clicked1();

    void on_getrtc_clicked1();

private slots:
    void timerUpDate();

private:
    void InitOther();//初始化基本设置
    QMap<int, QString>  m_lightItems;
private:
    Ui::SubWindowLocalConfig *ui;
};

#endif // SUBWINDOWLOCALCONFIG_H
