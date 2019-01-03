#include "beartypeedit.h"

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

BeartypeEdit::BeartypeEdit(QSqlTableModel *model,QWidget *parent) : QDialog(parent),model(model)
{
    this->setWindowTitle(tr("Bear-type Edit"));

    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->setMinimumSize(250,220);
    this->setMaximumSize(250,220);

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
    ptypeLabel->setText(tr("type:"));
    typeCombo = new QComboBox();
    typeCombo->addItem(tr("antifriction bearing"));
    typeCombo->addItem(tr("sliding bearing"));

    items->addWidget(ptypeLabel,1,0,1,1,Qt::AlignRight);
    items->addWidget(typeCombo,1,1,1,1);

    QLabel *prinLabel = new QLabel(this);
    prinLabel->setText(tr("rin:"));
    QLabel *prinMetricLabel = new QLabel(this);
    prinMetricLabel->setText(tr("mm"));
    prinEdit = new QLineEdit(this);
    prinEdit->setPlaceholderText(tr("Internal diameter"));

    items->addWidget(prinLabel,2,0,1,1,Qt::AlignRight);
    items->addWidget(prinEdit,2,1,1,1);
    items->addWidget(prinMetricLabel,2,2,1,1);

    QLabel *proutLabel = new QLabel(this);
    proutLabel->setText(tr("rout:"));
    QLabel *proutMetricLabel = new QLabel(this);
    proutMetricLabel->setText(tr("mm"));
    proutEdit = new QLineEdit(this);
    proutEdit->setPlaceholderText(tr("External diameter"));

    items->addWidget(proutLabel,3,0,1,1,Qt::AlignRight);
    items->addWidget(proutEdit,3,1,1,1);
    items->addWidget(proutMetricLabel,3,2,1,1);

    QLabel *pangleLabel = new QLabel(this);
    pangleLabel->setText(tr("contact angles:"));
    QLabel *pangleMetricLabel = new QLabel(this);
    pangleMetricLabel->setText(tr("°c"));
    pangleEdit = new QLineEdit(this);
    pangleEdit->setPlaceholderText(tr("Contact angles"));

    items->addWidget(pangleLabel,4,0,1,1,Qt::AlignRight);
    items->addWidget(pangleEdit,4,1,1,1);
    items->addWidget(pangleMetricLabel,4,2,1,1);

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

void BeartypeEdit::setmode(int value)
{
    mode = value;
    if (mode >= 0) //update bear
    {
        pmodelEdit->setEnabled(false);
        papplybtn->setEnabled(false);
        this->setWindowTitle(tr("Bear-type Edit"));
    }
    else
    {
        this->setWindowTitle(tr("Bear-type Add"));
    }
}

void BeartypeEdit::setmodel(QList<QString> values)
{
    QString modelstr;
    QString type;
    QString rin;
    QString rout;
    QString angle;

    modelstr = values.at(0);
    type = values.at(1);
    rin = values.at(2);
    rout = values.at(3);
    angle = values.at(4);

    qDebug() << type << rin << rout << angle;

    pmodelEdit->setText(modelstr);
    prinEdit->setText(rin);
    proutEdit->setText(rout);
    pangleEdit->setText(angle);

    QString type_s = typeCombo->currentText();
    if (type_s.compare(type) != 0)
    {
        typeCombo->setCurrentIndex(1);
    }
}

bool BeartypeEdit::editbear()
{
    QString modelstr = pmodelEdit->text().trimmed();
    QString type = typeCombo->currentText();
    QString rin = prinEdit->text().trimmed();
    QString rout = proutEdit->text().trimmed();
    QString angle = pangleEdit->text().trimmed();

    if (modelstr.length()==0 || rin.length()==0 || rout.length()==0 || angle.length()==0 || typeCombo->currentIndex() < 0)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter data!"));
        return false;
    }
    bool tok;
    rin.toFloat(&tok);
    if (!tok)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
        prinEdit->setFocus();
        return false;
    }

    rout.toFloat(&tok);
    if (!tok)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
        proutEdit->setFocus();
        return false;
    }

    angle.toFloat(&tok);
    if (!tok)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
        pangleEdit->setFocus();
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
        model->setData(model->index(row,2),type);
        model->setData(model->index(row,3),rin);
        model->setData(model->index(row,4),rout);
        model->setData(model->index(row,5),angle);
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
                record.setValue("btype", type);
                record.setValue("rin", rin);
                record.setValue("rout", rout);
                record.setValue("contact_angle", angle);
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

void BeartypeEdit::apply()
{
    this->editbear();
}

void BeartypeEdit::ok()
{
    if (this->editbear())
    {
        this->close();
    }
}

void BeartypeEdit::cancel()
{
    this->close();
}
