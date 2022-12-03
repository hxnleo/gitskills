#include "tabbutton.h"
#include "data/globaldata.h"
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>

TabButton::TabButton(QWidget *parent) : QWidget(parent),drawhigh(50),index(0),start(0),
    m_isMousePressed(false),btnumber(0)
{
    TAB_BUTTON_NAMES =
    {
        tr("系统"),
        tr("教导"),
        tr("参数"),
        tr("文件"),
        tr("诊断"),
    };
}

void TabButton::init(bool start, int number)
{//初始化
    if(start)
      width = GlobalData::widthRate*5.0/7;
    bodywith = drawhigh+drawhigh;//中间
    shadowwith = drawhigh*(2.0/5);//阴影
    drawwtidh = drawhigh+bodywith+drawhigh/4;//总宽度
    movewith = drawwtidh-shadowwith; //移动距离
    btnumber = number;
}

void TabButton::paintEvent(QPaintEvent *)
{
    if(width<drawwtidh+4*movewith||height()<=drawhigh)
    {//得到最合适的高度
        for(;;)
        {
            if(width>=drawwtidh+4*movewith&&height()>drawhigh)
            {
                break;
            }
            drawhigh--;
            init();
            //qDebug()<<drawwtidh+4*movewith;
        }
    }
    else
        init();
    if(GlobalData::screensize == SEVEN)
    {
        start = 2*(width-(drawwtidh+4*movewith));//得到顶格平移距离
    }
    else
        start = (width-(drawwtidh+4*movewith));//得到顶格平移距离
    for(int i=btnumber-1;i>=0;i--)
    {
        if(i!=index)
        {
            drawBady(i*(movewith),i);//画五个梯形按钮
            drawShaw(i*(movewith));
        }
    }
    drawBady(index*(movewith),index);//按下去的梯形按钮最后画
    drawShaw(index*(movewith));
    //出书宽和高
    //qDebug()<<width()<<height();
}
//tansform 为平移系数
void TabButton::drawBady(int tansform, int num)
{// 图形主体
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);//反走样
    // 设置画笔颜色
    //painter.setPen(QPen(Qt::black, 1));
    painter.setPen(QPen(QColor(168,168,168)));//轮廓
    //图形主体 x右边增大，y右边增大
    QPolygon elepolygon0;//保存点的个数
    elepolygon0<<QPoint(tansform+0, height());
    elepolygon0<<QPoint(tansform+drawhigh, height()-drawhigh);
    elepolygon0<<QPoint(tansform+drawhigh+bodywith,  height()-drawhigh);
    elepolygon0<<QPoint(tansform+drawhigh+bodywith, height()-drawhigh/4);
    elepolygon0<<QPoint(tansform+drawhigh+bodywith+drawhigh/4, height());
    //elepolygon0<<QPoint(tansform+drawhigh+bodywith+shadowwith, height());
    if(num == index)
    {
        painter.setBrush(QColor(0,0,160));
    }
    else
        painter.setBrush(QColor(168,168,168));//设置画刷填充
    painter.drawPolygon(elepolygon0);//设置范围点

    //描边
    painter.setPen(QPen(Qt::white,2));
    painter.drawLine(QPointF(tansform+0, height()), QPointF(tansform+drawhigh, height()-drawhigh-2));//开头的45°斜线

    QFont font;
    int texttansfrom;
    if(GlobalData::screensize == SEVEN)
    {
        font.setPointSize(14);
        texttansfrom = drawhigh-3;
    }
    else if(GlobalData::screensize == TEN)
    {
        texttansfrom = drawhigh;
    }
    else
    {
        texttansfrom = drawhigh-5;
    }
    painter.setFont(font);

    if(num == index)
    {
        painter.setPen(QPen(QColor(248,180,0),3));
        painter.drawLine(QPointF(tansform+drawhigh+2, height()-drawhigh-2), QPointF(tansform+drawhigh+bodywith, height()-drawhigh-2));//中间的横线

        painter.setPen(QPen(QColor(Qt::white),1));
        painter.drawText(tansform+texttansfrom+texttansfrom/3,height()-texttansfrom/3, TAB_BUTTON_NAMES.at(num));
    }
    else
    {
        painter.drawLine(QPointF(tansform+drawhigh, height()-drawhigh-2), QPointF(tansform+drawhigh+bodywith, height()-drawhigh-2));//中间的横线
        painter.setPen(QPen(QColor(Qt::black),1));
        painter.drawText(tansform+texttansfrom+texttansfrom/3,height()-texttansfrom/3, TAB_BUTTON_NAMES.at(num));
    }
    //    painter.drawLine(QPointF(tansform+drawhigh+bodywith,  height()-drawhigh), QPointF(tansform+drawhigh+bodywith, height()-drawhigh/4));//折线
    //    painter.drawLine(QPointF(tansform+drawhigh+bodywith, height()-drawhigh/4), QPointF(tansform+drawhigh+bodywith+drawhigh/4, height()));//小斜线
    //    painter.drawLine(QPointF(tansform+0, height()), QPointF(tansform+drawhigh+bodywith+shadowwith, height()));//下面封口线
    //文本绘制
    // 设置画笔颜色
}

