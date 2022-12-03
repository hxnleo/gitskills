/*
 * qdialogsettins.h
 *
 * 设置班次对话框
 *
 * Created on: 2020年 2月 18日 星期一 11:03:40 CST
 * Author: caojing
 */

#ifndef QDIALOGSETTINS_H
#define QDIALOGSETTINS_H

#include<QDialog>
#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QLabel>
#include<QLineEdit>
#include<QKeyEvent>
#include<QPushButton>

#define CLASSPRODUCTADDR 33288
#define CURRPRODUCTADDR 33294
class QDialogSettins : public QDialog
{
    Q_OBJECT
public:
    explicit QDialogSettins(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~QDialogSettins();
    unsigned int *value;
    QVector<QLabel*> labe_value;
    QVector<QLineEdit*> line_value;
    QList<int> param;

signals:


public slots:
    void accept();
    void reject();
    void onReadData(bool ok = false);
    void sendOffest(void* data, int addr, int size, bool write = false);


protected:
    void keyPressEvent(QKeyEvent* evt)
    {
        if(evt->key() == Qt::Key_Enter || evt->key() == Qt::Key_Return)
            return;

        QDialog::keyPressEvent(evt);
    }

private:
    static const QStringList NAMES;
};


#endif // QDIALOGSETTINS_H
