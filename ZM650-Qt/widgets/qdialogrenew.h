/*
 * qdialogrenew.h
 *
 * 重命名对话框
 *
 * Created on: 2019年 10月 22日 星期一 17:02:40 CST
 * Author: caojing
 */
#ifndef QDIALOGRENEW_H
#define QDIALOGRENEW_H
#include<QDialog>
#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QLabel>
#include<QKeyEvent>
#include<QLineEdit>
#include<QPushButton>


class QDialogReNew : public QDialog
{
    Q_OBJECT
public:
    explicit QDialogReNew(QWidget *parent = 0, Qt::WindowFlags f = 0);
    QLineEdit *edit;

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

#endif // QDIALOGRENEW_H
