#ifndef QMYPUSHBUTTON_U0_H
#define QMYPUSHBUTTON_U0_H

#include<QPushButton>

class QMyPushButton_U0 : public QPushButton
{
    Q_OBJECT
public:
    explicit QMyPushButton_U0(QWidget *parent = nullptr);

    void setImage(QString imagepath_up="",QString imagepath_press="");
    void setPressOrUp(bool PressOrUp=false);
    void setActive(bool status);

signals:

public slots:


protected:
    void paintEvent(QPaintEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;

private:
    QString image_path_up;
    QString image_Path_press;
    bool PressOrUpStatus;

};

#endif // QMYPUSHBUTTON_U0_H
