#include "devicetypeedit.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QSqlTableModel>
#include <QMessageBox>
#include <QSqlRecord>

#include <QList>

#include <QDebug>

DevicetypeEdit::DevicetypeEdit(QSqlTableModel *model,QWidget *parent) : QDialog(parent),model(model)
{
    this->setWindowTitle(tr("Device-type Edit"));

    //this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->setMinimumSize(250,220);
    //this->setMaximumSize(250,220);

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
    typeCombo->addItem(tr("Electric"),"E");
    typeCombo->addItem(tr("Vibrate"),"V");
    typeCombo->addItem(tr("Temperature"),"T");
    //typeclass << tr("E") << tr("V") << tr("T");
    //typeCombo->addItems(typeclass);

    items->addWidget(ptypeLabel,1,0,1,1,Qt::AlignRight);
    items->addWidget(typeCombo,1,1,1,1);

    QLabel *ppipesLabel = new QLabel(this);
    ppipesLabel->setText(tr("pipe number:"));
    ppipesEdit = new QLineEdit(this);

    items->addWidget(ppipesLabel,2,0,1,1,Qt::AlignRight | Qt::AlignTop);
    items->addWidget(ppipesEdit,2,1,1,1);

    QLabel *pdesLabel = new QLabel(this);
    pdesLabel->setText(tr("description:"));
    pdescriptionEdit = new QTextEdit(this);

    items->addWidget(pdesLabel,3,0,1,1,Qt::AlignRight | Qt::AlignTop);
    items->addWidget(pdescriptionEdit,3,1,1,3);

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

void DevicetypeEdit::setmode(int value)
{
    mode = value;
    if (mode >= 0) //update device type
    {
        pmodelEdit->setEnabled(false);
        papplybtn->setEnabled(false);
        this->setWindowTitle(tr("Device-type Edit"));
    }
    else
    {
        this->setWindowTitle(tr("Device-type Add"));
    }
}

void DevicetypeEdit::setmodel(QList<QString> values)
{
    QString modelstr;
    QString type;
    QString pipenum;
    QString description;

    modelstr = values.at(0);
    type = values.at(1);
    pipenum = values.at(2);
    description = values.at(3);

    pmodelEdit->setText(modelstr);
    pdescriptionEdit->setPlainText(description);
    ppipesEdit->setText(pipenum);

    QList<QString>::Iterator it = typeclass.begin(),itend = typeclass.end();
    int i = 0;
    for (;it != itend; it++,i++){
        if ((*it).compare(type) == 0){//找到，高亮显示
            typeCombo->setCurrentIndex(i);
            break;
        }
    }
}

bool DevicetypeEdit::editdevicetype()
{
    QString modelstr = pmodelEdit->text().trimmed();
    QString type = typeCombo->itemData(typeCombo->currentIndex()).toString();
    QString pipenum = ppipesEdit->text().trimmed();
    QString description = pdescriptionEdit->toPlainText();

    if (modelstr.length()==0 || typeCombo->currentIndex() < 0 || pipenum.length() == 0)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter data!"));
        return false;
    }

    bool tok;
    pipenum.toFloat(&tok);
    if (!tok)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter Number!"));
        ppipesEdit->setFocus();
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
        model->setData(model->index(row,3),pipenum);
        model->setData(model->index(row,4),description);
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
                record.setValue("dtype", type);
                record.setValue("pipenum", pipenum);
                record.setValue("description", description);
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

void DevicetypeEdit::apply()
{
    this->editdevicetype();
}

void DevicetypeEdit::ok()
{
    if (this->editdevicetype())
    {
        this->close();
    }
}

void DevicetypeEdit::cancel()
{
    this->close();
}
