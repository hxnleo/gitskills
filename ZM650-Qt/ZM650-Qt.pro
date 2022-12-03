#-------------------------------------------------
#
# Project created by QtCreator 2018-07-02T20:05:10
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#TARGET = ZM650-Qt
TARGET = adtrom
TEMPLATE = app

DEFINES+=ARM
DEFINES+=ZUANZHI
DEFINES+=Arm_lansi
#DEFINES+=Debug          #
#DEFINES+=Arm_GBK        #UTF8->GBK

contains(DEFINES,_MCGS){
    #include(co

    message($$QMAKESPEC)
    QMAKE_CFLAGS += -ftest-coverage -fprofile-arcs
    QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs

    LIBS +=-L/usr/local/MCGS/sysroot-mcgs/usr/lib -lmcgs-hal -lsqlite3
    LIBS += -lgcov
    QMAKE_CXXFLAGS += --coverage
}else{
message("no in msgc")
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# Enable C++11 standard
lessThan(QT_MAJOR_VERSION, 5): QMAKE_CXXFLAGS += -std=c++11

#tanslation config
TRANSLATIONS = ZM650_tr_en.ts
CODECFORTR=utf-8

# use PC screen hint, lixingcong@20190116
CONFIG(debug, debug|release) {
    message("Config: Debug with macros: DEBUG_USE_PCSCREEN")
    DEFINES+="DEBUG_USE_PCSCREEN"
}else{
    message("Config: Release")
}

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Include paths
INCLUDEPATH += ../ZM650-Public/
INCLUDEPATH += ../ZM650-Public/public.h

# To identify the source was built by qmake, not by Xilinx SDK.
DEFINES += BUILT_BY_QT4

SOURCES += \
        main.cpp \
    widgets/myinputpanel.cpp \
    widgets/myinputpanelcontext.cpp \
    widgets/qdialogmanage.cpp \
    widgets/qdialogoffset.cpp \
    widgets/qpushbuttonlongpress.cpp \
    data/utils.cpp \
    modbus/myudpclient.cpp \
    modbus/modbusclient.cpp \
    modbus/modbusqueue.cpp \
    widgets/qpushbuttonspage.cpp \
    widgets/qlabelmove.cpp \
    data/globaldata.cpp \
    widgets/qpushbuttonstab.cpp \
    widgets/qstyleditemdelegateteachcylinder.cpp \
    widgets/qdialogteachcylinder.cpp \
    widgets/qtablewidgetteach.cpp \
    widgets/qitemmodelteach.cpp \
    widgets/qdialogdataform.cpp \
    widgets/qstyleditemdelegateteachcombobox.cpp \
    widgets/qstyleditemdelegateteachbase.cpp \
    widgets/qstyleditemdelegateteachaxis.cpp \
    widgets/qpushbuttonchangeonclick.cpp \
    widgets/qlabelchangeonclick.cpp \
    widgets/qwidgetteachstatus.cpp \
    widgets/qlabelmarquee.cpp \
    widgets/qcomboboxforparams.cpp \
    widgets/qdialogparams_home.cpp \
    widgets/qdialogparams_limit.cpp \
    widgets/qdialogparams_originorder.cpp \
    widgets/qdialogparams_password.cpp \
    widgets/qdialogparams_writeanything.cpp \
    data/adapters.cpp \
    ../ZM650-Public/Teach.c \
    ../ZM650-Public/public.c \
    ../ZM650-Public/ParaManage.c \
    ../ZM650-Public/Parameter.c \
    widgets/qdialogparams_axisswitch.cpp \
    widgets/qwidgetshowparamsbase.cpp \
    widgets/qwidgetshowparams_paramtable.cpp \
    widgets/qwidgetshowparams_ioconfig.cpp \
    widgets/qdialogparams_ioconfig.cpp \
    widgets/qwidgetshowparams_iostatus.cpp \
    widgets/qlabelsystemalarm.cpp \
    widgets/qlineeditnumber.cpp \
    widgets/qwidgetshowparams_diagnoseutils.cpp \
    widgets/qwidgetdiagnoseversion.cpp \
    data/workfile.cpp \
    widgets/qdialogloadingworkfile.cpp \
    data/utilsforcontroller.cpp \
    data/uistyles.cpp \
    widgets/qwidgetdrawtrail.cpp \
    widgets/qstyleditemdelegateteachspeed.cpp \
    data/patterns.cpp \
    widgets/qdialogparams_patterns.cpp \
    widgets/qwidgetshowparams_patterns.cpp \
    widgets/qlistwidgetdragscroll.cpp \
    widgets/qdialogfindzero.cpp \
    widgets/qdialogclampout.cpp \
    widgets/qwidgetaxisposition.cpp \
    widgets/qmypushbutton_u0.cpp \
    ../ZM650-Public/SysTextUtf8.cpp \
    ../ZM650-Public/SysAlarmUtf8.cpp \
    ../ZM650-Public/TestIOUtf8.cpp \
    widgets/qmycombobox.cpp \
    widgets/qdialogrenew.cpp \
    widgets/qwidgetshowparams_daconfig.cpp \
    widgets/qwidgetdiagnosedog.cpp \
    data/dog.cpp \
    widgets/qwidgetdiagnose_dogencrypt.cpp \
    widgets/qwidgetdiagnose_dogdecrypt.cpp \
    widgets/qtablewidgetdog.cpp \
    widgets/qitemmodeldog.cpp \
    widgets/qdialogpassword.cpp \
    widgets/qdialogmature.cpp \
    widgets/tabbutton.cpp \
    widgets/icobutton.cpp \
    widgets/qdialogfailconnect.cpp \
    widgets/qdialogchecksafe.cpp \
    widgets/qstyleditemdelegateteachpushbutton.cpp \
    widgets/qdialogcontrol.cpp \
    widgets/qdialogparamoffest.cpp \
    widgets/qdialogsettins.cpp \
    widgets/qdialogspeed.cpp \
    widgets/qdialogteachcontrol.cpp \
    widgets/qwidgetshowparams_zm662daconfig.cpp \
    widgets/textbutton.cpp \
    widgets/qlineeditstandard.cpp \
    widgets/qdialogparams_authority.cpp \
    windows/mainwindow.cpp \
    windows/qwidgetshowparams_axisenable.cpp \
    windows/startupdialog.cpp \
    windows/subwindowdiagnose.cpp \
    windows/subwindowfilemanage.cpp \
    windows/subwindowlocalconfig.cpp \
    windows/subwindowparams.cpp \
    windows/subwindowpatterns.cpp \
    windows/subwindowrunstatus.cpp \
    windows/subwindowteachtable.cpp \
    windows/subwindowtrail.cpp

HEADERS += \
    widgets/myinputpanel.h \
    widgets/myinputpanelcontext.h \
    widgets/qdialogmanage.h \
    widgets/qdialogoffset.h \
    widgets/qpushbuttonlongpress.h \
    data/utils.h \
    modbus/myudpclient.h \
    modbus/modbusclient.h \
    modbus/modbusqueue.h \
    modbus/modbustypedef.h \
    widgets/qpushbuttonspage.h \
    widgets/qlabelmove.h \
    data/definitions.h \
    data/globaldata.h \
    widgets/qpushbuttonstab.h \
    widgets/qdialogteachcylinder.h \
    widgets/qstyleditemdelegateteachcylinder.h \
    widgets/qdialogdataform.h \
    widgets/qtablewidgetteach.h \
    widgets/qstyleditemdelegateteachcombobox.h \
    widgets/qstyleditemdelegateteachbase.h \
    widgets/qstyleditemdelegateteachaxis.h \
    widgets/qpushbuttonchangeonclick.h \
    widgets/qlabelchangeonclick.h \
    widgets/qitemmodelteach.h \
    widgets/qwidgetteachstatus.h \
    widgets/qlabelmarquee.h \
    widgets/_qpushbuttonparamsbase.h \
    widgets/qcomboboxforparams.h \
    widgets/qdialogparams_home.h \
    widgets/qdialogparams_limit.h \
    widgets/qdialogparams_originorder.h \
    widgets/qdialogparams_password.h \
    widgets/qdialogparams_writeanything.h \
    widgets/qpushbuttonparams.h \
    data/adapters.h \
    data/adt_datatype_qt.h \
    ../ZM650-Public/public.h \
    ../ZM650-Public/SysText.h \
    ../ZM650-Public/Teach.h \
    ../ZM650-Public/TestIO.h \
    ../ZM650-Public/SysAlarm.h \
    ../ZM650-Public/SysKey.h \
    ../ZM650-Public/Parameter.h \
    ../ZM650-Public/ParaManage.h \
    ../ZM650-Public/ManualMotion.h \
    widgets/qdialogparams_axisswitch.h \
    widgets/qwidgetshowparamsbase.h \
    widgets/qwidgetshowparams_paramtable.h \
    widgets/qwidgetshowparams_ioconfig.h \
    widgets/qdialogparams_ioconfig.h \
    widgets/qwidgetshowparams_iostatus.h \
    widgets/qlabelsystemalarm.h \
    widgets/qlineeditnumber.h \
    modbus/modbusaddress.h \
    widgets/qwidgetshowparams_diagnoseutils.h \
    widgets/qwidgetdiagnoseversion.h \
    data/workfile.h \
    widgets/qdialogloadingworkfile.h \
    ../ZM650-Public/SysKey.h \
    data/utilsforcontroller.h \
    data/uistyles.h \
    widgets/qwidgetdrawtrail.h \
    widgets/qstyleditemdelegateteachspeed.h \
    data/patterns.h \
    widgets/qdialogparams_patterns.h \
    widgets/qwidgetshowparams_patterns.h \
    widgets/qwidgetfieldlayout.h \
    widgets/qlistwidgetdragscroll.h \
    widgets/qlabelclick.h \
    widgets/qdialogfindzero.h \
    widgets/qdialogclampout.h \
    widgets/qwidgetaxisposition.h \
    widgets/qmypushbutton_u0.h \
    widgets/qmycombobox.h\
    platform/common.h \
    widgets/qdialogrenew.h \
    widgets/qwidgetshowparams_daconfig.h \
    ../ZM650/src/softdog/Dog_public.h \
    widgets/qwidgetdiagnosedog.h \
    data/dog.h \
    widgets/qwidgetdiagnose_dogencrypt.h \
    widgets/qwidgetdiagnose_dogdecrypt.h \
    widgets/qtablewidgetdog.h \
    widgets/qitemmodeldog.h \
    widgets/qdialogpassword.h \
    widgets/qdialogmature.h \
    widgets/tabbutton.h \
    widgets/icobutton.h \
    widgets/qdialogfailconnect.h \
    widgets/qdialogchecksafe.h \
    widgets/qstyleditemdelegateteachpushbutton.h \
    widgets/qdialogcontrol.h \
    widgets/qdialogparamoffest.h \
    widgets/qdialogsettins.h \
    widgets/qdialogspeed.h \
    widgets/qdialogteachcontrol.h \
    widgets/qwidgetshowparams_zm662daconfig.h \
    widgets/textbutton.h \
    widgets/qlineeditstandard.h \
    widgets/qdialogparams_authority.h \
    windows/mainwindow.h \
    windows/qwidgetshowparams_axisenable.h \
    windows/startupdialog.h \
    windows/subwindowdiagnose.h \
    windows/subwindowfilemanage.h \
    windows/subwindowlocalconfig.h \
    windows/subwindowparams.h \
    windows/subwindowpatterns.h \
    windows/subwindowrunstatus.h \
    windows/subwindowteachtable.h \
    windows/subwindowtrail.h

FORMS += \
    widgets/myinputpanelform.ui \
    windows/mainwindow.ui \
    windows/subwindowrunstatus.ui \
    windows/subwindowteachtable.ui \
    windows/startupdialog.ui \
    windows/subwindowtrail.ui \
    windows/subwindowlocalconfig.ui \
    widgets/dogdecryptform.ui \
    widgets/dogencryptform.ui \
    widgets/qwidgetshowparams_zm662daconfig.ui

RESOURCES += \
    resources/res.qrc \
    resources/translate.qrc

#MCGS
contains(DEFINES,_MCGS){
    INSTALLS +=target
    target.path=/root
}else{
message("no in msgc")
}

QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-unused-variable

DISTFILES +=

