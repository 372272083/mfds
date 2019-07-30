#include "paramsettingdialog.h"

#include <QGridLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>

#include <QMessageBox>
#include "globalvariable.h"

ParamSettingDialog::ParamSettingDialog(QDialog *parent):QDialog(parent)
{
    this->setMaximumSize(300,200);
    this->setMinimumSize(300,200);

    int index = 0;
    QGridLayout* layout = new QGridLayout();
    layout->setAlignment(Qt::AlignCenter);

    QLabel* f_l_Label = new QLabel(this);
    f_l_Label->setText(tr("Freqency Show limit:"));
    freqshowlimitEdit = new QLineEdit(this);
    freqshowlimitEdit->setText(QString::number(GlobalVariable::freqlimit));

    layout->addWidget(f_l_Label,index,0,1,1,Qt::AlignRight);
    layout->addWidget(freqshowlimitEdit,index,1,1,1,Qt::AlignLeft);

    QLabel* w_f_l_Label = new QLabel(this);
    w_f_l_Label->setText(tr("Waterfall Show limit:"));
    waterfalldepthlimitCB = new QComboBox(this);

    waterfalldepthlimitCB->addItem("50");
    waterfalldepthlimitCB->addItem("40");
    waterfalldepthlimitCB->addItem("30");
    waterfalldepthlimitCB->addItem("20");
    waterfalldepthlimitCB->addItem("10");

    switch (GlobalVariable::waterfalldepthlimit) {
    case 50:
        waterfalldepthlimitCB->setCurrentIndex(0);
        break;
    case 40:
        waterfalldepthlimitCB->setCurrentIndex(1);
        break;
    case 30:
        waterfalldepthlimitCB->setCurrentIndex(2);
        break;
    case 20:
        waterfalldepthlimitCB->setCurrentIndex(3);
        break;
    case 10:
        waterfalldepthlimitCB->setCurrentIndex(4);
        break;
    default:
        break;
    }

    index++;
    layout->addWidget(w_f_l_Label,index,0,1,1,Qt::AlignRight);
    layout->addWidget(waterfalldepthlimitCB,index,1,1,1,Qt::AlignLeft);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(layout);

    QHBoxLayout *btn = new QHBoxLayout(this);
    btn->addStretch();

    QPushButton *pokbtn = new QPushButton(this);
    connect(pokbtn, SIGNAL(clicked()),this, SLOT(ok()));
    pokbtn->setText(tr("ok"));
    btn->addWidget(pokbtn);

    QPushButton *pcancelbtn = new QPushButton(this);
    connect(pcancelbtn, SIGNAL(clicked()),this, SLOT(cancel()));
    pcancelbtn->setText(tr("cancel"));
    btn->addWidget(pcancelbtn);

    mainLayout->addLayout(btn);

    setLayout(mainLayout);
}

void ParamSettingDialog::ok()
{
    QString freq_limit_str = freqshowlimitEdit->text().trimmed();
    if(freq_limit_str.length() == 0)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter data!"));
        return;
    }
    int freq_limit_i;
    bool tok;
    freq_limit_i = freq_limit_str.toInt(&tok);
    if(freq_limit_i > 0)
    {
        GlobalVariable::freqlimit = freq_limit_i;
    }
    else
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter data!"));
        freqshowlimitEdit->setFocus();
        return;
    }

    QString waterfall_limit = waterfalldepthlimitCB->currentText();
    int tmp_i;
    tmp_i = waterfall_limit.toInt(&tok);
    GlobalVariable::waterfalldepthlimit = tmp_i;

    this->close();
}

void ParamSettingDialog::cancel()
{
    this->close();
}
