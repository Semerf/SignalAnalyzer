#include "secondspectrum.h"
#include "ui_SecondSpectrum.h"
#include "QFile"

SecondSpectrum::SecondSpectrum(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SecondSpectrum)
{
    ui->setupUi(this);
    customPlot = new QCustomPlot(); // Инициализируем графическое полотно
    ui->gridLayout->addWidget(customPlot);  // Устанавливаем customPlot в окно проложения
}

SecondSpectrum::~SecondSpectrum()
{
    delete ui;
}

void SecondSpectrum::on_pushButton_clicked()
{
    this->close();      // Закрываем окно
    emit firstWindow(); // И вызываем сигнал на открытие главного окна
}

void SecondSpectrum::on_pushButton_2_clicked()
{
    this->close();      // Закрываем окно
    emit anotherWindowOp(); // И вызываем сигнал на открытие главного окна
}

void SecondSpectrum::on_pushButton_3_clicked()
{
    this->close();      // Закрываем окно
    emit spectrumWindowOp(); // И вызываем сигнал на открытие главного окна
}


void SecondSpectrum::recieveData(QVector<double> x,QVector<double> y, double deltaT)
{
    ui->label->setText(QString::number(x[10]));
    ui->label_2->setText(QString::number(y[10]));
}
