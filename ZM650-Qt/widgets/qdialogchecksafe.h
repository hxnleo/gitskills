
/*
 * qdialogchecksafe.h
 *
 * 重命名对话框
 *
 * Created on: 2019年 10月 22日 星期一 17:02:40 CST
 * Author: caojing
 */
#ifndef QDIALOGCHECKSAFE_H
#define QDIALOGCHECKSAFE_H
#include<QDialog>
#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QLabel>
#include<QKeyEvent>
#include<QDebug>
#include<QMessageBox>
#include<QPushButton>
class QDialogCheckSafe : public QDialog
{
    enum BTN
    {
        BTN_OK,
        BTN_NO
    };
    Q_OBJECT
public:
    explicit QDialogCheckSafe(int i ,int type = 0,QWidget *parent = 0, Qt::WindowFlags f = 0);
    int mode;// 模式
    int type;// 平台模式


signals:
    void btn_ok(int);
    void btn_no(int);

public slots:
    void accept();
    void reject();


protected:
    void deelFixture();
    void keyPressEvent(QKeyEvent* evt)
    {
        if(evt->key() == Qt::Key_Enter || evt->key() == Qt::Key_Return)
            return;

        QDialog::keyPressEvent(evt);
    }
};

#endif // QDIALOGCHECKSAFE_H
