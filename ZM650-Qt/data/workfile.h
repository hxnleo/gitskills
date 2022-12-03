/*
 * workfile.h
 *
 * 加工点文件的读取写入
 *
 * Created on: 2019年 01月 21日 星期一 14:31:00 CST
 * Author: lixingcong
 */
#ifndef WORKFILE_H
#define WORKFILE_H

#include <QObject>
#include <Teach.h>
#include <QMutex>

class Workfile : public QObject
{
    Q_OBJECT
protected:
    Workfile();

public:
    static Workfile* getInstance();
    ~Workfile();

    bool readAll();
    bool read(int startPointIndex, int count);
    bool write(int startPointIndex, int count);

    // 是否已完整读过所有孔
    void setIsAlreadyReadAll(bool isReadAll)
    {
        this->_isAlreadReadAll = isReadAll;
    }
    bool isAlreadyReadAll()const
    {
        return _isAlreadReadAll;
    }

    TEACH_DATA* getTeachData()
    {
        return &teachData[0];
    }

protected: // functions
    bool loadTotalPointNumber(int* totalPoint);
    bool fifoReadWrite(int startPointIndex, int count, bool isRead, bool isFullyRead = false);

signals:
    void loading(int percent); // 百分比
    void fifoReadWriteDone(); // 用于通知其它UI线程更新TeachData教导界面
    void fifoReadWriteFail(); // 用于通知其它UI线程更新TeachData教导界面

protected slots:
    void _onNotifyFifoProgress(int percent);

protected:
    static Workfile* instance;
    TEACH_DATA teachData[MAXTEACHNUM];
    QMutex mutexLoading;
    bool _isAlreadReadAll;
};

#endif // WORKFILE_H
