#ifndef DEVICEPIPEINFO_H
#define DEVICEPIPEINFO_H

#include <QObject>

class DevicePipeInfo : public QObject
{
    Q_OBJECT
public:
    explicit DevicePipeInfo(QObject *parent = 0);

    void setDCode(QString dcode);
    void setDName(QString dcode);
    void setDType(QString dcode);
    void setMCode(QString dcode);
    void setPipeCode(QString pipecode);
    void setPipeName(QString name);

    QString getDCode();
    QString getDName();
    QString getDType();
    QString getPipeCode();
    QString getPipeName();
    QString getMCode();

private:
    QString dcode;
    QString dname;
    QString dtype;
    QString pipecode;
    QString pipename;
    QString mcode;

signals:

public slots:

};

#endif // DEVICEPIPEINFO_H
