/*
 * qdialogpassword.cpp
 * 登录加密狗界面
 *
 * Created on: 2019年 10月 22日 星期一 17:02:40 CST
 * Author: caojing
 */
#ifndef QDIALOGPASSWORD_H
#define QDIALOGPASSWORD_H
#include<QLineEdit>
#include<QDialog>
#include<QKeyEvent>
#define INPUT_MAX 99999999
class QDialogPassword : public QDialog
{
    Q_OBJECT
public:
    explicit QDialogPassword(QWidget *parent = 0, Qt::WindowFlags f = 0,int minVal = 0
            ,int maxVal = INPUT_MAX);
    QLineEdit *password;
    int pwd;
    int minVal;
    int maxVal;

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

#endif // QDIALOGPASSWORD_H
