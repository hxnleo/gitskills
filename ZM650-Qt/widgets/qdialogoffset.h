/*
 * qdialogrenew.h
 *
 * 重命名对话框
 *
 * Created on: 2019年 10月 22日 星期一 17:02:40 CST
 * Author: caojing
 */
#ifndef QDIALOGOFFSET_H
#define QDIALOGOFFSET_H
#include<QDialog>
#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QLabel>
#include<QKeyEvent>
#include<QLineEdit>
#include<QPushButton>

class QDialogOffset : public QDialog
{
    Q_OBJECT
public:
    explicit QDialogOffset(int first,int last,int type = 0,QWidget *parent = 0,Qt::WindowFlags f = 0);
    int last;//最后一个轴
    QList<QLabel*> label0;
    int temp;//添加 item
    int type;
    //QList<QLabel*> label1;

    enum OType
    {//用来去区别上面还是下面
        One,
        Two,
        Three,
    };

signals:
    void moveAxis(int axisNum, int direction, int pressType, double distance);

public slots:
    void accept();
    void reject();
    void _onMoveButtonPress(int);
    void timerUpDate();
    void _onMoveColor1(int);
    void _onMoveColor2(int);
    void _onMoveColor3(int);

protected:
    void keyPressEvent(QKeyEvent* evt)
    {
        if(evt->key() == Qt::Key_Enter || evt->key() == Qt::Key_Return)
            return;

        QDialog::keyPressEvent(evt);
    }
};

#endif // QDIALOGOFFSET_H
