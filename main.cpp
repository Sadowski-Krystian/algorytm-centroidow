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
using namespace std;
MainWindow *mainWindow;
vector <double> dataX = {}, dataY = {};
vector <double> clusterX = {}, clusterY = {};
vector <double> point_distance = {}, group = {};
int iterations = 0, groups = 0;
//double max_error = 0;
double destination_err = 0, prev_err = 0, err = 0, err_sum_g = 0;

QVector<double> switch_vectors(vector<double> toCopy){
    return QVector<double>::fromStdVector(toCopy);
}

void menage_data(string data_str){
    stringstream ss(data_str);
    string myText = "";
    int i = 0;
    while(getline(ss, myText, '\n')){
//        qDebug()<<i;
        if(myText.find("err") != string::npos){

                    destination_err = stod(myText.substr(myText.find(" ") + 1));
                }else if(myText.find("cluster") != string::npos){
                    groups = stoi(myText.substr(myText.find(" ") + 1));
                }else if(myText.find("iteration") != string::npos){
                    iterations = stoi(myText.substr(myText.find(" ") + 1));
                }else if(i>2){
//qDebug()<<"test";
                    stringstream linestream(myText);
                    string tmpposition;
                    int j = 0;
                    while(getline(linestream,tmpposition,','))
                    {
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

void show_groups_possition(vector <double> dataX){
    for (int i = 0; i < dataX.size(); ++i) {
        qDebug()<<"cluster "<<i+1<<" position: X:"<<dataX[i]<<", Y:"<<dataY[i]<<endl;
    }
}


void get_group_possition(){
    srand(time(0));
    for (int i = 0; i < groups; ++i) {
        clusterX.push_back(static_cast <double> (rand()) / (static_cast <double> (RAND_MAX/10)));
        clusterY.push_back(static_cast <double> (rand()) / (static_cast <double> (RAND_MAX/10)));
    }
}


void find_closer(){
    group = {};
    point_distance = {};
    for (int i = 0; i < dataX.size(); ++i) {
        vector<double> local_distance;
        for (int j = 0; j < clusterX.size(); ++j) {
            local_distance.push_back(sqrt(pow(dataX[i] -clusterX[j], 2) + pow(dataY[i] - clusterY[j], 2) * 1.0) );
        }
        auto result = std::min_element(local_distance.begin(), local_distance.end());

        group.push_back(std::distance(local_distance.begin(), result));
        point_distance.push_back(*result);

    }
}


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

void calculate_error(){
//    qDebug()<<"calculate error";

//    qDebug()<<dataX.size();
//    cout<<data.size();
    double sum_distance = 0;
    for (int i = 0; i < point_distance.size(); ++i) {
        sum_distance += point_distance[i];
//        qDebug()<<point_distance[i];
    }
//    qDebug()<<point_distance.size();
    err = sum_distance/point_distance.size();
    qDebug()<<err;
//    if(isnan(err)){
//        err = 0;
//    }
//    return "Błąd kwantyzacji: "+to_string(sum_distance/point_distance.size());


}

void save(){
    mainWindow->save_to_file(switch_vectors(dataX), switch_vectors(dataY), groups, iterations, err, destination_err, switch_vectors(point_distance), switch_vectors(group));
}


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
        }

    string output_text = "Nie udało się osiągnąć wymaganej dokładności.\n\nWymagana dokładność: "+to_string(destination_err)+" \n\nOsiągnięta dokładność: "+to_string(err);

    mainWindow->setLabelText(QString::fromStdString(output_text));
}




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
        try{
                 menage_data(informacje.toStdString());
            }catch (const invalid_argument e){
                return -1;
            }

        const string text = "iteracje: "+to_string(iterations)+"; centroidy: "+to_string(groups);
        mainWindow->setLabelText(QString::fromStdString(text));
//        get_group_possition();
        clusterX.push_back(2.5);
        clusterX.push_back(8);
        clusterY.push_back(3);
        clusterY.push_back(4.5);
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
