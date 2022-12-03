/*
 * qdialogdataform.h
 *
 * 填表对话框，支持多种数据类型(数字和字符串)
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QDialogDataForm_H
#define QDialogDataForm_H

#include <QDialog>
#include <QList>
#include <QVariant>
#include <QFormLayout>
#include <QPushButton>
#include "widgets/qlabelmarquee.h"
#include "widgets/qmycombobox.h"
#include "data/adapters.h"


class QDialogDataForm : public QDialog
{
    Q_OBJECT
public:
    struct item_t
    {
        QString name;
        Adapters::DataType type;
        void* data;
        bool isReadOnly;
        double minVal;
        double maxVal;

        // 可选
        QStringList* stringList;
    };

public:
    explicit QDialogDataForm(QWidget* parent = nullptr, Qt::WindowFlags f = 0);
    void showDialog(const QString& title, const QList<item_t>& items, bool marqueeEnable = false, QString stayle = "");

protected:
    QList<item_t> items;
    QList<QWidget*> itemWidgets;
    bool isInitialized;
    QFormLayout* formLayout;
    QPushButton* btn_ok;
    QPushButton* btn_cancel;
    QLabelMarquee* labelTitle;

signals:

public slots:
    void accept() override;

protected: // functions
    void keyPressEvent(QKeyEvent* evt);
};

#endif // QDialogDataForm_H
