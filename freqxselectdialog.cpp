#include "freqxselectdialog.h"

#include <QVBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QStandardItemModel>
#include <QLabel>
#include <QComboBox>

#include <QMessageBox>

FreqXSelectDialog::FreqXSelectDialog(QDialog *parent):QDialog(parent)
{
    this->setMaximumSize(250,330);
    this->setMinimumSize(250,330);
    QVBoxLayout* layout = new QVBoxLayout(this);

    listView = new QListWidget(this);

    QStringList strList;
    strList.append("0.5X");
    strList.append("1X");
    strList.append("2X");
    strList.append("3X");
    strList.append("4X");
    strList.append("5X");
    strList.append("6X");
    strList.append("7X");
    strList.append("8X");
    strList.append("9X");
    strList.append("10X");

    listView->addItems(strList);
    listView->setSelectionBehavior(QAbstractItemView::SelectItems);
    listView->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(listView,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(onItemDoubleClicked(QListWidgetItem*)));

    QPushButton *btnEnter = new QPushButton(tr("OK"), this);
    btnEnter->setMaximumWidth(100);
    connect(btnEnter, SIGNAL(clicked()),this, SLOT(ok()));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(btnEnter,0,Qt::AlignRight);

    QLabel* info = new QLabel(this);
    info->setText(tr("Please select a item:"));
    curTypeCB = new QComboBox(this);
    layout->addWidget(info);
    layout->addWidget(curTypeCB);
    layout->addWidget(listView);
    layout->addLayout(buttonLayout);
    setLayout(layout);
}

void FreqXSelectDialog::onItemDoubleClicked(QListWidgetItem* item)
{
    //QString text = item->text();
    int rIndex = listView->currentRow();
    int tn = rIndex;
    int curType = curTypeCB->currentIndex();
    QString title = "";
    if(0 == mode)
    {
        if(0==curType)
        {
            tn += 150;
            title = tr("Acc Freqency");
        }
        else
        {
            tn += 170;
            title = tr("Speed Freqency");
        }
    }
    else if(1 == mode)
    {
        if(0==curType)
        {
            tn += 190;
            title = tr("Voltage Freqency");
        }
        else
        {
            tn += 210;
            title = tr("Current Freqency");
        }
    }

    if(rIndex>0)
    {
        title += " " + QString::number(rIndex) + "X";
    }
    else
    {
        title += " 0.5X";
    }
    title += tr(" Trend");
    TREENODETYPE tnt = (TREENODETYPE)tn;
    emit itemSelected(tnt,title);
    this->close();
}

void FreqXSelectDialog::ok()
{
    if(listView->currentItem() != nullptr)
    {
        //QString text = listView->currentItem()->text();
        int rIndex = listView->currentRow();
        int tn = rIndex;
        int curType = curTypeCB->currentIndex();
        QString title = "";
        if(0 == mode)
        {
            if(0==curType)
            {
                tn += 150;
                title = tr("Acc Freqency");
            }
            else
            {
                tn += 170;
                title = tr("Speed Freqency");
            }
        }
        else if(1 == mode)
        {
            if(0==curType)
            {
                tn += 190;
                title = tr("Voltage Freqency");
            }
            else
            {
                tn += 210;
                title = tr("Current Freqency");
            }
        }

        if(rIndex>0)
        {
            title += " " + QString::number(rIndex) + "X";
        }
        else
        {
            title += " 0.5X";
        }
        title += tr(" Trend");
        TREENODETYPE tnt = (TREENODETYPE)tn;
        emit itemSelected(tnt,title);
        this->close();
    }
    else
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please select a item!"));
    }
}

void FreqXSelectDialog::setMode(int mode)
{
    this->mode = mode;
    curTypeCB->clear();
    if(0 == mode)
    {
        curTypeCB->addItem(tr("Acc Freqency"),0);
        curTypeCB->addItem(tr("Speed Freqency"),1);
        curTypeCB->setCurrentIndex(0);
    }
    else if(1 == mode)
    {
        curTypeCB->addItem(tr("Voltage Freqency"),0);
        curTypeCB->addItem(tr("Current Freqency"),1);
        curTypeCB->setCurrentIndex(0);
    }
}
