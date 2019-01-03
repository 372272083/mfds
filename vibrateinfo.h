#ifndef VIBRATEINFO_H
#define VIBRATEINFO_H

#include "queryinfo.h"

class VibrateInfo : public QueryInfo
{
    Q_OBJECT
public:
    explicit VibrateInfo(QObject *parent = 0);

    float vibrate_e;
    float speed_e;

    virtual QString toString();

signals:

public slots:
};

#endif // VIBRATEINFO_H
