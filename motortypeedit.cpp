#include "motortypeedit.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QSqlTableModel>
#include <QMessageBox>
#include <QSqlRecord>

#include <QList>

#include <QDebug>

MotortypeEdit::MotortypeEdit(QSqlTableModel *model,QWidget *parent) : QDialog(parent),model(model)
{
    this->setWindowTitle(tr("Motor-type Edit"));

    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->setMinimumSize(400,400);
    this->setMaximumSize(400,400);

    this->setWindowIcon(QIcon(":/images/icon"));

    QVBoxLayout *playout = new QVBoxLayout;

    QGridLayout *items = new QGridLayout;
    QLabel *pmodelLabel = new QLabel(this);
    pmodelLabel->setText(tr("model:"));
    pmodelEdit = new QLineEdit(this);
    pmodelEdit->setPlaceholderText(tr("please enter model"));

    items->addWidget(pmodelLabel,0,0,1,1,Qt::AlignRight);
    items->addWidget(pmodelEdit,0,1,1,1);

    QLabel *ptypeLabel = new QLabel(this);
    ptypeLabel->setText(tr("work mode:"));
    workMCombo = new QComboBox();
    workMCombo->addItem(tr("synchronous"));
    workMCombo->addItem(tr("asynchronous"));

    items->addWidget(ptypeLabel,1,0,1,1,Qt::AlignRight);
    items->addWidget(workMCombo,1,1,1,1);

    QLabel *ppowerLabel = new QLabel(this);
    ppowerLabel->setText(tr("power:"));
    QLabel *ppowerMetricLabel = new QLabel(this);
    ppowerMetricLabel->setText(tr("KW"));
    ppowerEdit = new QLineEdit(this);
    ppowerEdit->setPlaceholderText(tr("please enter power"));

    items->addWidget(ppowerLabel,2,0,1,1,Qt::AlignRight);
    items->addWidget(ppowerEdit,2,1,1,1);
    items->addWidget(ppowerMetricLabel,2,2,1,1);

    QLabel *pvoltageLabel = new QLabel(this);
    pvoltageLabel->setText(tr("voltage:"));
    QLabel *pvoltageMetricLabel = new QLabel(this);
    pvoltageMetricLabel->setText(tr("V"));
    pvoltageEdit = new QLineEdit(this);
    pvoltageEdit->setPlaceholderText(tr("please enter voltage"));

    items->addWidget(pvoltageLabel,3,0,1,1,Qt::AlignRight);
    items->addWidget(pvoltageEdit,3,1,1,1);
    items->addWidget(pvoltageMetricLabel,3,2,1,1);

    QLabel *pcurrentLabel = new QLabel(this);
    pcurrentLabel->setText(tr("current:"));
    QLabel *pcurrentMetricLabel = new QLabel(this);
    pcurrentMetricLabel->setText(tr("A"));
    pcurrentEdit = new QLineEdit(this);
    pcurrentEdit->setPlaceholderText(tr("please enter current"));

    items->addWidget(pcurrentLabel,4,0,1,1,Qt::AlignRight);
    items->addWidget(pcurrentEdit,4,1,1,1);
    items->addWidget(pcurrentMetricLabel,4,2,1,1);

    QLabel *ppolesLabel = new QLabel(this);
    ppolesLabel->setText(tr("Pole of Pairs:"));
    QLabel *ppolesMetricLabel = new QLabel(this);
    ppolesMetricLabel->setText(tr("p"));
    ppolesEdit = new QLineEdit(this);
    ppolesEdit->setPlaceholderText(tr("please enter poles"));

    items->addWidget(ppolesLabel,5,0,1,1,Qt::AlignRight);
    items->addWidget(ppolesEdit,5,1,1,1);
    items->addWidget(ppolesMetricLabel,5,2,1,1);

    QLabel *pcenterLabel = new QLabel(this);
    pcenterLabel->setText(tr("center height:"));
    QLabel *pcenterMetricLabel = new QLabel(this);
    pcenterMetricLabel->setText(tr("mm"));
    pcenterEdit = new QLineEdit(this);
    pcenterEdit->setPlaceholderText(tr("please enter center height"));

    items->addWidget(pcenterLabel,6,0,1,1,Qt::AlignRight);
    items->addWidget(pcenterEdit,6,1,1,1);
    items->addWidget(pcenterMetricLabel,6,2,1,1);

    QLabel *pfactorLabel = new QLabel(this);
    pfactorLabel->setText(tr("factor:"));
    pfactorEdit = new QLineEdit(this);
    pfactorEdit->setPlaceholderText(tr("please enter factor"));

    items->addWidget(pfactorLabel,7,0,1,1,Qt::AlignRight);
    items->addWidget(pfactorEdit,7,1,1,1);

    insulateClass<<tr("A")<<tr("E")<<tr("B")<<tr("F")<<tr("H");
    QLabel *pinsulateLabel = new QLabel(this);
    pinsulateLabel->setText(tr("insulate:"));
    QLabel *pinsulateMetricLabel = new QLabel(this);
    pinsulateMetricLabel->setText(tr("class"));
    pinsulateCombo = new QComboBox();
    pinsulateCombo->addItems(insulateClass);

    items->addWidget(pinsulateLabel,8,0,1,1,Qt::AlignRight);
    items->addWidget(pinsulateCombo,8,1,1,1);
    items->addWidget(pinsulateMetricLabel,8,2,1,1);

    QLabel *protateLabel = new QLabel(this);
    protateLabel->setText(tr("rotate:"));
    QLabel *protateMetricLabel = new QLabel(this);
    protateMetricLabel->setText(tr("r/min"));
    protateEdit = new QLineEdit(this);
    protateEdit->setPlaceholderText(tr("please enter ratate"));

    items->addWidget(protateLabel,9,0,1,1,Qt::AlignRight);
    items->addWidget(protateEdit,9,1,1,1);
    items->addWidget(protateMetricLabel,9,2,1,1);

    playout->addLayout(items);
    playout->setContentsMargins(10,10,10,10);

    QHBoxLayout *btn = new QHBoxLayout;
    btn->addStretch();
    papplybtn = new QPushButton(this);
    connect(papplybtn, SIGNAL(clicked()),this, SLOT(apply()));
    papplybtn->setText(tr("apply"));
    btn->addWidget(papplybtn);

    QPushButton *pokbtn = new QPushButton(this);
    connect(pokbtn, SIGNAL(clicked()),this, SLOT(ok()));
    pokbtn->setText(tr("ok"));
    btn->addWidget(pokbtn);

    QPushButton *pcancelbtn = new QPushButton(this);
    connect(pcancelbtn, SIGNAL(clicked()),this, SLOT(cancel()));
    pcancelbtn->setText(tr("cancel"));
    btn->addWidget(pcancelbtn);

    playout->addLayout(btn);
    this->setLayout(playout);
}

