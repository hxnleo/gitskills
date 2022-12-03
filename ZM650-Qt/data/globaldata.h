/*
 * globaldata.h
 *
 * 全局变量
 *
 * Created on: 2019年 01月 21日 星期一 14:31:00 CST
 * Author: lixingcong
 */
#ifndef GLOBALDATA_H
#define GLOBALDATA_H

#include <QObject>
#include <public.h>
#include <Teach.h>
#include <data/dog.h>
#include <QApplication>
#include <QAbstractItemModel>

class QPushButtonsPage;
class ModbusQueue;
class QLabel;

#define EDIT_MAXNUM 16
#define SEVEN 0     //七寸屏
#define TEN 1       //十寸屏
#define FIFTEEN 2   //十五寸屏

#define MAX_PATTERN_ROW 50 //花样组数
#define MAX_PATTERN_COL 10  //花样结构体
class GlobalData: public QObject
{
    Q_OBJECT
protected:
    GlobalData(): QObject(nullptr) {}
    ~GlobalData()
    {
        instance = nullptr;
    }
public:
    enum
    {
        USER_LEVEL_L = 0,
        USER_LEVEL_M,
        USER_LEVEL_H,
        USER_LEVEL_MAX,
    };
    enum RootPathType
    {
        Root_System,
        Root_UserProg,
        Root_HMI_UDisk,
        Root_None
    };

    typedef struct
    {
        RootPathType root;
        QString fullpath; // 完整的路径与文件名
        QString filename; // 仅文件名
        bool isValid;
    }CopySourceType;

    static GlobalData* getInstance();//单例

#ifndef LIXINGCONG_DEMO_UI
    static QPushButtonsPage* pushButtonPage;
#endif
    static const QStringList axisName;
    static QStringList realaxisName;//实际轴名字
    static QStringList teachaxisName;//实际轴名字
    static int axis[16];//轴顺序
    static int teachaxis[16];//教导界面轴顺序
    static double TEACH_STEPS[];//教导步长
    static size_t stepIndex;
    static int totalAxisNum; // 当前控制器已启用的轴数，常见值：2~6
    static int teachdisplay;//叫界面显示的轴数
    static int maxAxisNum; // 最大轴数，值固定为6
    static ModbusQueue* modbusQueue;//Modebug接口
    static STRANSLATE_T realTimeInfo;//实时通信结构体
    static GlobalData* instance;
    static QString workfileName;//当前加工文件
    static QLabel* labelTopTab;
    static quint16 lastAlarmNumber;
    static QString editnum ;//显示输入的值
    static QList<quint32> storenum;//存储输入的值
    static int num;//显示输入的值的个数
    static bool infile;//是否处于文件系统
    static bool inparem;//是否处于参数目录
    //static QApplication* app;//方便切换语言
    static int slectetype;
    static int selectedAxisNum[2];
    static int selectedAxisDirection[2];
    // SubWindowRunStatus中的双击计数
    static quint32 doubleClickNextWidget;
    static QString renewEdit;
    static bool online;
    static int widthRate;
    static int highRate;
    static int dogflag;
    static const QString filenameProgramA9;
    static const QString filenameProgramQt;
    static const QString filenameProgramMotion;
    static int screensize;//屏幕尺寸 做兼容处理
    static bool ScreenCapture;//截屏
    static bool backZeroIcon;//需要显示界面标志
    static bool backZeroFlag;//归零图标标志
    static quint16 closebtn;//刹车是否使能
    static int brakenum;//刹车索引
    static int motornum;//变频器索引
    static int iostatus;//参数IO输入状态
    static int systemaxis;//系统轴数
	static bool iodisplay;//显示诊断IO端口
    static float Color1;//保存毛道值
    static float Color2;
    static float Color3;
    static QString PicturePath;//系统图片
    static QString FontPath;//系统图片
    static QString Debugtext;//系统调试
    static int fontindex;//字体返回ID
    static INT32U parambite;//32参数 0样式1 1样式2 2： 3圆盘样式：平板 圆盘
    static int paramauthority;//用户操作权限 0-操作员 1-调试员 2-超级用户
    static QString patternarry[50][10];//保存花样数组
    static quint16 language;//保存语言类型
    static bool DISABLETOUCH;//禁用触摸事件
    static int swithcolumn;//选中框索引

    static bool parampassword;//参数界面密码
    void widgetEnable(QWidget* w, bool authority = true);
    static bool DataAuthority(bool type);

signals:
    void updateRealtimeInfo();

public slots:
    void _updateRealtimeInfo()
    {
        emit updateRealtimeInfo();
    }
};

#endif // GLOBALDATA_H
