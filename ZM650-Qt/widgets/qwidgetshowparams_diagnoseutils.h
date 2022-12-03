/*
 * qwidgetshowparams_diagnoseutils.h
 *
 * 诊断界面-辅助工具
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QWIDGETSHOWPARAMS_DIAGNOSEUTILS_H
#define QWIDGETSHOWPARAMS_DIAGNOSEUTILS_H

#include "qwidgetshowparamsbase.h"
#include "data/adapters.h"
#include <QDir>
#include <QProgressDialog>
#include <QThread>
#include <QKeyEvent>

class ToolThread;
class DataAdapter_DiagnoseUtils;

class QWidgetShowParams_DiagnoseUtils : public QWidgetShowParamsBase
{
    Q_OBJECT
public:
    typedef enum
    {
        Item_Reboot,//重启系统
        Item_Host,//重启上位机
        Item_Ftp,
        //Item_UpHost,//上位机更新
        Item_UpLower,//下位机更新
        Item_Motion,//下位机更新
        Item_EncryptDog,//软件加密
        Item_DecryptDog,//软件解密
        Item_Screen,//截屏工具
        Item_Picture,//图片加载
        Item_Font,//字体加载
        Item_Closebtn,//刹车关闭
        Item_IoDisplay,//显示IO
//        Item_PATTER1,//花样样式1
        Item_PATTER2,//花样样式2
        Item_PASSWOD,//参数密码
        Item_MaxCount // 必须指定最大数目



    } ITEM_T;
    QDir* localDir;
    QFileInfoList localFileList; 
    ToolThread* workThread;
    QString filenameLocal;
public:
    QWidgetShowParams_DiagnoseUtils(QWidget* parent = nullptr);
    ~QWidgetShowParams_DiagnoseUtils();


protected:
    QWidget* createFieldWidget(int index)override;
    QString getParamName(int offset)const override;
    bool getDataAuthority(int offset) const override;
    ModbusQueue::element_t* createModbusElement(int offset)const override;
    void onSendModbusOK(ModbusQueue::element_t* element) override;
    void onSendModbusFail(ModbusQueue::element_t* element) override;

signals:
    void dismissProgressDialog();
    void continu();
protected slots:
    void onClickUp();
    void onClickLower();
    void onReHost();
    void onEncrypt();//打开加密框
    void onDecrypt();//打开解密框
    void onWorkerThreadOperationDone(bool isOk);
    void decryptOk();
    void decryptFail();
    void onOpenEncrypt();
    void onReadwrite();
    void onOpenCut();
    void onPassWord();
    void onIoDisplay();
    void onLoad();
    void onLoad_Font();
    void onClosebtn();
    void onPatter1();
    void onPatter2();
    void onUpMotion();


protected:
    DataAdapter_DiagnoseUtils* adapter;

    void keyPressEvent(QKeyEvent* evt)
    {//不再向上传递  解决显示触发其他按钮BUG
        if(evt->key() == Qt::Key_Enter || evt->key() == Qt::Key_Return)
            return;

        QWidget::keyPressEvent(evt);
    }
    bool Lode_function(QString font);
};

class DataAdapter_DiagnoseUtils : public Adapters::DataAdapterBase<int>
{
public:
    DataAdapter_DiagnoseUtils(): Adapters::DataAdapterBase<int>(nullptr) {}
    QString getParamName(int /*offset*/) const override
    {
        return QString();
    }
    int getParamWidgetType(int /*offset*/) const override
    {
        return Adapters::WidgetType_NULL;
    }
    int getDataType(int /*offset*/) const override
    {
        return Adapters::TYPE_UNKNOWN;
    }
    bool getDataAuthority(int offset) const override{}
    void setData(int /*offset*/, const QVariant& /*val*/) override {}
    QVariant getData(int /*offset*/) override
    {
        return QVariant();
    }
};
class ToolThread : public QThread
{

    Q_OBJECT
public:
    ToolThread(QObject* parent = nullptr);
    enum WorkStep
    {
        WS_UPGRADE_COPY_TO_REMOTE,
        WS_UPGRADE_START,
        WS_UPGRADE_CHECK_IF_FINISH,
        WS_UPGRADE_COPY_TO_REMOTE_MOTIONLIB,
        WS_UPGRADE_MOTIONLIB_START,
        WS_DONE
    };
    int workstep;
    char filenameRemote[256];
    QString* filenameLocal;
    bool isStopped;
    void setFile(QString *local, int type);
    void run() override;

signals:
    void operationDone(bool isOk);
    void updateFileProgess(int progress);
protected slots:
    void _onNotifyFileProgress(int progress);
};
#endif // QWIDGETSHOWPARAMS_DIAGNOSEUTILS_H
