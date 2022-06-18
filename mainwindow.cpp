#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFile"
#include <QSizePolicy>
#include <QVBoxLayout>
#include "ipp.h"
#include "math.h"

int i = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    customPlot = new QCustomPlot(); // Инициализируем графическое полотно
    ui->gridLayout->addWidget(customPlot);  // Устанавливаем customPlot в окно проложения

    // Инициализируем второе окно
    sWindow = new AnotherWindow();
    // подключаем к слоту запуска главного окна по кнопке во втором окне
    connect(sWindow, &AnotherWindow::firstWindow, this, &MainWindow::show);

    spWindow = new SpectrumWindow();
    // подключаем к слоту запуска главного окна по кнопке в 3-м окне
    connect(spWindow, &SpectrumWindow::firstWindow, this, &MainWindow::show);

   /* secSpWindow = new SecondSpectrum();
    // подключаем к слоту запуска главного окна по кнопке в 4-м окне
    connect(secSpWindow, &SecondSpectrum::firstWindow, this, &MainWindow::show);*/

    //Подключаем к 2-му окну события открытия остальных окон.
    connect(sWindow, &AnotherWindow::spectrumWindowOp, spWindow, &SpectrumWindow::show);
   // connect(sWindow, &AnotherWindow::secSpectrumWindowOp, secSpWindow, &SecondSpectrum::show);

    //Подключаем к 3-му окну события открытия остальных окон.
    connect(spWindow, &SpectrumWindow::anotherWindowOp, sWindow, &AnotherWindow::show);
    //connect(spWindow, &SpectrumWindow::secSpectrumWindowOp, secSpWindow, &SecondSpectrum::show);


    //Подключаем к 4-му окну события открытия остальных окон.
   // connect(secSpWindow, &SecondSpectrum::anotherWindowOp, sWindow, &AnotherWindow::show);
    //connect(secSpWindow, &SecondSpectrum::spectrumWindowOp, spWindow, &SpectrumWindow::show);

    connect(this, SIGNAL(sendData(QVector<double>,QVector<double>, double)), sWindow, SLOT(recieveData(QVector<double>,QVector<double>, double))); // подключение сигнала к слоту нашей формы
    connect(this, SIGNAL(sendData(QVector<double>,QVector<double>, double)), spWindow, SLOT(recieveData(QVector<double>,QVector<double>, double))); // подключение сигнала к слоту нашей формы
    //connect(this, SIGNAL(sendData(QVector<double>,QVector<double>, double)), secSpWindow, SLOT(recieveData(QVector<double>,QVector<double>, double))); // подключение сигнала к слоту нашей формы

    connect(sWindow, SIGNAL(sendDataToMain(QVector<double>,QVector<double>)), this, SLOT(recieveDataToMain(QVector<double>,QVector<double>)));
    connect(sWindow, SIGNAL(sendDataToMain(QVector<double>,QVector<double>)), spWindow, SLOT(recieveDataToMain(QVector<double>,QVector<double>)));

    socket = new QTcpSocket(this);
            connect(socket,SIGNAL(readyRead()),this,SLOT(sockReady()));
            connect(socket,SIGNAL(disconnected()),this,SLOT(sockDisc()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::sockDisc()
{
    socket->deleteLater();
}

void MainWindow::sockReady()
{
    if (socket->waitForConnected(500))
    {
        QTextStream out(stdout);
        socket->waitForReadyRead(500);
        Data = socket->readAll();
        QString DataAsString = QString(Data);
        QStringList lst = DataAsString.split(QRegExp("[\t\n]"), QString::SkipEmptyParts);
        for(int a = 0; a<256; a++)
        {
            if(a%2==0) x.push_back(lst[a].toDouble());
            else y.push_back(lst[a].toDouble());
            out<<lst[a];
            out << "\t";
        }
        out << "\n";
    }
}





    void MainWindow::on_pushButton_4_clicked()
{
   if(x.isEmpty())
   {
        ui->label_13->setText("Ошибка! Данные отсутствуют для построения спектра!");
   }
   else
   {
        sWindow->show();  // Показываем второе окно
        emit sendData(x,y,deltaT); // вызываем сигнал, в котором передаём данные
        this->close();    // Закрываем основное окно
   }

}

void MainWindow::on_pushButton_5_clicked()
{
    if(x.isEmpty())
    {
        ui->label_13->setText("Ошибка! Данные отсутствуют для построения спектра!");
    }
    else
    {
        spWindow->show();  // Показываем второе окно
        emit sendData(x,y, deltaT); // вызываем сигнал, в котором передаём данные
        this->close();    // Закрываем основное окно
    }
}


/*void MainWindow::on_pushButton_6_clicked()
{
    if(x.isEmpty())
    {
        ui->label_13->setText("Ошибка! Данные отсутствуют для построения спектра!");
    }
    else
    {
        secSpWindow->show();  // Показываем 4-е окно
        //emit sendData(x,y); // вызываем сигнал, в котором передаём данные
        this->close();    // Закрываем основное окно
    }
}*/


void MainWindow::on_pushButton_clicked()
{
    x.clear();
    y.clear();
    QTextStream out(stdout);
    ippInit();                      /* Initialize Intel(R) IPP library */
    out << "Lib name of ipp is = "<<ippGetLibVersion()->Name;/* Get Intel(R) IPP library version info */
    out << "Lib version of ipp is = "<<ippGetLibVersion()->Version;/* Get Intel(R) IPP library version info */
        int i = 0;
        double sum = 0;
        double max = -1000;
       QFile file("D:/Study/Work/2.txt");

       if (file.open(QIODevice::ReadOnly))
       {
           QTextStream in(&file);

           while (!in.atEnd())
           {
               ui->label->setText("Файл открыт");
               str = in.readLine();
               QStringList lst = str.split("\t", QString::SkipEmptyParts);
               x.push_back(lst[0].toDouble());
               y.push_back(lst[1].toDouble());
               if(y[i]>max)
                   max=y[i];
               sum+=y[i];
               i++;
           }
           file.close();
           CountOfDot = i;
            sr = sum/i;
           out << "The size of the vector is: " << x.size() << endl;
           out << "Value of sr is: " << sr << endl;
           QString istr = QString::number(sr);
           out << "Value of istr is: " << istr << endl;
           ui->label_2->setText("Среднее значение = " + istr);
           deltaT = (x[1000]-x[0])/1000.0;
           double diskr = 1.0/(deltaT);
           QString diskrStr = QString::number(diskr);
           ui->label_3->setText("Частота дискретизации = " + diskrStr + " Гц");


        }
       i=0;
       double KSum =0;
       double KZG;
       for(int itx: qAsConst(x))
       {
            KSum+=(y[i]-sr)*(y[i]-sr);
            i++;
       }
       KZG=KSum/i;
       KZG = pow(KZG,0.5);
       QString KZGStr = QString::number(KZG);
       ui->label_4->setText("Среднеквадратичное значение = " + KZGStr);

       double PF = max/KZG;
       QString PFStr = QString::number(PF);
       ui->label_5->setText("Пик фактор = " + PFStr);

       int size=0;
       for(int itx: qAsConst(x))
       {
           y[size]-=sr;
           size++;
       }

       //customPlot->setSizePolicy ( QsizePolicy::Preferred, QsizePolicy::Preferred );


       customPlot->setInteraction(QCP::iRangeZoom,true);   // Включаем взаимодействие удаления/приближения
        customPlot->setInteraction(QCP::iRangeDrag, true);  // Включаем взаимодействие перетаскивания графика
       //ui-> widget->axisRect()->setRangeDrag(Qt::Horizontal);   // Включаем перетаскивание только по горизонтальной оси



       customPlot->clearGraphs();//Если нужно, но очищаем все графики
       //Добавляем один график в widget
       customPlot->addGraph();
       //Говорим, что отрисовать нужно график по нашим двум массивам x и y
       customPlot->graph(0)->setData(MsV(x), y);

       customPlot->graph(0)->setPen(QColor(50, 50, 50, 255));//задаем цвет точек
       //формируем вид точек
       customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 1));

       //Подписываем оси Ox и Oy
       customPlot->xAxis->setLabel("Время, мс");
       customPlot->yAxis->setLabel("A");

       //Установим область, которая будет показываться на графике
      // ui->widget->xAxis->setRange(0, 100);//Для оси Ox
      // ui->widget->yAxis->setRange(-100, 100);//Для оси Oy
       customPlot->rescaleAxes();

        connect(customPlot->xAxis,    SIGNAL(rangeChanged(QCPRange)), this,   SLOT(onXRangeChanged(QCPRange)));




       //И перерисуем график на нашем widget
       customPlot->replot();
}

