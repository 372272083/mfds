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
    header->setSectionResizeMode(QHeaderView::ResizeToContents);
    header->setStretchLastSection(true);

    tableView->setModel(model);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(tableView);

    this->setLayout(layout);

    this->setMinimumSize(700,400);
}

void FaultDetailResultDialog::setDetailInfo(QString mcode,int faultcode_l,int fault_type) // fault_type: 0 vibrate; 1 electric
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

            if(fault_type == 0)
            {
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
            }
            else if(fault_type == 1)
            {
                switch (faultcode_l) {
                case 1:

                    model->setItem(index, 0, new QStandardItem("Three-phase voltage imbalance"));

                    model->setItem(index, 1, new QStandardItem("< 3%"));
                    model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.vcp.u_balance,10,2)));
                    model->setItem(index, 3, new QStandardItem(""));
                    break;
                case 2:
                    for(int i=0;i<3;i++)
                    {
                        switch (i) {
                        case 0:
                            model->setItem(index, 0, new QStandardItem("Phase A Voltage Deviation Value"));
                            break;
                        case 1:
                            model->setItem(index, 0, new QStandardItem("Phase B Voltage Deviation Value"));
                            break;
                        case 2:
                            model->setItem(index, 0, new QStandardItem("Phase C Voltage Deviation Value"));
                            break;
                        default:
                            break;
                        }

                        model->setItem(index, 1, new QStandardItem("87.5% ~ 106%"));
                        model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.vcp.u_bias[i],10,2)));
                        model->setItem(index, 3, new QStandardItem(""));

                        index++;
                    }
                    index--;
                    break;
                case 3:
                    for(int i=0;i<3;i++)
                    {
                        switch (i) {
                        case 0:
                            model->setItem(index, 0, new QStandardItem("A-phase voltage spike coefficient"));
                            break;
                        case 1:
                            model->setItem(index, 0, new QStandardItem("B-phase voltage spike coefficiente"));
                            break;
                        case 2:
                            model->setItem(index, 0, new QStandardItem("C-phase voltage spike coefficient"));
                            break;
                        default:
                            break;
                        }

                        model->setItem(index, 1, new QStandardItem("1.35 ~ 1.45"));
                        model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.vcp.u_cf[i],10,2)));
                        model->setItem(index, 3, new QStandardItem(""));

                        index++;
                    }
                    index--;
                    break;
                case 4:
                    for(int i=0;i<3;i++)
                    {
                        switch (i) {
                        case 0:
                            model->setItem(index, 0, new QStandardItem("Total Harmonic Distortion of Phase A Voltage"));
                            break;
                        case 1:
                            model->setItem(index, 0, new QStandardItem("Total Harmonic Distortion of Phase B Voltage"));
                            break;
                        case 2:
                            model->setItem(index, 0, new QStandardItem("Total Harmonic Distortion of Phase C Voltage"));
                            break;
                        default:
                            break;
                        }

                        model->setItem(index, 1, new QStandardItem("0% ~ 5%"));
                        model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.vcp.u_thd[i],10,2)));
                        model->setItem(index, 3, new QStandardItem(""));

                        index++;
                    }
                    index--;
                    break;
                case 5:
                    for(int i=0;i<3;i++)
                    {
                        switch (i) {
                        case 0:
                            model->setItem(index, 0, new QStandardItem("A-phase power factor"));
                            break;
                        case 1:
                            model->setItem(index, 0, new QStandardItem("B-phase power factor"));
                            break;
                        case 2:
                            model->setItem(index, 0, new QStandardItem("C-phase power factor"));
                            break;
                        default:
                            break;
                        }

                        model->setItem(index, 1, new QStandardItem("nameplate value or 0.85 ~ 1.0"));
                        model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.vcp.p_pf[i],10,2)));
                        model->setItem(index, 3, new QStandardItem(""));

                        index++;
                    }
                    index--;
                    break;
                case 6:

                    model->setItem(index, 0, new QStandardItem("Load"));

                    model->setItem(index, 1, new QStandardItem("25% ~ 100%"));
                    model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.vcp.p_load,10,2)));
                    model->setItem(index, 3, new QStandardItem(""));
                    break;
                case 7:

                    model->setItem(index, 0, new QStandardItem("Three-phase current imbalance"));

                    model->setItem(index, 1, new QStandardItem("0% ~ 2%"));
                    model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.vcp.i_balance,10,2)));
                    model->setItem(index, 3, new QStandardItem(""));
                    break;
                case 8:
                    for(int i=0;i<3;i++)
                    {
                        switch (i) {
                        case 0:
                            model->setItem(index, 0, new QStandardItem("A-phase current spike coefficient"));
                            break;
                        case 1:
                            model->setItem(index, 0, new QStandardItem("B-phase current spike coefficiente"));
                            break;
                        case 2:
                            model->setItem(index, 0, new QStandardItem("C-phase current spike coefficient"));
                            break;
                        default:
                            break;
                        }

                        model->setItem(index, 1, new QStandardItem("1.35 ~ 1.45"));
                        model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.vcp.i_cf[i],10,2)));
                        model->setItem(index, 3, new QStandardItem(""));

                        index++;
                    }
                    index--;
                    break;
                case 9:
                    for(int i=0;i<3;i++)
                    {
                        switch (i) {
                        case 0:
                            model->setItem(index, 0, new QStandardItem("Total Harmonic Distortion of Phase A Current"));
                            break;
                        case 1:
                            model->setItem(index, 0, new QStandardItem("Total Harmonic Distortion of Phase B Current"));
                            break;
                        case 2:
                            model->setItem(index, 0, new QStandardItem("Total Harmonic Distortion of Phase C Current"));
                            break;
                        default:
                            break;
                        }

                        model->setItem(index, 1, new QStandardItem("0% ~ 5%"));
                        model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.vcp.i_thd[i],10,2)));
                        model->setItem(index, 3, new QStandardItem(""));

                        index++;
                    }
                    index--;
                    break;
                case 10:

                    model->setItem(index, 0, new QStandardItem("Rotor Health Grade(1-7)"));

                    model->setItem(index, 1, new QStandardItem("1 ~ 3"));
                    model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.diagnosis.rotor_health_level,10,2)));
                    model->setItem(index, 3, new QStandardItem(""));
                    index++;

                    model->setItem(index, 0, new QStandardItem("Rotor health status value"));

                    model->setItem(index, 1, new QStandardItem("below -48dB"));
                    model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.diagnosis.rotor_health_index,10,2)));
                    model->setItem(index, 3, new QStandardItem(""));

                    break;
                case 11:

                    model->setItem(index, 0, new QStandardItem("Loose state value of winding"));

                    model->setItem(index, 1, new QStandardItem("below -65dB"));
                    model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.diagnosis.windings_loose,10,2)));
                    model->setItem(index, 3, new QStandardItem(""));
                    index++;

                    model->setItem(index, 0, new QStandardItem("Interturn Short Circuit State Value"));

                    model->setItem(index, 1, new QStandardItem("below -65dB"));
                    model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.diagnosis.turntoturnshort,10,2)));
                    model->setItem(index, 3, new QStandardItem(""));

                    break;
                case 12:

                    model->setItem(index, 0, new QStandardItem("Static eccentricity value"));

                    model->setItem(index, 1, new QStandardItem("below -65dB"));
                    model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.diagnosis.static_eccentricity,10,2)));
                    model->setItem(index, 3, new QStandardItem(""));
                    index++;

                    model->setItem(index, 0, new QStandardItem("Dynamic eccentricity value"));

                    model->setItem(index, 1, new QStandardItem("below -65dB"));
                    model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.diagnosis.dynamic_eccentricity,10,2)));
                    model->setItem(index, 3, new QStandardItem(""));

                    break;
                case 13:

                    model->setItem(index, 0, new QStandardItem("State value of unbalanced or misaligned rotor"));

                    model->setItem(index, 1, new QStandardItem("below -65dB"));
                    model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.diagnosis.imbanlance_or_unbanlance_index,10,2)));
                    model->setItem(index, 3, new QStandardItem(""));

                    break;
                case 14:

                    model->setItem(index, 0, new QStandardItem("Mechanical Loosening State Value"));

                    model->setItem(index, 1, new QStandardItem("below -65dB"));
                    model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.diagnosis.looseness)));
                    model->setItem(index, 3, new QStandardItem(""));
                    index++;

                    break;
                case 15:

                    model->setItem(index, 0, new QStandardItem("Characteristic state value of cage 1"));

                    model->setItem(index, 1, new QStandardItem("below -65dB"));
                    model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.rollbearing.ftf_value1)));
                    model->setItem(index, 3, new QStandardItem(""));
                    index++;

                    model->setItem(index, 0, new QStandardItem("Roller Characteristic State Value 1"));

                    model->setItem(index, 1, new QStandardItem("below -65dB"));
                    model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.rollbearing.bsf_value1)));
                    model->setItem(index, 3, new QStandardItem(""));
                    index++;

                    model->setItem(index, 0, new QStandardItem("Outer Circle Eigenstate Value 1"));

                    model->setItem(index, 1, new QStandardItem("below -65dB"));
                    model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.rollbearing.bpfo_value1)));
                    model->setItem(index, 3, new QStandardItem(""));
                    index++;

                    model->setItem(index, 0, new QStandardItem("Inner Circle Eigenstate Value 1"));

                    model->setItem(index, 1, new QStandardItem("below -65dB"));
                    model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.rollbearing.bpfl_value1)));
                    model->setItem(index, 3, new QStandardItem(""));
                    index++;

                    /////// 2
                    model->setItem(index, 0, new QStandardItem("Characteristic state value of cage 2"));

                    model->setItem(index, 1, new QStandardItem("below -65dB"));
                    model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.rollbearing.ftf_value2)));
                    model->setItem(index, 3, new QStandardItem(""));
                    index++;

                    model->setItem(index, 0, new QStandardItem("Roller Characteristic State Value 2"));

                    model->setItem(index, 1, new QStandardItem("below -65dB"));
                    model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.rollbearing.bsf_value2)));
                    model->setItem(index, 3, new QStandardItem(""));
                    index++;

                    model->setItem(index, 0, new QStandardItem("Outer Circle Eigenstate Value 2"));

                    model->setItem(index, 1, new QStandardItem("below -65dB"));
                    model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.rollbearing.bpfo_value2)));
                    model->setItem(index, 3, new QStandardItem(""));
                    index++;

                    model->setItem(index, 0, new QStandardItem("Inner Circle Eigenstate Value 2"));

                    model->setItem(index, 1, new QStandardItem("below -65dB"));
                    model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.rollbearing.bpfl_value2)));
                    model->setItem(index, 3, new QStandardItem(""));
                    index++;

                    /////// 3
                    model->setItem(index, 0, new QStandardItem("Characteristic state value of cage 3"));

                    model->setItem(index, 1, new QStandardItem("below -65dB"));
                    model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.rollbearing.ftf_value3)));
                    model->setItem(index, 3, new QStandardItem(""));
                    index++;

                    model->setItem(index, 0, new QStandardItem("Roller Characteristic State Value 3"));

                    model->setItem(index, 1, new QStandardItem("below -65dB"));
                    model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.rollbearing.bsf_value3)));
                    model->setItem(index, 3, new QStandardItem(""));
                    index++;

                    model->setItem(index, 0, new QStandardItem("Outer Circle Eigenstate Value 3"));

                    model->setItem(index, 1, new QStandardItem("below -65dB"));
                    model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.rollbearing.bpfo_value3)));
                    model->setItem(index, 3, new QStandardItem(""));
                    index++;

                    model->setItem(index, 0, new QStandardItem("Inner Circle Eigenstate Value 3"));

                    model->setItem(index, 1, new QStandardItem("below -65dB"));
                    model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.rollbearing.bpfl_value3)));
                    model->setItem(index, 3, new QStandardItem(""));
                    index++;

                    /////// 4
                    model->setItem(index, 0, new QStandardItem("Characteristic state value of cage 4"));

                    model->setItem(index, 1, new QStandardItem("below -65dB"));
                    model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.rollbearing.ftf_value4)));
                    model->setItem(index, 3, new QStandardItem(""));
                    index++;

                    model->setItem(index, 0, new QStandardItem("Roller Characteristic State Value 4"));

                    model->setItem(index, 1, new QStandardItem("below -65dB"));
                    model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.rollbearing.bsf_value4)));
                    model->setItem(index, 3, new QStandardItem(""));
                    index++;

                    model->setItem(index, 0, new QStandardItem("Outer Circle Eigenstate Value 4"));

                    model->setItem(index, 1, new QStandardItem("below -65dB"));
                    model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.rollbearing.bpfo_value4)));
                    model->setItem(index, 3, new QStandardItem(""));
                    index++;

                    model->setItem(index, 0, new QStandardItem("Inner Circle Eigenstate Value 4"));

                    model->setItem(index, 1, new QStandardItem("below -65dB"));
                    model->setItem(index, 2, new QStandardItem(QString::number(fi.fault_electric_details.rollbearing.bpfl_value4)));
                    model->setItem(index, 3, new QStandardItem(""));

                    break;
                }
            }

            index++;
        }
    }
}
