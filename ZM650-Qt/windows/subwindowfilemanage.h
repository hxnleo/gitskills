/*
 * subwindowfilemanage.h
 *
 * 文件管理界面
 *
 * Created on: 2019年 02月 13日 星期三 18:26:02 CST
 * Author: lixingcong
 */

#ifndef SUBWINDOWFILEMANAGE_H
#define SUBWINDOWFILEMANAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QThread>
#include <QFileInfoList>
#include <QLineEdit>
#include "data/adt_datatype_qt.h"

#define FM_MAX_CHAR_DIRNAME 256

class QLabel;
class QListWidgetDragScroll;
class QListWidgetItem;
class QThreadFileManage;
class QDir;
class QProgressDialog;
class QLabelMarquee;
class QTimer;

class SubWindowFileManage : public QWidget
{
    Q_OBJECT
public:
    struct DIR_FILE_INFO
    {
        char file_name[24];		//文件名占用低12个地址
        quint32 file_type_size;	//类型bit31与大小bit30~bit0
        quint16 file_date;		//文件修改日期与FATFS定义相同
        quint16 file_time;		//文件修改时间
    };

    typedef struct {
        FP32 x;
        FP32 y;
        FP32 z;
        FP32 a;
        FP32 b;
        FP32 cr;
    }tLANSI_BIN;


    enum ActionType
    {
        Action_ListDirFiles,
        Action_NewFile,
        Action_Copy,
        Action_PasteRemoteToRemote,
        Action_PasteLocalToRemote,
        Action_PasteRemoteToLocal,
        Action_PasteLocalToLocal,
        Action_RefreshLocalDir,
        Action_Delete,
        Action_OpenWorkfile,
        Action_UpgradeA9Rom,
        Action_None
    };

    enum RootPathType
    {
        Root_System,
        Root_UserProg,
        Root_HMI_UDisk,
        Root_None
    };
    enum Replace
    {
        YSE,
        NOT,
        YES_ALL,
    };


    struct CopySourceType
    {
        RootPathType root;
        QString fullpath; // 完整的路径与文件名
        QString filename; // 仅文件名
        bool isValid;
    };

public:
    explicit SubWindowFileManage(QWidget* parent = nullptr);
    ~SubWindowFileManage();

signals:
    void upRate(int);
    void dismissProgressDialog();
    void resetScrollPostion();

protected slots:
    void _onClickedBottomButton(int btn_id);
    void onItemClicked(QListWidgetItem* item);
    void onItemDoubleClicked(QListWidgetItem* item);
    void onWorkerThreadOperationDone(bool isOk);
    void showRootPath();
    void onClickedRefreshDir();
    void levelUp();
    void onDialogAcceptedNewFile();
    void onDialogAcceptedNewFolder();
    void onTimeoutSingleClicked();
    void onOK();
    void onNot();
    void DelLanSiFile();
    void AnalyzeCSV();
    int  checkFile(QString remot,QFileInfoList lock);
    int  checkFile(QString remot,QList<DIR_FILE_INFO> lock);

protected:
    QVBoxLayout* layout_v;
    QLabel* labelCurrentDir;
    QLabel* labelFileInfo;
    QLabelMarquee* labelFilename;
    QListWidgetDragScroll* listWidget;
    QThreadFileManage* workerThread;
    QList<DIR_FILE_INFO> listFileInfo;//存储下位机当前文件
    ActionType currentAction;
    RootPathType currentRootPath;
    QTimer* timerSingleClick;
    bool isSingleClick;
    QString orignfile;//U盘原始csv文件路径

    char dirname[FM_MAX_CHAR_DIRNAME];
    char dirnameNew[FM_MAX_CHAR_DIRNAME]; // 打开新的目录
    char filenameRemote[FM_MAX_CHAR_DIRNAME]; // 远端完整文件名
    QString filenameLocal; // 本机完整文件名

    //本机就是上位机 远端
    char filenameNew[24]; // 新建文件名
    char filenamePaste[24]; // 远端粘贴文件名

    int pathDepth; // 文件夹深度，当0为主目录
    QString filenameTemp; // 用户输入文件名，或者是删除的文件名，新文件夹名
    QDir* localDir;
    QFileInfoList localFileList;//存储上位机当前文件
    CopySourceType copySource;
    QString strlast;

    //static const QStringList rootPathString;
    QStringList rootPathString;
    bool in_paste; //复制粘贴过程中
    int bytes_temp;//存储生成CSV源文件大小


protected: // functions
    bool isARemoteFile(const DIR_FILE_INFO& dirInfo);
    void refreshLocalDir(bool isDrive = false);
    QProgressDialog* createLoadingDialog(QString str="");
    void upgradeQtApp(const QString& filenameNewProgram);
    void startFileIntoUser();
    void onWaitFileFinish();
    INT16U ReadLanSiFile(QString filename);
    INT16U WriteLanSiFile(QString copyfile, QString orinefile);
	INT16U ReadCSVFile(QString filename,int StarAddr=0);
    INT16U WriteCSVFile(QString copyfile, QString orinefile,int addr=0);

};

class QThreadFileManage: public QThread
{
    Q_OBJECT

public:
    enum WorkStep
    {
        WS_WRITE_DIR,
        WS_READ_FILE_COUNT,
        WS_READ_FILE_CONTENT,
        WS_NEWFILE,
        WS_COPY,
        WS_PASTE_REMOTE_TO_REMOTE,
        WS_PASTE_LOCAL_TO_REMOTE,
        WS_PASTE_REMOTE_TO_LOCAL,
        WS_PASTE_LOCAL_TO_LOCAL,
        WS_OPEN_WORKFILE,
        WS_DELETE,
        WS_UPGRADE_COPY_TO_REMOTE,
        WS_UPGRADE_START,
        WS_UPGRADE_CHECK_IF_FINISH,
        WS_DONE
    };
public:
    QThreadFileManage(QList<SubWindowFileManage::DIR_FILE_INFO>* returnInfo, QObject* parent = nullptr);
    void run() override;

    void setDir(char* dir);
    void setNewFile(char* filename);
    void setCopy(char* filename);
    void setPasteRemoteToRemote(char* filename);
    void setPasteLocalToRemote(char* remote, QString* local);
    void setPasteRemoteToLocal(char* remote, QString* local);
    void setPasteLocalToLocal(QString* src, QString* dest);
    void setDelete(char* filename);
    void setWorkfile(char* filename);
    void setUpgrade(char* remote, QString* A9RomFile);

    void setStopFlag()
    {
        isStopped = true;
    }
    bool sendok;
    int workstep;
signals:
    void operationDone(bool isOk);
    void updateFileProgess(int progress);

protected slots:
    void _onNotifyFileProgress(int progress);

protected:

    char* filenameOrDir;//当前焦点文件名
    char* filenameRemote;
    QString* filenameLocal;
    QString* filenameLocalToPaste;
    QList<SubWindowFileManage::DIR_FILE_INFO>* dirFileInfo;
    bool isStopped;


protected: // functions
    void _setWorkstepAndPath(char* path, int workstep);
    void _setWorkstepAndPath(char* remote, QString* local, int workstep);
};

#endif // SUBWINDOWFILEMANAGE_H
