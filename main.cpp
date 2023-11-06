#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <array>
#include <bits/stdc++.h>
using namespace std;
//zrobić zapis do pliku
int groups = 0, iterations = 0;
double destination_err = 0, err = 0;


void save_to_file(vector <array<double, 4>> data){
    string output_path = "E:\\pliki\\projekty\\programowanie-lab\\algorytm-centroidow\\output.txt";
    ofstream output(output_path);

    output<<"błąd kwantyzacji: "<<err<<"\nzdefiniowany błąd: "<<destination_err<<"\nilość iteracji: "<<iterations<<"\nilość clusterów: "<<groups<<"\nX, Y, Cluster, Distance\n";
    output.close();

    output.open(output_path ,ios_base::app);
    for (int i = 0; i < data.size(); ++i) {
        output<<data[i][0]<<", "<<data[i][1]<<", "<<data[i][2]<<", "<<data[i][3]<<endl;
    }
}

vector <array<double, 4>> get_data(string path){
    ifstream MyReadFile("E:\\pliki\\projekty\\programowanie-lab\\algorytm-centroidow\\data.txt");
    int i = 0;
    vector <array<double, 4>> position;
    string  myText;
    while (getline (MyReadFile, myText)) {
        if(i == 0){
            destination_err = stod(myText);
            i++;
        }else if(i ==1){
            groups = stoi(myText);
            i++;

        }else if(i == 2){
            iterations = stoi(myText);
            i++;
        }else{
            stringstream linestream(myText);
            string tmpposition;
            array <double, 4> pos = {};
            int j = 0;
            while(getline(linestream,tmpposition,','))
            {
                pos[j] = stof(tmpposition);
                j++;
            }
            pos[2] = -1;
            pos[3] = -1;
            position.push_back(pos);
        }



    }
    MyReadFile.close();

    return position;
}

void calculate_error(vector <array<double, 4>> data){
//    cout<<data.size()<<endl;
    double sum_distance = 0;
    for (int i = 0; i < data.size(); ++i) {
        sum_distance += data[i][3];
//        cout<<data[i][3]<<endl;
    }
    err = sum_distance/data.size();
    cout<<"error for this iteration: "<<err<<endl;


}

vector <array<double, 2>> get_group_possition(int cluster, int max){
    vector <array<double,2>> position;
    srand(time(0));
    for (int i = 0; i < cluster; ++i) {
        array<double, 2> pos = {};
        for (int j = 0; j < 2; ++j) {
            pos[j] = static_cast <double> (rand()) / (static_cast <double> (RAND_MAX/max));;
        }
        position.push_back(pos);
    }
    return position;
}

double calculate_distance(array<double, 4> point1, array<double, 2> point2){
//    cout<<point1.size()<<endl;
//    cout<<point2.size()<<endl;


    double distance = 0.0;


    for (int i = 0; i < point2.size(); i++) {
        distance += pow(point1[i] - point2[i], 2);
    }


    return sqrt(distance);
}



vector <array<double, 4>> find_closer(vector<array<double,4>> data, vector<array<double,2>> cluster){
    for (int i = 0; i < data.size(); ++i) {
        vector<double> distance;
        for (int j = 0; j < cluster.size(); ++j) {
            double local_distance = calculate_distance(data[i], cluster[j]);
            distance.push_back(local_distance);
        }
        auto result = std::min_element(distance.begin(), distance.end());
        data[i][2] = std::distance(distance.begin(), result);
        data[i][3] = *result;


    }
//    cout<<data[9][3]<<endl;
    return data;
}

vector <array<double, 2>> find_new_possition(vector <array<double, 2>> cluster, vector <array<double, 4>> data){
    for (int i = 0; i < cluster.size(); ++i) {
        double localX = 0, localY = 0;
        int count = 0;
        for (int j = 0; j < data.size(); ++j) {
            if(data[j][2] == i){
                localX += data[j][0];
                localY += data[j][1];
                count ++;
            }
        }
        if(count!=0){
            double avgX = localX/(double)count;
            double avgY = localY/(double)count;
            if(avgX != cluster[i][0] || avgY != cluster[i][1]){
                cluster[i][0] = avgX;
                cluster[i][1] = avgY;
            }
        }

    }
    return cluster;
}
void show_points_to_cluster(int cluster, vector <array<double, 4>> data){
    for (int i = 0; i < data.size(); ++i) {
        if(data[i][2] == cluster){
            cout<<"("<<data[i][0]<<", "<<data[i][1]<<")"<<endl;
        }
    }
}

void show_groups_possition(vector <array<double, 2>> cluster, vector <array<double, 4>> data){
    for (int i = 0; i < cluster.size(); i++) {
        cout<<"cluster "<<i+1<<" position: X:"<<cluster[i][0]<<", Y:"<<cluster[i][1]<<endl;
        cout<<"points: "<<endl;
        show_points_to_cluster(i, data);
    }
}
void show_groups_possition_data(vector <array<double, 4>> cluster){
    string pos ="";
    for (int i = 0; i < cluster.size(); ++i) {
        pos += to_string(cluster[i][3]) + "\nj";
    }
    cout<<"cluster pos: "<<pos<<endl;
}
int main() {

    int maxX = 10, maxY = 10;

    string path;
    cout << "podaj ścieżke do pliku"<<endl;
    cin >> path;
    vector <array<double, 4>> data = get_data(path);
    vector <array<double, 2>> groups_position = {{2.5, 3}, {8, 4.5}};
//    vector <array<double, 2>> groups_position = get_group_possition(groups, maxX);
//    for (int j = 0; j < data.size(); ++j) {
//        cout << data[j][0] << " : "<<data[j][1] << endl;
//    }

//    for (int j = 0; j < groups_position.size(); ++j) {
//        cout << groups_position[j][0] << " : "<<groups_position[j][1] << endl;
//    }
    cout<<"Total clusters: "<<groups<<endl;
    cout<<"Total iterations: "<<iterations<<endl;


    for (int i = 0; i < iterations; ++i) {
        cout<<"--------------------------"<<endl;
        cout<<"iteration -> "<<i+1<<"/"<<iterations<<endl;
        data = find_closer(data, groups_position);
//        cout<<data[9][3]<<endl;

//        show_groups_possition_data(data);
        vector <array<double, 2>> tmpGroups = find_new_possition(groups_position, data);
        if(tmpGroups!= groups_position){
            groups_position = tmpGroups;
        }


        calculate_error(data);

        show_groups_possition(groups_position, data);
        if(err<destination_err){
            cout<<"osiągnięto wymaganą dokładność. Przerywam program...."<<endl;
            cout<<"wymagana dokładność: "<<destination_err;
            save_to_file(data);
            return 1;
        }
    }
    save_to_file(data);
    cout<<"nie udało się osiągnąc wymaganej dokładności: "<<destination_err;




//    for (int i = 0; i < data.size(); ++i) {
//        cout << data[i][2]<< endl;
//    }
//    for (int i = 0; i < distance.size(); ++i) {
//        cout << distance[i] << endl;
//    }


    return 0;
}

