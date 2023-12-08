#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <array>
#include <cmath>
#include <filesystem>
#include <cfloat>
#include <algorithm>
#include <cstdlib>

using namespace std;

/**
 * Save to file
 *
 * Save output to file named output in the data directory
 *
 * @param data Contains points position
 * @param path Contains path to output
 * @param err Contains error of operations
 * @param destination_err Contains wanted max error for operation
 * @param iterations Contains iteration of main loop
 * @param groups Contains number of cluster
 */
void save_to_file(vector <array<double, 4>> data, filesystem::path path, double err, double destination_err, int iterations, int groups){
    string output_path = path.u8string() + "\\output.txt";
    ofstream output(output_path);

    output<<"Error of Quantization: "<<err<<"\nDefined error: "<<destination_err<<"\nIteration count: "<<iterations<<"\nCluster count: "<<groups<<"\nX, Y, Cluster, Distance\n";
    output.close();

    output.open(output_path ,ios_base::app);
    for (int i = 0; i < data.size(); ++i) {
        output<<data[i][0]<<", "<<data[i][1]<<", "<<data[i][2]<<", "<<data[i][3]<<endl;
    }
}

/**
 * Get data from file
 *
 * Get data from txt file where path is given
 *
 * @param path Contains path to file
 * @param destination_err reference to varible in main fucntion contains wanted operation error
 * @param groups reference to varible in main function contains number of clusters
 * @param iterations reference to varible in main function contains number of iterations main loop
 * @param max_dim reference to varible in main function contains maximum size of plot
 * @return vector of array with points positions
 */
vector <array<double, 4>> get_data(string path, double& destination_err, int& groups, int& iterations, int& max_dim){

    ifstream MyReadFile(path);
    vector <array<double, 4>> position;
    string  myText;
    int i =0;
    while (getline (MyReadFile, myText)) {
        if(myText.find("err") != string::npos){

            destination_err = stod(myText.substr(myText.find(" ") + 1));
        }else if(myText.find("cluster") != string::npos){
            groups = stoi(myText.substr(myText.find(" ") + 1));
        }else if(myText.find("iteration") != string::npos){
            iterations = stoi(myText.substr(myText.find(" ") + 1));
        }else if(i>2){
            if(myText.length() < 3){
                break;
            }
            stringstream linestream(myText);
            string tmpposition;
            array <double, 4> pos = {};
            int j = 0;
            while(getline(linestream,tmpposition,','))
            {
                if(stod(tmpposition) > max_dim){

                    max_dim = ceil(stod(tmpposition));

                }
                pos[j] = stof(tmpposition);
                j++;
            }
            pos[2] = -1;
            pos[3] = -1;
            position.push_back(pos);
        }

        i++;



    }
    MyReadFile.close();
    if(destination_err == 0 || groups == 0 || iterations == 0){
        throw invalid_argument("something is frong while procesing file.");
    };
// old method to pass data back from function
//    position.push_back({destination_err, static_cast<double>(groups), static_cast<double>(iterations), static_cast<double>(max_dim)});
    return position;
}
/**
 * Calculate Quantization error
 *
 * Calculate Quantization error for any iteration
 *
 * @param data vector of array with points position
 * @return error for current loop iteration
 */
double calculate_error(vector <array<double, 4>> data){
    double err = 0;
    double sum_distance = 0;
    for (int i = 0; i < data.size(); ++i) {
        sum_distance += data[i][3];
    }
    err = sum_distance/data.size();
    cout<<"error for this iteration: "<<err<<endl;

    return err;
}
/**
 * Randomly generate cluster
 *
 * Generate number of cluster
 *
 * @param cluster amount cluster to generate
 * @param max_dim maximum size of plot
 * @return vector of array with cluster positions
 */
vector <array<double, 2>> get_group_possition(int cluster, int max_dim){
    vector <array<double,2>> position;
    srand(time(0));
    for (int i = 0; i < cluster; ++i) {
        array<double, 2> pos = {};
        for (int j = 0; j < 2; ++j) {
            pos[j] = static_cast <double> (rand()) / (static_cast <double> (RAND_MAX/max_dim));;
        }
        position.push_back(pos);
    }
    return position;
}

/**
 * Calculate distance between two points
 *
 * Calculate distance bettween two points in same dimention
 * !!! One array have bigger size becouse of store distance and parent cluster index !!!
 *
 * @param point1 point from data
 * @param point2 cluster
 * @return distance bettwen points
 */
