#include "mainwindow.h"

#include <bits/stdc++.h>
#include <QApplication>
#include <QDebug>

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <array>
#include <windows.h>
#include <float.h>
using namespace std;
MainWindow *mainWindow;
vector <double> dataX = {}, dataY = {};
vector <double> clusterX = {}, clusterY = {};
vector <double> point_distance = {}, group = {}, used_points = {};
int iterations = 0, groups = 0, max_dim = 0;
//double max_error = 0;
double destination_err = 0, prev_err = 0, err = 0, err_sum_g = 0;
/**
 * convert vectors
 *
 * save file data to local varibles
 *
 * @param Vector to convert
 * @return Qvector with same content
 */
QVector<double> switch_vectors(vector<double> toCopy){
    return QVector<double>::fromStdVector(toCopy);
}
/**
 * menage file data
 *
 * save file data to local varibles
 *
 * @param data Contains points position
 */
void menage_data(string data_str){
    stringstream ss(data_str);
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

}
/**
 * show cluster position
 *
 * debug function - show cluster position in debug console
 *
 * @param one of data vectors
 */
void show_groups_possition(vector <double> dataX){
    for (int i = 0; i < dataX.size(); ++i) {
        qDebug()<<"cluster "<<i+1<<" position: X:"<<dataX[i]<<", Y:"<<dataY[i]<<endl;
    }
}
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



void kmeanspp() {
    srand(time(0));
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



}
/**
 * Randomly generate cluster
 *
 * Generate number of cluster in random way
 */
void get_group_possition(){
    srand(time(0));
    for (int i = 0; i < groups; ++i) {
        clusterX.push_back(static_cast <double> (rand()) / (static_cast <double> (RAND_MAX/max_dim)));
        clusterY.push_back(static_cast <double> (rand()) / (static_cast <double> (RAND_MAX/max_dim)));
    }
}

/**
 * Find closer cluster to point
 *
 * Check the distance between all clusters and find the one closer
 *
 */
void find_closer(){
    group = {};
    point_distance = {};
    for (int i = 0; i < dataX.size(); ++i) {
        vector<double> local_distance;
        for (int j = 0; j < clusterX.size(); ++j) {
            local_distance.push_back(calculate_distance(dataX[i], dataY[i], clusterX[j], clusterY[j]) );
        }
        auto result = min_element(local_distance.begin(), local_distance.end());

        group.push_back(distance(local_distance.begin(), result));
        point_distance.push_back(*result);

    }
}
/**
 * Find new position for cluster
 *
 * Calculate avg distance and find new position for cluster
 *
 */

void find_new_possition(){
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
}
/**
 * Calculate distance between two points
 *
 * Calculate distance bettween two points in same dimention
 *
 */
void calculate_error(){

    double sum_distance = 0;
    for (int i = 0; i < point_distance.size(); ++i) {
        sum_distance += point_distance[i];
    }

    err = sum_distance/point_distance.size();


}
/**
 * Save to file
 *
 * Call methode in ui to save file
 *
 *
 */
void save(){
    mainWindow->save_to_file(switch_vectors(dataX), switch_vectors(dataY), groups, iterations, err, destination_err, switch_vectors(point_distance), switch_vectors(group));
}
/**
 * Main algorithm function
 *
 * Main algorithm function with iteration loop
 *
 */

void init(){
    for (int i = 0; i < iterations; i++) {
            qDebug()<<"iteration";
            qDebug()<<i;
            find_closer();
            find_new_possition();


            calculate_error();

            mainWindow->clearPlot();
            mainWindow->drawPoints(switch_vectors(dataX), switch_vectors(dataY), switch_vectors(group), groups);
            mainWindow->drawClusters(switch_vectors(clusterX), switch_vectors(clusterY));
            if(err<destination_err){
                string local_text = "osiągnięto wymaganą dokładność. \n\nWymagana dokładność: "+to_string(destination_err)+" \n\nOsiągnięta dokładność: "+to_string(err);
                mainWindow->setLabelText(QString::fromStdString(local_text));
                return;
            }
//            show_groups_possition(groups_position);
//            Sleep(500); sleep
        }

    string output_text = "Nie udało się osiągnąć wymaganej dokładności.\n\nWymagana dokładność: "+to_string(destination_err)+" \n\nOsiągnięta dokładność: "+to_string(err);

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
    QObject::connect(&w, &MainWindow::file_data, [](QString informacje){
        dataX = {};
        dataY = {};
        clusterX = {};
        clusterY = {};
        group = {};
        point_distance = {};
        used_points ={};
        try{
                 menage_data(informacje.toStdString());
            }catch (const invalid_argument e){
                return -1;
            }

        const string text = "iteracje: "+to_string(iterations)+"; centroidy: "+to_string(groups);
        mainWindow->setLabelText(QString::fromStdString(text));
//        get_group_possition();
        kmeanspp();
        mainWindow->drawPoints(switch_vectors(dataX), switch_vectors(dataY), switch_vectors(group), groups);
        mainWindow->drawClusters(switch_vectors(clusterX), switch_vectors(clusterY));



    });

    QObject::connect(&w, &MainWindow::start, [](){
        init();

    });
    QObject::connect(&w, &MainWindow::save, [](){
        save();

    });
    return a.exec();
}
