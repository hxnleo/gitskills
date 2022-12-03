
/*
 * qdialogmanage.h
 * 修改参数密码
 *
 * Created on: 2019年 10月 22日 星期一 17:02:40 CST
 * Author: caojing
 */
#ifndef QDIALOGMANAGE_H
#define QDIALOGMANAGE_H
#include<QLineEdit>
#include<QDialog>
#include<QKeyEvent>
#define PASSWORD 654321

class QDialogManage : public QDialog
{
    Q_OBJECT
public:
    explicit QDialogManage(QWidget *parent = 0, Qt::WindowFlags f = 0);
    QLineEdit *password;

signals:
    void decrypt();//解密成功

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

#endif // QDIALOGMANAGE_H