void MotortypeEdit::setmode(int value)
{
    mode = value;
    if (mode >= 0) //update bear
    {
        pmodelEdit->setEnabled(false);
        papplybtn->setEnabled(false);
        this->setWindowTitle(tr("Motor-type Edit"));
    }
    else
    {
        this->setWindowTitle(tr("Motor-type Add"));
    }
}

void MotortypeEdit::setmodel(QList<QString> values)
{
    QString modelstr;
    QString workmode;

    QString power_l;
    QString voltage_l;
    QString current_l;
    QString poles_l;
    QString centerh_l;
    QString factor_l;
    QString insulate_l;
    QString rotate_l;

    modelstr = values.at(0);
    workmode = values.at(1);
    power_l = values.at(2);
    voltage_l = values.at(3);
    current_l = values.at(4);
    poles_l = values.at(5);
    centerh_l = values.at(6);
    factor_l = values.at(7);
    insulate_l = values.at(8);
    rotate_l = values.at(9);

    bool tok;
    pmodelEdit->setText(modelstr);
    ppowerEdit->setText(power_l);
    pvoltageEdit->setText(voltage_l);
    pcurrentEdit->setText(current_l);
    ppolesEdit->setText(poles_l);
    pcenterEdit->setText(centerh_l);
    pfactorEdit->setText(QString("%4").arg(factor_l.toFloat(&tok)));
    protateEdit->setText(rotate_l);

    QString type_s = workMCombo->currentText();
    if (type_s.compare(workmode) != 0)
    {
        workMCombo->setCurrentIndex(1);
    }

    QList<QString>::Iterator it = insulateClass.begin(),itend = insulateClass.end();
    int i = 0;
    for (;it != itend; it++,i++){
        if ((*it).compare(insulate_l) == 0){//找到，高亮显示
            pinsulateCombo->setCurrentIndex(i);
            break;
        }
    }
}

