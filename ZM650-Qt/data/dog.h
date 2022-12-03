/*
 * dog.h
 *
 * 诊断界面的加密狗数据
 *
 * Created on: 2019年 11月 4日 星期一 11:30:40 CST
 * Author: Caojing
 */
#ifndef DOG_H
#define DOG_H

#include <QObject>
#include <QMessageBox>
#include "modbus/modbusaddress.h"
#include "modbus/modbusqueue.h"
#include <../ZM650/src/softdog/Dog_public.h>

class QDog : public QObject
{
    Q_OBJECT
protected:
    QDog();
    static QDog* instance;
public:
    static QDog* getInstance();
    ~QDog();
    DOG m_dog;
    SYSTIME dog_time;
    bool init(); // 初始化狗DOG结构体，即重新生成随机数
    bool writeDog();
    bool readDog();
    bool writeDogProgress(int num);

    bool validAdminPassword(int pwd);
    short getRemainDays();
    short getDogLevel();

    // 厂商序列号，机器序列号
    int getSerialNumber();
    int getFactoryNumber();

    // 狗1专用 begin
    bool unlockDogProgress(); // 让下位机解锁DogProgress
    bool setUserLevel(unsigned short newLevel);
    bool validUserPassword(int pwd, unsigned short* newLevel);
    // 狗1专用 end

    // 狗2专用 begin
    bool writeUserPassword(int pwd);
    // 狗2专用 end

    int getDogProcessCode(); // 当前加密Progress状态

    // 是否锁住用户
    bool isUnlocked();
    void setUnlock(bool isUnlock);

    // 狗1，狗2类型
    void setDogType(int dog_type);
    int getDogType();

    void copyToDog(DOG* destDog);
    void copyFromDog(const DOG* srcDog);
    bool readWriteModbus(void* data, unsigned short len, unsigned short addr, bool isRead);
protected:

    bool isReadOk; // 是否已经从下位机读取Dog结构体
    unsigned short mb_addr_dogProgress; // modbus address
    unsigned short m_dogProcess; // 加密狗限制标志


};

#endif // DOG_H
