#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setLabelText(const QString text);
    void drawPoints(const QVector<double> dataX, const QVector<double> dataY, const QVector<double> group, const int groups);
    void drawClusters(const QVector<double> dataX, const QVector<double> dataY);
    void throwMyErr(const QString err);
    void save_to_file(const QVector<double> dataX, const QVector<double> dataY,  const int groups,  const int iterations,  const double error,  const double max_error, const QVector<double> distance, const QVector<double> group);
    void clearPlot();
    void setData(int iterations, QVector<double> dataX, QVector<double>dataY, QVector<double> clusterX, QVector<double>clusterY, int groups, QVector<double> group, double destination_err, double err);

signals:
    void file_data(QString data);
    void start(int iterations, QVector<double> dataX, QVector<double>dataY, QVector<double> clusterX, QVector<double>clusterY, int groups, QVector<double> group, double destination_err);

private slots:
    void on_actionopen_triggered();

    void on_actionclear_triggered();

    void on_start_clicked();

    void on_actionsave_triggered();

private:
    // private class varible to pass data
    QVector<double> dataX, dataY, clusterX, clusterY, group;
    int groups, iterations;
    double destination_err, err;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
