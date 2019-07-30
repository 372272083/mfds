#ifndef TEMPERATUREINFO_H
#define TEMPERATUREINFO_H

#include "queryinfo.h"

class TemperatureInfo : public QueryInfo
{
    Q_OBJECT
public:
    explicit TemperatureInfo(QObject *parent = 0);
    TemperatureInfo(const TemperatureInfo&);
    TemperatureInfo& operator=(const TemperatureInfo &c);

    float temp;

    virtual QString toString();

signals:

public slots:
};

#endif // TEMPERATUREINFO_H
