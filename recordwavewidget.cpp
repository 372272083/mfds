#include "recordwavewidget.h"

#include <QComboBox>
#include <QLineEdit>
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QProgressBar>
#include <QTimer>

#include "constants.h"

#include <QMessageBox>

RecordWaveWidget::RecordWaveWidget(QWidget *parent) : QDialog(parent)
{
    this->setWindowTitle(tr("Record Wave"));

    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    this->setMinimumSize(300,200);
    this->setMaximumSize(300,200);

    QGridLayout *items = new QGridLayout();
    items->setMargin(5);
    items->setVerticalSpacing(5);
    items->setAlignment(Qt::AlignCenter);

    int index = 0;
    QLabel *pintervalLabel = new QLabel(this);
    pintervalLabel->setText(tr("interval:"));
    intervalCombo = new QComboBox(this);

    intervalCombo->addItem(tr("0.5"),0.5);
    intervalCombo->addItem(tr("1"),1);
    intervalCombo->addItem(tr("2"),2);
    intervalCombo->addItem(tr("5"),5);
    intervalCombo->addItem(tr("10"),10);

    QLabel *pintervalUnitLabel = new QLabel(this);
    pintervalUnitLabel->setText(tr("Min"));

    items->addWidget(pintervalLabel,index,0,1,1,Qt::AlignRight);
    items->addWidget(intervalCombo,index,1,1,1);
    items->addWidget(pintervalUnitLabel,index,2,1,1);

    QLabel *pnameLabel = new QLabel(this);
    pnameLabel->setText(tr("name:"));
    pnameEdit = new QLineEdit(this);

    index++;
    items->addWidget(pnameLabel,index,0,1,1,Qt::AlignRight);
    items->addWidget(pnameEdit,index,1,1,1);

    pbar = new QProgressBar(this);

    index++;
    pbar->setMinimumWidth(250);
    items->addWidget(pbar,index,0,1,3,Qt::AlignRight);
    pbar->setVisible(false);

    QVBoxLayout* playout = new QVBoxLayout(this);
    playout->addLayout(items);
    //playout->setContentsMargins(10,10,10,10);

    QHBoxLayout *btnlayout = new QHBoxLayout();
    btnlayout->addStretch();

    pokbtn = new QPushButton(this);
    connect(pokbtn, SIGNAL(clicked()),this, SLOT(ok()));
    pokbtn->setText(tr("ok"));
    btnlayout->addWidget(pokbtn);

    pcancelbtn = new QPushButton(this);
    connect(pcancelbtn, SIGNAL(clicked()),this, SLOT(cancel()));
    pcancelbtn->setText(tr("cancel"));
    btnlayout->addWidget(pcancelbtn);

    playout->addLayout(btnlayout);
    this->setLayout(playout);

    timer = nullptr;

    pnameEdit->setFocus();
}

void RecordWaveWidget::timeUpdate()
{
    QDateTime dt = QDateTime::currentDateTime();
    int secs = initTime.secsTo(dt);
    if(secs <= recv_secs)
    {
        float percentage = ((float)secs)/(recv_secs);
        percentage *= 100.0;
        pbar->setValue(percentage);
    }
    else
    {
        pbar->setValue(100);
        timer->stop();
        delete timer;
        timer = nullptr;
        this->close();
    }
}

void RecordWaveWidget::ok()
{
    QString udata = intervalCombo->itemData(intervalCombo->currentIndex()).toString();
    QString name = pnameEdit->text().trimmed();
    if(name.length() == 0)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter data!"));
        return;
    }

    bool tok;
    float recv_f = udata.toFloat(&tok);
    recv_secs = recv_f * 60;
    emit recorwWaveWithInfo(recv_secs,name);

    initTime = QDateTime::currentDateTime();
    pbar->setVisible(true);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeUpdate()));
    timer->start(BASE_INTERVAL * 2);
    pcancelbtn->setEnabled(false);
    pokbtn->setEnabled(false);
}

void RecordWaveWidget::cancel()
{
    this->close();
}