double calculate_distance(array<double, 4> point1, array<double, 2> point2){


    double distance = 0.0;


    for (int i = 0; i < point2.size(); i++) {
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
 * @param groups contains number of cluster
 * @return vector of array with cluster positions
 */

vector <array<double, 2>> kmeanspp(vector<array<double, 4>> data, int groups){
    srand(time(0));
    int firstRandom = rand() % data.size();
    vector<int> used_points = {};
    used_points.push_back(firstRandom);
    vector<array<double, 2>> clusters ={};

    clusters.push_back({data[firstRandom][0], data[firstRandom][1]});
    cout<<data.size()<<endl;
    for(int k =0; k < groups-1; k++){
        array<double, 2> farthestPoint = {};
        double maxDistance = 0.0;

        for (int i=0; i<data.size(); i++) {
            double minDistance = DBL_MAX;
            for (int j=0; j<clusters.size(); j++) {
                double distance = calculate_distance(data[i],clusters[j]);
                if (distance < minDistance) {
                    minDistance = distance;
                }
            }

            if (minDistance > maxDistance) {
                maxDistance = minDistance;
                farthestPoint[0] = data[i][0];
                farthestPoint[1] = data[i][1];
            }
        }
        clusters.push_back({farthestPoint[0], farthestPoint[1]});
    }
    return clusters;
}

/**
 * Find closer cluster to point
 *
 * Check the distance between all clusters and find the one closer
 *
 * @param data vector of array with points position
 * @param cluster vector of array with cluster position
 * @return vector of array with data with closer cluster index and distance
 */

vector <array<double, 4>> find_closer(vector<array<double,4>> data, vector<array<double,2>> cluster){
    for (int i = 0; i < data.size(); ++i) {
        vector<double> distances;
        for (int j = 0; j < cluster.size(); ++j) {
            double local_distance = calculate_distance(data[i], cluster[j]);
            distances.push_back(local_distance);
        }
        auto result = std::min_element(distances.begin(), distances.end());
        data[i][2] = std::distance(distances.begin(), result);
        data[i][3] = *result;


    }
    return data;
}

/**
 * Find new position for cluster
 *
 * Calculate avg distance and find new position for cluster
 *
 * @param data vector of array with points position
 * @param cluster vector of array with cluster position
 * @return vector of array with cluster position
 */
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
/**
// * Show points under cluster in output
// *
// *
// * @param data vector of array with points position
// * @param cluster id of cluster to show
// */
void show_points_to_cluster(int cluster, vector <array<double, 4>> data){
    for (int i = 0; i < data.size(); ++i) {
        if(data[i][2] == cluster){
            cout<<"("<<data[i][0]<<", "<<data[i][1]<<")"<<endl;
        }
    }
}
/**
 * Show cluster position in output
 *
 *
 * @param data vector of array with points position
 * @param cluster vector of array with cluster position
 */
void show_groups_possition(vector <array<double, 2>> cluster, vector <array<double, 4>> data){
    for (int i = 0; i < cluster.size(); i++) {
        cout<<"cluster "<<i+1<<" position: X:"<<cluster[i][0]<<", Y:"<<cluster[i][1]<<endl;
        cout<<"points: "<<endl;
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

    int groups = 0, iterations = 0, max_dim;

    double destination_err = 0, err = 0;
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
    vector <array<double, 4>> data = {};
    try{
         data = get_data(path, destination_err, groups, iterations, max_dim);
    }catch (const invalid_argument e){
        cout << e.what()<<endl;
        cout<<"Check the file pattern:"<<endl;
        file_Pattern(dir);
        return -1;
    }
//    old system to get data form file
//    destination_err = data[data.size()-1][0];
//    groups = data[data.size()-1][1];
//    iterations = data[data.size()-1][2];
//    max_dim = data[data.size()-1][3];
//    data.pop_back();
    vector <array<double, 2>> groups_position = {};
    //test data
//    groups_position = {{2.5, 3}, {8, 4.5}};
    int algorithm = 0;

    while(algorithm < 1 || algorithm > 2){
        cout<<"Select cluster generation algorithm\n[1] - random generation\n[2] - k-means++"<<endl;
        cin>>algorithm;
    }

    switch (algorithm) {
        case 1:
            groups_position = get_group_possition(groups, max_dim);
            break;
        case 2:
            groups_position = kmeanspp(data, groups);
            break;
    }

//    vector <array<double, 2>> groups_position = get_group_possition(groups);
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


        err = calculate_error(data);

        show_groups_possition(groups_position, data);
        if(err<destination_err){
            cout<<"Required accuracy achieved. Program stopped...."<<endl;
            cout<<"Defined error: "<<destination_err;
            save_to_file(data, dir, err, destination_err, iterations, groups);
            return 1;
        }
    }
    save_to_file(data, dir, err, destination_err, iterations, groups);
    cout<<"Failed to achieve required accuracy: "<<destination_err;







    return 0;
}



