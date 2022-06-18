#include "anotherwindow.h"
#include "ui_anotherwindow.h"
#include "QFile"
#include "ipp.h"

AnotherWindow::AnotherWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnotherWindow)
{
    ui->setupUi(this);
    customPlot = new QCustomPlot(); // Инициализируем графическое полотно
    ui->gridLayout->addWidget(customPlot);  // Устанавливаем customPlot в окно проложения
}

void AnotherWindow::onXRangeChanged(const QCPRange &range)
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

AnotherWindow::~AnotherWindow()
{
    delete ui;
}

void AnotherWindow::on_pushButton_clicked()
{
    QTextStream out(stdout);
    out << "x[0] = " << x[0];
    emit sendDataToMain(x,y); // вызываем сигнал, в котором передаём данные
    this->close();      // Закрываем окно
    emit firstWindow(); // И вызываем сигнал на открытие главного окна
}

void AnotherWindow::on_pushButton_3_clicked()
{
    this->close();      // Закрываем окно
    emit sendDataToMain(x,y); // вызываем сигнал, в котором передаём данные
    emit spectrumWindowOp(); // И вызываем сигнал на открытие окна спектра
}

/*void AnotherWindow::on_pushButton_4_clicked()
{
    this->close();      // Закрываем окно
    emit secSpectrumWindowOp(); // И вызываем сигнал на открытие окна спектра огибающей
}*/

void AnotherWindow::recieveData(QVector<double> x,QVector<double> y,double deltaT)
{
    this->x = x;
    this->y = y;
    WasWorked = false;
    this->DeltaT = deltaT;
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


void AnotherWindow::on_pushButton_5_clicked()
{
    if(WasWorked == true)
    {
        return ;
    }

    double sum_push2=0;
    double max_push2=-1000;
    double sr_push2 = 0;
    int length = x.size();
    WasWorked =true;
      int amount = x.size();

      double *pSrc = new double[amount];
      double *pDst = new double[amount];
      bool okL = true;
      bool okH = true;

      for(int i = 0; i<amount; i++)
      {
          pSrc[i]=y[i];
      }
      int iCount = amount ;
      int iFilterCoeffCount = (ui->lineEdit_3->text()).toInt();
      double dLowFreq = (ui->lineEdit->text()).toDouble(&okL);
      double dHighFreq = (ui->lineEdit_2->text()).toDouble(&okH);
      double dSampleRate = 1/DeltaT;

    Ipp64f* pTaps = new double[iFilterCoeffCount];



    Ipp64f rLowFreq = dLowFreq/dSampleRate;
    Ipp64f rHighFreq = dHighFreq/dSampleRate;
    int BufSize;
    int BufBandSize;
    int SpecSize;
    IppStatus status;
    IppsFIRSpec_64f *pSpec;
    Ipp8u* pBuf;
    Ipp8u* pBufBand;



    double* pDlyS = ippsMalloc_64f(iFilterCoeffCount-1);
    double* pDlyD = ippsMalloc_64f(iFilterCoeffCount-1);

    for(int i = 0; i < iFilterCoeffCount-1;i++)
    {
        pDlyS[i]=0;
        pDlyD[i]=0;
    }


    status = ippsFIRSRGetSize(iFilterCoeffCount, ipp64f, &SpecSize, &BufSize);

    ippsFIRGenGetBufferSize(iFilterCoeffCount, &BufBandSize);

    pSpec = (IppsFIRSpec_64f*)ippsMalloc_8u(SpecSize);
    pBuf = ippsMalloc_8u(BufSize);
    pBufBand = ippsMalloc_8u(BufBandSize);

    if((okH != 0)&&(okL != 0)) status = ippsFIRGenBandpass_64f(rLowFreq, rHighFreq, pTaps, iFilterCoeffCount, ippWinHann, ippFalse, pBufBand);


    status = ippsFIRSRInit_64f(pTaps, iFilterCoeffCount, ippAlgDirect, pSpec);


    status = ippsFIRSR_64f(pSrc, pDst, iCount, pSpec, NULL, NULL, pBuf);

    for(int i = 0; i<amount; i++)
    {
        y[i]=pDst[i];
    }


    for(int i = 0; i <length; i++ )
    {
        sum_push2+=y[i];
        if(y[i]>max_push2)
        {
            max_push2 = y[i];
        }
    }

    sr_push2 = sum_push2/x.size();

    QString istr = QString::number(sr_push2);
    ui->label_4->setText("Среднее значение = " + istr);

    double diskr = 1.0/(DeltaT);
    QString diskrStr = QString::number(diskr);
    ui->label_5->setText("Частота дискретизации = " + diskrStr + " Гц");

    double KSum =0;
    double KZG;
    for(int i = 0; i <length; i++ )
    {
         KSum+=(y[i]-sr_push2)*(y[i]-sr_push2);
    }
    KZG=KSum/length;
    KZG = pow(KZG,0.5);
    QString KZGStr = QString::number(KZG);
    ui->label_6->setText("Среднеквадратичное значение = " + KZGStr);

    double PF = max_push2/KZG;
    QString PFStr = QString::number(PF);
    ui->label_7->setText("Пик фактор = " + PFStr);





    //status = ippsFIRMRGetSize(iCount, dHighFreq, dLowFreq,ipp64f, &SpecSize, &BufSize);
   //ippsFIRMRGetSize(tapsLen, upFactor, downFactor, ipp32f, &specSize, &bufSize )
    //Вывод статуса операции...

    //status = ippsFIRMRInit_64f ( pTaps,iCount, upFactor, upPhase, downFactor, downPhase, pSpec);


    //Отрисовка графика
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

QVector<double> AnotherWindow::MsV(QVector<double> x)
{
    QVector<double> newX;
    int size = x.size();
    for(int i =0 ; i < size; i++)
    {
        newX.push_back(x[i]*1000);
    }
    return newX;
}
