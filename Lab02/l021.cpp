#include <iostream>
#include <cmath>
#include <vector>
#include <time.h>
#include <iomanip>
#include <fstream>

using namespace std;

struct Point{
    double x, y;
    friend ostream & operator << (ostream &out, const Point& c) {
        out << "(" << setprecision(17) << c.x << ", " << c.y << ")";
        return out;
    }
};

double my_random(){
    return (double)rand()/(double)RAND_MAX;
}   
double distance(double x1, double y1, double x2, double y2){
    return pow(x1 - x2, 2) + pow(y1 - y2, 2);
}
double min_distance(struct Point p1){
    vector<double> dist {distance(p1.x, p1.y, 0, 0), distance(p1.x, p1.y, 0, 1), distance(p1.x, p1.y, 1, 1), distance(p1.x, p1.y, 1, 0)};
    auto min_dist = dist[0];
    for (auto& c : dist) min_dist = min(c, min_dist);
    return min_dist;
}
int main(){
    srand (time(NULL));
    vector<struct Point> pts {Point{my_random(), my_random()}, Point{my_random(), my_random()}, Point{my_random(), my_random()}};
    auto max_dist = 0;
    for(int i = 0; i < pts.size(); i++) max_dist = min_distance(pts[i]) > min_distance(pts[max_dist]) ? i : max_dist;
    auto p3 = pts[max_dist];
    auto p1 = pts[(max_dist + 1) % 3];
    auto p2 = pts[(max_dist + 2) % 3];
    auto m = (p1.y - p2.y) / (p1.x - p2.x);
    auto lower_bound = (-1 * p3.y + m * p3.x) / m;
    auto upper_bound = (1 - p3.y + m * p3.x) / m;
    if(lower_bound > upper_bound) swap(lower_bound, upper_bound);
    lower_bound = max(lower_bound, 0.0);
    upper_bound = min(upper_bound, 1.0);
    auto y_in_bounds = false;
    auto x = my_random() * abs(upper_bound - lower_bound) + lower_bound;
    Point p4 {x, m * (x - p3.x) + p3.y};
    while(!y_in_bounds){
        if(p4.y > 0 && p4.y < 1 && p4.x != p3.x)
            y_in_bounds = true;
        else{
            x = my_random() * abs(upper_bound - lower_bound) + lower_bound;
            p4 = {x, m * (x - p3.x) + p3.y};
        }
    }
    
    ofstream ofs("points.txt");
    ofs << p1 << " , " << p2 << " , " << p3 << " , " << p4;
    ofs.close();
    return 0;
}