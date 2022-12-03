/*
 * uistyles.h
 *
 * 用户界面的共享变量，例如CSS
 *
 * Created on: 2019年 01月 21日 星期一 20:20:56 CST
 * Author: lixingcong
 */

#ifndef UISTYLES_H
#define UISTYLES_H

class QString;
class QLayout;
class QColor;

#include <QList>
#define NORMAL 0
namespace UIStyles
{
extern const QString CSS_IO_HIGHLIGHT;
extern const QString CSS_IO_LOWIGHT;
extern const QString CSS_INPUTPANEL_SHIFT;
extern const QString CSS_PUSHBUTTON_TAB_ACTIVE;
extern const QString CSS_DIAGNOSE_VERSION_LABEL;
extern const QString CSS_TEACHSTAUS_LABEL;
extern const QString CSS_TEACHSTAUS_EDITE;
extern const QString CSS_TEACHSTAUS_EDITE1;
extern const QString CSS_TEACHSTAUS_EDITE2;
extern const QString CSS_TEACHSTAUS_BTN_STEP;
extern const QString CSS_TEACHSTAUS_BRODER;
extern const QString CSS_RUNSTATUS_POS_LABEL;
extern const QString CSS_RUNSTATUS_BACKGROUND;

// alarm
extern const QString CSS_ALARMMSG_LEVEL_1;
extern const QString CSS_ALARMMSG_LEVEL_2_1;
extern const QString CSS_ALARMMSG_LEVEL_2_2;

extern const QString CSS_ALARMMSG_LEVEL_3;
extern const QString CSS_ALARMMSG_LEVEL_3_1;
extern const QString CSS_ALARMMSG_LEVEL_3_2;

extern const QString CSS_TEACH_COMBOBOX;
extern const QList<QColor> COLOR_TRAIL;
extern const QString css_QRadioButton;
//main color config
extern const QString css_maincolorconf_u0;
extern const QString css_maincolorconf_u1;
extern const QString css_maincolorconf_u2;
extern const QString css_maincolorconf_u3;
extern const QString css_maincolorconf_u4;
extern const QString css_maincolorconf_u5;
extern const QString css_maincolorconf_u6;
extern const QString css_maincolorconf_u7;
extern const QString css_maincolorconf_u8;
extern const QString css_maincolorconf_u9;
extern const QString css_maincolorconf_u10;
extern const QString css_maincolorconf_u11;
extern const QString css_maincolorconf_u12;
extern const QString css_maincolorconf_u13;
extern const QString css_maincolorconf_u14;
extern const QString css_maincolorconf_u15;

extern const QString css_maincolorconf_cylinder;
extern const QString css_maincolorconf_cylinder1;
extern const QString css_maincolorconf_cylinder2;

//teach color config
extern const QString css_teachcolorconf_u0;//X+,X-
extern const QString css_teachcolorconf_u1;//X+,X-

//para color config
extern const QString css_paracolorconf_u0;
extern const QString css_paracolorconf_u1;
extern const QString css_paracolorconf_u2;
extern const QString css_paracolorconf_u3;
extern const QString css_paracolorconf_u4;


void setMarginAndSpacing(QLayout* layout, int margins = 0, int spacing = 1);
};

#endif // UISTYLES_H
