#ifndef FFTPARAMSETTINGDIALOG_H
#define FFTPARAMSETTINGDIALOG_H

#include <QDialog>

class QLineEdit;
class QComboBox;

class FFTParamSettingDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FFTParamSettingDialog(QDialog *parent = 0);

private:
    QLineEdit *frequpthreshold;
    QLineEdit *freqdownthreshold;

    QComboBox *fft_windowCB;
    QComboBox *fft_filterCB;

private slots:

    void cancelbtn(bool);
    void okbtn(bool);
};

#endif // FFTPARAMSETTINGDIALOG_H
