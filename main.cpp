#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <array>
#include <bits/stdc++.h>
using namespace std;

int groups = 0, iterations = 0;

vector <array<double, 4>> get_data(string path){
    ifstream MyReadFile("E:\\pliki\\projekty\\programowanie-lab\\algorytm-centroidow\\data.txt");
    int i = 0;
    vector <array<double, 4>> position;
    string  myText;
    while (getline (MyReadFile, myText)) {
        if(i ==0){
            groups = stoi(myText);
            i++;

        }else if(i == 1){
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
//    cout<<data.size();
    double sum_distance = 0;
    for (int i = 0; i < data.size(); ++i) {
        sum_distance += data[i][3];
    }

    cout<<"error for this iteration: "<<sum_distance/data.size()<<endl;


}

vector <array<double, 2>> get_group_possition(int groups, int max){
    vector <array<double,2>> position;
    srand(time(0));
    for (int i = 0; i < groups; ++i) {
        array<double, 2> pos = {};
        for (int j = 0; j < 2; ++j) {
            pos[j] = static_cast <double> (rand()) / (static_cast <double> (RAND_MAX/max));;
        }
        position.push_back(pos);
    }
    return position;
}

vector <array<double, 4>> find_closer(vector<array<double,4>> data, vector<array<double,2>> groups){
    for (int i = 0; i < data.size(); ++i) {
        vector<double> distance;
        for (int j = 0; j < groups.size(); ++j) {
            distance.push_back(sqrt(pow(data[i][0] -groups[j][0], 2) + pow(data[i][1] - groups[j][1], 2) * 1.0) );
        }
        auto result = std::min_element(distance.begin(), distance.end());
        data[i][2] = std::distance(distance.begin(), result);
        data[i][3] = *result;

    }
    return data;
}

vector <array<double, 2>> find_new_possition(vector <array<double, 2>> groups, vector <array<double, 4>> data){
    for (int i = 0; i < groups.size(); ++i) {
        double localX = 0, localY = 0;
        int count = 0;
        for (int j = 0; j < data.size(); ++j) {
            if(data[j][2] == i){
                localX += data[j][0];
                localY += data[j][1];
                count ++;
            }
        }
        double avgX = localX/count;
        double avgY = localY/count;
        if(avgX != groups[i][0] || avgY != groups[i][1]){
            groups[i][0] = avgX;
            groups[i][1] = avgY;
        }
    }
    return groups;
}

void show_groups_possition(vector <array<double, 2>> groups){
    for (int i = 0; i < groups.size(); ++i) {
        cout<<"cluster "<<i+1<<" position: X:"<<groups[i][0]<<", Y:"<<groups[i][1]<<endl;
    }
}

int main() {

    int maxX = 10, maxY = 10;

    string path;
    cout << "podaj ścieżke do pliku"<<endl;
    cin >> path;
    vector <array<double, 4>> data = get_data(path);
    vector <array<double, 2>> groups_position = get_group_possition(groups, maxX);
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
        vector <array<double, 2>> tmpGroups = find_new_possition(groups_position, data);
        if(tmpGroups!= groups_position){
            groups_position = tmpGroups;
        }
        calculate_error(data);
        show_groups_possition(groups_position);
    }



//    for (int i = 0; i < data.size(); ++i) {
//        cout << data[i][2]<< endl;
//    }
//    for (int i = 0; i < distance.size(); ++i) {
//        cout << distance[i] << endl;
//    }


    return 0;
}

