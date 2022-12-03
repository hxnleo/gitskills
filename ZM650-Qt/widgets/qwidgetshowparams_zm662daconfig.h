#ifndef QWIDGETSHOWPARAMS_ZM662DACONFIG_H
#define QWIDGETSHOWPARAMS_ZM662DACONFIG_H

#include <QWidget>

namespace Ui {
class QWidgetShowParams_ZM662DAConfig;
}

class QWidgetShowParams_ZM662DAConfig : public QWidget
{
    enum DATypeData
    {//用来去区别上面还是下面
        One,
        Two,
    };
    Q_OBJECT

public:
    explicit QWidgetShowParams_ZM662DAConfig(QWidget *parent = 0);
    ~QWidgetShowParams_ZM662DAConfig();

protected slots:
    void onBtn_Voltage1();
    void onBtn_Voltage2();
    void onBtn_Speed1();
    void onBtn_Speed2();
    void onBtn_SpeedTest();
    void onBtn_WireTest();
    void onTimeOut();


private:
    Ui::QWidgetShowParams_ZM662DAConfig *ui;
};

#endif // QWIDGETSHOWPARAMS_ZM662DACONFIG_H
