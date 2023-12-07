#include "mainwindow.h"

#include <QApplication>
#include <QDebug>

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <array>
#include <windows.h>
#include <cfloat>
#include <cmath>
using namespace std;
MainWindow *mainWindow;

/**
 * convert vectors
 *
 * save file data to local varibles
 *
 * @param Vector to convert
 * @return Qvector with same content
 */
QVector<double> switch_vectors_to_Qt(vector<double> toCopy){
    return QVector<double>::fromStdVector(toCopy);
}
//vector<double> switch_vectors(QVector<double> toCopy){
//    return vector<double>::(toCopy);
//}
/**
 * menage file data
 *
 * save file data to local varibles
 *
 * @param data Contains points position
 */
array<QVector<double>,3> menage_data(string data_str){
    double destination_err =0;
    double groups = 0;
    double iterations = 0;
    double max_dim =0;
    QVector<double> dataX ={}, dataY = {};
    stringstream ss(data_str);
    QVector<double> output={};
    string myText = "";
    int i = 0;
    while(getline(ss, myText, '\n')){
        if(myText.find("err") != string::npos){

                    destination_err = stod(myText.substr(myText.find(" ") + 1));
                }else if(myText.find("cluster") != string::npos){
                    groups = stoi(myText.substr(myText.find(" ") + 1));
                }else if(myText.find("iteration") != string::npos){
                    iterations = stoi(myText.substr(myText.find(" ") + 1));
                }else if(i>2){
                    stringstream linestream(myText);
                    string tmpposition;
                    int j = 0;
                    while(getline(linestream,tmpposition,','))
                    {
                        if(stod(tmpposition) > max_dim){

                            max_dim = ceil(stod(tmpposition));


                        }
                        switch (j) {
                            case 0:
                                dataX.push_back(stod(tmpposition));
                                break;
                            case 1:
                                dataY.push_back(stod(tmpposition));
                                break;
                        }
                        j++;
                    }




    }
        i++;
    }


    if(destination_err == 0 || groups == 0 || iterations == 0){
           mainWindow->throwMyErr("wrong file pattern was provide");
           throw invalid_argument("something is frong while procesing file.");
        }
    output.push_back(destination_err);
    output.push_back(groups);
    output.push_back(iterations);
    output.push_back(max_dim);
    return {dataX, dataY, output};

}
/**
 * show cluster position
 *
 * debug function - show cluster position in debug console
 *
 * @param one of data vectors
 */
//void show_groups_possition(vector <double> dataX){
//    for (int i = 0; i < dataX.size(); ++i) {
//        qDebug()<<"cluster "<<i+1<<" position: X:"<<dataX[i]<<", Y:"<<dataY[i]<<endl;
//    }
//}
/**
 * Calculate distance between two points
 *
 * Calculate distance bettween two points in same dimention
 *
 * @param dx dataX of point
 * @param dy dataY of point
 * @param cx dataX of cluster
 * @param cy dataY of cluster
 */
double calculate_distance(double dx, double dy, double cx, double cy){


    double distance = 0.0;


    distance += pow(dx - cx, 2);
    distance += pow(dy - cy, 2);

    return sqrt(distance);
}

/**
 * K-means++ algorithm to generate cluster
 *
 * Generate number of cluster with logical way
 *
 */



array<QVector<double>,2> kmeanspp(int groups, QVector<double> dataX, QVector<double>dataY) {
    srand(time(0));
    QVector<double> used_points={};
    QVector<double> clusterX = {},clusterY = {};
    int firstRandom = rand() % dataX.size();
    used_points.push_back(firstRandom);
    clusterX.push_back(dataX[firstRandom]);
    clusterY.push_back(dataY[firstRandom]);

    for(int i =0; i < groups-1; i++){
        array<double, 2> farthestPoint = {};
        double maxDistance = 0.0;

        for (int i=0; i<dataX.size(); i++) {
            double minDistance = DBL_MAX;

            for (int j=0; j<clusterX.size(); j++) {
                double distance = calculate_distance(dataX[i], dataY[i], clusterX[j], clusterY[j]);
                if (distance < minDistance) {
                    minDistance = distance;
                }
            }

            if (minDistance > maxDistance) {
                maxDistance = minDistance;
                farthestPoint[0] = dataX[i];
                farthestPoint[1] = dataY[i];
            }
        }
        clusterX.push_back(farthestPoint[0]);
        clusterY.push_back(farthestPoint[1]);
    }


    return {clusterX, clusterY};
}
///**
// * Randomly generate cluster
// *
// * Generate number of cluster in random way
// */
//void get_group_possition(){
//    srand(time(0));
//    for (int i = 0; i < groups; ++i) {
//        clusterX.push_back(static_cast <double> (rand()) / (static_cast <double> (RAND_MAX/max_dim)));
//        clusterY.push_back(static_cast <double> (rand()) / (static_cast <double> (RAND_MAX/max_dim)));
//    }
//}

