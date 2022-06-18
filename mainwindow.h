#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <anotherwindow.h>
#include <spectrumwindow.h>
#include <qcustomplot.h>
#include <QTcpSocket>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QTcpSocket* socket;
    QByteArray Data;

public slots:
    void sockReady();
    void sockDisc();
    void recieveDataToMain(QVector<double> x,QVector<double> y);


private slots:
    void on_pushButton_clicked();
    QVector<double> MsV(QVector<double>);
    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

//    void on_pushButton_6_clicked();

    void onXRangeChanged(const QCPRange &range);

    void on_spinBox_valueChanged(int arg1);

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

signals:
    void sendData(QVector<double> x,QVector<double> y, double deltaT);

private:
    Ui::MainWindow *ui;

    AnotherWindow *sWindow;
    SpectrumWindow *spWindow;
   // SecondSpectrum *secSpWindow;

    QCustomPlot *customPlot;    // Объявляем графическое полотно

    QCPGraph *graphic;          // Объявляем график

    QVector<double> x, y; //Массивы координат точек
       QString str;
    double sr;

    int CountOfDot;
    double deltaT;
};


#endif // MAINWINDOW_H
