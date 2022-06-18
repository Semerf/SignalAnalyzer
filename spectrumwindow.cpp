#include "spectrumwindow.h"
#include "ui_spectrumwindow.h"
#include "ipp.h"
#include "math.h"
#include "QFile"

SpectrumWindow::SpectrumWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SpectrumWindow)
{
    ui->setupUi(this);
    customPlot = new QCustomPlot(); // Инициализируем графическое полотно
    ui->gridLayout->addWidget(customPlot);  // Устанавливаем customPlot в окно проложения
}

SpectrumWindow::~SpectrumWindow()
{
    delete ui;
}

void SpectrumWindow::on_pushButton_clicked()
{
    this->close();      // Закрываем окно
    emit firstWindow(); // И вызываем сигнал на открытие главного окна
}
void SpectrumWindow::on_pushButton_2_clicked()
{
    this->close();      // Закрываем окно
    emit anotherWindowOp(); // И вызываем сигнал на открытие 2-го окна
}
/*void SpectrumWindow::on_pushButton_4_clicked()
{
    this->close();      // Закрываем окно
    emit secSpectrumWindowOp(); // И вызываем сигнал на открытие 4-го окна
}*/

void SpectrumWindow::recieveData(QVector<double> x,QVector<double> y, double deltaT)
{
    QTextStream out(stdout);
    ippInit();                      /* Initialize Intel(R) IPP library */
    out << "Hello from spectrum window!\n";
    out << "Lib name of ipp is = "<<ippGetLibVersion()->Name << endl;/* Get Intel(R) IPP library version info */
    out << "Lib version of ipp is = "<<ippGetLibVersion()->Version << endl;/* Get Intel(R) IPP library version info */



    amount = x.size();

    Ipp64f *PrePreSrc = new Ipp64f[amount];
    dst = new Ipp64fc[amount];

    for(int i = 0; i<amount; i++)
    {
        PrePreSrc[i]=y[i];
    }



    Ipp64f *presrc = new Ipp64f[amount];
    ippsWinHann_64f(PrePreSrc, presrc, amount);

    Ipp64fc *src = new Ipp64fc[amount];
    Ipp64f *PowerSpectrDst = new Ipp64f[amount];
    for(int i = 0; i<amount; i++)
    {
        src[i].re = presrc[i];
        src[i].im = 0;
    }
    ippsPowerSpectr_64fc(src, PowerSpectrDst, amount);


    int FFTOrder = (int)(log(amount)/log(2));
    IppsFFTSpec_C_64fc *pSpec = 0;
    Ipp8u *pMemSpec = 0;
    Ipp8u *pMemInit = 0;
    Ipp8u *pMemBuffer = 0;
    int sizeSpec = 0;
    int sizeInit = 0;
    int sizeBuffer = 0;
    int flag =  IPP_FFT_DIV_FWD_BY_N;
    /// получить размеры необходимых буферов
    ippsFFTGetSize_C_64fc(FFTOrder, flag, ippAlgHintNone, &sizeSpec, &sizeInit, &sizeBuffer);
    /// Выделите память для необходимых буферов
    pMemSpec = (Ipp8u*) ippMalloc(sizeSpec);
    if (sizeInit > 0)
    {
    pMemInit = (Ipp8u*) ippMalloc(sizeInit);
    }
    if (sizeBuffer > 0)
    {
    pMemBuffer = (Ipp8u*) ippMalloc(sizeBuffer);
    }
    /// инициализация структуры спецификации БПФ
    ippsFFTInit_C_64fc(&pSpec, FFTOrder, flag, ippAlgHintNone, pMemSpec, pMemInit);

    /// освободить буфер инициализации
    if (sizeInit >
    0)
    {
    ippFree(pMemInit);
    }
    /// выполнить прямое БПФ
    ippsFFTFwd_CToC_64fc(src, dst, pSpec, pMemBuffer);
    /// ...
    /// освободить буферы
    if (sizeBuffer > 0)
    {
    ippFree(pMemBuffer);
    }
    ippFree(pMemSpec);

    // делаем ippMagnitude

    double* Re = new double[amount];
        double* Im = new double[amount];
        for(int i=0;i<amount;i++){
            Re[i]=dst[i].re;
            Im[i]=dst[i].im;
        }
       Ipp64f *out1 = new Ipp64f[amount/2];
        ippsMagnitude_64fc( dst, out1, amount/2 );

        double* m_SignalSpectr= new double[amount/2];
        ippsMulC_64f(out1,(double)1/(double)(amount/2), m_SignalSpectr,amount/2);
        //delete out1;






    QVector<double> dstVec;

    QVector<double> newX;

    double res;

    for(int j = 0; j<amount/2; j++)
      {
        //res = sqrt(dst[j].im*dst[j].im+dst[j].re*dst[j].re);
        //res = sqrt((dst[j].im*dst[j].im)+(dst[j].re*dst[j].re));
        //res = m_SignalSpectr[j];
        //res = PowerSpectrDst[j];
        res = out1[j]*4.07;
        dstVec.push_back(res);
        //dstVec.push_back(m_SignalSpectr[j]);
      }





        out << deltaT;
     for(int j = 0; j<amount/2; j++)
     {

         res = j/(amount*deltaT);
         newX.push_back(res);
     }



    customPlot->setInteraction(QCP::iRangeZoom,true);   // Включаем взаимодействие удаления/приближения
     customPlot->setInteraction(QCP::iRangeDrag, true);  // Включаем взаимодействие перетаскивания графика
    //ui-> widget->axisRect()->setRangeDrag(Qt::Horizontal);   // Включаем перетаскивание только по горизонтальной оси



    customPlot->clearGraphs();//Если нужно, но очищаем все графики
    //Добавляем один график в widget
    customPlot->addGraph();
    //Говорим, что отрисовать нужно график по нашим двум массивам x и y
    customPlot->graph(0)->setData(newX, dstVec);

    customPlot->graph(0)->setPen(QColor(50, 50, 50, 255));//задаем цвет точек
    //формируем вид точек
    customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 1));

    //Подписываем оси Ox и Oy
    customPlot->xAxis->setLabel("Частота, Гц");
    customPlot->yAxis->setLabel("A");

    //Установим область, которая будет показываться на графике
   // ui->widget->xAxis->setRange(0, 100);//Для оси Ox
   // ui->widget->yAxis->setRange(-100, 100);//Для оси Oy
    customPlot->rescaleAxes();

     connect(customPlot->xAxis,    SIGNAL(rangeChanged(QCPRange)), this,   SLOT(onXRangeChanged(QCPRange)));




    //И перерисуем график на нашем widget
    customPlot->replot();
}


