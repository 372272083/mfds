#ifndef FAULTDETAILRESULTDIALOG_H
#define FAULTDETAILRESULTDIALOG_H

#include <QDialog>
#include <QMap>
#include "faultinfo.h"

class QTableView;
class QStandardItemModel;

class FaultDetailResultDialog : public QDialog
{
public:
    explicit FaultDetailResultDialog(QDialog *parent = 0);

    void setDetailInfo(QString,int);
private:
    QTableView* tableView;
    QStandardItemModel* model;

    int faultType;
};

#endif // FAULTDETAILRESULTDIALOG_H
