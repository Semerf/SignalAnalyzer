#ifndef SPECTRUMWINDOW_H
#define SPECTRUMWINDOW_H

#include <QWidget>
#include <qcustomplot.h>
#include "ipp.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SpectrumWindow; }
QT_END_NAMESPACE

class SpectrumWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SpectrumWindow(QWidget *parent = 0);
    ~SpectrumWindow();

public slots:
    void recieveData(QVector<double> x,QVector<double> y,  double deltaT);
    void recieveDataToMain(QVector<double> x,QVector<double> y);

signals:
    void firstWindow();  // Сигнал для первого окна на открытие
    void anotherWindowOp();  // Сигнал для 2-го окна на открытие
    void secSpectrumWindowOp();  // Сигнал для 4-го окна на открытие

private slots:
    // Слот-обработчик нажатия кнопки
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
//    void on_pushButton_4_clicked();


private:
    Ui::SpectrumWindow *ui;

    QCustomPlot *customPlot;    // Объявляем графическое полотно

    QCPGraph *graphic;          // Объявляем график
    Ipp64fc *dst;
    int amount;

};

#endif // SPECTRUMWINDOW_H
