/*
 * uistyles.cpp
 *
 * 用户界面的共享变量，例如CSS
 *
 * Created on: 2019年 01月 21日 星期一 20:20:56 CST
 * Author: lixingcong
 */

#include "uistyles.h"
#include <QString>
#include <QLayout>
#include <QColor>

namespace UIStyles
{
// IO高电平
const QString CSS_IO_HIGHLIGHT = "QPushButton {color:white; background-color: #FF6666;}";

// 输入面板的Shift按键
const QString CSS_INPUTPANEL_SHIFT = "QPushButton {" "background-color: blue" "}";

// 顶部标签页的按钮
const QString CSS_PUSHBUTTON_TAB_ACTIVE = "QPushButton {background-color: #3cbaa2;}";

// 诊断信息：软件版本
const QString CSS_DIAGNOSE_VERSION_LABEL = "QLabel{border: 2px solid gray;}";

// 教导列表 左侧的状态显示
const QString CSS_TEACHSTAUS_LABEL = "color:white; background-color:#6880C0;";
const QString CSS_TEACHSTAUS_BTN_STEP = "Text-align:left;";
const QString CSS_TEACHSTAUS_EDITE = "font-size:20px";
const QString CSS_TEACHSTAUS_EDITE1 = "font-size:17px";
const QString CSS_TEACHSTAUS_EDITE2 = "font-size:14px";
const QString CSS_TEACHSTAUS_BRODER = "QWidget{border:1px solid blue;}";


// 主界面运行状态
const QString CSS_RUNSTATUS_POS_LABEL = "color:white;"
                                        ;
const QString CSS_RUNSTATUS_BACKGROUND = "background-color:black;";

// 报警栏背景
const QString CSS_ALARMMSG_LEVEL_1 = "QLabel{color:black;background:#66cc99;border: 3px groove gray;}";
const QString CSS_ALARMMSG_LEVEL_2_1 = "QLabel{color:black;background:#ffff00;border: 3px groove gray;}";
const QString CSS_ALARMMSG_LEVEL_2_2 = "QLabel{color:black;background:#ffff99;border: 3px groove gray;}";


const QString CSS_ALARMMSG_LEVEL_3 = "QLabel{color:white;background:#0000A8;border: 3px groove gray;}";
const QString CSS_ALARMMSG_LEVEL_3_1 = "QLabel{color:white;background:#ff0000;border: 3px groove gray;}";
const QString CSS_ALARMMSG_LEVEL_3_2 = "QLabel{color:white;background:#cc0033;border: 3px groove gray;}";

// 教导列表下拉框
const QString CSS_TEACH_COMBOBOX = "QComboBox QAbstractItemView::item { min-height: 1em;}";

// 轨迹颜色
const QList<QColor> COLOR_TRAIL =
{
    QColor("#1A4CDC"),
    QColor("#6D49C7"),
    QColor("#1DAE26"),
    QColor("#C0D817")
};


//QRadioButton checked
const QString css_QRadioButton="\
        QRadioButton::indicator:unchecked {\
            image: url(:/images/mainIco/checkbox_off.ico);}\
        QRadioButton::indicator:checked {\
            image: url(:/images/mainIco/checkbox_on.ico);}";


//main color
const QString css_maincolorconf_u0="QPushButton{background:qlineargradient("
                                   "spread:pad,x1:0,y1:0,x2:0,y2:1,"
                                   "stop:0 #B0B4F8,"
                                   "stop:0.5 #B0B4F8,"
                                   "stop:1 #B0B4F8);\
                color: black;   border-radius: 10px;  border: 2px groove gray;\
                border-style: outset;}"
                "QPushButton:pressed{background-color:rgb(85, 170, 255);\
                       border-style: inset; }";

const QString css_maincolorconf_u1="QComboBox{background:qlineargradient("
                                   "spread:pad,x1:0,y1:0,x2:0,y2:1,"
                                   "stop:0 #F0FFF0,"
                                   "stop:0.5 #6880C0,"
                                   "stop:1 #F0FFF0);\
               color: black;   border: 2px groove gray;\
               border-style: outset;}"
               "QComboBox:pressed{background-color:rgb(85, 170, 255);\
                              //border-style: inset;"
               "}"
               "QComboBox QAbstractItemView::item { min-height: 40px; min-width: 60px;border: 1px solid rgb(161,161,161); }"
               "QComboBox QAbstractItemView { border: 2px solid darkgray; selection-background-color: lightgray;}";



const QString css_maincolorconf_u2="QPushButton{background:qlineargradient("
                                   "spread:pad,x1:0,y1:0,x2:0,y2:1,"
                                   "stop:0 #D8BFD8,"
                                   "stop:0.5 #989898,"
                                   "stop:1 #D8BFD8);\
                color: black;   border-radius: 10px;  border: 2px groove gray;\
                border-style: outset;}"
                "QPushButton:pressed{background-color:rgb(85, 170, 255);\
                       border-style: inset; }";
const QString css_maincolorconf_u3="QLabel{background-color: #D3D3D3;\
               color: black;  border: 2px groove gray;\
               border-style: inset;\
               }";
const QString css_maincolorconf_u4="QPushButton{background-color:blue;\
              color: white;   border-radius: 10px;  border: 2px groove gray;\
              border-style: outset;}"
              "QPushButton:pressed{background-color:rgb(85, 170, 255);\
                     border-style: inset; }";
const QString css_maincolorconf_u5= "QPushButton{background-color: #d2d4fe;\
                 color: black; border: 2px groove gray;\
                 border-style: outset;}"
                 "QPushButton:pressed{background-color:rgb(85, 170, 255);\
                        border-style: inset; }";


const QString css_maincolorconf_u6="QPushButton{background-color:blue;\
                        color: white;\
                        }"
                      "QPushButton:pressed{background-color:rgb(85, 170, 255);\
                                       border-style: inset; }";

const QString css_maincolorconf_u7="QLabel{background-color: #B0C4DE;color: red;}";

const QString css_maincolorconf_u8= "QPushButton{background-color:#A8A8A8;\
               border: 2px groove gray;\
               color: black;\
                border-style: outset;\
               }"
              "QPushButton:pressed{background-color:rgb(211, 211, 211);\
               border-style: inset; }";

const QString css_maincolorconf_u9="QLabel{background:qlineargradient("
                                   "spread:pad,x1:0,y1:0,x2:0,y2:1,"
                                  "stop:0 #D8BFD8,"
                                  "stop:0.5 #989898,"
                                  "stop:1 #D8BFD8);\
              color: black;     border: 2px groove gray;\
              border-style: outset;}";

const QString css_maincolorconf_u10="QPushButton{background:qlineargradient("
                                                 "spread:pad,x1:0,y1:0,x2:0,y2:1,"
                                                "stop:0 #D8BFD8,"
                                                "stop:0.5 #989898,"
                                                "stop:1 #D8BFD8);\
                              color: black;    border: 2px groove gray;\
                              border-style: outset;}"
                              "QPushButton:pressed{background-color:rgb(85, 170, 255);\
                                     border-style: inset; }";
const QString css_maincolorconf_u11="QLineEdit{background-color: #EEE9E9;\
                color: black;  border: 2px groove gray;\
                border-style: inset;\
                }";
const QString css_maincolorconf_u12="QLineEdit{background-color: white;\
                color: black;  border: 2px groove gray;\
                border-style: inset;\
                }";
const QString css_maincolorconf_u13= "QPushButton{background-color:#FF0404;\
                               border: 2px groove gray;\
                               color: wihte;\
                                border-style: outset;\
                               }"
                              "QPushButton:pressed{background-color:rgb(85, 170, 255);\
                               border-style: inset; }";
const QString css_maincolorconf_u14="QLabel{background-color:#D0D3D8;}";

const QString css_maincolorconf_u15="QLabel{background:qlineargradient("
                                    "spread:pad,x1:0,y1:0,x2:0,y2:1,"
                                   "stop:0 #EEE9E9,"
                                   "stop:0.5 #EEE9E9,"
                                   "stop:1 #EEE9E9);\
               color: black;     border: 2px groove gray;\
               border-style: inset;}";

//#19a7ff
const QString css_maincolorconf_cylinder="QPushButton{background-color:#4f78ff;\
                                color: black;   border-radius: 5px;  border: 2px groove gray;\
                                border-style: outset;}"
                                "QPushButton:pressed{background-color:rgb(85, 170, 255);\
                                       border-style: inset; }";



const QString css_maincolorconf_cylinder1="QWidget{background-color:#765951;\
                               color: white;   border-radius: 5px;  border: 2px groove gray;\
                               }";

const QString css_maincolorconf_cylinder2="QPushButton{background-color:#FF6666;\
                               color: black;   border-radius: 5px;  border: 2px groove gray;\
                               }";

//teach color
const QString css_teachcolorconf_u0="QPushButton{background-color:#3064F8;\
               color: white;\
               }"
               "QPushButton:pressed{background-color:rgb(85, 170, 255);\
                      }";

//teach color
const QString css_teachcolorconf_u1="QPushButton{background-color:red;\
                 color: white;}";



//para color
const QString css_paracolorconf_u0="QLabel{background-color:#cccccc;\
               color: black;\
               }";

const QString css_paracolorconf_u1="QLabel{background-color:#7a9ec2;\
               color: black;\
               }";

const QString css_paracolorconf_u2="QTableView{\
        selection-background-color:#44c767;\
        background-color:white;/*整个表格的背景色，这里为白色*/\
        border:1px solid #E0DDDC;/*边框为1像素，灰色*/\
        gridline-color:lightgray;/*这个是表格的格子线的颜色，为亮灰*/\
    }";
const QString css_paracolorconf_u3="QLabel{background-color:#00cccc;\
                       color: black;text-align:center;\
                       }";
const QString css_paracolorconf_u4="QPushButton{background-color:#EEE9E9;\
        border: 2px groove gray;\
        color: black;\
         border-style: outset;\
        }"
       "QPushButton:pressed{background-color:#EEE9E9;\
        border-style: inset; }";

void setMarginAndSpacing(QLayout* layout, int margins, int spacing)
{
    layout->setMargin(margins);
    layout->setSpacing(spacing);
}

}