bool MotortypeEdit::editmotor()
{
    QString modelstr = pmodelEdit->text().trimmed();
    QString workmode = workMCombo->currentText();

    QString power_l = ppowerEdit->text().trimmed();
    QString voltage_l = pvoltageEdit->text().trimmed();
    QString current_l = pcurrentEdit->text().trimmed();
    QString poles_l = ppolesEdit->text().trimmed();
    QString centerh_l = pcenterEdit->text().trimmed();
    QString factor_l = pfactorEdit->text().trimmed();
    QString insulate_l = pinsulateCombo->currentText();
    QString rotate_l = protateEdit->text().trimmed();

    if (modelstr.length()==0 || power_l.length()==0 || voltage_l.length()==0 || current_l.length()==0 || workMCombo->currentIndex() < 0 \
            || poles_l.length()==0 || centerh_l.length()==0 || factor_l.length()==0 || pinsulateCombo->currentIndex() < 0 || rotate_l.length()==0)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter data!"));
        return false;
    }
    bool tok;
    power_l.toFloat(&tok);
    if (!tok)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
        ppowerEdit->setFocus();
        return false;
    }

    voltage_l.toFloat(&tok);
    if (!tok)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
        pvoltageEdit->setFocus();
        return false;
    }

    current_l.toFloat(&tok);
    if (!tok)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
        pcurrentEdit->setFocus();
        return false;
    }

    poles_l.toInt(&tok);
    if (!tok)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
        ppolesEdit->setFocus();
        return false;
    }

    centerh_l.toFloat(&tok);
    if (!tok)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
        pcenterEdit->setFocus();
        return false;
    }

    factor_l.toFloat(&tok);
    if (!tok)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
        pfactorEdit->setFocus();
        return false;
    }

    rotate_l.toFloat(&tok);
    if (!tok)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
        protateEdit->setFocus();
        return false;
    }

    if (0 > mode)
    {
        QString filter = "model='" + modelstr + "'";

        model->setFilter(filter);
        if (model->select()) {
            if (model->rowCount() > 0)
            {
                QMessageBox::information(this, tr("Infomation"), tr("Model has Existed!"));
                model->setFilter("id>0");
                model->select();
                pmodelEdit->setFocus();
                return false;
            }
            model->setFilter("id>0");
            model->select();
        }

        int row=0;
        model->insertRow(row);
        model->setData(model->index(row,1),modelstr);
        model->setData(model->index(row,2),workmode);
        model->setData(model->index(row,3),power_l);
        model->setData(model->index(row,4),voltage_l);
        model->setData(model->index(row,5),current_l);
        model->setData(model->index(row,6),poles_l);
        model->setData(model->index(row,7),centerh_l);
        model->setData(model->index(row,8),factor_l);
        model->setData(model->index(row,9),insulate_l);
        model->setData(model->index(row,10),rotate_l);
        model->submitAll();
    }
    else
    {
        QString filter = "model='" + modelstr + "'";
        model->setFilter(filter);
        if (model->select()) {
            if (model->rowCount() == 1)
            {
                QSqlRecord record = model->record(0);
                record.setValue("work_mode", workmode);
                record.setValue("power_rating", power_l);
                record.setValue("rated_voltage", voltage_l);
                record.setValue("rated_current", current_l);
                record.setValue("poleNums", poles_l);
                record.setValue("center_height", centerh_l);
                record.setValue("factor", factor_l);
                record.setValue("insulate", insulate_l);
                record.setValue("rotate", rotate_l);
                model->setRecord(0, record);
                model->submitAll();
            }
            else
            {
                return false;
            }
            model->setFilter("id>0");
            model->select();
        }
    }

    return true;
}

void MotortypeEdit::apply()
{
    this->editmotor();
}

void MotortypeEdit::ok()
{
    if (this->editmotor())
    {
        this->close();
    }
}

void MotortypeEdit::cancel()
{
    this->close();
}
