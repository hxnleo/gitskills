/*
 * qdialogcontrol.h
 *
 * 控制对话框
 *
 * Created on: 2020年 2月 17日 星期一 17:21:40 CST
 * Author: caojing
 */

#ifndef QDIALOGCONTROL_H
#define QDIALOGCONTROL_H

#include<QDialog>
#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QLabel>
#include<QKeyEvent>
#include<QPushButton>

class QDialogControl : public QDialog
{
    Q_OBJECT
public:
    explicit QDialogControl(QWidget *parent = 0, Qt::WindowFlags f = 0);

    int locatePointIndex;

signals:
    void onWorkBench();
    void onbackZero();
    void onfristHole();
    void onPosition(int);

public slots:
    void accept();
    void reject();
    void OnWorkBench();
    void OnbackZero();
    void OnfristHole();
    void OnPosition();
    void OnSendPostion();

protected:
    void keyPressEvent(QKeyEvent* evt)
    {
        if(evt->key() == Qt::Key_Enter || evt->key() == Qt::Key_Return)
            return;

        QDialog::keyPressEvent(evt);
    }
};


#endif // QDIALOGCONTROL_H
