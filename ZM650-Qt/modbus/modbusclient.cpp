/*
 * modbusclient.cpp
 *
 * Modbus客户端实现
 *
 * Created on: 2019年 01月 21日 星期一 14:34:20 CST
 * Author: lixingcong
 */
#include "modbusclient.h"
#include "data/utils.h"
#include <unistd.h>
#include <QThread>
#include <QFile>

#define FifoFrameSize	(MODBUS_RTU_DATA_SIZE-6)
#define FileFrameSize   (MODBUS_RTU_DATA_SIZE-6)

#define MB_FUNC_CODE_READ_FIFO 0x26
#define MB_FUNC_CODE_WRITE_FIFO 0x27
#define MB_FUNC_CODE_LOAD_FILE_FROM_REMOTE 0x24
#define MB_FUNC_CODE_STORE_FILE_TO_REMOTE 0x25

ModbusClient::ModbusClient(QObject* parent): QObject(parent),
    isInitialized(false),
    myUDPClient(nullptr)
{
#ifdef MOBBUSCLIENT_ENABLE_STATISTIC
    counterPacketSent = 0;
    counterPacketSentFail = 0;
#endif
}

ModbusClient::~ModbusClient()
{
    stop();
#ifdef MOBBUSCLIENT_ENABLE_STATISTIC
    debug_printf("Packet Fail Rate: %.2f %%", getPacketFailRate() * 100);
#endif
}

void ModbusClient::init(QString& host, quint32 timeout, quint32 retry_times)
{
    stop();

    myUDPClient = new MyUDPClient(QHostAddress(host), MODBUS_UDP_DEST_PORT, timeout);

    memset(&modparam, 0, sizeof(ModParam));
    modparam.cmd_type = MODBUS_INTERFACENET;
    modparam.cmd_station = MODBUS_STATION_POLL;
    modparam.repeat_times = retry_times;
    modparam.timeouts = timeout;
    modparam.id = MODBUS_UDP_ID;

    isInitialized = true;
}

void ModbusClient::stop()
{
    if(false == isInitialized)
        return;

    Utils::delete_pointer(myUDPClient);
    isInitialized = false;
}

bool ModbusClient::readData(INT8U id, INT16U start_addr, void* pReadBuff, INT16U len, INT32U mode)
{
    INT8U       cmd_mode, data_mode, data_type;
    INT8U       old_id, fun_code = 0;
    INT16U      reg_num, offset;
    INT8U       read_size;
    INT8U*      pTmpBuff = static_cast<INT8U*>(pReadBuff);;
    INT16U      count;
    bool        isOk;

    if(false == isInitialized)
        return false;

    if(nullptr == pReadBuff || 0 == len)
        return false;

    cmd_mode  = (mode >> 16) & 0xff;    //通讯方式选择: RTU, ASCII
    data_mode = (mode >> 8) & 0xff;     //数据方式: 可读写(RW), 只读(IN)
    data_type = mode & 0xff;            //数据类型: 字(WORD), 位(BIT)

    if(data_type == MODBUS_DATA_BIT)
    {
        offset = BIT_MAX_REGNUM;
        reg_num = len;  //线圈或离散量的数量--位数

        if(data_mode == MODBUS_MODE_IN)
        {
            fun_code = 0x02;            //读离散量输入
        }
        else if(data_mode == MODBUS_MODE_RW)
        {
            fun_code = 0x01;            //读线圈
        }
    }
    else if(data_type == MODBUS_DATA_WORD)
    {
        offset = WORD_MAX_REGNUM;
        reg_num = (len >> 1) + (len & 0x1); //将字节数转换为寄存器数

        if(data_mode == MODBUS_MODE_IN)
        {
            fun_code = 0x04;        //读输入寄存器
        }
        else if(data_mode == MODBUS_MODE_RW)
        {
            fun_code = 0x03;        //读保持寄存器
        }
    }

    if(fun_code == 0)
    {
        // debug_printf("CC 1\n");
        return false;
    }

    //todo: 等待信号量 以下对ModParam成员变量有写操作,需要进行互斥保护

    isOk = false;
    old_id = modparam.id;

    count = reg_num / offset + ((reg_num % offset) ? 1 : 0); //计算需要访问的次数

    while(count--)
    {
        modparam.id = id;
        modparam.fun_code = fun_code;

        if(count != 0)
        {
            modparam.start_addr = start_addr;

            start_addr += offset;
            modparam.reg_num = offset;
            reg_num -= offset;
        }
        else     //第一帧起始地址即为作为参数传入的地址
        {
            modparam.start_addr = start_addr;
            modparam.reg_num = reg_num;
        }

        isOk = funRead01H02H03H04H(cmd_mode, &modparam, pTmpBuff, &read_size);
        // debug_printf("funRead01H02H03H04H Ok? %d\n",isOk?1:0);
#if 0
        {
            INT8U i;

            for(i = 0; i < read_size; i++)
            {
                ps_debugout("%02X \r\n", pTmpBuff[i]);
            }
        }
#endif
        pTmpBuff += read_size;

        if(false == isOk)
            break;
    }

    modparam.id =
        old_id;       //还原先前设备ID，主要为不影响设备作为服务端时ID号的唯一性

    // todo: 释放信号量

    return isOk;
}