void MainWindow::onXRangeChanged(const QCPRange &range)
{
    QCPRange boundedRange = range;
    double lowerRangeBound = 0;
    double size = boundedRange.size();

    if(boundedRange.lower < lowerRangeBound) {

        boundedRange.lower = 0;
        boundedRange.upper = size;
   }

    customPlot->xAxis->setRange(boundedRange);
}


void MainWindow::on_pushButton_2_clicked()
{
    double sum_push2=0;
    double max_push2=-1000;
    double sr_push2 = 0;
    int length = x.size();
    QTextStream out(stdout);
    out << "The size of the vector is: " << x.size() << endl;

    QString Astr = ui->lineEdit->text();
    out <<"ASTR = "<< Astr<<endl;
    double A = Astr.toDouble();

    QString WUstr = ui->lineEdit_2->text();
    out <<"WU = "<< WUstr<<endl;
    double WU = WUstr.toDouble();


    if(A == NULL || WU == NULL)
    {
        out << "Warning!"<<endl;
    }

    WU /= 0.1591549;

    double result;

    for(int i = 0; i <length; i++ )
    {
        result = A * sin(x[i]*WU);
        y[i]+=result;
        sum_push2+=y[i];
        if(result>max_push2)
        {
            max_push2 = result;
        }
    }

    sr_push2 = sum_push2/x.size();

    QString istr = QString::number(sr_push2);
    out << "Value of istr is: " << istr << endl;
    ui->label_2->setText("Среднее значение = " + istr);

    double diskr = 1.0/(deltaT);
    QString diskrStr = QString::number(diskr);
    ui->label_3->setText("Частота дискретизации = " + diskrStr + " Гц");

    double KSum =0;
    double KZG;
    for(int i = 0; i <length; i++ )
    {
         KSum+=(y[i]-sr_push2)*(y[i]-sr_push2);
    }
    KZG=KSum/length;
    KZG = pow(KZG,0.5);
    QString KZGStr = QString::number(KZG);
    ui->label_4->setText("Среднеквадратичное значение = " + KZGStr);

    double PF = max_push2/KZG;
    QString PFStr = QString::number(PF);
    ui->label_5->setText("Пик фактор = " + PFStr);


    customPlot->setInteraction(QCP::iRangeZoom,true);   // Включаем взаимодействие удаления/приближения
     customPlot->setInteraction(QCP::iRangeDrag, true);  // Включаем взаимодействие перетаскивания графика
    //ui-> widget->axisRect()->setRangeDrag(Qt::Horizontal);   // Включаем перетаскивание только по горизонтальной оси



    customPlot->clearGraphs();//Если нужно, но очищаем все графики
    //Добавляем один график в widget
    customPlot->addGraph();
    //Говорим, что отрисовать нужно график по нашим двум массивам x и y
    customPlot->graph(0)->setData(MsV(x), y);

    customPlot->graph(0)->setPen(QColor(50, 50, 50, 255));//задаем цвет точек
    //формируем вид точек
    customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 1));

    //Подписываем оси Ox и Oy
    customPlot->xAxis->setLabel("Время, мс");
    customPlot->yAxis->setLabel("A");

    //Установим область, которая будет показываться на графике
   // ui->widget->xAxis->setRange(0, 100);//Для оси Ox
   // ui->widget->yAxis->setRange(-100, 100);//Для оси Oy
    customPlot->rescaleAxes();

     connect(customPlot->xAxis,    SIGNAL(rangeChanged(QCPRange)), this,   SLOT(onXRangeChanged(QCPRange)));




    //И перерисуем график на нашем widget
    customPlot->replot();
}






