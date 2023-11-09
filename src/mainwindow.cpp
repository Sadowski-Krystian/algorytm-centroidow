#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <array>
#include <bits/stdc++.h>

#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
QVector<QPen> pens = {QPen(Qt::green, 3), QPen(Qt::red, 3), QPen(Qt::yellow, 3), QPen(Qt::blue, 3)};
QVector<QPen> pensPoints = {QPen(Qt::green, 10), QPen(Qt::red, 10), QPen(Qt::yellow, 10), QPen(Qt::blue, 10)};
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
//    ui->start->setDisabled(true);
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
//    ui->label->setText(fileName);
    ui->start->setDisabled(false);
    emit file_data(text);
    file.close();

}

void MainWindow::setLabelText(const QString text){
    ui->dataLoaded->setText(text);
}

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

    for (int i=0; i<groups ;i++ ) {
        int g_count = ui->customplot->graphCount();

        ui->customplot->addGraph();
        ui->customplot->graph(g_count)->setScatterStyle(QCPScatterStyle::ssDot);
        ui->customplot->graph(g_count)->setLineStyle(QCPGraph::lsNone);
        ui->customplot->graph(g_count)->setPen(pensPoints[i]);
        for (int j=0;j<group.size() ;j++ ) {
            if(group[j] == i){
//                qDebug()<<j;
//                qDebug()<<dataX.size();
//                qDebug()<<group.size();
                ui->customplot->graph(g_count)->addData(dataX[j],dataY[j]);
            }
        }


        ui->customplot->update();
        ui->customplot->replot();

    }




}
void MainWindow::drawClusters(const QVector<double> dataX, const QVector<double> dataY){

     if(dataX.size() > pens.size()){
         return;
     }
//    qDebug()<<dataX;
//    qDebug()<<dataY;

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





void MainWindow::on_actionclear_triggered()
{
    clearPlot();
}


void MainWindow::on_start_clicked()
{
    ui->start->setDisabled(true);
    emit start();

}

void MainWindow::clearPlot(){
    ui->start->setDisabled(true);
    ui->customplot->clearGraphs();
    ui->customplot->replot();
}


void MainWindow::on_actionsave_triggered()
{
    if(ui->customplot->graphCount() == 0 || ui->start->isEnabled()){
        QMessageBox::warning(this, "Warning", "Cannot save before operations");
        return;
    }
    emit save();
}

void MainWindow::save_to_file(const QVector<double> dataX, const QVector<double> dataY,  const int groups,  const int iterations,  const double error,  const double max_error, const QVector<double> distance, const QVector<double> group){

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
           out<<"błąd kwantyzacji: "<<error<<"\nzdefiniowany błąd: "<<max_error<<"\nilość iteracji: "<<iterations<<"\nilość clusterów: "<<groups<<"\nX, Y, Cluster, Distance\n";
           for (int i = 0; i < dataX.size(); ++i) {
                   out<<dataX[i]<<", "<<dataY[i]<<", "<<group[i]<<", "<<distance[i]<<endl;
               }
    }


}
void MainWindow::throwMyErr(const QString err){
     QMessageBox::warning(this, "Warning", err);
     ui->start->setDisabled(true);
}



