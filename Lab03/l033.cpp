#include <iostream> 
#include <list> 
#include <iterator> 
#include <cmath>
#include <iomanip>
#include <vector>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <sstream>
#include <string>
#define SIZE 800
#define NPOINTS 1000
using namespace std;
using namespace std::chrono;


class Point{
    private:
        double x, y;
    public:
    Point(){
        x = random_point();
        y = random_point();
    }
    Point(double x1, double y1){
        x = x1;
        y = y1;
    }
    friend ostream & operator << (ostream &out, const Point& c) {
        out << fixed << setprecision(17) << scientific << c.x << "   " << c.y << endl;
        return out;
    }
    static double random_point(){
        return (double)rand()/(double)RAND_MAX;
    }
    double get_x(){
        return x;
    }
    double get_y(){
        return y;
    }
    void set_x(double x){
        this->x = x;
    }
    void set_y(double y){
        this->y = y;
    }
};
double distance(Point &a, Point &b){
    //return pow(a.get_x() - b.get_x(), 2) + pow(a.get_y() - b.get_y(), 2); 
    return pow((a.get_x() - b.get_x()) * (a.get_x() - b.get_x()) + (a.get_y() - b.get_y()) * (a.get_y() - b.get_y()), .5);
}

vector<Point> get_vector(int n){
    srand (time(NULL));
    vector<Point> pts{};
    for(int i = 0; i < n; i++){
        pts.push_back(Point());
    }
    ofstream ofs("points.txt");
    for(auto it = pts.begin(); it != pts.end(); it++) 
        ofs << *it;
    ofs.close();
    return pts;
} 

vector<int> p1(vector<Point> &pts){
    int pt1, pt2;
    double min_distance = 2;
    
    for(auto i = 0; i < pts.size(); i++){
        for(auto i2 = 0; i2 < i; i2++){
            auto temp = distance(pts[i], pts[i2]);
            if(temp < min_distance){
                min_distance = temp;
                pt1 = i;
                pt2 = i2;
            } 
        }
    }
    return vector<int> {pt1, pt2};
}
vector<int> recur(vector<Point> &pts, int lower_bound, int upper_bound){
    if(abs(upper_bound - lower_bound) == 2)
        return vector<int> {lower_bound, upper_bound - 1};
    if(abs(upper_bound - lower_bound) == 3){
        auto d1 = distance(pts[lower_bound], pts[upper_bound - 1]);

        auto d2 = distance(pts[lower_bound], pts[upper_bound - 2]);

        auto d3 = distance(pts[upper_bound - 1], pts[upper_bound - 2]);

        if(d1 <= d2 && d1 <= d3) return vector<int> {lower_bound, upper_bound - 1};
        if(d2 <= d1 && d2 <= d3) return vector<int> {lower_bound, upper_bound - 2};
        return vector<int> {upper_bound - 1, upper_bound - 2};
    }
    auto mid = (upper_bound + lower_bound) / 2;
    auto l = recur(pts, lower_bound, mid);
    auto r = recur(pts, mid, upper_bound);
    auto dist_l = distance(pts[l[0]], pts[l[1]]);
    auto dist_r = distance(pts[r[0]], pts[r[1]]);

    double d;
    vector<int> min_split;
    if(dist_l < dist_r){
        d = dist_l;
        min_split = l;
    }
    else{
        d = dist_r;
        min_split = r;
    }

    auto upper = pts[mid].get_x() + d;
    auto lower = pts[mid].get_x() - d;

    vector<Point> slice{};
    int offset = -1;
    for(int i = lower_bound; i < upper_bound; i++){
        if(pts[i].get_x() < upper && pts[i].get_x() > lower) slice.push_back(pts[i]);
        if(offset < 0 && pts[i].get_x() > lower) offset = i;
    }
    auto s = min_split;
    auto dist_s = d;
    if(slice.size() > 1){
        s = p1(slice);
        s[0] += offset;
        s[1] += offset;
        dist_s = distance(pts[s[0]], pts[s[1]]);

    }

    if(dist_s < d) return s;
    return min_split;
}
vector<int> p3_helper(vector<vector<Point>> pts, double d){
    int pt1 = -1;
    int pt2 = -1;
    for(int i = 0; i < pts.size(); i++){
        for(int i2 = i + 1; i2 < i + 8; i2++){
            if(i2 < pts.size() && distance(pts[i][0], pts[i2][0]) < d){
                d = distance(pts[i][0], pts[i2][0]);
                pt1 = pts[i][1].get_x();
                pt2 = pts[i2][1].get_x();
            }
        }
    }
    return vector<int> {pt1, pt2};
}
vector<int> recur_pt3(vector<Point> &pts, int lower_bound, int upper_bound){
    if(abs(upper_bound - lower_bound) == 2)
        return vector<int> {lower_bound, upper_bound - 1};
    if(abs(upper_bound - lower_bound) == 3){
        auto d1 = distance(pts[lower_bound], pts[upper_bound - 1]);

        auto d2 = distance(pts[lower_bound], pts[upper_bound - 2]);

        auto d3 = distance(pts[upper_bound - 1], pts[upper_bound - 2]);

        if(d1 <= d2 && d1 <= d3) return vector<int> {lower_bound, upper_bound - 1};
        if(d2 <= d1 && d2 <= d3) return vector<int> {lower_bound, upper_bound - 2};
        return vector<int> {upper_bound - 1, upper_bound - 2};
    }
    auto mid = (upper_bound + lower_bound) / 2;
    auto l = recur(pts, lower_bound, mid);
    auto r = recur(pts, mid, upper_bound);
    auto dist_l = distance(pts[l[0]], pts[l[1]]);
    auto dist_r = distance(pts[r[0]], pts[r[1]]);

    double d;
    vector<int> min_split;
    if(dist_l < dist_r){
        d = dist_l;
        min_split = l;
    }
    else{
        d = dist_r;
        min_split = r;
    }

    auto upper = pts[mid].get_x() + d;
    auto lower = pts[mid].get_x() - d;

    vector<vector<Point>> slice;
    int offset = -1;
    for(int i = lower_bound; i < upper_bound; i++){
        if(pts[i].get_x() < upper && pts[i].get_x() > lower){
            slice.push_back(vector<Point> {pts[i], Point(i, i)});
        }
        if(offset < 0 && pts[i].get_x() > lower) offset = i;
    }
    auto s = min_split;
    auto dist_s = d;
    if(slice.size() > 1){
        sort(slice.begin(), slice.end(), 
        [](vector<Point> &a, vector<Point> &b) -> bool
        { 
            return a[0].get_x() < b[0].get_x(); 
        });

        s = p3_helper(slice, d);
        if(s[0] > 0){
            s[0] += offset;
            s[1] += offset;
            dist_s = distance(pts[s[0]], pts[s[1]]);
        }
    }

    if(dist_s < d) return s;
    return min_split;
}