void SpectrumWindow::recieveDataToMain(QVector<double> x,QVector<double> y)
{
    QTextStream out(stdout);
    ippInit();                      /* Initialize Intel(R) IPP library */
    out << "Hello from sspectrum window!\n";
    out << "Lib name of ipp is = "<<ippGetLibVersion()->Name << endl;/* Get Intel(R) IPP library version info */
    out << "Lib version of ipp is = "<<ippGetLibVersion()->Version << endl;/* Get Intel(R) IPP library version info */

    double  deltaT = (x[100]-x[0])/100;

    amount = x.size();

    Ipp64f *PrePreSrc = new Ipp64f[amount];
    dst = new Ipp64fc[amount];

    for(int i = 0; i<amount; i++)
    {
        PrePreSrc[i]=y[i];
    }



    Ipp64f *presrc = new Ipp64f[amount];
    ippsWinHann_64f(PrePreSrc, presrc, amount);

    Ipp64fc *src = new Ipp64fc[amount];
    Ipp64f *PowerSpectrDst = new Ipp64f[amount];
    for(int i = 0; i<amount; i++)
    {
        src[i].re = presrc[i];
        src[i].im = 0;
    }
    ippsPowerSpectr_64fc(src, PowerSpectrDst, amount);


    int FFTOrder = (int)(log(amount)/log(2));
    IppsFFTSpec_C_64fc *pSpec = 0;
    Ipp8u *pMemSpec = 0;
    Ipp8u *pMemInit = 0;
    Ipp8u *pMemBuffer = 0;
    int sizeSpec = 0;
    int sizeInit = 0;
    int sizeBuffer = 0;
    int flag =  IPP_FFT_DIV_FWD_BY_N;
    /// получить размеры необходимых буферов
    ippsFFTGetSize_C_64fc(FFTOrder, flag, ippAlgHintNone, &sizeSpec, &sizeInit, &sizeBuffer);
    /// Выделите память для необходимых буферов
    pMemSpec = (Ipp8u*) ippMalloc(sizeSpec);
    if (sizeInit > 0)
    {
    pMemInit = (Ipp8u*) ippMalloc(sizeInit);
    }
    if (sizeBuffer > 0)
    {
    pMemBuffer = (Ipp8u*) ippMalloc(sizeBuffer);
    }
    /// инициализация структуры спецификации БПФ
    ippsFFTInit_C_64fc(&pSpec, FFTOrder, flag, ippAlgHintNone, pMemSpec, pMemInit);

    /// освободить буфер инициализации
    if (sizeInit >
    0)
    {
    ippFree(pMemInit);
    }
    /// выполнить прямое БПФ
    ippsFFTFwd_CToC_64fc(src, dst, pSpec, pMemBuffer);
    /// ...
    /// освободить буферы
    if (sizeBuffer > 0)
    {
    ippFree(pMemBuffer);
    }
    ippFree(pMemSpec);

    // делаем ippMagnitude

    double* Re = new double[amount];
        double* Im = new double[amount];
        for(int i=0;i<amount;i++){
            Re[i]=dst[i].re;
            Im[i]=dst[i].im;
        }
       Ipp64f *out1 = new Ipp64f[amount/2];
        ippsMagnitude_64fc( dst, out1, amount/2 );

        double* m_SignalSpectr= new double[amount/2];
        ippsMulC_64f(out1,(double)1/(double)(amount/2), m_SignalSpectr,amount/2);
        //delete out1;






    QVector<double> dstVec;

    QVector<double> newX;

    double res;

    for(int j = 0; j<amount/2; j++)
      {
        //res = sqrt(dst[j].im*dst[j].im+dst[j].re*dst[j].re);
        //res = sqrt((dst[j].im*dst[j].im)+(dst[j].re*dst[j].re));
        //res = m_SignalSpectr[j];
        //res = PowerSpectrDst[j];
        res = out1[j]*4.07;
        dstVec.push_back(res);
        //dstVec.push_back(m_SignalSpectr[j]);
      }





        out << deltaT;
     for(int j = 0; j<amount/2; j++)
     {

         res = j/(amount*deltaT);
         newX.push_back(res);
     }



    customPlot->setInteraction(QCP::iRangeZoom,true);   // Включаем взаимодействие удаления/приближения
     customPlot->setInteraction(QCP::iRangeDrag, true);  // Включаем взаимодействие перетаскивания графика
    //ui-> widget->axisRect()->setRangeDrag(Qt::Horizontal);   // Включаем перетаскивание только по горизонтальной оси



    customPlot->clearGraphs();//Если нужно, но очищаем все графики
    //Добавляем один график в widget
    customPlot->addGraph();
    //Говорим, что отрисовать нужно график по нашим двум массивам x и y
    customPlot->graph(0)->setData(newX, dstVec);

    customPlot->graph(0)->setPen(QColor(50, 50, 50, 255));//задаем цвет точек
    //формируем вид точек
    customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 1));

    //Подписываем оси Ox и Oy
    customPlot->xAxis->setLabel("Частота, Гц");
    customPlot->yAxis->setLabel("A");

    //Установим область, которая будет показываться на графике
   // ui->widget->xAxis->setRange(0, 100);//Для оси Ox
   // ui->widget->yAxis->setRange(-100, 100);//Для оси Oy
    customPlot->rescaleAxes();

     connect(customPlot->xAxis,    SIGNAL(rangeChanged(QCPRange)), this,   SLOT(onXRangeChanged(QCPRange)));




    //И перерисуем график на нашем widget
    customPlot->replot();
}

