/*
 * qdialogsettins.h
 *
 * 设置班次对话框
 *
 * Created on: 2020年 2月 18日 星期一 11:03:40 CST
 * Author: caojing
 */

#ifndef QDIALOGPARAMOFFEST_H
#define QDIALOGPARAMOFFEST_H

#include<QDialog>
#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QLabel>
#include<QLineEdit>
#include<QKeyEvent>
#include<QPushButton>
#include<QComboBox>
#include <QListView>
#include "qlineeditstandard.h"
#include "Teach.h"

class QDialogParamOffest : public QDialog
{
    Q_OBJECT
public:
    explicit QDialogParamOffest(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~QDialogParamOffest();
    int *value;
    QVector<QLabel*> labe_value;
    QVector<QWidget*> line_value;
    QList<QString> NAMES;
    TEACH_SEND* teachRead;

    void ReadData();
signals:


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

private:
    //static const QStringList NAMES;
};


#endif // QDIALOGPARAMOFFEST_H
