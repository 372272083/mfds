#ifndef UDISK_H
#define UDISK_H
#include <QWidget>
#include <windows.h>
#include <QAbstractNativeEventFilter>
#include <dbt.h>
class uDisk : public QWidget, public QAbstractNativeEventFilter
{
    Q_OBJECT
protected:
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result);
signals:
    void sigUDiskCome(QString uDiskName);
    void sigUDiskRemove();
private:
    char FirstDriveFromMask(ULONG unitmask);
};
#endif // UDISK_H
