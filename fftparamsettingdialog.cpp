#include "fftparamsettingdialog.h"

#include <QGridLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>

#include <QMessageBox>
#include "globalvariable.h"

FFTParamSettingDialog::FFTParamSettingDialog(QDialog *parent) : QDialog(parent)
{
    this->setMaximumSize(300,200);
    this->setMinimumSize(300,200);

    int index = 0;
    QGridLayout* layout = new QGridLayout();
    layout->setAlignment(Qt::AlignCenter);

    QLabel* f_l_Label = new QLabel(this);
    f_l_Label->setText(tr("FFT filter low:"));
    freqdownthreshold = new QLineEdit(this);
    freqdownthreshold->setText(QString::number(GlobalVariable::fft_params.filter_lower,10,4));

    layout->addWidget(f_l_Label,index,0,1,1,Qt::AlignRight);
    layout->addWidget(freqdownthreshold,index,1,1,1,Qt::AlignLeft);

    QLabel* f_u_Label = new QLabel(this);
    f_u_Label->setText(tr("FFT filter high:"));
    frequpthreshold = new QLineEdit(this);
    frequpthreshold->setText(QString::number(GlobalVariable::fft_params.filter_up,10,4));

    index++;
    layout->addWidget(f_u_Label,index,0,1,1,Qt::AlignRight);
    layout->addWidget(frequpthreshold,index,1,1,1,Qt::AlignLeft);

    QLabel* f_window_Label = new QLabel(this);
    f_window_Label->setText(tr("FFT Window Type:"));
    fft_windowCB = new QComboBox(this);

    fft_windowCB->addItem(tr("Rectangle"),1);
    fft_windowCB->addItem(tr("Hanning"),2);
    fft_windowCB->addItem(tr("Hamming"),3);
    fft_windowCB->addItem(tr("Blackman"),4);

    index++;
    layout->addWidget(f_window_Label,index,0,1,1,Qt::AlignRight);
    layout->addWidget(fft_windowCB,index,1,1,1,Qt::AlignLeft);

    switch (GlobalVariable::fft_params.fft_w) {
    case WINDOW_RECT:
        fft_windowCB->setCurrentIndex(0);
        break;
    case WINDOW_HANNING:
        fft_windowCB->setCurrentIndex(1);
        break;
    case WINDOWN_HAMMING:
        fft_windowCB->setCurrentIndex(2);
        break;
    case WINDOW_BLACKMAN:
        fft_windowCB->setCurrentIndex(3);
        break;
    default:
        break;
    }

    QLabel* f_filter_Label = new QLabel(this);
    f_filter_Label->setText(tr("FFT Filter Type:"));
    fft_filterCB = new QComboBox(this);

    fft_filterCB->addItem(tr("LowPass"),1);
    fft_filterCB->addItem(tr("HighPass"),2);
    fft_filterCB->addItem(tr("BandPass"),3);
    fft_filterCB->addItem(tr("BandStopPass"),4);

    index++;
    layout->addWidget(f_filter_Label,index,0,1,1,Qt::AlignRight);
    layout->addWidget(fft_filterCB,index,1,1,1,Qt::AlignLeft);

    switch (GlobalVariable::fft_params.fft_filter) {
    case LowPassFilter:
        fft_filterCB->setCurrentIndex(0);
        break;
    case HghPassFilter:
        fft_filterCB->setCurrentIndex(1);
        break;
    case BndPassFilter:
        fft_filterCB->setCurrentIndex(2);
        break;
    case BndStopFilter:
        fft_filterCB->setCurrentIndex(3);
        break;
    default:
        break;
    }

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(layout);

    QHBoxLayout *btn = new QHBoxLayout(this);
    btn->addStretch();

    QPushButton *pokbtn = new QPushButton(this);
    connect(pokbtn, SIGNAL(clicked(bool)),this, SLOT(okbtn(bool)));
    pokbtn->setText(tr("ok"));
    btn->addWidget(pokbtn);

    QPushButton *pcancelbtn = new QPushButton(this);
    connect(pcancelbtn, SIGNAL(clicked(bool)),this, SLOT(cancelbtn(bool)));
    pcancelbtn->setText(tr("cancel"));
    btn->addWidget(pcancelbtn);

    mainLayout->addLayout(btn);

    setLayout(mainLayout);
}

void FFTParamSettingDialog::okbtn(bool)
{
    QString freq_l_str = freqdownthreshold->text().trimmed();
    QString freq_u_str = frequpthreshold->text().trimmed();
    if(freq_l_str.length() == 0 || freq_u_str.length() == 0)
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter data!"));
        return;
    }
    float freq_l_i,freq_u_i;
    bool tok;
    freq_l_i = freq_l_str.toFloat(&tok);
    if(tok)
    {
        if(freq_l_i>0 || qAbs(freq_l_i+1.0) < 0.00001)
        {
            GlobalVariable::fft_params.filter_lower = freq_l_i;
        }
        else
        {
            QMessageBox::information(this, tr("Infomation"), tr("Please enter data!"));
            freqdownthreshold->setFocus();
            return;
        }
    }
    else
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter data!"));
        freqdownthreshold->setFocus();
        return;
    }

    freq_u_i = freq_u_str.toFloat(&tok);
    if(tok)
    {
        if(freq_u_i>0 || qAbs(freq_u_i+1.0) < 0.00001)
        {
            GlobalVariable::fft_params.filter_up = freq_u_i;
        }
        else
        {
            QMessageBox::information(this, tr("Infomation"), tr("Please enter data!"));
            freqdownthreshold->setFocus();
            return;
        }
    }
    else
    {
        QMessageBox::information(this, tr("Infomation"), tr("Please enter data!"));
        frequpthreshold->setFocus();
        return;
    }

    int fft_window_type = fft_windowCB->itemData(fft_windowCB->currentIndex()).toString().toInt(&tok);
    FFT_WINDOW fw = (FFT_WINDOW)fft_window_type;
    GlobalVariable::fft_params.fft_w = fw;

    int fft_filter_type = fft_filterCB->itemData(fft_filterCB->currentIndex()).toString().toInt(&tok);
    FFT_FilterType ff = (FFT_FilterType)fft_filter_type;
    GlobalVariable::fft_params.fft_filter = ff;

    this->close();
}

void FFTParamSettingDialog::cancelbtn(bool)
{
    this->close();
}



