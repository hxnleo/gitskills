/*
 * modbusaddress.h
 *
 * Modbus地址表
 *
 * Created on: 2019年 01月 21日 星期一 14:34:20 CST
 * Author: lixingcong
 */
#ifndef MODBUSADDRESS_H
#define MODBUSADDRESS_H

namespace ModbusAddress
{
enum
{
    IOStatus_In = 1000,
    IOStatus_Out = 1400,
    Authority = 3000,
    Interface = 3001,
    HeartBeat = 3005,
    HandBox = 3009,
    Motioninit = 3010,
    FtpConnect = 3011,
    language = 3020,//语言
    SyetemAxis = 3021,//系统
    ParamUse = 3022,//32位可以使用的地址,划分为32个参数存储
	AxisSize = 3500,
    ZD_Offset = 3600,
    LinkMove = 3910,
    Reboot = 3960,
    Upgrade = 3961,
    Motion = 3963,//运动库升级
    WorkCommand = 4300,
     AXIS_ENABLE = 4340,//总线轴使能
    ManulMove = 5100,
    IOStatus_DA = 5200,//修改
    Addr_Dog = 5600,
    Dog_Progress  = 5700,
    Dog_SysTime  = 5701,
    Dog_Init = 5714,
    Dog2_Progress  = 5850,
    Dog2_Pwd  = 5852,
    InConfig = 7000,
    OutConfig = 7500,
    FileManage_GetInfo = 20000,
    FileManage_SetDir = 33100,
    FileManage_GetFileCount = 33228,
    FileManage_NewFile = 33230,
    FileManage_Delete = 33238,
    FileManage_SetCopy = 33246,
    FileManage_SetPaste = 33254,
    CurrentWorkFilename = 33264,
    ControllerVersion = 33270,
    WorkfileTotalPoint = 33281,
    ProducedCount = 33282,
    WorkMode = 33301,//工作模式
    RunMode = 33302,//植钻模式
    MainSpeed = 33304,
    CLoseBtn  = 33306,
    VFD = 33414, // 变频器
    VFD2 = 33415, // 变频器2
    AutoMode = 33317,//毛道
    ButtonOperate = 33500,
    TeachWorkfileHead = 34000,
    Workfile = 34100,
    g_IsGoHome = 33420,
    g_IsGoStart = 33421,
    ControllerRealTimeInfo = 34400,
    g_WheelGear=33316,//手动速度裆位
};
}


#endif // MODBUSADDRESS_H