void MainWindow::on_spinBox_valueChanged(int arg1)
{
    CountOfDot=pow(2,arg1);
    ui->label_12->setText(" = " + QString::number(CountOfDot));

}


void MainWindow::on_pushButton_7_clicked()
{
    if(x.isEmpty())
    {
        QTextStream out(stdout);
        QString deltaStr = ui->lineEdit_3->text();
        out <<"delta t = "<< deltaStr<<endl;
        out <<"CountOfDot = "<< QString::number(CountOfDot)<<endl;
        deltaT = deltaStr.toDouble();

        for(int i = 0; i<CountOfDot; i++)
        {
            x.push_back(i*deltaT);
            y.push_back(0);
        }
        out <<"Dots x added "<<endl;
    }
    else
    {
        ui->label_13->setText("Ошибка! Данные времени уже записаны!");
    }

}
void MainWindow::on_pushButton_8_clicked()
{
    x.clear();
    y.clear();
    customPlot->clearGraphs();//Если нужно, но очищаем все графики
    //И перерисуем график на нашем widget
    customPlot->replot();
}


void MainWindow::on_pushButton_9_clicked()
{
    socket->connectToHost("127.0.0.1",9091);
}


void MainWindow::recieveDataToMain(QVector<double> a,QVector<double> b)
{
    this->x = a;
    this->y = b;
    QTextStream out(stdout);

    out << "a[0] = " << a[0];

    customPlot->setInteraction(QCP::iRangeZoom,true);   // Включаем взаимодействие удаления/приближения
     customPlot->setInteraction(QCP::iRangeDrag, true);  // Включаем взаимодействие перетаскивания графика

    customPlot->clearGraphs();//Если нужно, но очищаем все графики
    //Добавляем один график в widget
    customPlot->addGraph();
    //Говорим, что отрисовать нужно график по нашим двум массивам x и y
     customPlot->graph(0)->setData(MsV(x), y);

    customPlot->graph(0)->setPen(QColor(50, 50, 50, 255));//задаем цвет точек
    //формируем вид точек
    customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 1));

    //Подписываем оси Ox и Oy
    customPlot->xAxis->setLabel("Время, мс");
    customPlot->yAxis->setLabel("A");

    //Установим область, которая будет показываться на графике
    customPlot->rescaleAxes();

     connect(customPlot->xAxis,SIGNAL(rangeChanged(QCPRange)), this,SLOT(onXRangeChanged(QCPRange)));

    //И перерисуем график на нашем widget
    customPlot->replot();
}


    QVector<double> MainWindow::MsV(QVector<double> x)
    {
        QVector<double> newX;
        int size = x.size();
        for(int i =0 ; i < size; i++)
        {
            newX.push_back(x[i]*1000);
        }
        return newX;
    }
