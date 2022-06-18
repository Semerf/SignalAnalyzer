#ifndef ANOTHERWINDOW_H
#define ANOTHERWINDOW_H

#include <QWidget>
#include <qcustomplot.h>
#include "ipp.h"

QT_BEGIN_NAMESPACE
namespace Ui { class AnotherWindow; }
QT_END_NAMESPACE

class AnotherWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AnotherWindow(QWidget *parent = 0);
    ~AnotherWindow();

public slots:
    void recieveData(QVector<double> x,QVector<double> y, double deltaT);

signals:
    void firstWindow();  // Сигнал для первого окна на открытие
    void spectrumWindowOp();  // Сигнал для 3-го окна на открытие
    void secSpectrumWindowOp();  // Сигнал для 4-го окна на открытие
    void sendDataToMain(QVector<double> x,QVector<double> y);

private slots:
    // Слот-обработчик нажатия кнопки
    void on_pushButton_clicked();
    void on_pushButton_3_clicked();
    QVector<double> MsV(QVector<double>);
    void onXRangeChanged(const QCPRange &range);
  //  void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

private:
    Ui::AnotherWindow *ui;
    QVector<double> x, y;
    double DeltaT;
    QCustomPlot *customPlot;    // Объявляем графическое полотно
    bool WasWorked = false;
    QCPGraph *graphic;          // Объявляем график
};

#endif // ANOTHERWINDOW_H
