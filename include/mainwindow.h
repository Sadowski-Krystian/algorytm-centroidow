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

signals:
    void file_data(QString data);
    void start();
    void save();

private slots:
    void on_actionopen_triggered();

    void on_actionclear_triggered();

    void on_start_clicked();

    void on_actionsave_triggered();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
