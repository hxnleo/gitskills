/*
 * adt_datatype_qt.h
 *
 * 众为兴数据类型声明，对应adt_datatype.h
 *
 * Created on: 2019年 01月 21日 星期一 14:31:00 CST
 * Author: lixingcong
 */
#ifndef ADT_TYPEDEF_QT_H
#define ADT_TYPEDEF_QT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char    BYTE;
typedef unsigned char    BOOLEAN;
typedef unsigned char    INT8U;
typedef   signed char    INT8S;
typedef unsigned short   INT16U;
typedef   signed short   INT16S;
typedef unsigned int     INT32U;
typedef   signed int     INT32S;

typedef float            FP32;                     /* Single precision floating point                    */
typedef double           FP64;                     /* Double precision floating point                    */

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef BOOL
#define BOOL BOOLEAN
#endif

typedef struct tagDATE
{
    unsigned short year;
    unsigned short month;
    unsigned short week;//for padding
    unsigned short day;
} DATE_T;

typedef struct tagTIME
{
    unsigned short hour;
    unsigned short minute;
    unsigned short second;
} TIME_T;

#ifdef __cplusplus
}
#endif

#endif // ADT_TYPEDEF_QT_H
