#ifndef TABBUTTON_H
#define TABBUTTON_H

#include <QWidget>
#include <QEvent>
class TabButton : public QWidget
{
    Q_OBJECT
public:
    explicit TabButton(QWidget *parent = nullptr);
    void init(bool start = false,int nuber = 5);
    int drawhigh;//图形的高度
    int drawwtidh;//图形的宽度
    int bodywith;//中间的宽度
    int shadowwith;//阴影的宽度
    int movewith;//图形的总宽度
    float width;//总长度
    int index;//按键索引
    int start;//起始位子
    bool m_isMousePressed;//按下标志位
    int btnumber;
private:
     // 初始化按钮;
    void initButton();
    // 绘制按钮;
    void paintEvent(QPaintEvent *);
    // 鼠标事件;
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
protected:
    void drawBady(int tansform = 0,int num = 0);//画主题部分
    void drawShaw(int tansform = 0);//画阴影部分
private:
    QList<QString> TAB_BUTTON_NAMES;
signals:
    void indexchange(int);
public slots:
};

#endif // TABBUTTON_H

