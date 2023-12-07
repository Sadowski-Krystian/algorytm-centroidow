#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <array>

#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->customplot->xAxis->setLabel("długość");
    ui->customplot->yAxis->setLabel("szerokość");
    ui->customplot->xAxis->setRange(0, 100);
    ui->customplot->yAxis->setRange(0, 100);
    ui->customplot->setInteraction(QCP::iRangeZoom);
    ui->customplot->setInteraction(QCP::iRangeDrag);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionopen_triggered()
{

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open The File"), "", tr("text file (*.txt)"));
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot open file: "+file.errorString());
        return;

    }
    ui->customplot->clearGraphs();
    ui->customplot->replot();
    ui->start->setDisabled(false);
    setWindowTitle(fileName);
    QTextStream in(&file);
    QString text = in.readAll();
    ui->start->setDisabled(false);
    emit file_data(text);

    file.close();

}
/**
 * Set data from file
 * Set Data to Class Varibles
 *\
 *
 * @param iteration set private class field iteration
 * @param dataX set private class field dataX
 * @param dataY set private class field dataY
 * @param clusterX set private class field clusterX
 * @param clusterY set private class field clusterY
 */
void MainWindow::setData(int iterations, QVector<double> dataX, QVector<double> dataY, QVector<double> clusterX, QVector<double> clusterY, int groups, QVector<double> group, double destination_err, double err){
    this->iterations = iterations;
    this->dataX = dataX;
    this->dataY = dataY;
    this->clusterX = clusterX;
    this->clusterY = clusterY;
    this->group = group;
    this->groups = groups;
    this->destination_err = destination_err;
    this->err = err;
}

/**
 * Set Label Text
 *
 * Set any text to labalel
 *
 *
 * @param text to set on label
 */
void MainWindow::setLabelText(const QString text){
    ui->dataLoaded->setText(text);
}
/**
 * Draw points on plot
 *
 * Draw data points on plot
 *
 *
 * @param dataX point X coordinates
 * @param dataY point Y coordinates
 * @param gorup vector of cluster indext for points
 * @param groups a amount of clusters
 */
void MainWindow::drawPoints(const QVector<double> dataX, const QVector<double> dataY, const QVector<double> group, const int groups){
    if(group.empty()){
        ui->customplot->addGraph();
        ui->customplot->graph(0)->setScatterStyle(QCPScatterStyle::ssDot);
        ui->customplot->graph(0)->setLineStyle(QCPGraph::lsNone);
        ui->customplot->graph(0)->setPen(QPen(Qt::gray, 10));
        ui->customplot->graph(0)->setData(dataX,dataY);
        ui->customplot->replot();
        ui->customplot->update();
        return;
    }
    QVector<QPen> pensPoints = {QPen(Qt::green, 10), QPen(Qt::red, 10), QPen(Qt::yellow, 10), QPen(Qt::blue, 10)};
    for (int i=0; i<groups ;i++ ) {
        int g_count = ui->customplot->graphCount();

        ui->customplot->addGraph();
        ui->customplot->graph(g_count)->setScatterStyle(QCPScatterStyle::ssDot);
        ui->customplot->graph(g_count)->setLineStyle(QCPGraph::lsNone);
        ui->customplot->graph(g_count)->setPen(pensPoints[i]);
        for (int j=0;j<group.size() ;j++ ) {
            if(group[j] == i){
                ui->customplot->graph(g_count)->addData(dataX[j],dataY[j]);
            }
        }


        ui->customplot->update();
        ui->customplot->replot();

    }




}
/**
 * Draw closter on plot
 *
 *
 *
 *
 * @param dataX cluster X coordinates
 * @param dataY cluster Y coordinates

 */

void MainWindow::drawClusters(const QVector<double> dataX, const QVector<double> dataY){
    QVector<QPen> pensPoints = {QPen(Qt::green, 10), QPen(Qt::red, 10), QPen(Qt::yellow, 10), QPen(Qt::blue, 10)};
     if(dataX.size() > pensPoints.size()){
         return;
     }

    for (int i =0;i<dataX.size() ;i++ ) {
        int g_count = ui->customplot->graphCount();
        ui->customplot->addGraph();
        ui->customplot->graph(g_count)->setScatterStyle(QCPScatterStyle::ssCircle);
        ui->customplot->graph(g_count)->setLineStyle(QCPGraph::lsNone);
        ui->customplot->graph(g_count)->setPen(pensPoints[i]);
        ui->customplot->graph(g_count)->addData(dataX[i],dataY[i]);
        ui->customplot->replot();
        ui->customplot->update();
    }
    ui->customplot->replot();
    ui->customplot->update();

}



/**
 * Clear Plot
 *
 */

void MainWindow::on_actionclear_triggered()
{
    clearPlot();
}

/**
 * Start algorithm
 *
 */
void MainWindow::on_start_clicked()
{

    ui->start->setDisabled(true);
    emit start(this->iterations, this->dataX, this->dataY, this->clusterX, this->clusterY, this->groups, this->group, this->destination_err);

}
/**
 * Clear Plot
 *
 */
void MainWindow::clearPlot(){
    ui->start->setDisabled(true);
    ui->customplot->clearGraphs();
    ui->customplot->replot();
}

/**
 * Save to file
 *
 */
void MainWindow::on_actionsave_triggered()
{
    if(ui->customplot->graphCount() == 0 || ui->start->isEnabled()){
        QMessageBox::warning(this, "Warning", "Cannot save before operations");
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this,
           tr("Save Operation results"), "test",
           tr("text file (*.txt);;All Files (*)"));

    if (fileName.isEmpty()){

           return;
      } else {
           QFile file(fileName);
           if (!file.open(QIODevice::WriteOnly)) {
               QMessageBox::information(this, tr("Unable to open file"),
                   file.errorString());
               return;
           }
           QTextStream out(&file);
           out.setCodec("UTF-8");
           out<<"błąd kwantyzacji: "<<this->err<<"\nzdefiniowany błąd: "<<this->destination_err<<"\nilość iteracji: "<<this->iterations<<"\nilość clusterów: "<<this->groups<<"\nX, Y, Cluster\n";
           for (int i = 0; i < this->dataX.size(); i++) {
                   out<<this->dataX[i]<<", "<<this->dataY[i]<<", "<<this->group[i]<<", "<<endl;
               }
    }
}


/**
 * Throw any error
 *
 * Open Dialog window with warning/error
 *
 *
 * @param message/error
 */
void MainWindow::throwMyErr(const QString err){
     QMessageBox::warning(this, "Warning", err);
     ui->start->setDisabled(true);
}