///**
// * Find closer cluster to point
// *
// * Check the distance between all clusters and find the one closer
// *
// */
array<QVector<double>, 2> find_closer(QVector<double> dataX, QVector<double> dataY, QVector<double> clusterX, QVector<double> clusterY){
    QVector<double> group = {};
    QVector<double> point_distance = {};
    for (int i = 0; i < dataX.size(); ++i) {
        vector<double> local_distance;
        for (int j = 0; j < clusterX.size(); ++j) {
            local_distance.push_back(calculate_distance(dataX[i], dataY[i], clusterX[j], clusterY[j]) );
        }
        auto result = min_element(local_distance.begin(), local_distance.end());

        group.push_back(distance(local_distance.begin(), result));
        point_distance.push_back(*result);

    }
    return {point_distance, group};
}
/**
 * Find new position for cluster
 *
 * Calculate avg distance and find new position for cluster
 *
 */

array<QVector<double>,2> find_new_possition(QVector<double> dataX, QVector<double> dataY, QVector<double> clusterX, QVector<double> clusterY, QVector<double> group){
    for (int i = 0; i < clusterX.size(); ++i) {
        double localX = 0, localY = 0;
        int count = 0;
        for (int j = 0; j < dataX.size(); ++j) {
            if(group[j] == i){
                localX += dataX[j];
                localY += dataY[j];
                count ++;
            }
        }
        double avgX = localX/count;
        double avgY = localY/count;
        if(avgX != clusterX[i] || avgY != clusterY[i]){
            clusterX[i] = avgX;
            clusterY[i] = avgY;
        }
    }
    return {clusterX, clusterY};
}
/**
 * Calculate distance between two points
 *
 * Calculate distance bettween two points in same dimention
 *
 */
double calculate_error(QVector<double> point_distance){
    double err = 0;
    double sum_distance = 0;
    for (int i = 0; i < point_distance.size(); ++i) {
        sum_distance += point_distance[i];
    }

    err = sum_distance/point_distance.size();
    return err;

}

/**
 * Main algorithm function
 *
 * Main algorithm function with iteration loop
 *
 */

void init(int iterations, QVector<double> dataX, QVector<double>dataY, QVector<double> clusterX, QVector<double>clusterY, int groups, QVector<double> group, double destination_err){
    double err = 0;
    for (int i = 0; i < iterations; i++) {
            qDebug()<<"iteration";
            qDebug()<<i;

            array<QVector<double>, 2> closer = find_closer(dataX, dataY, clusterX, clusterY);
            array<QVector<double>, 2> new_position = find_new_possition(dataX, dataY, clusterX, clusterY, closer[1]);
            clusterX = new_position[0];
            clusterY = new_position[1];
            err = calculate_error(closer[0]);
            group = closer[1];
            mainWindow->clearPlot();
            mainWindow->drawPoints(dataX, dataY, group, groups);
            mainWindow->drawClusters(clusterX, clusterY);
            if(err<destination_err){
                string local_text = "osiągnięto wymaganą dokładność. \n\nWymagana dokładność: "+to_string(destination_err)+" \n\nOsiągnięta dokładność: "+to_string(err);
                mainWindow->setLabelText(QString::fromStdString(local_text));
                return;
            }
////            show_groups_possition(groups_position);
////            Sleep(500); sleep
        }

    string output_text = "Nie udało się osiągnąć wymaganej dokładności.\n\nWymagana dokładność: "+to_string(destination_err)+" \n\nOsiągnięta dokładność: "+to_string(err);
    mainWindow->setData(iterations, dataX, dataY, clusterX, clusterY, groups, group, destination_err, err);
    mainWindow->setLabelText(QString::fromStdString(output_text));
}


/**
 * Main program function
 *
 *
 */

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();



    mainWindow = &w;
    QObject::connect(&w, &MainWindow::file_data,[=](QString informacje){

        QVector <double> dataX = {}, dataY = {}, group={};
                int iterations = 0, groups = 0, max_dim = 0;
                QVector<double> clusterX = {}, clusterY = {};
                //double max_error = 0;
                double destination_err = 0;
        array<QVector<double>,3> data ={};
        array<QVector<double>, 2> kmeanspp_data = {};
        try{
                data = menage_data(informacje.toStdString());
            }catch (const invalid_argument e){
                return -1;
            }
        dataX = data[0];
        dataY = data[1];
        destination_err = data[2][0];
        groups = data[2][1];
        iterations = data[2][2];
        max_dim = data[2][3];

        const string text = "iteracje: "+to_string(iterations)+"; centroidy: "+to_string(groups);
        mainWindow->setLabelText(QString::fromStdString(text));
//        get_group_possition();
        kmeanspp_data = kmeanspp(groups, dataX, dataY);
        clusterX = kmeanspp_data[0];
        clusterY = kmeanspp_data[1];
        mainWindow->drawPoints(dataX, dataY, group, groups);
        mainWindow->drawClusters(clusterX, clusterY);
        mainWindow->setData(iterations, dataX, dataY, clusterX, clusterY, groups, group, destination_err, 0);

    });

    QObject::connect(&w, &MainWindow::start, [](int iterations, QVector<double> dataX, QVector<double>dataY, QVector<double> clusterX, QVector<double>clusterY, int groups, QVector<double> group, double destination_err){
        init(iterations, dataX, dataY, clusterX, clusterY, groups, group, destination_err);
//        int iterations, QVector<double> dataX, QVector<double>dataY, QVector<double> clusterX, QVector<double>clusterY, int groups, QVector<double> group, double destination_err
//                iterations, dataX, dataY, clusterX, clusterY, groups, group, destination_err

    });
    return a.exec();
}
