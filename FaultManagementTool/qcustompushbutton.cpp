#include "qcustompushbutton.h"

QCustomPushButton::QCustomPushButton()
{

}

void QCustomPushButton::emitClickedData() {
    emit clickedData(field);
}
