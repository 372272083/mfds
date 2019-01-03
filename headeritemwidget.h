#ifndef HEADERITEMWIDGET_H
#define HEADERITEMWIDGET_H

#include <QPushButton>

class HeaderItemWidget : public QPushButton
{
    Q_OBJECT
public:
    HeaderItemWidget();

    QString motor;
    QString device;
    QString pipe;
    QString deviceType;
    QString deviceModel;
};

#endif // HEADERITEMWIDGET_H
