
#include "qwidgetdiagnosedog.h"
#include "data/dog.h"
#include "data/globaldata.h"
#include "data/utils.h"
#include <QGridLayout>
#include <QLabel>
#include "data/uistyles.h"
QWidgetDiagnoseDog::QWidgetDiagnoseDog(QWidget* parent,Qt::WindowFlags f):
    QDialog(parent, f)
{

}
QWidgetDiagnoseDog::~QWidgetDiagnoseDog()
{

}

void QWidgetDiagnoseDog::onModbusQueryCallbackEnd(bool isOk, void* flag)
{

}