void TabButton::drawShaw(int tansform)
{//图形阴影
    QPainter painter(this);
    //tansform+=start;
    //painter.setPen(Qt::SolidLine);
    painter.setRenderHint(QPainter::Antialiasing, true);//反走样
    // 设置画笔颜色
    painter.setPen(QPen(Qt::NoPen));

    QPolygon elepolygon1;//保存点的个数
    QLinearGradient gradient1(tansform+drawhigh+bodywith,height(),tansform+drawhigh+bodywith+shadowwith,height());//渐变范围和开始点结束点
    elepolygon1<<QPoint(tansform+drawhigh+bodywith,  height()-drawhigh);
    elepolygon1<<QPoint(tansform+drawhigh+bodywith, height()-drawhigh/4);
    elepolygon1<<QPoint(tansform+drawhigh+bodywith+drawhigh/4,  height());
    elepolygon1<<QPoint(tansform+drawhigh+bodywith+shadowwith,  height());
    elepolygon1<<QPoint(tansform+drawhigh+bodywith+shadowwith,  height()-drawhigh-2);
    elepolygon1<<QPoint(tansform+drawhigh+bodywith+2,  height()-drawhigh-2);
    painter.setBrush(gradient1);//设置画笔
    gradient1.setColorAt(0, QColor(0,0,0));
    gradient1.setColorAt(0.5, QColor(0,0,0));
    gradient1.setColorAt(1, QColor(168,168,168));
    painter.drawPolygon(elepolygon1);//设置范围点

    painter.setPen(QPen(QColor(168,168,168), 1));
    painter.drawLine(QPointF(tansform+drawhigh+bodywith+shadowwith-1,height()-drawhigh-1), QPointF(tansform+drawhigh+bodywith+shadowwith-1,height()));//x右边增大，y右边增大
    //    painter.drawLine(QPointF(tansform+drawhigh+bodywith+shadowwith,height()-drawhigh), QPointF(tansform+drawhigh+bodywith+shadowwith,height()));//x右边增大，y右边增大
}

void TabButton::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug()<<event->pos();
}
void TabButton::mousePressEvent(QMouseEvent *event)
{
    QPoint mousePressPoint = event->pos();
    for(int i = 0;i<btnumber;i++)
    {//判断区域
        if(mousePressPoint.y()>=10&&mousePressPoint.y()<=50)
        {
            if(mousePressPoint.x()>=start+i*(movewith)&&mousePressPoint.x()<=start+i*(movewith)+drawhigh+bodywith+2)
            {
                index = i;
                m_isMousePressed = true;
                emit indexchange(index);
                update();
                //qDebug()<<i<<m_isMousePressed;
            }
        }
    }
    QWidget::mousePressEvent(event);
}

void TabButton::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if (m_isMousePressed)
    {
        m_isMousePressed = false;
//        emit indexchange(index);
//        update();
    }
    QWidget::mouseReleaseEvent(event);
}