bool ModbusClient::writeData(INT8U id, INT16U start_addr, void* pWriteBuff, INT16U len, INT32U mode)
{
    bool        isOk;
    INT8U       old_id, fun_code = 0;
    INT8U       cmd_mode, data_mode, data_type;
    INT16U      reg_num, offset;
    INT8U       write_size;
    INT8U*      pTmpBuff = static_cast<INT8U*>(pWriteBuff);
    INT16U      count;

    if(nullptr == pWriteBuff || 0 == len)
        return false;

    cmd_mode  = (mode >> 16) & 0xff;    //通讯方式选择: RTU, ASCII
    data_mode = (mode >> 8) & 0xff;     //数据方式: 可读写(RW), 只读(IN)
    data_type = mode & 0xff;            //数据类型: 字(WORD), 位(BIT)

    if(data_mode != MODBUS_MODE_RW)
    {
        return false;//MODBUS_PFM_UNDEFINED;
    }

    if(data_type == MODBUS_DATA_BIT)
    {
        offset = BIT_MAX_REGNUM;
        reg_num = len;
        fun_code = 0x0f;            //写多个线圈
    }
    else if(data_type == MODBUS_DATA_WORD)
    {
        offset = WORD_MAX_REGNUM;
        reg_num = (len >> 1) + (len & 0x1);     //将字节数转换为寄存器数
        fun_code = 0x10;            //写多个寄存器
    }

    if(fun_code == 0)
    {
        return false;//MODBUS_INVALID_FUNC;
    }

    //todo: 等待信号量 以下对ModParam成员变量有写操作,需要进行互斥保护
    old_id = modparam.id;
    isOk = false;

    count = reg_num / offset + ((reg_num % offset) ? 1 : 0);

    while(count--)
    {
        modparam.id = id;
        modparam.fun_code = fun_code;

        if(count != 0)
        {
            modparam.start_addr = start_addr;
            start_addr += offset;
            modparam.reg_num = offset;
            reg_num -= offset;
        }
        else
        {
            modparam.start_addr = start_addr;
            modparam.reg_num = reg_num;
        }

        if(data_type == MODBUS_DATA_BIT)
            write_size = (modparam.reg_num >> 3) + ((modparam.reg_num & 0x7) ? 1 : 0); //获取字节数
        else
            write_size = modparam.reg_num << 1;

        isOk = funWrite0FH10H(cmd_mode, &modparam, pTmpBuff, write_size);
        pTmpBuff += write_size;

        if(false == isOk)
            break;
    }

    modparam.id =
        old_id;       //还原先前设备ID，主要为不影响设备作为服务端时ID号的唯一性

    // todo: 释放信号量

    return isOk;
}

bool ModbusClient::readDataFifo(INT8U id, INT32S FifoCodeRead, void* pReadBuff, INT32S ReadAddressOffset, INT32S len,
                                INT32U mode)
{
    bool        isOk = false;
    INT8U		old_id;
    INT8U		cmd_mode;
    INT32U      read_size;

    if(false == isInitialized)
        return false;

    if(nullptr == pReadBuff || 0 == len)
        return false;

    cmd_mode  = (mode >> 16) & 0xff;		//通讯方式选择: RTU, ASCII
    modparam.fun_code = MB_FUNC_CODE_READ_FIFO;//自定义读取缓存功能号
    old_id = modparam.id;
    modparam.id = id;

    isOk = funRead26H(cmd_mode, &modparam, FifoCodeRead, reinterpret_cast<INT8U*>(pReadBuff), ReadAddressOffset, len,
                      &read_size);

    modparam.id =
        old_id;       //还原先前设备ID，主要为不影响设备作为服务端时ID号的唯一性

    return isOk;
}

bool ModbusClient::writeDataFifo(INT8U id, INT32S FifoCodeWrite, void* pWriteBuff, INT32S WriteAddressOffset,
                                 INT32S len, INT32U mode)
{
    bool        isOk = false;
    INT8U		old_id;
    INT8U		cmd_mode;
    INT32U      read_size;

    if(false == isInitialized)
        return false;

    if(nullptr == pWriteBuff || 0 == len)
        return false;

    cmd_mode  = (mode >> 16) & 0xff;		//通讯方式选择: RTU, ASCII
    modparam.fun_code = MB_FUNC_CODE_WRITE_FIFO;//自定义读取缓存功能号
    old_id = modparam.id;
    modparam.id = id;

    isOk = funWrite27H(cmd_mode, &modparam, FifoCodeWrite, reinterpret_cast<INT8U*>(pWriteBuff), WriteAddressOffset, len,
                       &read_size);

    modparam.id =
        old_id;       //还原先前设备ID，主要为不影响设备作为服务端时ID号的唯一性

    return isOk;
}

