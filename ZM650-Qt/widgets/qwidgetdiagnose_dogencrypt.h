/*
 * qwidgetdiagnose_dogencrypt.h
 *
 * 加密狗加密窗口
 *
 * Created on: 2019年 11月 4日 星期一 11:30:40 CST
 * Author: Caojing
 */
#ifndef QWIDGETDIAGNOSE_DOGENCRYPT_H
#define QWIDGETDIAGNOSE_DOGENCRYPT_H
#include <QDialog>
#include "ui_dogencryptform.h"
#include "data/globaldata.h"
#include <QThread>
#include <QKeyEvent>
#define PRE  0
#define NEXT 1
class QLabel;
class DogEncryptThreadThread;
class DogEncryptThread;
class QWidgetDiagnose_DogEncrypt : public QDialog
{
    Q_OBJECT
public:
    explicit QWidgetDiagnose_DogEncrypt(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~QWidgetDiagnose_DogEncrypt();

    DogEncryptThread* workTread;
    bool writeOk;
protected:
    void refreshData(bool isNeedRefresh);
    void updateData();
    void display();
    DOG dog;
signals:
    void success();
    void fail();

protected slots:
    void upDataTime();
    void onSateChanged(int index);
    void onInit(bool);
    void onAppliy();
    void onOk();
    void onCombox(int index);
    void onTimeout();
    void onPre();
    void onNext();


protected:
    void keyPressEvent(QKeyEvent* evt)
    {//不再向上传递  解决显示触发其他按钮BUG
        if(evt->key() == Qt::Key_Enter || evt->key() == Qt::Key_Return)
            return;

        QDialog::keyPressEvent(evt);
    }

private:
    Ui::DogEncryptForm from;
};


//线程
class DogEncryptThread : public QThread
{
    Q_OBJECT
public:
    DogEncryptThread(QObject* parent = nullptr);
    void run() override;
    bool isStopped;
    bool out;
signals:
    void sendOk();
protected slots:
    void onTimeout();

};

#endif // QWIDGETDIAGNOSE_DOGENCRYPT_H
