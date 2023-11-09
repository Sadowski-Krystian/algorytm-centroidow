#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <array>
#include <cmath>
#include <algorithm>
#include <filesystem>

using namespace std;

int groups = 0, iterations = 0, dimensions = 0;
double destination_err = 0, err = 0;

void save_to_file(vector <vector<double>> data, filesystem::path dir){
    string output_path = dir.u8string()+"\\output.txt";
    ofstream output(output_path);

    output<<"błąd kwantyzacji: "<<err<<"\nzdefiniowany błąd: "<<destination_err<<"\nilość iteracji: "<<iterations<<"\nilość clusterów: "<<groups<<endl;
    output.close();

    output.open(output_path ,ios_base::app);
    for (int i = 0; i < dimensions; ++i) {
        output<<"Dim"<<i+1<<", ";
    }
    output<<"Cluster, Distance"<<endl;
    for (int i = 0; i < data.size(); ++i) {
        for (int j = 0; j < data[i].size(); ++j) {
            output<<data[i][j]<<", ";
        }
        output<<endl;
    }
}

void show_points_to_cluster(int cluster, vector<vector<double>> data){
    for (int i = 0; i < data.size(); ++i) {
        if(data[i][data[i].size()-2] == cluster){
            cout<<"( ";
            for (int j = 0; j < data[i].size()-2; ++j) {
                cout<<data[i][j]<<", ";
            }
            cout<<")"<<endl;
        }
    }
}
void show_groups_possition(vector <vector<double>> clusters, vector<vector<double>> data){
    for (int i = 0; i < clusters.size(); i++) {
        string pos="";
        for (int j = 0; j < clusters[i].size(); j++) {
            pos += to_string(clusters[i][j]) + ", ";
        }
        cout<< "Cluster Position: "<<pos<<endl;
        cout<<"Points: "<<endl;
        show_points_to_cluster(i, data);
    }
}

vector <vector<double>> get_data(string path){
    ifstream MyReadFile(path);
    int i = 0;
    vector <vector<double>> position;
    string  myText;
    while (getline (MyReadFile, myText)) {
        if(myText.find("err") != string::npos){

            destination_err = stod(myText.substr(myText.find(" ") + 1));
        }else if(myText.find("cluster") != string::npos){
            groups = stoi(myText.substr(myText.find(" ") + 1));
        }else if(myText.find("iteration") != string::npos){
            iterations = stoi(myText.substr(myText.find(" ") + 1));
        }else if(i>3){
            if(myText.length() < 3){
                break;
            }
            stringstream linestream(myText);
            string tmpposition;
            vector<double> pos = {};

            while(getline(linestream,tmpposition,','))
            {
                pos.push_back(stod(tmpposition));

            }
            if(pos.size() > dimensions){
                dimensions = pos.size();
            }
            pos.push_back(-1);
            pos.push_back(-1);
            position.push_back(pos);
        }

    i++;

    }
    MyReadFile.close();
    if(destination_err == 0 || groups == 0 || iterations == 0){
        throw invalid_argument("something is frong while procesing file.");
    }
    return position;
}

double calculate_distance(vector<double> point1, vector<double> point2){
//    cout<<point1.size()<<endl;
//    cout<<point2.size()<<endl;
//    dump(cout, point2);
//    cout<<point2[0]<<endl;
//    cout<<point2[1]<<endl;
    if(point1.size() != point2.size()-2){
        throw  invalid_argument("Jeden z punktów znajduje się w innym wymiarze nie można obliczyć odległości");
    }

    double distance = 0.0;


    for (int i = 0; i < point1.size(); i++) {
        distance += pow(point1[i] - point2[i], 2);
    }


    return sqrt(distance);
}

vector <vector<double>> find_closer(vector<vector<double>> data, vector<vector<double>> centroids){
    for (int i = 0; i < data.size(); i++) {
        vector<double> distance;
        for (int j = 0; j < centroids.size(); j++) {
            // zmienić

            double local_distance = calculate_distance(centroids[j], data[i]);
            distance.push_back(local_distance);
        }
        auto result = std::min_element(distance.begin(), distance.end());
        data[i][data[i].size()-2] = std::distance(distance.begin(), result);
        data[i][data[i].size()-1] = *result;

    }
    return data;
}

