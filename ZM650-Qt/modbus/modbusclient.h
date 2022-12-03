/*
 * modbusclient.h
 *
 * Modbus客户端实现
 *
 * Created on: 2019年 01月 21日 星期一 14:34:20 CST
 * Author: lixingcong
 */
#ifndef MODBUSCLIENT_H
#define MODBUSCLIENT_H

#include <QObject>
#include "myudpclient.h"
#include "modbustypedef.h"

class ModbusClient : public QObject
{
    Q_OBJECT
public:
    explicit ModbusClient(QObject* parent = nullptr);
    ~ModbusClient();

    void init(QString& host, quint32 timeout, quint32 retry_times);
    void stop();

    bool readData(INT8U id, INT16U start_addr, void* pReadBuff, INT16U len, INT32U mode);
    bool writeData(INT8U id, INT16U start_addr, void* pWriteBuff, INT16U len, INT32U mode);

    bool readDataFifo(INT8U id, INT32S FifoCodeRead, void* pReadBuff, INT32S ReadAddressOffset, INT32S len, INT32U mode);
    bool writeDataFifo(INT8U id, INT32S FifoCodeWrite, void* pWriteBuff, INT32S WriteAddressOffset, INT32S len,
                       INT32U mode);

    bool readFile(INT8U id, char* remoteFile, const QString& localFile);
    bool writeFile(INT8U id, char* remoteFile, const QString& localFile);

    bool getIsInitialized() const
    {
        return isInitialized;
    }

signals:
    void updateFifoProgress(int percent);
    void updateFileProgress(int percent);

public slots:

protected: // members
    bool isInitialized;
    ModParam modparam;
    MyUDPClient* myUDPClient; //网络

    INT8U g_ucRecvBuf[MODBUS_ASCII_FRAME_SIZE];   //接收缓冲区
    INT8U g_ucSendBuf[MODBUS_ASCII_FRAME_SIZE];   //发送缓冲区
    INT8U g_ucFrameBufTmp[MODBUS_ASCII_FRAME_SIZE];   //发送临时缓冲区

protected: // functions
    bool funRead01H02H03H04H(INT8U cmd_mode, ModParam* pData, INT8U* pReadBuff, INT8U* size);
    bool funWrite0FH10H(INT8U cmd_mode, ModParam* pData, INT8U* pWriteBuff, INT8U size);
    bool funRead26H(INT8U cmd_mode, ModParam* pData, INT32S FifoCodeRead, INT8U* pReadBuff, INT32S ReadAddressOffset,
                    INT32U len, INT32U* size);
    bool funWrite27H(INT8U cmd_mode, ModParam* pData, INT32S FifoCodeWrite, INT8U* pWriteBuff, INT32S WriteAddressOffset,
                     INT32U len, INT32U* size);
    bool funLoadFile(INT8U cmd_mode, ModParam* pData, INT8U* pFileBuffer, INT32U iFrameNum, INT16U size);
    bool funStoreFile(INT8U cmd_mode, ModParam* pData, INT32U iFrameNum);
    bool sendData(ModParam* pData, INT8U cmd_mode);
    bool verifySend(ModParam* pData);
    bool verifyRecv(ModParam* pData);
    //unsigned short ModbusRtuCRC(unsigned char* data, unsigned char len); // 串口通讯需要该函数
    //bool isCRCGood(unsigned char* data, unsigned char len); // 串口通讯需要该函数

    // TODO: 这两个函数需要删掉，改用标准的Modbus封装函数
    void GetIntFromDataBuff(INT32S *piValue, INT8U* pbyDataBuff, INT8U byBytes, INT16U* pnOffset);
    void SetIntToDataBuff(INT32S iValue, INT8U* pbyDataBuff, INT8U byBytes, INT16U* pnOffset);

#define MOBBUSCLIENT_ENABLE_STATISTIC // 开启统计

#ifdef MOBBUSCLIENT_ENABLE_STATISTIC
    double getPacketFailRate()const
    {
        if(0 == counterPacketSent)
            return 0.0;

        return static_cast<double>(counterPacketSentFail) / counterPacketSent;
    }
    quint32 counterPacketSent;
    quint32 counterPacketSentFail;
#endif

};

#endif // MODBUSCLIENT_H
