#include "faultdetailresultdialog.h"

#include <QTableView>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QHeaderView>

#include "globalvariable.h"

FaultDetailResultDialog::FaultDetailResultDialog(QDialog *parent) : QDialog(parent)
{
    model = new QStandardItemModel();
    tableView = new QTableView(this);
    QStringList labels = tr("Name,Threshold,Value,Ratio,IsFire").simplified().split(",");
    model->setHorizontalHeaderLabels(labels);

    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);

    QHeaderView *header = tableView->horizontalHeader();
    header->setStretchLastSection(true);

    tableView->setModel(model);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(tableView);

    this->setLayout(layout);

    this->setMinimumSize(700,400);
}

void FaultDetailResultDialog::setDetailInfo(QString mcode,int faultcode_l)
{
    faultType = faultcode_l;

    int index = 0;
    if(GlobalVariable::diagnose_result.contains(mcode))
    {
        QList<QString>::ConstIterator in_it;
        QMap<QString,FaultInfo> fis = GlobalVariable::diagnose_result[mcode];
        QList<QString> in_keys = fis.keys();

        for(in_it=in_keys.constBegin();in_it!=in_keys.constEnd();in_it++)
        {
            QString key = *in_it;
            FaultInfo fi = fis[key];
            double ratio = 0.0;
            switch (faultcode_l) {
            case 1:
                for(int n=0;n<2;n++)
                {
                    switch (n) {
                    case 0:
                        model->setItem(index, 0, new QStandardItem("RMS"));
                        break;
                    case 1:
                        model->setItem(index, 0, new QStandardItem("1ST"));
                        break;
                    default:
                        break;
                    }

                    model->setItem(index, 1, new QStandardItem(QString::number(fi.fault_detail_thd.diaUnbalance.inf[n],10,2)));
                    model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_detail_cur.diaUnbalance.inf[n],10,2)));
                    model->setItem(index, 3, new QStandardItem(QString::number(fi.fault_detail_cof.diaUnbalance.inf[n],10,2)));
                    if(fi.fault_detail_trigger.diaUnbalance.inf[0] > 0.5)
                    {
                        model->setItem(index, 4, new QStandardItem(tr("Yes")));
                    }
                    else
                    {
                        model->setItem(index, 4, new QStandardItem(tr("No")));
                    }
                    index++;
                }
                index--;
                break;
            case 5:
                for(int n=0;n<12;n++)
                {
                    switch (n) {
                    case 0:
                        model->setItem(index, 0, new QStandardItem("RMS"));
                        break;
                    case 1:
                        model->setItem(index, 0, new QStandardItem("0.5ST"));
                        break;
                    case 2:
                        model->setItem(index, 0, new QStandardItem("1ST"));
                        break;
                    case 3:
                        model->setItem(index, 0, new QStandardItem("2ST"));
                        break;
                    case 4:
                        model->setItem(index, 0, new QStandardItem("3ST"));
                        break;
                    case 5:
                        model->setItem(index, 0, new QStandardItem("4ST"));
                        break;
                    case 6:
                        model->setItem(index, 0, new QStandardItem("5ST"));
                        break;
                    case 7:
                        model->setItem(index, 0, new QStandardItem("6ST"));
                        break;
                    case 8:
                        model->setItem(index, 0, new QStandardItem("7ST"));
                        break;
                    case 9:
                        model->setItem(index, 0, new QStandardItem("8ST"));
                        break;
                    case 10:
                        model->setItem(index, 0, new QStandardItem("9ST"));
                        break;
                    case 11:
                        model->setItem(index, 0, new QStandardItem("10ST"));
                        break;
                    default:
                        break;
                    }

                    model->setItem(index, 1, new QStandardItem(QString::number(fi.fault_detail_thd.diaLooseness.inf[n],10,2)));
                    model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_detail_cur.diaLooseness.inf[n],10,2)));
                    model->setItem(index, 3, new QStandardItem(QString::number(fi.fault_detail_cof.diaLooseness.inf[n],10,2)));
                    if(fi.fault_detail_trigger.diaLooseness.inf[0] > 0.5)
                    {
                        model->setItem(index, 4, new QStandardItem(tr("Yes")));
                    }
                    else
                    {
                        model->setItem(index, 4, new QStandardItem(tr("No")));
                    }
                    index++;
                }
                index--;
                break;
            case 6:
                for(int n=0;n<21;n++)
                {
                    switch (n) {
                    case 0:
                        model->setItem(index, 0, new QStandardItem("RMS"));
                        break;
                    case 1:
                        model->setItem(index, 0, new QStandardItem("Inner[1]"));
                        break;
                    case 2:
                        model->setItem(index, 0, new QStandardItem("Inner[2]"));
                        break;
                    case 3:
                        model->setItem(index, 0, new QStandardItem("Inner[3]"));
                        break;
                    case 4:
                        model->setItem(index, 0, new QStandardItem("Inner[4]"));
                        break;
                    case 5:
                        model->setItem(index, 0, new QStandardItem("Inner[5]"));
                        break;
                    case 6:
                        model->setItem(index, 0, new QStandardItem("Outer[1]"));
                        break;
                    case 7:
                        model->setItem(index, 0, new QStandardItem("Outer[2]"));
                        break;
                    case 8:
                        model->setItem(index, 0, new QStandardItem("Outer[3]"));
                        break;
                    case 9:
                        model->setItem(index, 0, new QStandardItem("Outer[4]"));
                        break;
                    case 10:
                        model->setItem(index, 0, new QStandardItem("Outer[5]"));
                        break;
                    case 11:
                        model->setItem(index, 0, new QStandardItem("Cage[1]"));
                        break;
                    case 12:
                        model->setItem(index, 0, new QStandardItem("Cage[2]"));
                        break;
                    case 13:
                        model->setItem(index, 0, new QStandardItem("Cage[3]"));
                        break;
                    case 14:
                        model->setItem(index, 0, new QStandardItem("Cage[4]"));
                        break;
                    case 15:
                        model->setItem(index, 0, new QStandardItem("Cage[5]"));
                        break;
                    case 16:
                        model->setItem(index, 0, new QStandardItem("Roller[1]"));
                        break;
                    case 17:
                        model->setItem(index, 0, new QStandardItem("Roller[2]"));
                        break;
                    case 18:
                        model->setItem(index, 0, new QStandardItem("Roller[3]"));
                        break;
                    case 19:
                        model->setItem(index, 0, new QStandardItem("Roller[4]"));
                        break;
                    case 20:
                        model->setItem(index, 0, new QStandardItem("Roller[5]"));
                        break;
                    default:
                        break;
                    }

                    model->setItem(index, 1, new QStandardItem(QString::number(fi.fault_detail_thd.diaRollBear.inf[n],10,2)));
                    model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_detail_cur.diaRollBear.inf[n],10,2)));
                    model->setItem(index, 3, new QStandardItem(QString::number(fi.fault_detail_cof.diaRollBear.inf[n],10,2)));
                    if(fi.fault_detail_trigger.diaRollBear.inf[0] > 0.5)
                    {
                        model->setItem(index, 4, new QStandardItem(tr("Yes")));
                    }
                    else
                    {
                        model->setItem(index, 4, new QStandardItem(tr("No")));
                    }
                    index++;
                }
                index--;
                break;
            default:
                break;
            }

            index++;
        }
    }
}
