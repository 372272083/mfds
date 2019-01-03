#ifndef DEVICEPIPESELECTWIDGET_H
#define DEVICEPIPESELECTWIDGET_H

#include <QWidget>
class DevicePipeInfo;
class QHBoxLayout;
class HeaderItemWidget;
class QLabel;
class QColor;

class DevicePipeSelectWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DevicePipeSelectWidget(QWidget *parent = 0);

    void setupPipes(QList<DevicePipeInfo *>);

private:
    QHBoxLayout* pLayout;
    QList<HeaderItemWidget*> buttonItems;
    QLabel *banner;
    QColor buttonOriginColor;

signals:
    void pipeChanged(QString,QString,QString,QString);

public slots:
    void pipeToggle();
};

#endif // DEVICEPIPESELECTWIDGET_H