bool ModbusClient::readFile(INT8U id, char* remoteFile, const QString& localFile)
{
    bool isOk = false;
    INT8U  pTmpBuff[FileFrameSize];
    INT16U count ;			//计算传输总帧数
    INT16U send_size;		//数据长度(不包含地址码,功能码,自身)4字节帧号+文件名
    INT16U iFrameSize;		//帧数据大小
    INT32U iFrameNum = 0;
    INT32U iTotalFrame; //帧号
    INT32U iTotalSize;		//服务器返回的文件大小,用于比较
    INT8U old_id;

    QFile file(localFile);

    if(false == file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false; // 不能创建文件

    auto pData = &modparam;

    old_id = pData->id; 	//保存自身ID,
    pData->id = id; 		//写入寻址的ID
    pData->fun_code = MB_FUNC_CODE_LOAD_FILE_FROM_REMOTE; //加载文件功能码

    //头帧数据:帧号0+文件名相关数据
    iFrameNum = 0;
    iFrameSize = 0;
    pData->frame_len = 0;
    pData->frame_buff[pData->frame_len++] = pData->id;
    pData->frame_buff[pData->frame_len++] = pData->fun_code;

    //发送的数据长度(帧号4+文件名长度)
    send_size = 4 + strlen(remoteFile);
    pData->frame_buff[pData->frame_len++] = (send_size >> 8) & 0xff;
    pData->frame_buff[pData->frame_len++] = send_size & 0xff;

    //发送的帧号0
    pData->frame_buff[pData->frame_len++] = (iFrameNum >> 24) & 0xff;
    pData->frame_buff[pData->frame_len++] = (iFrameNum >> 16) & 0xff;
    pData->frame_buff[pData->frame_len++] = (iFrameNum >> 8) & 0xff;
    pData->frame_buff[pData->frame_len++] = (iFrameNum >> 0) & 0xff;

    //保存文件名
    strcpy((char*)(pData->frame_buff + pData->frame_len), remoteFile);
    pData->frame_len += send_size - 4;

    isOk = funLoadFile(MODBUS_MODE_RTU, pData, pTmpBuff, iFrameNum, iFrameSize); // 首帧

    if(false == isOk)
        goto OVER;

    iTotalFrame = (pData->frame_buff[8] << 24 |	//总帧数
                   pData->frame_buff[9] << 16 |
                   pData->frame_buff[10] << 8 |
                   pData->frame_buff[11] << 0);

    iTotalSize	= (pData->frame_buff[12] << 24 |	//文件大小
                   pData->frame_buff[13] << 16 |
                   pData->frame_buff[14] << 8 |
                   pData->frame_buff[15] << 0);

    //比较帧数是否正确
    count = (iTotalSize + FileFrameSize - 1) / FileFrameSize;

    if(count != iTotalFrame)
    {
        isOk = false;
        goto OVER;
    }

    for(iFrameNum = 1; iFrameNum <= iTotalFrame; iFrameNum++)
    {
        pData->frame_len = 2;

        //其余帧只发送帧号(1~N)
        send_size = 4;

        if(iFrameNum < count)	//非最后一帧数据为最大
            iFrameSize = FileFrameSize;
        else		//最后一帧数据可能未达到满帧256-1-1-2-4-2
            iFrameSize = iTotalSize - (iFrameNum - 1) * FileFrameSize;

        // 进度显示处理
        int percent = static_cast<int>(100.0 * (iFrameNum) / iTotalFrame);
        //qDebug("read file: i1=%d, i2=%d, percent=%d",iFrameNum,iTotalFrame,percent);
        emit updateFileProgress(percent);

        pData->frame_buff[pData->frame_len++] = (send_size >> 8) & 0xff;	//发送的数据长度
        pData->frame_buff[pData->frame_len++] = send_size & 0xff;

        pData->frame_buff[pData->frame_len++] = (iFrameNum >> 24) & 0xff;	//发送的帧号
        pData->frame_buff[pData->frame_len++] = (iFrameNum >> 16) & 0xff;
        pData->frame_buff[pData->frame_len++] = (iFrameNum >> 8) & 0xff;
        pData->frame_buff[pData->frame_len++] = (iFrameNum >> 0) & 0xff;

        isOk = funLoadFile(MODBUS_MODE_RTU, pData, pTmpBuff, iFrameNum, iFrameSize);

        if(isOk)
        {
            file.write(reinterpret_cast<char*>(pTmpBuff), static_cast<qint64>(iFrameSize));
        }
        else
        {
            goto OVER;
        }
    }

    //传输结束,发送结束指令
OVER:
    pData->frame_len = 2;
    pData->frame_buff[pData->frame_len++] = 0x00;	//发送的数据长度为4
    pData->frame_buff[pData->frame_len++] = 0x04;

    pData->frame_buff[pData->frame_len++] = 0xff;	//发送的帧号为0xffffffff
    pData->frame_buff[pData->frame_len++] = 0xff;
    pData->frame_buff[pData->frame_len++] = 0xff;
    pData->frame_buff[pData->frame_len++] = 0xff;

    isOk = (isOk && funLoadFile(MODBUS_MODE_RTU, pData, pTmpBuff, 0xffffffff, 0));
#ifdef  Q_OS_LINUX
    file.flush();
    fsync(file.handle());
#endif
    file.close();

    pData->id = old_id; 		//还原先前设备ID，主要为不影响设备作为服务端时ID号的唯一性

    return isOk;
}

bool ModbusClient::writeFile(INT8U id, char* remoteFile, const QString& localFile)
{
    bool isOk = false;
    INT32U buffer_count ;	//计算传输总帧数
    INT16U last_frame_count;//最后一次使用缓冲区的帧数
    INT16U last_frame_size;	//最后一帧包含数据大小
    INT16U send_size;		//数据长度(不包含地址码,功能码,自身)4字节帧号+文件名
    INT16U iFrameSize;		//帧数据大小
    INT32U iFrameNum = 0;
    INT32U iTotalFrame;	//帧号
    INT32U frame_num, i, j;
    INT8U  old_id;

    QFile file(localFile);

    //打开后保存在数据流
    if(false == file.open(QIODevice::ReadOnly))
        return false; // 不能打开文件

    auto FileSize = static_cast<quint32>(file.size());

    if(0 == FileSize)
        return true;

#define	SetBuffFrameNum			(1024)					//设置缓冲区所用帧数
#define SetSendPackBuf_Size		(SetBuffFrameNum*FileFrameSize)	//设置多帧发送时的发送缓存(从)大小
    INT8U		g_aSendPackBuff[SetSendPackBuf_Size];

    iTotalFrame = (FileSize + FileFrameSize - 1) / FileFrameSize;			//总帧数1~N
    buffer_count = (iTotalFrame + SetBuffFrameNum - 1) / SetBuffFrameNum;	//缓冲区使用次数1~N
    last_frame_count = iTotalFrame % SetBuffFrameNum;					//最后一次使用缓冲区的帧数1~SetBuffFrameNum
    last_frame_size = FileSize % FileFrameSize;						//最后一帧传送数据量1~FileFrameSize

    if(last_frame_count == 0)
        last_frame_count = SetBuffFrameNum;

    if(last_frame_size == 0)
        last_frame_size = FileFrameSize;

    auto pData = &modparam;

    old_id = pData->id;		//保存自身ID,
    pData->id = id; 		//写入寻址的ID
    pData->fun_code = MB_FUNC_CODE_STORE_FILE_TO_REMOTE;	//保存文件功能码

    iFrameNum = 0;
    iTotalFrame = (FileSize + FileFrameSize - 1) / FileFrameSize;

    pData->frame_len = 0;
    pData->frame_buff[pData->frame_len++] = pData->id;
    pData->frame_buff[pData->frame_len++] = pData->fun_code;

    //发送的数据长度=当前帧号4+总帧号4+文件大小4+文件名长度
    send_size = 12 + strlen(remoteFile);
    pData->frame_buff[pData->frame_len++] = (send_size >> 8) & 0xff;
    pData->frame_buff[pData->frame_len++] = send_size & 0xff;

    //当前帧号0
    pData->frame_buff[pData->frame_len++] = (iFrameNum >> 24) & 0xff;
    pData->frame_buff[pData->frame_len++] = (iFrameNum >> 16) & 0xff;
    pData->frame_buff[pData->frame_len++] = (iFrameNum >> 8) & 0xff;
    pData->frame_buff[pData->frame_len++] = (iFrameNum >> 0) & 0xff;

    //总帧号
    pData->frame_buff[pData->frame_len++] = (iTotalFrame >> 24) & 0xff;
    pData->frame_buff[pData->frame_len++] = (iTotalFrame >> 16) & 0xff;
    pData->frame_buff[pData->frame_len++] = (iTotalFrame >> 8) & 0xff;
    pData->frame_buff[pData->frame_len++] = (iTotalFrame >> 0) & 0xff;

    //总大小
    pData->frame_buff[pData->frame_len++] = (FileSize >> 24) & 0xff;
    pData->frame_buff[pData->frame_len++] = (FileSize >> 16) & 0xff;
    pData->frame_buff[pData->frame_len++] = (FileSize >> 8) & 0xff;
    pData->frame_buff[pData->frame_len++] = (FileSize >> 0) & 0xff;

    //文件名
    strcpy((char*)(pData->frame_buff + pData->frame_len), remoteFile);	//保存文件名
    pData->frame_len += send_size - 12;

    isOk = funStoreFile(MODBUS_MODE_RTU, pData, iFrameNum);

    if(false == isOk)
        goto OVER;

    //缓冲区使用次数
    for(i = 0; i < buffer_count; i++)
    {
        //清空缓冲区
        memset(g_aSendPackBuff, 0x00, SetSendPackBuf_Size);
        file.read(reinterpret_cast<char*>(g_aSendPackBuff), SetSendPackBuf_Size);

        //本次缓冲区包含帧数
        frame_num = SetBuffFrameNum;

        if(i == (buffer_count - 1))
            frame_num = last_frame_count;

        //本次缓冲区帧数
        for(j = 0; j < frame_num; j++)
        {
            iFrameNum ++;						//当前帧号

            //进度显示处理
            int percent = static_cast<int>(100.0 * (iFrameNum) / iTotalFrame);
            //qDebug("write file: i1=%d, i2=%d, percent=%d",iFrameNum,iTotalFrame,percent);
            emit updateFileProgress(percent);

            //本帧包含的数据量
            iFrameSize = FileFrameSize;

            if(iFrameNum == iTotalFrame)		//最后一帧
            {
                iFrameSize = last_frame_size;
            }

            pData->frame_len = 2;

            //数据长度
            send_size = iFrameSize + 4;
            pData->frame_buff[pData->frame_len++] = (send_size >> 8) & 0xff;
            pData->frame_buff[pData->frame_len++] = send_size & 0xff;

            //当前帧号
            pData->frame_buff[pData->frame_len++] = (iFrameNum >> 24) & 0xff;
            pData->frame_buff[pData->frame_len++] = (iFrameNum >> 16) & 0xff;
            pData->frame_buff[pData->frame_len++] = (iFrameNum >> 8) & 0xff;
            pData->frame_buff[pData->frame_len++] = (iFrameNum >> 0) & 0xff;

            //数据内容
            memcpy((pData->frame_buff + pData->frame_len), &g_aSendPackBuff[j * FileFrameSize], iFrameSize);
            pData->frame_len += iFrameSize;

            isOk = funStoreFile(MODBUS_MODE_RTU, pData, iFrameNum);

            if(false == isOk)
                goto OVER;
        }
    }

OVER:

    //只在通讯错误是发送终止码,
    if(false == isOk)
    {
        pData->frame_len = 2;
        pData->frame_buff[pData->frame_len++] = 0x00;	//发送的数据长度为4
        pData->frame_buff[pData->frame_len++] = 0x04;

        pData->frame_buff[pData->frame_len++] = 0xff;	//发送的帧号为0xffffffff
        pData->frame_buff[pData->frame_len++] = 0xff;
        pData->frame_buff[pData->frame_len++] = 0xff;
        pData->frame_buff[pData->frame_len++] = 0xff;

        funStoreFile(MODBUS_MODE_RTU, pData, 0xffffffff);
    }

    file.close();
    pData->id = old_id;			//还原先前设备ID，主要为不影响设备作为服务端时ID号的唯一性

    return isOk;
}

bool ModbusClient::funRead01H02H03H04H(INT8U cmd_mode, ModParam* pData, INT8U* pReadBuff,
                                       INT8U* size)
{
    unsigned int i;

    pData->frame_len = 0;
    pData->frame_buff[pData->frame_len++] = pData->id;
    pData->frame_buff[pData->frame_len++] = pData->fun_code;
    pData->frame_buff[pData->frame_len++] = (pData->start_addr >> 8) & 0xff;
    pData->frame_buff[pData->frame_len++] = pData->start_addr & 0xff;
    pData->frame_buff[pData->frame_len++] = (pData->reg_num >> 8) & 0xff;
    pData->frame_buff[pData->frame_len++] = pData->reg_num & 0xff;

    if(false == sendData(pData, cmd_mode))
        return false;

    pData->recv_len = pData->frame_buff[2];     //提取字节数
    *size = pData->recv_len;

    switch(pData->fun_code)
    {
    case 0x01:  //位操作
    case 0x02:
        if(((pData->reg_num >> 3) + ((pData->reg_num & 0x7) ? 1 : 0)) !=
                pData->recv_len)   //判断获取位数是否与接收实际位数相等
        {
            return false;//MODBUS_INVALID_DATA;
        }

        memcpy(pReadBuff, pData->frame_buff + 3, *size);
        break;

    case 0x03:  //字操作
    case 0x04:
        if((pData->reg_num << 1) != pData->recv_len)
        {
            // debug_printf("funRead01H02H03H04H: 4\n");
            return false;//MODBUS_INVALID_DATA;
        }

        for(i = 0; i < *size; i += 2)
        {
            pReadBuff[i]   = pData->frame_buff[3 + i + 1];
            pReadBuff[i + 1] = pData->frame_buff[3 + i];
            //ps_debugout("%2X  %2X\r\n",pReadBuff[i],pReadBuff[i+1]);
        }

        break;

    default:
        /// debug_printf("funRead01H02H03H04H: 5\n");
        return false;//MODBUS_INVALID_FUNC;
        break;
    }

    return true;
}

bool ModbusClient::funWrite0FH10H(INT8U cmd_mode, ModParam* pData, INT8U* pWriteBuff, INT8U size)
{
    unsigned int i;

    pData->frame_len = 0;
    pData->frame_buff[pData->frame_len++] = pData->id;
    pData->frame_buff[pData->frame_len++] = pData->fun_code;
    pData->frame_buff[pData->frame_len++] = (pData->start_addr >> 8) & 0xff;
    pData->frame_buff[pData->frame_len++] = pData->start_addr & 0xff;
    pData->frame_buff[pData->frame_len++] = (pData->reg_num >> 8) & 0xff;
    pData->frame_buff[pData->frame_len++] = pData->reg_num & 0xff;
    pData->frame_buff[pData->frame_len++] = size;   //写数据字节长度

    if(pData->fun_code == 0x0F)
    {
        memcpy(pData->frame_buff + pData->frame_len, pWriteBuff, size);
        pData->frame_len += size;
    }
    else
    {
        for(i = 0; i < size; i += 2)
        {
            pData->frame_buff[pData->frame_len++] = pWriteBuff[i + 1];
            pData->frame_buff[pData->frame_len++] = pWriteBuff[i];
        }
    }

    if(false == sendData(pData, cmd_mode))
        return false;

    if(pData->start_addr != (pData->frame_buff[2] << 8 |
                             pData->frame_buff[3]))   //检查起始地址是否正确
    {
        return false;//MODBUS_INVALID_ADDR;
    }

    if(pData->reg_num != (pData->frame_buff[4] << 8 |
                          pData->frame_buff[5]))    //检查写数量是否正确
    {
        return false;//MODBUS_INVALID_DATA;
    }

    return true;
}

bool ModbusClient::funRead26H(INT8U cmd_mode, ModParam* pData, INT32S FifoCodeRead, INT8U* pReadBuff,
                              INT32S ReadAddressOffset, INT32U len, INT32U* size)
{
    INT32S	 iFrameNum = 0;//当前帧号
    INT32S	 iFrameNumTmp;//临时存储当前帧号
    INT32S	 iFrameTotal;//读取数据的总的帧数
    INT32S	 iFrameTotalSlave;//读取数据的总的帧数
    INT32S	 iTempNum;//帧数据长度
    INT16U	 nOffset;
    INT32S	 iTotalSize;
    INT8U*	pRecvDat;
    bool isSendFinished = false;

    iFrameTotal = (len + (FifoFrameSize - 1)) / FifoFrameSize;

    *size = 0;

    while(false == isSendFinished)//一次性加载缓存数据
    {
        if(iFrameNum == 0)
        {
            iTempNum = 16; //第一帧16个字节
            pData->frame_len = 0;
            pData->frame_buff[pData->frame_len++] = pData->id;
            pData->frame_buff[pData->frame_len++] = pData->fun_code;

            pData->frame_buff[pData->frame_len++] = (iTempNum >> 8) & 0xff;
            pData->frame_buff[pData->frame_len++] = iTempNum & 0xff;

            nOffset = 0;
            //帧号
            SetIntToDataBuff(iFrameNum, &pData->frame_buff[pData->frame_len], 4, &nOffset);
            //缓存号
            SetIntToDataBuff(FifoCodeRead, &pData->frame_buff[pData->frame_len], 4, &nOffset);
            //地址偏移量
            SetIntToDataBuff(ReadAddressOffset, &pData->frame_buff[pData->frame_len], 4, &nOffset);
            //请求数据长度
            SetIntToDataBuff(len, &pData->frame_buff[pData->frame_len], 4, &nOffset);

            pData->frame_len += nOffset;
        }
        else if(iFrameNum <= iFrameTotal)
        {
            iTempNum = 4; //数据请求帧4个字节
            pData->frame_len = 0;
            pData->frame_buff[pData->frame_len++] = pData->id;
            pData->frame_buff[pData->frame_len++] = pData->fun_code;

            pData->frame_buff[pData->frame_len++] = (iTempNum >> 8) & 0xff;
            pData->frame_buff[pData->frame_len++] = iTempNum & 0xff;

            nOffset = 0;
            //帧号
            SetIntToDataBuff(iFrameNum, &pData->frame_buff[pData->frame_len], 4, &nOffset);

            pData->frame_len += nOffset;
        }
        else if(iFrameNum > iFrameTotal)
        {
            iFrameNum = 0x0fffffff;
            iTempNum = 4; //数据请求帧4个字节
            pData->frame_len = 0;
            pData->frame_buff[pData->frame_len++] = pData->id;
            pData->frame_buff[pData->frame_len++] = pData->fun_code;

            pData->frame_buff[pData->frame_len++] = (iTempNum >> 8) & 0xff;
            pData->frame_buff[pData->frame_len++] = iTempNum & 0xff;

            nOffset = 0;
            //帧号
            SetIntToDataBuff(iFrameNum, &pData->frame_buff[pData->frame_len], 4, &nOffset);

            pData->frame_len += nOffset;
        }

        if(false == sendData(pData, cmd_mode))
            return false;

        pData->recv_len = pData->frame_buff[2] << 8 | pData->frame_buff[3]; //提取字节数

        if(MB_FUNC_CODE_READ_FIFO == pData->fun_code)
        {
            pRecvDat = pData->frame_buff + 4;
            nOffset = 0;
            GetIntFromDataBuff(&iFrameNumTmp, pRecvDat, 4, &nOffset);	//获取当前帧数

            if(iFrameNum != iFrameNumTmp) //得到的数据帧不是需要的数据帧
                continue;//重新去请求需要的数据帧

            if(iFrameNum == 0) //请求第1帧
            {
                GetIntFromDataBuff(&iFrameTotalSlave, pRecvDat, 4, &nOffset);	//获取需要接收总帧数

                if(iFrameTotalSlave !=
                        iFrameTotal) //上位机根据读取长度计算的总帧数不等于下位机根据读取长度计算的总长度，传输数据出错
                    return false; // MODBUS_FRAME_ERR;

                GetIntFromDataBuff(&iTotalSize, pRecvDat, 4, &nOffset);	//获取需要接收总缓存的大小

                if(iTotalSize !=
                        len) //上位机请求读取长度计不等于下位机根据请求读取长度计算的长度，传输数据出错
                    return false;// MODBUS_FRAME_ERR;

                iFrameNum++;
            }
            else if(iFrameNum < iFrameTotal) //请求数据帧
            {
                memcpy(pReadBuff + *size, pRecvDat + 4, pData->recv_len - 4);
                (*size) += pData->recv_len - 4;
                iFrameNum++;
            }
            else if(iFrameNum == iFrameTotal) //请求最后一帧数据帧
            {
                memcpy(pReadBuff + *size, pRecvDat + 4, pData->recv_len - 4);
                (*size) += pData->recv_len - 4;

                if(*size != len)
                    return false; // MODBUS_FRAME_ERR;

                iFrameNum++;
            }
            else if(iFrameNum == 0x0fffffff) //收到结束帧
                isSendFinished = true;
            else
                return false; // MODBUS_FRAME_ERR;
        }
        else
        {
            return false; //MODBUS_INVALID_FUNC;
        }

        // 通知进度
        if(0x0fffffff != iFrameNum)
        {
            int percent = static_cast<int>(100.0 * (iFrameNum - 1) / iFrameTotal);
            //qDebug("i1=%d, i2=%d, percent=%d",iFrameNum,iFrameTotal,percent);
            emit updateFifoProgress(percent);
        }

    } // while(false==isSendFinished) loop

    return true;
}

bool ModbusClient::funWrite27H(INT8U cmd_mode, ModParam* pData, INT32S FifoCodeWrite, INT8U* pWriteBuff,
                               INT32S WriteAddressOffset, INT32U len, INT32U* size)
{
    INT32S	 iFrameNum = 0;//当前帧号
    INT32S	 iFrameNumTmp;//临时存储当前帧号
    INT32S	 iFrameTotal;//写数据的总的帧数
    INT32S	 iTempNum;//帧数据长度
    INT16U	 nOffset;
    INT8U*	pRecvDat;
    INT8U*	pTempBuff = pWriteBuff;
    bool isSendFinished = false;

    iFrameTotal = (len + (FifoFrameSize - 1)) / FifoFrameSize;

    *size = 0;

    while(false == isSendFinished)//一次性加载缓存数据
    {
        if(iFrameNum == 0)
        {
            iTempNum = 20; //第一帧16个字节
            pData->frame_len = 0;
            pData->frame_buff[pData->frame_len++] = pData->id;
            pData->frame_buff[pData->frame_len++] = pData->fun_code;

            pData->frame_buff[pData->frame_len++] = (iTempNum >> 8) & 0xff;
            pData->frame_buff[pData->frame_len++] = iTempNum & 0xff;


            nOffset = 0;
            //帧号
            SetIntToDataBuff(iFrameNum, &pData->frame_buff[pData->frame_len], 4, &nOffset);
            //缓存号
            SetIntToDataBuff(FifoCodeWrite, &pData->frame_buff[pData->frame_len], 4, &nOffset);

            //总帧数
            SetIntToDataBuff(iFrameTotal, &pData->frame_buff[pData->frame_len], 4, &nOffset);

            //请求写数据长度
            SetIntToDataBuff(len, &pData->frame_buff[pData->frame_len], 4, &nOffset);
            //地址偏移量
            SetIntToDataBuff(WriteAddressOffset, &pData->frame_buff[pData->frame_len], 4, &nOffset);

            pData->frame_len += nOffset;

        }
        else if(iFrameNum < iFrameTotal)
        {
            iTempNum = 4 + FifoFrameSize; //数据请求帧字节
            pData->frame_len = 0;
            pData->frame_buff[pData->frame_len++] = pData->id;
            pData->frame_buff[pData->frame_len++] = pData->fun_code;

            pData->frame_buff[pData->frame_len++] = (iTempNum >> 8) & 0xff;
            pData->frame_buff[pData->frame_len++] = iTempNum & 0xff;

            nOffset = 0;
            //帧号
            SetIntToDataBuff(iFrameNum, &pData->frame_buff[pData->frame_len], 4, &nOffset);
            pData->frame_len += nOffset;

            memcpy(&pData->frame_buff[pData->frame_len], pTempBuff + (*size), FifoFrameSize);
            (*size) += FifoFrameSize;
            pData->frame_len += FifoFrameSize;
        }
        else if(iFrameNum == iFrameTotal) //最后一帧数据帧
        {
            iTempNum = 4 + len - (*size); //数据请求帧字节
            pData->frame_len = 0;
            pData->frame_buff[pData->frame_len++] = pData->id;
            pData->frame_buff[pData->frame_len++] = pData->fun_code;

            pData->frame_buff[pData->frame_len++] = (iTempNum >> 8) & 0xff;
            pData->frame_buff[pData->frame_len++] = iTempNum & 0xff;

            nOffset = 0;
            //帧号
            SetIntToDataBuff(iFrameNum, &pData->frame_buff[pData->frame_len], 4, &nOffset);
            pData->frame_len += nOffset;

            memcpy(&pData->frame_buff[pData->frame_len], pTempBuff + (*size), len - (*size));
            pData->frame_len += len - (*size);
            (*size) = len;
        }
        else if(iFrameNum > iFrameTotal) //结束帧
        {
            iFrameNum = 0x0fffffff;
            iTempNum = 4; //数据请求帧4个字节
            pData->frame_len = 0;
            pData->frame_buff[pData->frame_len++] = pData->id;
            pData->frame_buff[pData->frame_len++] = pData->fun_code;

            pData->frame_buff[pData->frame_len++] = (iTempNum >> 8) & 0xff;
            pData->frame_buff[pData->frame_len++] = iTempNum & 0xff;

            nOffset = 0;
            //帧号
            SetIntToDataBuff(iFrameNum, &pData->frame_buff[pData->frame_len], 4, &nOffset);

            pData->frame_len += nOffset;
        }

        if(false == sendData(pData, cmd_mode))
            return false;

        pData->recv_len = pData->frame_buff[2] << 8 | pData->frame_buff[3]; //提取字节数

        if(MB_FUNC_CODE_WRITE_FIFO == pData->fun_code)
        {
            pRecvDat = pData->frame_buff + 4;
            nOffset = 0;
            GetIntFromDataBuff(&iFrameNumTmp, pRecvDat, 4, &nOffset);	//获取当前帧数

            if(iFrameNum != iFrameNumTmp) //得到的数据帧不是需要的数据帧
                continue;//重新去存储需要的数据帧

            if(iFrameNum <= iFrameTotal) //请求数据帧
                iFrameNum++;
            else if(iFrameNum == 0x0fffffff) //收到结束帧
                isSendFinished = true;
            else
                return false; // MODBUS_FRAME_ERR;
        }
        else
        {
            return false; //MODBUS_INVALID_FUNC;
        }

        // 通知进度
        if(0x0fffffff != iFrameNum)
        {
            int percent = static_cast<int>(100.0 * (iFrameNum - 1) / iFrameTotal);
            //qDebug("i1=%d, i2=%d, percent=%d",iFrameNum,iFrameTotal,percent);
            emit updateFifoProgress(percent);
        }

    } // while(false==isSendFinished) loop

    return true;
}

bool ModbusClient::funLoadFile(INT8U cmd_mode, ModParam* pData, INT8U* pFileBuffer, INT32U iFrameNum, INT16U size)
{
    INT16U		CurFrameSize;
    INT32U		CurFrameNum;

    if(false == sendData(pData, cmd_mode))
        return false;

    //检查数据帧大小帧,第0帧与结束帧无文件数据
    if(iFrameNum != 0 && iFrameNum != 0xffffffff)
    {
        CurFrameSize = (pData->frame_buff[2] << 8) | (pData->frame_buff[3]);

        if(size != CurFrameSize - 4)	//减去表示帧号的4字节
            return false; //MODBUS_FRAME_ERR;
    }

    //检查帧号是否对应
    CurFrameNum = ((pData->frame_buff[4] << 24) | (pData->frame_buff[5] << 16)
                   | (pData->frame_buff[6] << 8) | pData->frame_buff[7]);

    if(iFrameNum != CurFrameNum)
        return false; //MODBUS_FRAME_ERR;

    //验证完毕,通信成功,获取文件数据
    if(iFrameNum != 0 && iFrameNum != 0xffffffff)	//头帧与尾帧不包含数据
    {
        for(auto i = 0; i < size; i++)
            pFileBuffer[i] = pData->frame_buff[i + 8];
    }

    return true;
}

bool ModbusClient::funStoreFile(INT8U cmd_mode, ModParam* pData, INT32U iFrameNum)
{
    INT32U		CurFrameNum;

    if(false == sendData(pData, cmd_mode))
        return false;

    //不检查数据长度,应为返回固定大小为4

    //检查帧号是否对应
    CurFrameNum = ((pData->frame_buff[4] << 24) | (pData->frame_buff[5] << 16)
                   | (pData->frame_buff[6] << 8) | pData->frame_buff[7]);

    if(iFrameNum != CurFrameNum)
        return false; // MODBUS_FRAME_ERR;

    //验证完毕,通信成功,获取文件数据

    return true;
}

bool ModbusClient::sendData(ModParam* pData, INT8U cmd_mode)
{
    unsigned int i = 0; // set i as retry counter

    auto frameLength = pData->frame_len;
    memcpy(g_ucFrameBufTmp, pData->frame_buff, pData->frame_len);

    while(1)
    {
        pData->cmd_mode = cmd_mode;

        bool isSendOk = verifySend(pData); //添加校验码并发送

#ifdef MOBBUSCLIENT_ENABLE_STATISTIC
        ++counterPacketSent;
#endif

        if(true == isSendOk)
        {
            if(true == verifyRecv(pData) && pData->cmd_mode == cmd_mode)
            {
                break;      //接收并校验无错
            }
        }

#ifdef MOBBUSCLIENT_ENABLE_STATISTIC
        ++counterPacketSentFail;
#endif

        if(false == isSendOk)
        {
            //OSTimeDly(100, OS_OPT_TIME_DLY, &ERR);        //发送失败,延时100ms重发
            Utils::sleep(50);
        }

        if(i >= pData->repeat_times)
        {
            // debug_printf("ModbusClient::sendData(): too more retries\n");
            return false;//MODBUS_DEVICE_NO_RESPOND;
        }

        pData->frame_len = frameLength;
        memcpy(pData->frame_buff, g_ucFrameBufTmp, pData->frame_len);

        ++i;
    }

    if(pData->id != pData->frame_buff[0])       //检查设备地址是否正确
    {
        // debug_printf("ModbusClient::sendData(): 2, pData->id=%u, frame[0]=%u\n",pData->id, pData->frame_buff[0]);
        return false;//MODBUS_FRAME_ERR;
    }

    if(pData->fun_code != pData->frame_buff[1])   //检查功能码是否正确
    {
        // debug_printf("ModbusClient::sendData(): 3 pData->fun_code=%u, frame[1]=%u\n",pData->fun_code, pData->frame_buff[1]);
        return false;//MODBUS_INVALID_FUNC;
    }

    return true;
}

bool ModbusClient::verifySend(ModParam* pData)
{
    bool isOk = false;
    quint16 sendLength = 0;

    switch(pData->cmd_type)
    {
    case MODBUS_INTERFACENET:
        ModbusInt16ToBuff(pData->net_affair, g_ucSendBuf, 0);
        ModbusInt16ToBuff(pData->net_protocol, g_ucSendBuf, 2);
        ModbusInt16ToBuff(pData->frame_len, g_ucSendBuf, 4);
        memcpy(&g_ucSendBuf[6], pData->frame_buff, pData->frame_len);
        sendLength = pData->frame_len + 6;

        isOk = myUDPClient->send(g_ucSendBuf, sendLength);
        break;

    case MODBUS_INTERFACECOM:
        /*
        memcpy(g_ucSendBuf, pData->frame_buff, pData->frame_len);

        // 校验
        crc=ModbusRtuCRC(g_ucSendBuf,static_cast<unsigned char>(pData->frame_len));
        g_ucSendBuf[pData->frame_len]=crc&0xff;
        g_ucSendBuf[pData->frame_len+1]=(crc>>8);

        sendLength = pData->frame_len + 2;

        isOk = p_serial->SendData(g_ucSendBuf,sendLength);
        */
        break;

    default:
        break;
    }

    return isOk;
}

bool ModbusClient::verifyRecv(ModParam* pData)
{
    bool isOk = false;
    int iPackLen = 0;
    quint16 recvLength = 0;

    switch(pData->cmd_type)
    {
    case MODBUS_INTERFACENET:
        isOk = myUDPClient->recv(g_ucRecvBuf, &iPackLen, MODBUS_RTU_FRAME_SIZE + 5);

        if(iPackLen < 7)
            isOk = false;

        if(isOk)
        {
            recvLength = static_cast<quint16>(iPackLen);

            pData->frame_len = ModbusBuffToInt16(g_ucRecvBuf, 4);

            if(pData->frame_len == (recvLength - 6))
            {
                pData->net_affair = ModbusBuffToInt16(g_ucRecvBuf, 0);
                pData->net_protocol = ModbusBuffToInt16(g_ucRecvBuf, 2);
                memcpy(pData->frame_buff, &g_ucRecvBuf[6], pData->frame_len);
                pData->id = pData->frame_buff[0];//网络通讯ID无意义,但回复时要与发送来的ID值相同
            }
        }

        break;

    case MODBUS_INTERFACECOM:
        /*
        Sleep(50);
        isOk=p_serial->ReadData(g_ucRecvBuf,MODBUS_RTU_FRAME_SIZE,&iPackLen,pData->timeouts);

        if(isOk){

            if(iPackLen > MODBUS_RTU_FRAME_SIZE){
                isOk=false;
                break;
            }

            //for(int i=0;i<iPackLen-2;i++)
            //  debug_printf("%02x ",g_ucRecvBuf[i]);
            //debug_printf("\n");

            recvLength = static_cast<INT16U>(iPackLen);

            isOk=isCRCGood(g_ucRecvBuf,static_cast<unsigned char>(iPackLen));
            //debug_printf("crc is good? %d\n",isOk?1:0);

            if(isOk){
                // 除去2个末尾的CRC
                pData->frame_len = recvLength-2;
                memcpy(pData->frame_buff, &g_ucRecvBuf[0], pData->frame_len);
            }
        }
        */
        break;

    default:
        break;

    }

    return isOk;
}

void ModbusClient::GetIntFromDataBuff(INT32S* piValue, INT8U* pbyDataBuff, INT8U byBytes, INT16U* pnOffset)
{
    *piValue = 0;

    for(INT8U i = 0; i < byBytes; i++)
        *piValue = *piValue | (pbyDataBuff[(*pnOffset)++] << (byBytes - (i + 1)) * 8);
}

void ModbusClient::SetIntToDataBuff(INT32S iValue, INT8U* pbyDataBuff, INT8U byBytes, INT16U* pnOffset)
{
    for(INT8U i = 0; i < byBytes; i++)
        pbyDataBuff[(*pnOffset)++] = iValue >> (byBytes - (i + 1)) * 8;
}
