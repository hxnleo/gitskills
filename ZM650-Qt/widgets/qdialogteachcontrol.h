/*
 * qdialogteachcontrol.h
 *
 * 面板对话框
 *
 * Created on: 2020年 2月 18日 星期一 16:31:40 CST
 * Author: caojing
 */

#ifndef QDIALOGTEACHCONTROL_H
#define QDIALOGTEACHCONTROL_H

#include<QDialog>
#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QLabel>
#include<QKeyEvent>
#include<QPushButton>

class QDialogTeachControl : public QDialog
{
    Q_OBJECT
public:
    explicit QDialogTeachControl(QWidget *parent = 0, Qt::WindowFlags f = 0);

signals:
    void mainbackzero();
    void drill();
    void workbackzero();
    void mainbrake();
    void fixture();

public slots:
    void accept();
    void reject();
    void OnMainBackZero();
    void OnDrill();
    void OnWorkBackZero();
    void OnMainBrake();
    void OnParamOffset();
    void OnFixture();

protected:
    void keyPressEvent(QKeyEvent* evt)
    {
        if(evt->key() == Qt::Key_Enter || evt->key() == Qt::Key_Return)
            return;

        QDialog::keyPressEvent(evt);
    }
};


#endif // QDIALOGTEACHCONTROL_H
