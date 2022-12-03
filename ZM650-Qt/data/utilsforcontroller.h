/*
 * utilsforcontroller.h
 *
 * 公用的工具函数，主要是下位机部份
 *
 * Created on: 2019年 01月 21日 星期一 14:31:00 CST
 * Author: lixingcong
 */
#ifndef UTILSFORCONTROLLER_H
#define UTILSFORCONTROLLER_H

#include "utils.h"
#include "globaldata.h"
#include <QProgressDialog>

namespace Utils
{
bool checkWorkstatusIfStop(bool isNeedCreateDialog = false, QProgressDialog** dialogCreated = nullptr,
                           QWidget* dialogParent = nullptr);
bool checkWorkstatusIfPause();

bool readIOStatus(bool isInput, int functionNumber, quint16* result);
bool writeOutputStatus(int functionNumber, quint16 status);
};

#endif // UTILSFORCONTROLLER_H
