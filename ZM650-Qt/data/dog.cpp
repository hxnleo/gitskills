/*
 * dog.h
 *
 * 诊断界面的加密狗数据
 *
 * Created on: 2019年 11月 4日 星期一 11:30:40 CST
 * Author: Caojing
 */
#include "dog.h"
#include "QDebug"
#include "data/utils.h"
#include "data/globaldata.h"
#define UNLOCKED_CODE 12345678
QDog* QDog::instance = nullptr;

QDog::QDog(): QObject(nullptr)
{
    memset(&m_dog,0,sizeof(m_dog));//初始化狗
    memset(&dog_time,0,sizeof(dog_time));//初始化时间
}
QDog* QDog::getInstance()
{
    if(nullptr == instance)
        instance = new QDog;

    return instance;
}
QDog::~QDog()
{
    instance = nullptr;
}

bool QDog::readWriteModbus(void* data, unsigned short len, unsigned short addr, bool isRead)
{
    ModbusQueue::element_t element;
    element.data = data;
    element.len = len;
    element.operation = isRead?ModbusQueue::OP_READ_DATA:ModbusQueue::OP_WRITE_DATA;
    element.startAddress = addr;
    return GlobalData::modbusQueue->sendQuery(&element, false);
}


bool QDog::init()
{
    if(false==isReadOk)
        return false;

    bool isOk;
    unsigned short dummmy_code=1;

    isOk= (readWriteModbus(&dummmy_code,sizeof(dummmy_code),ModbusAddress::Dog_Init,false) && // 写入让其初始化
           readDog()); // 重新从下位机读取DOG结构体

    return isOk;
}

bool QDog::readDog()
{//加密狗读取
    isReadOk =readWriteModbus(&m_dog,sizeof(m_dog),ModbusAddress::Addr_Dog,true);
    qDebug()<<"read dog is"<<isReadOk;
    //狗标志
    if(true == isReadOk){
        mb_addr_dogProgress = (m_dog.Dog_Type == 2 ? ModbusAddress::Dog2_Progress : ModbusAddress::Dog_Progress);
        isReadOk =readWriteModbus(&m_dogProcess,sizeof(m_dogProcess),mb_addr_dogProgress,true);
    }
    return isReadOk;
}

bool QDog::writeDog()
{
    if(false==isReadOk)
        return false;

    return readWriteModbus(&m_dog,sizeof(m_dog),ModbusAddress::Addr_Dog,false);
}

bool QDog::writeDogProgress(int num)
{
    m_dogProcess = num;
    return readWriteModbus(&num,sizeof(num),ModbusAddress::Dog_Progress,false);
}

bool QDog::validAdminPassword(int pwd)
{
    if(false==isReadOk)
        return false;

    int pwd_serial, pwd_isetl, pwd_iseth;

    pwd_serial=pwd; // 根据输入密码计算出的值

    pwd_isetl=(pwd_serial&0x3ff) << 10;
    pwd_iseth=(pwd_serial>>10) & 0x3ff;

    pwd_serial>>=20;
    pwd_serial<<=20;

    pwd_serial = (pwd_serial | pwd_iseth | pwd_isetl);
    pwd_serial -= 130509;

    // debug_printf("validAdminPassword=%d\n",pwd_serial);

    // 正确的Admin密码有三个
    return (pwd==m_dog.Machine_Password ||  // 1、设定值
            pwd==SuperDogPassW ||  // 2、后门密码
            pwd_serial==m_dog.Machine_SerialNo); // 3、根据输入密码计算出的值与机器序列号对比
}
// 狗1专用 begin
bool QDog::unlockDogProgress()
{
    unsigned short dummmy_code=0;
    return readWriteModbus(&dummmy_code,sizeof(dummmy_code),mb_addr_dogProgress,false);
}

bool QDog::setUserLevel(unsigned short newLevel)
{
    if(false==isReadOk)
        return false;

    if(newLevel>24){
        setUnlock(true);
        return true;
    }
    // 累加天数
    unsigned short freedomDays=0; // 新的使用天数
    unsigned short level=m_dog.User_Level;

    while(level<newLevel){
        freedomDays+=m_dog.User_Days[level-1];
        level++;
    }

    m_dog.User_Level=newLevel;

    if(m_dog.User_RemainDay < 1) // 逾期验证，有效期从当前天开始计算
        m_dog.User_RemainDay=freedomDays;
    else // 提前验证，有效期延长累加
        m_dog.User_RemainDay+=freedomDays;

    return true;
}

bool QDog::validUserPassword(int pwd, unsigned short* unlockedLevel)
{
    if(false==isReadOk)
        return false;

    // 正确的User密码有两个：备用密码和设定值
    int superPwd=(267*m_dog.RandomNum)%1000000; // 计算备用密码

    if(pwd==superPwd){
        *unlockedLevel=m_dog.User_Level;
        return true;
    }

    if(m_dog.User_Level<1 || m_dog.User_Level>24)
        return false;

    for(unsigned short level=m_dog.User_Level; level<=24; ++level){
        if(pwd==m_dog.User_Password[level-1]){
            *unlockedLevel=level;
            return true;
        }
    }

    return false;
}
// 狗1专用 end

// 狗2专用 begin
bool QDog::writeUserPassword(int pwd)
{
    return readWriteModbus(&pwd,sizeof(int),ModbusAddress::Dog2_Pwd,false);
}
// 狗2专用 end

int QDog::getDogProcessCode()
{
    qDebug()<<"m_dogProcess"<<m_dogProcess;
    return m_dogProcess;
}


short QDog::getDogLevel()
{
    //qDebug()<<m_dog.User_Level;
    return m_dog.User_Level;
}

short QDog::getRemainDays()
{
    qDebug()<<"day"<<m_dog.User_RemainDay;
    return m_dog.User_RemainDay;
}

void QDog::setUnlock(bool isUnlock)
{
    m_dog.User_Active=isUnlock?UNLOCKED_CODE:0;
}

bool QDog::isUnlocked()
{
    if(m_dog.User_Active==UNLOCKED_CODE)
        return true;
    else
        return false;

}

void QDog::copyToDog(DOG* destDog)
{
    memcpy(destDog,&m_dog,sizeof(m_dog));
}

void QDog::copyFromDog(const DOG* srcDog)
{
    memcpy(&m_dog,srcDog,sizeof(m_dog));
}

int QDog::getSerialNumber()
{
    return m_dog.Machine_SerialNo;
}

int QDog::getFactoryNumber()
{
    return m_dog.Factory_SerialNo;
}

void QDog::setDogType(int dog_type)
{
    m_dog.Dog_Type=dog_type;
}

int QDog::getDogType()
{
    return m_dog.Dog_Type;
}

