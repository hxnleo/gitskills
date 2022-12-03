/*
 * qdialogfindzero.h
 * 
 * 归零对话框
 * 
 * Created on: 2019年 02月 26日 星期二 11:46:59 CST
 * Author: lixingcong
 */

#ifndef QDIALOGFINDZERO_H
#define QDIALOGFINDZERO_H

#include <QDialog>

class QRadioButton;

class QDialogFindZero:public QDialog
{
    Q_OBJECT
public:
    explicit QDialogFindZero(QWidget* parent = nullptr, Qt::WindowFlags f = 0);

public slots:
    void accept();

signals:
    void zeroMachine();//工作台归零
    void zeroMainAxis();//主轴归零
    void startTable();//工作台回起
    void startTable1();//工作台回起第二工位

protected:
    QRadioButton* radioButton[2];
    static const QString radioButtonText[2];
};

#endif // QDIALOGFINDZERO_H