vector <vector<double>> get_group_possition(int centroids, int max){
    vector <vector<double>> position;
    srand(time(0));
    for (int i = 0; i < centroids; i++) {
        vector<double> pos = {};
        for (int j = 0; j < dimensions; j++) {
            pos.push_back(static_cast <double> (rand()) / (static_cast <double> (RAND_MAX/max)));
        }
        position.push_back(pos);
    }
    return position;
}

vector <vector<double>> find_new_possition(vector <vector<double>> centroids, vector <vector<double>> data){




    for(int i =0; i<centroids.size(); i++){
        vector<double> local_dimension ={};

        for (int k = 0; k < dimensions; k++) {
            double local_data = 0;
            int counter = 0;
            for(int j=0; j<data.size(); j++){

                if(data[j][data[j].size()-2] == i){
//                    cout<<data[j][data[j].size()-1];
                    local_data += data[j][k];
                    counter++;
                }
            }

            if(counter!= 0){
                double avg = local_data/(double)counter;

                centroids[i][k] = avg;
            }

        }

//        cout<<local_dimension[0];



    }

    return centroids;
}

void calculate_error(vector <vector<double>> data){
//    cout<<data.size();
    double sum_distance = 0;
    for (int i = 0; i < data.size(); i++) {
        sum_distance += data[i][data[i].size()-1];
    }
    err = sum_distance/data.size();
//    cout<<"error for this iteration: "<<sum_distance/data.size()<<endl;


}


void file_Pattern(filesystem::path path) {
    cout<<"---------------------"<<endl;
    cout<<"err: 23.2342"<<endl;
    cout<<"clusters: 12423"<<endl;
    cout<<"iterations: 2342"<<endl;
    cout<<"2.6, 3"<<endl;
    cout<<"3, 9"<<endl;
    cout<<"...."<<endl;
    cout<<"---------------------"<<endl;
    string pattern_path = path.u8string()+"\\pattern.txt";
    ofstream output(pattern_path, ios_base::app);
    output<<"err: 23.2342"<<endl;
    output<<"clusters: 12423"<<endl;
    output<<"iterations: 2342"<<endl;
    output<<"2.6, 3"<<endl;
    output<<"3, 9"<<endl;
    output<<"...."<<endl;
    output.close();
    cout<<"saving pattern in the same path.";

}



int main() {
    string path;
    cout << "podaj ścieżke do pliku"<<endl;
    cin >> path;
    filesystem::path p(path);
    filesystem::path dir = p.parent_path();
    vector <vector<double>> data = {};
    try{
        data = get_data(path);
    }catch (const invalid_argument e){
        cout << e.what()<<endl;
        cout<<"check the file pattern:"<<endl;
        file_Pattern(dir);
        return -1;
    }
//    show_groups_possition(data);
//    vector <vector<double>> groups_position = get_group_possition(groups, 10);
    vector <vector<double>> groups_position = {{2.5, 3}, {8, 4.5}};
//    show_groups_possition(groups_position);
//    for (int j = 0; j < data.size(); ++j) {
//        cout << data[j][0] << " : "<<data[j][1] << endl;
//    }

//    for (int j = 0; j < groups_position.size(); ++j) {
//        cout << groups_position[j][0] << " : "<<groups_position[j][1] << endl;
//    }
    cout<<"Total clusters: "<<groups<<endl;
    cout<<"Total iterations: "<<iterations<<endl;


    for (int i = 0; i < iterations; i++) {
        cout<<"--------------------------"<<endl;
        cout<<"iteration -> "<<i+1<<"/"<<iterations<<endl;
        data = find_closer(data, groups_position);

//        cout<<"przeszło find";
//        show_groups_possition(data);
        groups_position = find_new_possition(groups_position, data);
//        cout<<"przeszło znalezienie pozycji";
//        if(tmpGroups!= groups_position){
//            groups_position = tmpGroups;
//        }

        calculate_error(data);
        show_groups_possition(groups_position, data);
        if(err<destination_err){
            cout<<"osiągnięto wymaganą dokładność. Przerywam program...."<<endl;
            cout<<"wymagana dokładność: "<<destination_err;
            save_to_file(data, dir);
            return 1;
        }
    }
    save_to_file(data, dir);
    cout<<"nie udało się osiągnąc wymaganej dokładności: "<<destination_err;
    return 0;
}
