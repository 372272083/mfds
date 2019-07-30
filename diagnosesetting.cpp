#include "diagnosesetting.h"

#include <QLineEdit>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>

#include <QMessageBox>

#include "globalvariable.h"

DiagnoseSetting::DiagnoseSetting(QDialog *parent) : QDialog(parent)
{
    this->setMaximumSize(350,250);
    this->setMinimumSize(350,250);

    QVBoxLayout *playout = new QVBoxLayout(this);
    this->setLayout(playout);

    QGridLayout *layout = new QGridLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    int index = 0;

    /*
    QLabel* numlabel = new QLabel(this);
    numlabel->setText(tr("Study Num:"));
    study_num_edit = new QLineEdit(this);
    study_num_edit->setText(QString::number(GlobalVariable::study_num));

    layout->addWidget(numlabel,index,0,1,1,Qt::AlignRight);
    layout->addWidget(study_num_edit,index,1,1,1);

    index++;
    */

    QLabel* intervallabel = new QLabel(this);
    intervallabel->setText(tr("Study Interval:"));
    study_interval_edit = new QLineEdit(this);
    study_interval_edit->setText(QString::number(GlobalVariable::study_time,10,2));
    QLabel* intervalunitlabel = new QLabel(this);
    intervalunitlabel->setText(tr("Hour"));

    layout->addWidget(intervallabel,index,0,1,1,Qt::AlignRight);
    layout->addWidget(study_interval_edit,index,1,1,1);
    layout->addWidget(intervalunitlabel,index,2,1,1);

    index++;
    QLabel* studylabel = new QLabel(this);
    studylabel->setText(tr("Study mode:"));
    study_CB = new QCheckBox(this);
    study_CB->setChecked(GlobalVariable::is_study);
    layout->addWidget(studylabel,index,0,1,1,Qt::AlignRight);
    layout->addWidget(study_CB,index,1,1,1);

    index++;

    QHBoxLayout *btn = new QHBoxLayout;
    btn->addStretch();

    QPushButton *prestudybtn = new QPushButton(this);
    connect(prestudybtn, SIGNAL(clicked()),this, SLOT(onStudy()));
    prestudybtn->setText(tr("study again"));
    btn->addWidget(prestudybtn);

    QPushButton *pokbtn = new QPushButton(this);
    connect(pokbtn, SIGNAL(clicked()),this, SLOT(ok()));
    pokbtn->setText(tr("ok"));
    btn->addWidget(pokbtn);

    QPushButton *pcancelbtn = new QPushButton(this);
    connect(pcancelbtn, SIGNAL(clicked()),this, SLOT(cancel()));
    pcancelbtn->setText(tr("cancel"));
    btn->addWidget(pcancelbtn);

    playout->addLayout(layout);
    playout->addLayout(btn);
    //layout->addItem(btn,index,0,1,2,Qt::AlignRight);
}

void DiagnoseSetting::cancel()
{
    this->close();
}

void DiagnoseSetting::ok()
{
    //QString num_s = study_num_edit->text().trimmed();
    QString interval_s = study_interval_edit->text().trimmed();

    bool isStudy = study_CB->isChecked();

    bool tok;

    //int num = num_s.toInt(&tok);
    //if(!tok)
    //{
    //    return;
    //}
    float interval_f = interval_s.toFloat(&tok);
    if(!tok)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
        study_interval_edit->setFocus();
        return;
    }

    //GlobalVariable::study_num = num;
    GlobalVariable::study_time = interval_f;
    GlobalVariable::is_study = isStudy;

    this->close();
}

void DiagnoseSetting::onStudy()
{
    QMessageBox::StandardButton rb = QMessageBox::question(this,tr("Question"),tr("Study again will delete all previous data,do you want to continue?"),QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if(rb == QMessageBox::Yes)
    {
        //QString num_s = study_num_edit->text().trimmed();
        QString interval_s = study_interval_edit->text().trimmed();

        bool isStudy = study_CB->isChecked();

        bool tok;

        float interval_f = interval_s.toFloat(&tok);
        if(!tok)
        {
            QMessageBox::information(this, tr("Infomation"), tr("Please enter number!"));
            study_interval_edit->setFocus();
            return;
        }

        GlobalVariable::is_study_again = true;
        GlobalVariable::study_time = interval_f;
        GlobalVariable::is_study = isStudy;
    }


    this->close();
}
