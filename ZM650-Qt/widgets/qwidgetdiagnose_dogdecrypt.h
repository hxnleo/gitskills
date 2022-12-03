/*
 * qwidgetdiagnose_dogdecrypt.h
 *
 * 加密狗解密窗口
 *
 * Created on: 2019年 11月 4日 星期一 11:30:40 CST
 * Author: Caojing
 */
#ifndef QWIDGETDIAGNOSE_DOGDECRYPT_H
#define QWIDGETDIAGNOSE_DOGDECRYPT_H
#include <QDialog>
#include <QThread>
#include <QKeyEvent>
#include "ui_dogdecryptform.h"
#include "data/globaldata.h"
class QLabel;
class DogDecryptThread;
class QWidgetDiagnose_DogDecrypt : public QDialog
{
    Q_OBJECT
public:
    explicit QWidgetDiagnose_DogDecrypt(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~QWidgetDiagnose_DogDecrypt();
    DogDecryptThread* workTread;
protected:
    void updateData();
signals:
    void success();
    void fail();

protected slots:
    void upDataTime();
    void onOk();

protected:
    void keyPressEvent(QKeyEvent* evt)
    {//不再向上传递  解决显示触发其他按钮BUG
        if(evt->key() == Qt::Key_Enter || evt->key() == Qt::Key_Return)
            return;

        QDialog::keyPressEvent(evt);
    }

private:
    Ui::DogDecryptForm from;
};

//线程
class DogDecryptThread : public QThread
{
    Q_OBJECT
public:
    DogDecryptThread(QObject* parent = nullptr);
    void run() override;
    bool isStopped;
    bool out;
    QTimer* timer;
signals:
    void sendOk();
protected slots:
    void onTimeout();

};

#endif // QWIDGETDIAGNOSE_DOGDECRYPT_H
