/*
 * qdialogfailconnect.h
 *
 * 通信失败界面框，用于更新系统程序
 *
 * Created on: 2020年 3月 17日 星期一 17:02:40 CST
 * Author: caojing
 */
#ifndef QDIALOGFAILCONNECT_H
#define QDIALOGFAILCONNECT_H
#include<QDialog>
#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QDesktopWidget>
#include<QProgressDialog>
#include<QKeyEvent>
#include<QPushButton>
#include <QProcess>
#include <QDir>
#include "qwidgetshowparams_diagnoseutils.h"

class QDialogFailConnect : public QDialog
{
    Q_OBJECT
public:
    explicit QDialogFailConnect(QWidget *parent = 0, Qt::WindowFlags f = 0);

signals:
    void dismissProgressDialog();
public slots:
    void accept();
    void reject();
    void close();
    void onLoad_Font();
    void onUpdataDown();
    void onUpdataUp();
    void onReboot();
    void onWorkerThreadOperationDone(bool);

private:
    QDir* localDir;
    QFileInfoList localFileList;
    ToolThread* workThread;
    QString filenameLocal;
    bool Lode_function(QString font);

protected:
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent* evt)
    {
        if(evt->key() == Qt::Key_Enter || evt->key() == Qt::Key_Return)
            return;

        QDialog::keyPressEvent(evt);
    }
};

#endif // QDIALOGFAILCONNECT_H
