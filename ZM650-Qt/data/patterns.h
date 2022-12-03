/*
 * patterns.h
 *
 * 花样类，添加花样都在这里！
 *
 * Created on: 2019年 01月 26日 星期六 17:03:39 CST
 * Author: lixingcong
 */

#ifndef PATTERNS_H
#define PATTERNS_H

#include "data/utils.h"
#include "adapters.h"

Q_DECLARE_METATYPE(QList<float>)

namespace Adapters
{

// 某花样的单个参数
struct PatternParamItem
{
    QString name;
    double range[2]; // minVal maxVal
    DataType dataType;
    QPoint position; // x y
    int width;
    bool isHorizontal; // 是否文字与输入框齐平
    float value; // 值

    // 可选参数，当数据为StringList时候可用
    QStringList stringList;
};

// 一个花样
struct PatternItem
{
    QString name;
    QString resourceName[2]; // 0: 外层图片 1: 内层图片
    QList<PatternParamItem> params;
};

class DataAdapter_Patterns : public DataAdapterBase<PatternItem>
{
public:
    DataAdapter_Patterns();
    QString getParamName(int offset) const override;
    int getParamWidgetType(int offset) const override
    {
        return WidgetType_PATTERN;
    }
    int getDataType(int offset) const override
    {
        return TYPE_UNKNOWN;
    }
    bool getDataAuthority(int offset) const override{}

    void setData(int offset, const QVariant& val) override;
    QVariant getData(int offset) override;

    int getCount()const
    {
        return patterns.size();
    }

protected:
    QList<PatternItem> patterns;

};
} // namespace

Q_DECLARE_METATYPE(Adapters::PatternItem)

#endif // PATTERNS_H
