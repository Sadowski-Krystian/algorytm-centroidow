#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <array>
#include <cmath>
#include <algorithm>
#include <filesystem>
#include <cfloat>

using namespace std;

int groups = 0, iterations = 0, dimensions = 0, max_dim = 0;
double destination_err = 0, err = 0;
/**
 * Save to file
 *
 * Save output to file named output in the data directory
 *
 * @param data Contains points position
 * @param path Contains path to output
 */
void save_to_file(vector <vector<double>> data, filesystem::path dir){
    string output_path = dir.u8string()+"\\output.txt";
    ofstream output(output_path);

    output<<"Error of Quantization: "<<err<<"\nDefined error: "<<destination_err<<"\nIteration count: "<<iterations<<"\nCluster count: "<<groups<<endl;
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
/**
 * Get data from file
 *
 * Get data from txt file where path is given
 *
 * @param path Contains path to file
 * @return vector of vector with n-dimentions points positions
 */
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
                if(stod(tmpposition) > max_dim){

                    max_dim = ceil(stod(tmpposition));

                }
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

/**
 * Calculate Quantization error
 *
 * Calculate Quantization error for any iteration
 *
 * @param data vector of array with points position
 */
void calculate_error(vector <vector<double>> data){
    double sum_distance = 0;
    for (int i = 0; i < data.size(); i++) {
        sum_distance += data[i][data[i].size()-1];
    }
    err = sum_distance/data.size();

    cout<<"error for this iteration: "<<err<<endl;
}
/**
 * Generate cluster
 *
 * Generate number of cluster
 *
 * @param cluster amount cluster to generate
 * @return vector of array with cluster positions
 */
vector <vector<double>> get_group_possition(int centroids){
    vector <vector<double>> position;
    srand(time(0));
    for (int i = 0; i < centroids; i++) {
        vector<double> pos = {};
        for (int j = 0; j < dimensions; j++) {
            pos.push_back(static_cast <double> (rand()) / (static_cast <double> (RAND_MAX/max_dim)));
        }
        position.push_back(pos);
    }
    return position;
}

/**
 * Calculate distance between two points
 *
 * Calculate distance bettween two points in same dimention
 * !!! One vector have bigger size becouse of store distance and parent cluster index !!!
 *
 * @param point2 point from data
 * @param point1 cluster
 * @return distance bettwen points
 */
double calculate_distance(vector<double> point1, vector<double> point2){

    if(point1.size() != point2.size()-2){
        throw  invalid_argument("One of the points is in another dimension, cannot calculate distance");
    }

    double distance = 0.0;


    for (int i = 0; i < point1.size(); i++) {
        distance += pow(point1[i] - point2[i], 2);
    }


    return sqrt(distance);
}

/**
 * K-means++ algorithm to generate cluster
 *
 * Generate number of cluster
 *
 * @param cluster amount cluster to generate
 * @return vector of array with cluster positions
 */
vector <vector<double>> kmeanspp(vector<vector<double>> data){
    srand(time(0));
    int firstRandom = rand() % data.size();
    vector<int> used_points = {};
    used_points.push_back(firstRandom);
    vector<vector<double>> clusters ={};
    vector<double> pointsDim = {};
    for (int i = 0; i < dimensions; ++i) {
        pointsDim.push_back(data[firstRandom][i]);

    }
    clusters.push_back(pointsDim);
    cout<<data.size()<<endl;
    for(int k =0; k < groups-1; k++){
        vector<double> farthestPoint = {};
        double maxDistance = 0.0;

        for (int i=0; i<data.size(); i++) {
            double minDistance = DBL_MAX;
            for (int j=0; j<clusters.size(); j++) {
                double distance = calculate_distance(clusters[j], data[i]);
                if (distance < minDistance) {
                    minDistance = distance;
                }
            }

            if (minDistance > maxDistance) {
                maxDistance = minDistance;
                farthestPoint = {};
                for (int j = 0; j < dimensions; ++j) {
                    farthestPoint.push_back(data[i][j]);
                }
            }
        }
        clusters.push_back(farthestPoint);
    }
    return clusters;
}
/**
 * Find closer cluster to point
 *
 * Check the distance between all clusters and find the one closer
 *
 * @param data vector of vector with points position
 * @param cluster vector of vector with cluster position
 * @return vector of vector with data with closer cluster index and distance
 */
vector <vector<double>> find_closer(vector<vector<double>> data, vector<vector<double>> centroids){
    for (int i = 0; i < data.size(); i++) {
        vector<double> distance;
        for (int j = 0; j < centroids.size(); j++) {
            double local_distance = 0;
            try {
                local_distance = calculate_distance(centroids[j], data[i]);
            }catch (const invalid_argument e) {
                throw  invalid_argument(e.what());
            }

            distance.push_back(local_distance);
        }
        auto result = std::min_element(distance.begin(), distance.end());
        data[i][data[i].size()-2] = std::distance(distance.begin(), result);
        data[i][data[i].size()-1] = *result;

    }
    return data;
}

/**
 * Find new position for cluster
 *
 * Calculate avg distance and find new position for cluster
 *
 * @param data vector of vector with points position
 * @param cluster vector of vector with cluster position
 * @return vector of vector with cluster position
 */
vector <vector<double>> find_new_possition(vector <vector<double>> centroids, vector <vector<double>> data){




    for(int i =0; i<centroids.size(); i++){
        vector<double> local_dimension ={};

        for (int k = 0; k < dimensions; k++) {
            double local_data = 0;
            int counter = 0;
            for(int j=0; j<data.size(); j++){

                if(data[j][data[j].size()-2] == i){
                    local_data += data[j][k];
                    counter++;
                }
            }

            if(counter!= 0){
                double avg = local_data/(double)counter;

                centroids[i][k] = avg;
            }

        }




    }

    return centroids;
}
/**
 * Show points under cluster in output
 *
 *
 * @param data vector of vector with points position
 * @param cluster id of cluster to show
 */
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
/**
 * Show cluster position in output
 *
 *
 * @param data vector of vector with points position
 * @param cluster vector of vector with cluster position
 */
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












/**
 * Give file pattern
 *
 * Generate and give fille pattern when wrong file was provide
 *
 *
 * @param path data path where pattern was saved
 */

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


/**
 * Main function
 *
 * Main function with iteration loop
 *
 *
 * @return operation code
 * 0 - operation completed error not achieved
 * 1 - operation completed error achieved
 * -1 - operation incompleted: an error has occurred
 */
int main() {
    string path;
    cout << "Give path to file"<<endl;
    cin >> path;
    if(path.rfind('"', 0) == 0){
        path = path.substr(1);
    }
    string suffix = "\"";

    string substring = path.substr(path.length() - 1);
    if(substring == suffix){
        path = path.substr(0, path.size()-1);
    }
    filesystem::path p(path);
    filesystem::path dir = p.parent_path();
    vector <vector<double>> data = {};
    try{
        data = get_data(path);
    }catch (const invalid_argument e){
        cout << e.what()<<endl;
        cout<<"Check the file pattern:"<<endl;
        file_Pattern(dir);
        return -1;
    }

    vector <vector<double>> groups_position ={};
    int algorithm = 0;

    while(algorithm < 1 || algorithm > 2){
        cout<<"Select cluster generation algorithm\n[1] - random generation\n[2] - k-means++"<<endl;
        cin>>algorithm;
    }

    switch (algorithm) {
        case 1:
            groups_position = get_group_possition(groups);
            break;
        case 2:
            groups_position = kmeanspp(data);
            break;
    }
//test data
//    vector <vector<double>> groups_position = {{2.5, 3}, {8, 4.5}};

    cout<<"Total clusters: "<<groups<<endl;
    cout<<"Total iterations: "<<iterations<<endl;


    for (int i = 0; i < iterations; i++) {
        cout<<"--------------------------"<<endl;
        cout<<"iteration -> "<<i+1<<"/"<<iterations<<endl;
        try{
            data = find_closer(data, groups_position);
        }catch (const invalid_argument e){
            cout << e.what()<<endl;
            return -1;
        }


        groups_position = find_new_possition(groups_position, data);


        calculate_error(data);
        show_groups_possition(groups_position, data);
        if(err<destination_err){
            cout<<"Required accuracy achieved. Program stopped...."<<endl;
            cout<<"Defined error: "<<destination_err;
            save_to_file(data, dir);
            return 1;
        }
    }
    save_to_file(data, dir);
    cout<<"Failed to achieve required accuracy: "<<destination_err;
    return 0;
}
