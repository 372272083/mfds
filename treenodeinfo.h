#ifndef TREENODEINFO_H
#define TREENODEINFO_H

#include <QObject>
#include <QString>

#include "constants.h"

class TreeNodeInfo : public QObject
{
    Q_OBJECT
public:
    TreeNodeInfo();

    int id;
    TREENODETYPE type;
    QString title;
    int pid;
};

#endif // TREENODEINFO_H
