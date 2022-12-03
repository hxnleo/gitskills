/*
 * patterns.cpp
 *
 * 花样类，添加花样都在这里！
 *
 * Created on: 2019年 01月 26日 星期六 17:03:39 CST
 * Author: lixingcong
 */

#include "patterns.h"
#include <QList>
#include <QPoint>
#include <QSize>

namespace Adapters
{

//static PatternParamItem Item[]=
//{
//    {QObject::tr("X方向孔数"),{1,9999},TYPE_U32,QPoint(270, 10),60,true,0,QStringList()},
//    {QObject::tr("Y方向孔数"),{1,9999},TYPE_U32,QPoint(550, 200),30,false,0,QStringList()},
//    {QObject::tr("X方向长度"),{1,9999},TYPE_F32,QPoint(270, 390),60,true,0,QStringList()},
//    {QObject::tr("Y方向长度"),{1,9999},TYPE_F32,QPoint(70, 200),30,true,0,QStringList()},
//    {QObject::tr("Y方向长度"),{1,9999},TYPE_StringList,QPoint(550, 300),30,false,0,QStringList()<< QObject::tr("先Y后X") << QObject::tr("先X后Y")}
//};

DataAdapter_Patterns::DataAdapter_Patterns(): DataAdapterBase<PatternItem>(nullptr)
{
    //0-普通方孔(X-Y)
    {
        PatternItem pattern =
        {
            QObject::tr("0-普通方孔(X-Y)"),
            {QString(":/images/patterns/Disp0.bmp"), QString(":/images/patterns/Pic0.bmp")}
        };

        PatternParamItem paramItem;

        paramItem.name = QObject::tr("X方向孔数");
        paramItem.range[0] = 1;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_U32;
        paramItem.position = QPoint(300, 10);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("Y方向孔数");
        paramItem.range[0] = 1;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_U32;
        paramItem.position = QPoint(580, 170);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("X方向长度");
        paramItem.range[0] = 0.001;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(270, 390);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("Y方向长度");
        paramItem.range[0] = 0.001;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(70, 170);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方向");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(550, 250);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList() << QObject::tr("先Y后X") << QObject::tr("先X后Y");
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("顺序");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(550, 300);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList() << QObject::tr("先外后里") << QObject::tr("先里后外");
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("左右");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(550, 350);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList() << QObject::tr("先左后右") << QObject::tr("先右后左");
        pattern.params.append(paramItem);

//        paramItem.name = QObject::tr("平移距离");
//        paramItem.range[0] = 0.001;
//        paramItem.range[1] = 9999;
//        paramItem.dataType = TYPE_F32;
//        paramItem.position = QPoint(550, 10);
//        paramItem.width = 100;
//        paramItem.isHorizontal = true;
//        paramItem.stringList = QStringList();
//        pattern.params.append(paramItem);

//        paramItem.name = QObject::tr("平移个数");
//        paramItem.range[0] = 0;
//        paramItem.range[1] = 9999;
//        paramItem.dataType = TYPE_F32;
//        paramItem.position = QPoint(550, 40);
//        paramItem.width = 100;
//        paramItem.isHorizontal = true;
//        paramItem.stringList = QStringList();
//        pattern.params.append(paramItem);

        patterns.append(pattern); // 添加一个花样
    }

    //1-梅花孔(X-Y)
    {
        PatternItem pattern =
        {
            QObject::tr("1-梅花孔(X-Y)"),
            {QString(":/images/patterns/Disp1.bmp"), QString(":/images/patterns/Pic1.bmp")}
        };

        PatternParamItem paramItem;

        paramItem.name = QObject::tr("X方向孔数");
        paramItem.range[0] = 1;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_U32;
        paramItem.position = QPoint(300, 10);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("Y方向孔数");
        paramItem.range[0] = 1;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_U32;
        paramItem.position = QPoint(580, 170);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("X方向长度");
        paramItem.range[0] = 0.001;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(270, 390);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("Y方向长度");
        paramItem.range[0] = 0.001;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(70, 170);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方向");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(550, 250);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList() << QObject::tr("先Y后X") << QObject::tr("先X后Y");
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("顺序");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(550, 300);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList() << QObject::tr("先外后里") << QObject::tr("先里后外");
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("左右");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(550, 350);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList() << QObject::tr("先左后右") << QObject::tr("先右后左");
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("平移距离");
        paramItem.range[0] = 0.001;
        paramItem.range[1] = 9999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(550, 10);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("平移个数");
        paramItem.range[0] = 0;
        paramItem.range[1] = 9999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(550, 40);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        patterns.append(pattern); // 添加一个花样
    }

    //2-普通方孔(X-Z)
    {
        PatternItem pattern =
        {
            QObject::tr("2-普通方孔(X-Z)"),
            {QString(":/images/patterns/Disp2.bmp"), QString(":/images/patterns/Pic2.bmp")}
        };

        PatternParamItem paramItem;

        paramItem.name = QObject::tr("孔距");
        paramItem.range[0] = 0.001;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(520, 200);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("分组数(图2组)");
        paramItem.range[0] = 1;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_U32;
        paramItem.position = QPoint(250, 390);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("组行数(图3行)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(60, 330);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("X方向总长度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(70, 180);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("排间距");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(540, 50);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方向");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(590, 320);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList() << QObject::tr("先Z后X") << QObject::tr("先X后Z");
        pattern.params.append(paramItem);

        patterns.append(pattern); // 添加一个花样
    }

    //3-梅花孔(X-Z)
    {
        PatternItem pattern =
        {
            QObject::tr("3-梅花孔(X-Z)"),
            {QString(":/images/patterns/Disp3.bmp"), QString(":/images/patterns/Pic3.bmp")}
        };

        PatternParamItem paramItem;

        paramItem.name = QObject::tr("孔距");
        paramItem.range[0] = 0.001;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(520, 170);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("分组数(图2组)");
        paramItem.range[0] = 1;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_U32;
        paramItem.position = QPoint(250, 390);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("组行数(图3行)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(60, 330);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("总长度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(80, 160);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("排间距");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(540, 50);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方向");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(590, 320);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList() << QObject::tr("先Z后X") << QObject::tr("先X后Z");
        pattern.params.append(paramItem);

        patterns.append(pattern); // 添加一个花样
    }

    //4-普通人形孔(X-Z)
    {
        PatternItem pattern =
        {
            QObject::tr("4-普通人形孔(X-Z)"),
            {QString(":/images/patterns/Disp4.bmp"), QString(":/images/patterns/Pic4.bmp")}
        };

        PatternParamItem paramItem;

        paramItem.name = QObject::tr("孔距");
        paramItem.range[0] = 0.001;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(190, 20);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("分组数(图2组)");
        paramItem.range[0] = 1;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_U32;
        paramItem.position = QPoint(580, 150);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("组行数(图2行)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(570, 20);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("人形排数(图2排)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(370, 20);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("长度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(190, 290);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("角度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(10, 260);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("行间距");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(80, 360);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方向");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(590, 320);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList() << QObject::tr("先Z后X") << QObject::tr("先X后Z");
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方式");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(590, 260);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList()<< QObject::tr("顺旋") << QObject::tr("逆旋");
        pattern.params.append(paramItem);

        patterns.append(pattern); // 添加一个花样
    }

    //5-梅花人形孔(X-Z)
    {
        PatternItem pattern =
        {
            QObject::tr("5-梅花人形孔(X-Z)"),
            {QString(":/images/patterns/Disp5.bmp"), QString(":/images/patterns/Pic5.bmp")}
        };

        PatternParamItem paramItem;

        paramItem.name = QObject::tr("孔距");
        paramItem.range[0] = 0.001;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(190, 20);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("分组数(图2组)");
        paramItem.range[0] = 1;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_U32;
        paramItem.position = QPoint(570, 150);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("组行数(图3行)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(570, 20);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("人形排数(图2排)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(370, 20);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("长度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(190, 290);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("角度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(10, 260);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("行间距");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(80, 360);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方向");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(590, 320);
        paramItem.width = 30;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList() << QObject::tr("先Z后X") << QObject::tr("先X后Z");
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方式");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(590, 260);
        paramItem.width = 30;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList()<< QObject::tr("顺旋") << QObject::tr("逆旋");
        pattern.params.append(paramItem);

        patterns.append(pattern); // 添加一个花样
    }

    //6-普通螺旋孔(X-Z)
    {
        PatternItem pattern =
        {
            QObject::tr("6-普通螺旋孔(X-Z)"),
            {QString(":/images/patterns/Disp6.bmp"), QString(":/images/patterns/Pic6.bmp")}
        };

        PatternParamItem paramItem;

        paramItem.name = QObject::tr("孔距");
        paramItem.range[0] = 0.001;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(150, 190);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("分组数(图2组)");
        paramItem.range[0] = 1;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_U32;
        paramItem.position = QPoint(270, 390);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("组行数(图2行)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(130, 290);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("总长度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(500, 200);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("螺旋角度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(180, 40);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("排间距");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(500, 40);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方向");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(590, 320);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList() << QObject::tr("先Z后X") << QObject::tr("先X后Z");
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方式");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(590, 260);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList() << QObject::tr("顺旋") << QObject::tr("逆旋");
        pattern.params.append(paramItem);

        patterns.append(pattern); // 添加一个花样
    }

    //7-梅花螺旋孔(X-Z)
    {
        PatternItem pattern =
        {
            QObject::tr("7-梅花螺旋孔(X-Z)"),
            {QString(":/images/patterns/Disp7.bmp"), QString(":/images/patterns/Pic7.bmp")}
        };

        PatternParamItem paramItem;

        paramItem.name = QObject::tr("孔距");
        paramItem.range[0] = 0.001;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(130, 190);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("分组数(图2组)");
        paramItem.range[0] = 1;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_U32;
        paramItem.position = QPoint(270, 390);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("组行数(图3行)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(130, 290);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("总长度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(500, 180);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("螺旋角度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(180, 20);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("排间距");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(500, 40);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方向");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(590, 320);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList() << QObject::tr("先Z后X") << QObject::tr("先X后Z");
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方式");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(590, 260);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList()<< QObject::tr("顺旋") << QObject::tr("逆旋");
        pattern.params.append(paramItem);

        patterns.append(pattern); // 添加一个花样
    }
    //8-梯形孔(X-Z)
    {
        PatternItem pattern =
        {
            QObject::tr("8-梯形孔(X-Z)"),
            {QString(":/images/patterns/Disp8.bmp"), QString(":/images/patterns/Pic8.bmp")}
        };

        PatternParamItem paramItem;

        paramItem.name = QObject::tr("圈数(图4圈)");
        paramItem.range[0] = 1;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_U32;
        paramItem.position = QPoint(190, 30);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("分组数(图2组)");
        paramItem.range[0] = 1;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_U32;
        paramItem.position = QPoint(570, 150);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("组行数(图2行)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(570, 50);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("梯形排数(图2排)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(375, 30);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("长度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(170, 290);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("角度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(10, 250);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("行间距");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(70, 370);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方向");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(590, 320);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList() << QObject::tr("先Z后X") << QObject::tr("先X后Z");
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方式");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(590, 260);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList()<< QObject::tr("顺旋") << QObject::tr("逆旋");
        pattern.params.append(paramItem);

        patterns.append(pattern); // 添加一个花样
    }

    //9-特殊普通人形孔(X-Z)
    {
        PatternItem pattern =
        {
            QObject::tr("9-特殊普通人形孔(X-Z)"),
            {QString(":/images/patterns/Disp9.bmp"), QString(":/images/patterns/Pic9.bmp")}
        };

        PatternParamItem paramItem;

        paramItem.name = QObject::tr("分组数(图2组)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_U32;
        paramItem.position = QPoint(570, 160);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("组行数(图2行)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(570, 50);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("每圈孔数(图6孔)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(80, 360);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("总长度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(200, 280);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("角度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(20, 260);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("圈距离");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(400, 30);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方式");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(590, 260);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList()<< QObject::tr("顺旋") << QObject::tr("逆旋");
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("前置圈数(图2圈)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_U32;
        paramItem.position = QPoint(370, 320);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        patterns.append(pattern); // 添加一个花样
    }

    //10-排对齐螺旋孔
    {
        PatternItem pattern =
        {
            QObject::tr("10-排对齐螺旋孔"),
            {QString(":/images/patterns/Disp10.bmp"), QString(":/images/patterns/Pic10.bmp")}
        };

        PatternParamItem paramItem;

        paramItem.name = QObject::tr("排对齐数(图4排)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_U32;
        paramItem.position = QPoint(120, 200);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("分组数(图2组)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(270, 390);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("组行数(图2行)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(100, 300);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("总长度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(500, 180);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("孔距");
        paramItem.range[0] = 0.001;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(200, 10);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("排间距");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(500, 30);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方向");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(590, 320);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList() << QObject::tr("先Z后X") << QObject::tr("先X后Z");
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方式");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(590, 260);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList()<< QObject::tr("顺旋") << QObject::tr("逆旋");
        pattern.params.append(paramItem);


        patterns.append(pattern); // 添加一个花样
    }

    //11-普通长短毛(X-Z)
    {
        PatternItem pattern =
        {
            QObject::tr("11-普通长短毛(X-Z)"),
            {QString(":/images/patterns/Disp11.bmp"), QString(":/images/patterns/Pic11.bmp")}
        };

        PatternParamItem paramItem;

        paramItem.name = QObject::tr("每圈孔数");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_U32;
        paramItem.position = QPoint(230, 390);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("长毛圈数(图3圈)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(550, 100);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("长毛组数(图1组)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(40, 100);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("短毛圈数(图2圈)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(550, 200);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("短毛组数(图2组)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(550, 30);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);



        paramItem.name = QObject::tr("总长度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(80, 200);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方向");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(590, 320);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList() << QObject::tr("先Z后X") << QObject::tr("先X后Z");
        pattern.params.append(paramItem);

        patterns.append(pattern); // 添加一个花样
    }


    //12-梅花长短毛(X-Z)
    {
        PatternItem pattern =
        {
            QObject::tr("12-梅花长短毛(X-Z)"),
            {QString(":/images/patterns/Disp12.bmp"), QString(":/images/patterns/Pic12.bmp")}
        };

        PatternParamItem paramItem;

        paramItem.name = QObject::tr("每圈孔数");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_U32;
        paramItem.position = QPoint(230, 390);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("长毛圈数(图3圈)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(550, 100);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("长毛组数(图1组)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(40, 100);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("短毛圈数(图2圈)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(550, 200);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("短毛组数(图2组)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(550, 30);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);



        paramItem.name = QObject::tr("总长度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(80, 200);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方向");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(590, 320);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList() << QObject::tr("先Z后X") << QObject::tr("先X后Z");
        pattern.params.append(paramItem);

        patterns.append(pattern); // 添加一个花样
    }

    //13-普通螺旋长短毛
    {
        PatternItem pattern =
        {
            QObject::tr("13-普通螺旋长短毛"),
            {QString(":/images/patterns/Disp13.bmp"), QString(":/images/patterns/Pic13.bmp")}
        };

        PatternParamItem paramItem;

        paramItem.name = QObject::tr("孔距");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(150, 200);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("长毛条数(图2条)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(100, 300);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("短毛条数(图4条)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(500, 60);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("总长度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(490, 200);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("螺旋角度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(230, 20);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);



        paramItem.name = QObject::tr("方向");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(590, 320);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList() << QObject::tr("先Z后X") << QObject::tr("先X后Z");
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方式");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(590, 260);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList()<< QObject::tr("顺旋") << QObject::tr("逆旋");
        pattern.params.append(paramItem);

        patterns.append(pattern); // 添加一个花样
    }


    //14-梅花螺旋长短毛
    {
        PatternItem pattern =
        {
            QObject::tr("14-梅花螺旋长短毛"),
            {QString(":/images/patterns/Disp14.bmp"), QString(":/images/patterns/Pic14.bmp")}
        };

        PatternParamItem paramItem;

        paramItem.name = QObject::tr("孔距");
        paramItem.range[0] = 0.001;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(120, 190);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("长毛条数(图为5条)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(100, 280);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("短毛条数(图为6条)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(500, 40);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("总长度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(490, 200);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("螺旋角度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(180, 10);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方向");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(590, 320);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList() << QObject::tr("先Z后X") << QObject::tr("先X后Z");
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("分组数");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(590, 260);
        paramItem.width = 50;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方式");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(590, 290);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList()<< QObject::tr("顺旋") << QObject::tr("逆旋");
        pattern.params.append(paramItem);

        patterns.append(pattern); // 添加一个花样
    }
    //15-圆 盘 孔
    {
        PatternItem pattern =
        {
            QObject::tr("15-圆 盘 孔"),
            {QString(":/images/patterns/Disp15.bmp"), QString(":/images/patterns/Pic15.bmp")}
        };

        PatternParamItem paramItem;

        paramItem.name = QObject::tr("孔距");
        paramItem.range[0] = 0.001;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(90, 295);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("圈数(图4圈)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(60, 200);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("内半径");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(550, 60);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("外半径");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(580, 180);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方向");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(560, 320);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList()<< QObject::tr("先里后外") << QObject::tr("先外后里");
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("坐标");
        paramItem.range[0] = 0;
        paramItem.range[1] = 3;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(560, 270);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList()<< QObject::tr("X-Y") << QObject::tr("X-Z") << QObject::tr("X-A")<< QObject::tr("X-B")<<QObject::tr("Y-Z");
        pattern.params.append(paramItem);

        patterns.append(pattern); // 添加一个花样
    }

    //16-特殊梅花人形孔
    {
        PatternItem pattern =
        {
            QObject::tr("16-特殊梅花人形孔"),
            {QString(":/images/patterns/Disp16.bmp"), QString(":/images/patterns/Pic16.bmp")}
        };

        PatternParamItem paramItem;

        paramItem.name = QObject::tr("分组数(图2组)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_U32;
        paramItem.position = QPoint(570, 140);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("组行数(图2行)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(570, 40);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("每圈孔数(图6孔)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(80, 370);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("总长度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(200, 280);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("角度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(5, 230);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);



        paramItem.name = QObject::tr("圈距离");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(370, 20);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方式");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(590, 260);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList()<< QObject::tr("顺旋") << QObject::tr("逆旋");
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("前置圈数(2圈)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(360, 320);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        patterns.append(pattern); // 添加一个花样
    }

    //17-普通方孔(X-Z)
    {
        PatternItem pattern =
        {
            QObject::tr("17-普通方孔(X-Z)"),
            {QString(":/images/patterns/Disp17.bmp"), QString(":/images/patterns/Pic17.bmp")}
        };

        PatternParamItem paramItem;

        paramItem.name = QObject::tr("X方向孔数");
        paramItem.range[0] = 1;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_U32;
        paramItem.position = QPoint(270, 10);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("Z方向孔数");
        paramItem.range[0] = 1;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_U32;
        paramItem.position = QPoint(550, 200);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("X方向长度");
        paramItem.range[0] = 0.001;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(270, 390);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("Z方向长度");
        paramItem.range[0] = 0.001;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(70, 200);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方向");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(590, 320);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList() << QObject::tr("先Z后X") << QObject::tr("先X后Z");
        pattern.params.append(paramItem);

        patterns.append(pattern); // 添加一个花样
    }

    //18-梅 花 孔(X-Z)
    {
        PatternItem pattern =
        {
            QObject::tr("18-梅 花 孔(X-Z)"),
            {QString(":/images/patterns/Disp18.bmp"), QString(":/images/patterns/Pic18.bmp")}
        };

        PatternParamItem paramItem;

        paramItem.name = QObject::tr("X方向孔数(图11)");
        paramItem.range[0] = 1;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_U32;
        paramItem.position = QPoint(270, 10);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("Z方向孔数(图9)");
        paramItem.range[0] = 1;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_U32;
        paramItem.position = QPoint(550, 200);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("X方向长度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(270, 390);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("Z方向长度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(70, 200);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方向");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(590, 320);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList() << QObject::tr("先Z后X") << QObject::tr("先X后Z");
        pattern.params.append(paramItem);


        patterns.append(pattern); // 添加一个花样
    }

    //19-圆盘孔(X-Y)
    {
        PatternItem pattern =
        {
            QObject::tr("19-圆盘孔(X-Y)"),
            {QString(":/images/patterns/Disp19.bmp"), QString(":/images/patterns/Pic19.bmp")}
        };

        PatternParamItem paramItem;

        paramItem.name = QObject::tr("孔距");
        paramItem.range[0] = 0.001;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(90, 295);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("圈数(图4圈)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(60, 200);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("内半径");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(550, 60);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("外半径");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(580, 180);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方向");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(560, 320);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList()<< QObject::tr("先里后外") << QObject::tr("先外后里");
        pattern.params.append(paramItem);

        patterns.append(pattern); // 添加一个花样
    }


    //20-圆盘孔(X-A)
    {
        PatternItem pattern =
        {
            QObject::tr("20-圆盘孔(X-A)"),
            {QString(":/images/patterns/Disp20.bmp"), QString(":/images/patterns/Pic20.bmp")}
        };

        PatternParamItem paramItem;

        paramItem.name = QObject::tr("孔距");
        paramItem.range[0] = 0.001;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(90, 295);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("圈数(图4圈)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(60, 200);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("内半径");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(550, 60);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("外半径");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(580, 180);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方向");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(560, 320);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList()<< QObject::tr("先里后外") << QObject::tr("先外后里");
        pattern.params.append(paramItem);

        patterns.append(pattern); // 添加一个花样
    }

    //21-圆盘孔(颜色)
    {
        PatternItem pattern =
        {
            QObject::tr("21-圆盘孔(颜色)"),
            {QString(":/images/patterns/Disp21.bmp"), QString(":/images/patterns/Pic21.bmp")}
        };

        PatternParamItem paramItem;

        paramItem.name = QObject::tr("孔距");
        paramItem.range[0] = 0.001;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(90, 295);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("圈数(图4圈)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(60, 200);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("内半径");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(550, 60);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("外半径");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(580, 180);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("毛皮比(毛)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_U32;
        paramItem.position = QPoint(50, 150);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("毛皮比(皮)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_U32;
        paramItem.position = QPoint(50, 250);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方向");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(560, 320);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList()<< QObject::tr("先里后外") << QObject::tr("先外后里");
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("坐标");
        paramItem.range[0] = 0;
        paramItem.range[1] = 3;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(560, 270);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList()<< QObject::tr("X-Y") << QObject::tr("X-Z") << QObject::tr("X-A")<< QObject::tr("X-B");
        pattern.params.append(paramItem);

        patterns.append(pattern); // 添加一个花样
    }
    //22-特殊方孔(X-Y)
    {
        PatternItem pattern =
        {
            QObject::tr("22-特殊方孔(X-Y)"),
            {QString(":/images/patterns/Disp22.bmp"), QString(":/images/patterns/Pic22.bmp")}
        };

        PatternParamItem paramItem;

        paramItem.name = QObject::tr("位置2X坐标");
        paramItem.range[0] = -99999;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(510, 10);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("位置2Y坐标");
        paramItem.range[0] = -99999;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(510, 50);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("X方向孔数");
        paramItem.range[0] = 1;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_U32;
        paramItem.position = QPoint(270, 360);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("Y方向孔数");
        paramItem.range[0] = 1;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_U32;
        paramItem.position = QPoint(60, 200);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);


        patterns.append(pattern); // 添加一个花样
    }
    //23-弧形滚筒(X-Z)
    {

        PatternItem pattern =
        {
            QObject::tr("23-弧形滚筒(X-Z)"),
            {QString(":/images/patterns/Disp16.bmp"), QString(":/images/patterns/Pic16.bmp")}
        };

        PatternParamItem paramItem;

        paramItem.name = QObject::tr("分组数(图2组)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_U32;
        paramItem.position = QPoint(570, 140);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("组行数(图2行)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(570, 40);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("每圈孔数(图6孔)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(80, 370);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("总长度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(200, 280);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("弧形高度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(5, 230);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("圈距离");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(370, 20);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方式");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(590, 260);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList()<< QObject::tr("顺旋") << QObject::tr("逆旋");
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("前置圈数(2圈)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(360, 320);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        patterns.append(pattern); // 添加一个花样
    }

    //24-角度圆盘孔
    {
        PatternItem pattern =
        {
            QObject::tr("24-角度圆盘孔"),
            {QString(":/images/patterns/Disp15.bmp"), QString(":/images/patterns/Pic15.bmp")}
        };

        PatternParamItem paramItem;

        paramItem.name = QObject::tr("孔距");
        paramItem.range[0] = 0.001;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(90, 295);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("圈数(图4圈)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(60, 200);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("内半径");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(550, 60);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("外半径");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(560, 180);
        paramItem.width = 80;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方向");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(560, 320);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList()<< QObject::tr("先里后外") << QObject::tr("先外后里");
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("坐标");
        paramItem.range[0] = 0;
        paramItem.range[1] = 3;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(560, 270);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList()<< QObject::tr("X-Y") << QObject::tr("X-Z") <<QObject::tr("Y-Z");
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("分组");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(60, 150);
        paramItem.width = 50;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("角度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 360;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(60, 100);
        paramItem.width = 50;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

//        paramItem.name = QObject::tr("摇摆");
//        paramItem.range[0] = 0;
//        paramItem.range[1] = 6;
//        paramItem.dataType = TYPE_StringList;
//        paramItem.position = QPoint(60, 50);
//        paramItem.width = 100;
//        paramItem.isHorizontal = false;
//        paramItem.stringList = QStringList()<< QObject::tr("Z") << QObject::tr("A") << QObject::tr("B");
//        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("距离");
        paramItem.range[0] = 0;
        paramItem.range[1] = 9999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(560, 220);
        paramItem.width = 50;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        patterns.append(pattern); // 添加一个花样
    }
    //25-角度圆盘孔
    {
        PatternItem pattern =
        {
            QObject::tr("25-角度圆盘孔(跳色)"),
            {QString(":/images/patterns/Disp23.bmp"), QString(":/images/patterns/Pic23.bmp")}
        };

        PatternParamItem paramItem;

        paramItem.name = QObject::tr("孔距");
        paramItem.range[0] = 0.001;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(90, 295);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("圈数(图4圈)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(60, 200);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("内半径");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(550, 60);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("外半径");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(560, 180);
        paramItem.width = 80;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方向");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(560, 320);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList()<< QObject::tr("先里后外") << QObject::tr("先外后里");
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("坐标");
        paramItem.range[0] = 0;
        paramItem.range[1] = 3;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(560, 270);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList()<< QObject::tr("X-Y") << QObject::tr("X-Z") <<QObject::tr("Y-Z");
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("分组");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(60, 150);
        paramItem.width = 50;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("角度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 360;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(60, 100);
        paramItem.width = 50;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

//        paramItem.name = QObject::tr("摇摆");
//        paramItem.range[0] = 0;
//        paramItem.range[1] = 6;
//        paramItem.dataType = TYPE_StringList;
//        paramItem.position = QPoint(60, 50);
//        paramItem.width = 100;
//        paramItem.isHorizontal = false;
//        paramItem.stringList = QStringList()<< QObject::tr("Z") << QObject::tr("A") << QObject::tr("B");
//        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("距离");
        paramItem.range[0] = 0;
        paramItem.range[1] = 9999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(560, 220);
        paramItem.width = 50;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        patterns.append(pattern); // 添加一个花样
    }
	
	//26-圆盘孔(三色)
    {
        PatternItem pattern =
        {
            QObject::tr("26-圆盘孔(三色)"),
            {QString(":/images/patterns/Disp15.bmp"), QString(":/images/patterns/Pic15.bmp")}
        };

        PatternParamItem paramItem;

        paramItem.name = QObject::tr("孔距");
        paramItem.range[0] = 0.001;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(90, 295);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("圈数(图4圈)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(60, 200);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("内半径");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(550, 60);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("外半径");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(580, 180);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方向");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(560, 320);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList()<< QObject::tr("先里后外") << QObject::tr("先外后里");
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("坐标");
        paramItem.range[0] = 0;
        paramItem.range[1] = 3;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(560, 270);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList()<< QObject::tr("X-Y") << QObject::tr("X-Z") << QObject::tr("X-A")<< QObject::tr("X-B");
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("毛数类型");
        paramItem.range[0] = 0;
        paramItem.range[1] = 3;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(90, 340);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList()<< QObject::tr("AABC") << QObject::tr("ABBC") << QObject::tr("ABCC");

        pattern.params.append(paramItem);

        patterns.append(pattern); // 添加一个花样
    }


    //25-角度圆盘孔
    {
        PatternItem pattern =
        {
            QObject::tr("25-角度圆盘孔(M:N)"),
            {QString(":/images/patterns/Disp23.bmp"), QString(":/images/patterns/Pic23.bmp")}
        };

        PatternParamItem paramItem;

        paramItem.name = QObject::tr("孔距");
        paramItem.range[0] = 0.001;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(90, 295);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("圈数(图4圈)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(60, 200);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("内半径");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(550, 60);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("外半径");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(560, 180);
        paramItem.width = 80;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方向");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(560, 320);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList()<< QObject::tr("先里后外") << QObject::tr("先外后里");
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("坐标");
        paramItem.range[0] = 0;
        paramItem.range[1] = 3;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(560, 270);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList()<< QObject::tr("X-Y") << QObject::tr("X-Z") <<QObject::tr("Y-Z");
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("分组");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(60, 150);
        paramItem.width = 50;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("角度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 360;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(60, 100);
        paramItem.width = 50;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

//        paramItem.name = QObject::tr("摇摆");
//        paramItem.range[0] = 0;
//        paramItem.range[1] = 6;
//        paramItem.dataType = TYPE_StringList;
//        paramItem.position = QPoint(60, 50);
//        paramItem.width = 100;
//        paramItem.isHorizontal = false;
//        paramItem.stringList = QStringList()<< QObject::tr("Z") << QObject::tr("A") << QObject::tr("B");
//        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("距离");
        paramItem.range[0] = 0;
        paramItem.range[1] = 9999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(560, 220);
        paramItem.width = 50;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("毛皮比(毛)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_U32;
        paramItem.position = QPoint(80, 340);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("毛皮比(皮)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_U32;
        paramItem.position = QPoint(80, 380);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        patterns.append(pattern); // 添加一个花样
    }
    //28-12扇6角度角度圆盘孔
    {
        PatternItem pattern =
        {
            QObject::tr("28-12扇6角度角度圆盘孔"),
            {QString(":/images/patterns/Disp23.bmp"), QString(":/images/patterns/Pic23.bmp")}
        };

        PatternParamItem paramItem;

        paramItem.name = QObject::tr("孔距");
        paramItem.range[0] = 0.001;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(90, 295);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("圈数(图4圈)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(60, 200);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("内半径");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(550, 60);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("外半径");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(560, 180);
        paramItem.width = 80;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("方向");
        paramItem.range[0] = 0;
        paramItem.range[1] = 1;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(560, 320);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList()<< QObject::tr("先里后外") << QObject::tr("先外后里");
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("坐标");
        paramItem.range[0] = 0;
        paramItem.range[1] = 3;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(560, 270);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList()<< QObject::tr("X-Y") << QObject::tr("X-Z") <<QObject::tr("Y-Z");
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("分组");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(60, 150);
        paramItem.width = 50;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("角度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 360;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(60, 100);
        paramItem.width = 50;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

//        paramItem.name = QObject::tr("摇摆");
//        paramItem.range[0] = 0;
//        paramItem.range[1] = 6;
//        paramItem.dataType = TYPE_StringList;
//        paramItem.position = QPoint(60, 50);
//        paramItem.width = 100;
//        paramItem.isHorizontal = false;
//        paramItem.stringList = QStringList()<< QObject::tr("Z") << QObject::tr("A") << QObject::tr("B");
//        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("距离");
        paramItem.range[0] = 0;
        paramItem.range[1] = 9999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(560, 220);
        paramItem.width = 50;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        patterns.append(pattern); // 添加一个花样
    }
    //29-圆盘斜孔(三色)
    {
        PatternItem pattern =
        {
            QObject::tr("29-圆盘斜孔(三色)"),
            {QString(":/images/patterns/Disp15.bmp"), QString(":/images/patterns/Pic15.bmp")}
        };

        PatternParamItem paramItem;

        paramItem.name = QObject::tr("孔距");
        paramItem.range[0] = 0.001;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(90, 295);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("圈数(图4圈)");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(60, 200);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("内半径");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(550, 60);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("外半径");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(580, 180);
        paramItem.width = 100;
        paramItem.isHorizontal = false;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("坐标");
        paramItem.range[0] = 0;
        paramItem.range[1] = 3;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(560, 270);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList()<< QObject::tr("X-Y") << QObject::tr("X-Z") << QObject::tr("X-A")<< QObject::tr("X-B");
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("毛数类型");
        paramItem.range[0] = 0;
        paramItem.range[1] = 3;
        paramItem.dataType = TYPE_StringList;
        paramItem.position = QPoint(90, 340);
        paramItem.width = 100;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList()<< QObject::tr("AABC") << QObject::tr("ABC");
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("分组");
        paramItem.range[0] = 0;
        paramItem.range[1] = 99999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(60, 150);
        paramItem.width = 50;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("角度");
        paramItem.range[0] = 0;
        paramItem.range[1] = 360;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(60, 100);
        paramItem.width = 50;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        paramItem.name = QObject::tr("距离");
        paramItem.range[0] = 0;
        paramItem.range[1] = 9999;
        paramItem.dataType = TYPE_F32;
        paramItem.position = QPoint(560, 220);
        paramItem.width = 50;
        paramItem.isHorizontal = true;
        paramItem.stringList = QStringList();
        pattern.params.append(paramItem);

        patterns.append(pattern); // 添加一个花样
    }

    //-----------------------------------------------
}

QString DataAdapter_Patterns::getParamName(int offset) const
{
    return patterns.at(offset).name;
}

void DataAdapter_Patterns::setData(int offset, const QVariant& val)
{
    auto valueList = val.value<QList<float>>();
    PatternItem& pattern = patterns[offset];

    for(auto i = 0; i < valueList.size(); ++i)
    {
        pattern.params[i].value = valueList.at(i);
    }
}

QVariant DataAdapter_Patterns::getData(int offset)
{
    QVariant v;
    v.setValue(patterns.at(offset));
    return v;
}

} // namespace Adapters
