/*
 * qdialogloadingworkfile.h
 *
 * 加载加工文件进度条弹出对话框
 *
 * Created on: 2019年 01月 21日 星期一 14:57:40 CST
 * Author: lixingcong
 */
#ifndef QDIALOGLOADINGWORKFILE_H
#define QDIALOGLOADINGWORKFILE_H

#include <QProgressDialog>
#include <QThread>

class QThreadLoadingWorkfile;

class QDialogLoadingWorkfile : public QProgressDialog
{
    Q_OBJECT
public:
    explicit QDialogLoadingWorkfile(QWidget* parent = nullptr);
    ~QDialogLoadingWorkfile();

    void readWrite(bool isRead, int startIndex, int count, bool isHide=false); // 读模式下，传入startIndex为负数则全部数据

protected:
    QThreadLoadingWorkfile* workerThread;
    static const QStringList hintText;

protected slots:
    void onQueryFail();
};

class QThreadLoadingWorkfile: public QThread
{
    Q_OBJECT
public:
    QThreadLoadingWorkfile(QObject* parent = nullptr);
    void run() override;
    void setMode(int startIndex, int count, bool isRead)
    {
        this->startIndex = startIndex;
        this->count = count;
        this->isRead = isRead;
    }

protected:
    int startIndex, count;
    bool isRead;
};

#endif // QDIALOGLOADINGWORKFILE_H
