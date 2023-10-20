#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <array>
#include <bits/stdc++.h>
using namespace std;

vector <array<float, 3>> get_data(string path){
    ifstream MyReadFile("E:\\pliki\\projekty\\programowanie-lab\\algorytm-centroidow\\data.txt");
    int i = 0;
    vector <array<float, 3>> position;
    float groups, iterations;
    string  myText;
    while (getline (MyReadFile, myText)) {
        if(i ==0){
            groups = stoi(myText);
            i++;

        }else if(i == 1){
            iterations = stoi(myText);
            position.push_back({groups, iterations});
            i++;
        }else{
            stringstream linestream(myText);
            string tmpposition;
            array <float, 3> pos = {};
            int j = 0;
            while(getline(linestream,tmpposition,','))
            {
                pos[j] = stof(tmpposition);
                j++;
            }
            pos[2] = -1;
            position.push_back(pos);
        }



    }
    MyReadFile.close();

    return position;
}

vector <array<float, 2>> get_group_possition(int groups, int max){
    vector <array<float,2>> position;
    srand(time(0));
    for (int i = 0; i < groups; ++i) {
        array<float, 2> pos = {};
        for (int j = 0; j < 2; ++j) {
            pos[j] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/max));;
        }
        position.push_back(pos);
    }
    return position;
}

vector <array<float, 3>> find_closer(vector<array<float,3>> data, vector<array<float,2>> groups){
    for (int i = 1; i < data.size(); ++i) {
        vector<double> distance;
        for (int j = 0; j < groups.size(); ++j) {
            distance.push_back(sqrt(pow(data[i][0] -groups[j][0], 2) + pow(data[i][1] - groups[j][1], 2) * 1.0) );
        }
        auto result = std::min_element(distance.begin(), distance.end());
        data[i][2] = std::distance(distance.begin(), result);

    }
    return data;
}

vector <array<float, 2>> find_new_possition(vector <array<float, 2>> groups, vector <array<float, 3>> data){
    for (int i = 0; i < groups.size(); ++i) {
        float localX = 0, localY = 0;
        int count = 0;
        for (int j = 0; j < data.size(); ++j) {
            if(data[j][2] == i){
                localX += data[j][0];
                localY += data[j][1];
                count ++;
            }
        }
        float avgX = localX/count;
        float avgY = localY/count;
        if(avgX != groups[i][0] || avgY != groups[i][1]){
            groups[i][0] = avgX;
            groups[i][1] = avgY;
        }
    }
    return groups;
}

void show_groups_possition(vector <array<float, 2>> groups){
    for (int i = 0; i < groups.size(); ++i) {
        cout<<"cluster "<<i+1<<" position: X:"<<groups[i][0]<<", Y:"<<groups[i][1]<<endl;
    }
}

int main() {

    int maxX = 10, maxY = 10;

    string path;
    cout << "podaj ścieżke do pliku"<<endl;
    cin >> path;
    vector <array<float, 3>> data = get_data(path);
    int groups = data[0][0];
    int iterations = data[0][1];
    vector <array<float, 2>> groups_position = get_group_possition(groups, maxX);
//    for (int j = 0; j < data.size(); ++j) {
//        cout << data[j][0] << " : "<<data[j][1] << endl;
//    }

//    for (int j = 0; j < groups_position.size(); ++j) {
//        cout << groups_position[j][0] << " : "<<groups_position[j][1] << endl;
//    }
    cout<<"Total clusters: "<<groups<<endl;
    cout<<"Total iterations: "<<iterations<<endl;

    data = find_closer(data, groups_position);
    vector <array<float, 2>> tmpGroups = find_new_possition(groups_position, data);

    for (int i = 0; i < iterations; ++i) {
        cout<<"iteration -> "<<i+1<<"/"<<iterations<<endl;
        data = find_closer(data, groups_position);
        vector <array<float, 2>> tmpGroups = find_new_possition(groups_position, data);
        if(tmpGroups!= groups_position){
            groups_position = tmpGroups;
        }
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

