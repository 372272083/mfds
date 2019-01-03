#ifndef COMSETTING_H
#define COMSETTING_H

#include <QDialog>
#include <QMap>

class DeviceLinkInfo;
class QTabWidget;

class ComSetting : public QDialog
{
    Q_OBJECT
public:
    explicit ComSetting(QWidget *parent = 0);

    void setupDevice(QMap<QString,DeviceLinkInfo*>);
protected:
    void resizeEvent(QResizeEvent* event);
private:
    QTabWidget* mainWidget;
    QMap<QString,DeviceLinkInfo*> deviceInfo;
    QMap<QString,QWidget*> controls;
signals:

public slots:
    void ok();
    void cancel();
};

#endif // COMSETTING_H
