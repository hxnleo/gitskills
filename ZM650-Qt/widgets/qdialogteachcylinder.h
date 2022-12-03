/*
 * qdialogteachcylinder.h
 *
 * 教导界面的气缸编辑弹出对话框
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef DIALOGPOPUP_H
#define DIALOGPOPUP_H

#include <QDialog>
#include <QBitArray>
#include <QList>

class QPushButtonChangeOnClick;

class QDialogTeachCylinder : public QDialog
{
    Q_OBJECT

public:
    explicit QDialogTeachCylinder(int cylinderCount, QWidget* parent = 0, Qt::WindowFlags f = 0, bool type = 0);
    ~QDialogTeachCylinder();

    QBitArray getBitArray() const;
    void setBitArray(const QBitArray& bitArray);

signals:
    void writeCylinder(int cylinderIndex, int status);

protected slots:
    void _onButtonClicked(int ioStatus);

protected:
    int cylinderCount;
    QList<QPushButtonChangeOnClick*> listButtons;
    QList<QString> NAMES;
    QList<QString> MAINCYLINDER;
};


#endif // DIALOGPOPUP_H
