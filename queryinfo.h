#ifndef QUERYINFO_H
#define QUERYINFO_H

#include <QObject>

class QueryInfo : public QObject
{
    Q_OBJECT
public:
    explicit QueryInfo(QObject *parent = 0);

    int id;
    QString dcode;
    QString mcode;
    QString pipe;
    QString rksj;

    int info_type;

    virtual QString toString();
signals:

public slots:
};

#endif // QUERYINFO_H