vector<int> p2(vector<Point> &pts){
    sort(pts.begin(), pts.end(), 
        [](Point &a, Point &b) -> bool
    { 
        return a.get_x() < b.get_x(); 
    });
    return recur(pts, 0, pts.size());
}

vector<int> p3(vector<Point> &pts){
    sort(pts.begin(), pts.end(), 
        [](Point &a, Point &b) -> bool
    { 
        return a.get_x() < b.get_x(); 
    });
    return recur_pt3(pts, 0, pts.size());
}

void part1(vector<Point> &points){
    ofstream ofs("results.txt");
    auto start = high_resolution_clock::now(); 
    auto closest_part1 = p1(points);
    cout << "Part 1" << endl;
    cout << points[closest_part1[0]] << points[closest_part1[1]];
    ofs << "Part 1" << endl;
    ofs << points[closest_part1[0]] << points[closest_part1[1]];
    auto stop = high_resolution_clock::now(); 
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "distance: " << distance(points[closest_part1[0]], points[closest_part1[1]]) << endl;
    cout << duration.count() << " milliseconds" << endl << endl;
    ofs << "distance: " << distance(points[closest_part1[0]], points[closest_part1[1]]) << endl;
    ofs << duration.count() << " milliseconds" << endl << endl;
    ofs.close();
}
void part2(vector<Point> &points){
    ofstream ofs;
    ofs.open("results.txt", ios_base::app);
    auto start = high_resolution_clock::now(); 
    auto closest_part2 = p2(points);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Part 2" << endl;
    cout << points[closest_part2[0]] << points[closest_part2[1]];
    cout << "distance: " << distance(points[closest_part2[0]], points[closest_part2[1]]) << endl;
    cout << duration.count() << " microseconds" << endl;

    ofs << "Part 2" << endl;
    ofs << points[closest_part2[0]] << points[closest_part2[1]];
    ofs << "distance: " << distance(points[closest_part2[0]], points[closest_part2[1]]) << endl;
    ofs << duration.count() << " microseconds";
    ofs.close();
}
void part3(vector<Point> &points){
    ofstream ofs;
    ofs.open("results.txt", ios_base::app);
    auto start = high_resolution_clock::now(); 
    auto closest_part2 = p3(points);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Part 3" << endl;
    cout << points[closest_part2[0]] << points[closest_part2[1]];
    cout << "distance: " << distance(points[closest_part2[0]], points[closest_part2[1]]) << endl;
    cout << duration.count() << " microseconds" << endl;

    ofs << "Part 3" << endl;
    ofs << points[closest_part2[0]] << points[closest_part2[1]];
    ofs << "distance: " << distance(points[closest_part2[0]], points[closest_part2[1]]) << endl;
    ofs << duration.count() << " microseconds";
    ofs.close();
}
vector<Point> read_file(string path){
    ifstream infile;
    string points_str;
    vector< Point> pts2 {};
    infile.open(path);
    while(getline(infile, points_str)){
        replace(points_str.begin(), points_str.end(), ' ', '\n');
        istringstream f(points_str);
        string line, next;    

        while (getline(f, line)) {
            getline(f, next);
            while(next == "")
                getline(f, next);
            pts2.push_back(Point{stod(line), stod(next)});
        }
    }
    return pts2;
}
auto points = get_vector(NPOINTS);
//auto points = read_file("points100k.dat"); 
int main(int argc, char** argv){
    part2(points);
    part3(points);
    return 0;
}   