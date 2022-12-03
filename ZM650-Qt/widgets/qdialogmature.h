#ifndef QDIALOGMATURE_H
#define QDIALOGMATURE_H

#include<QDialog>
#include<QKeyEvent>
#include <data/dog.h>
#include "qlineeditstandard.h"

class QDialogMature : public QDialog
{
    Q_OBJECT
public:
    explicit QDialogMature(QWidget *parent = 0, Qt::WindowFlags f = 0);
    QLineEditStandard *edit_Password;

signals:
    void success();
    void fail();

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

#endif // QDIALOGMATURE_H
