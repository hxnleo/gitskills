/*
 * qdialogmainspeed.h
 *
 * 设置班次对话框
 *
 * Created on: 2020年 2月 20日 星期一 15:25:40 CST
 * Author: caojing
 */
#ifndef QDIALOGSPEED_H
#define QDIALOGSPEED_H

#include<QDialog>
#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QLabel>
#include<QLineEdit>
#include<QKeyEvent>
#include<QPushButton>
#include "qlineeditstandard.h"

class QDialogSpeed : public QDialog
{
    Q_OBJECT
public:
    explicit QDialogSpeed(QWidget *parent = 0,QString str = 0, Qt::WindowFlags f = 0);

    QLineEditStandard *line;
signals:
    void changeSpeed(int);

public slots:
    void accept();
    void reject();

protected:
    void keyPressEvent(QKeyEvent* evt)
    {
        if(evt->key() == Qt::Key_Enter || evt->key() == Qt::Key_Return)
            return;

        QDialog::keyPressEvent(evt);
    }
};
#endif // QDIALOGSPEED_H
