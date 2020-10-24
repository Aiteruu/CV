#include <iostream>
#include <cmath>
#include <vector>
#include <time.h>
#include <iomanip>
#include <fstream>
#include <string>
#include <algorithm>
#include <list>
#include <sstream>
#define SIZE 800
using namespace std;

struct Point{
    double x, y;
    friend ostream & operator << (ostream &out, const Point& c) {
        out << "(" << setprecision(17) << c.x << ", " << c.y << ")";
        return out;
    }
};
struct Line{
    double m, b;
    friend ostream & operator << (ostream &out, const Line& c) {
        out << "(" << setprecision(17) << c.m << ", " << c.b << ")";
        return out;
    }
};
double my_random(){
    return (double)rand()/(double)RAND_MAX;
}   

double distance(double x1, double y1, double x2, double y2){
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

double min_distance(struct Point p1){
    vector<double> dist {distance(p1.x, p1.y, 0, 0), distance(p1.x, p1.y, 0, 1), distance(p1.x, p1.y, 1, 1), distance(p1.x, p1.y, 1, 0)};
    auto min_dist = dist[0];
    for (auto& c : dist) min_dist = min(c, min_dist);
    return min_dist;
}
double slope(struct Point p1, struct Point p2){
    return (p2.y - p1.y) / (p2.x - p1.x);
}

double intercept(double x, double y, double m){
    return -m * x + y;
}
double x_intersect(double m1, double b1, double m2, double b2){
    return (b2 - b1) / (m1 - m2);
}
void part1(vector<struct Point> pts){
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
}

void draw(vector<vector<vector<int>>> &pix){
    ofstream ofs("output.ppm");
    ofs << "P3" << endl << SIZE << ' ' << SIZE << endl << "255" << endl;
    for(int x = 0; x < SIZE; x++)
        for(int y = 0; y < SIZE; y++)
            ofs << pix[y][x][0] << " " << pix[y][x][1] << " " << pix[y][x][2] << " ";
    ofs.close();
}

void drawLine(int x1, int y1, int x2, int y2, vector<int> color, vector<vector<vector<int>>> &pix){
    auto &c = color;
    bool drivingAxis = abs(x1 - x2) >= abs(y2 - y1); //True if x is the driving axis, False if y is the driving axis
    if(!drivingAxis){
        swap(x1, y1);
        swap(x2, y2);
    }
    if(x2 < x1){
        swap(x1, x2);
        swap(y1, y2);
    }
    int dir = y2 - y1 > 0 ? 1 : -1;
    int dx = x2 - x1, dy = y2 - y1;
    int p = 2 * dy - dx;
    for(int x = x1, y = y1; x < x2; x++){
        if (x >= 0 && x < SIZE && y >=0 && y < SIZE)
            drivingAxis ? pix[x][y] = c : pix[y][x] = c;
        p += dir * 2 * dy;
        if(p > 0){
            y += dir;
            p -= 2 * dx;
        }
    }        
}

void placePoint(int x, int y, int dx, int dy, vector<int> &color, vector<vector<vector<int>>> &pix){
    for(int i = 0; i < 8; i++){
        int dx_n = (i < 4 ? dx : dy) * (i % 4 < 2 ? 1 : -1);
        int dy_n = (i < 4 ? dy : dx) * (i % 2 == 0 ? 1 : -1);
        if(x + dx_n < SIZE && x + dx_n >= 0 && y + dy_n < SIZE && y + dy_n >= 0)
            pix[x + dx_n][y + dy_n] = color;
    }
}

void drawCircle(int x, int y, int r, vector<int> color, vector<vector<vector<int>>> &pix){
    int dx = 0, dy = r;
    int p = 3 - 2 * r;
    placePoint(x, y, dx, dy, color, pix);
    while(dx < dy){
        if(p < 0)
            p = p + 4 * dx + 6;
        else{
            p = p + 4 * (dx - dy) + 10;
            dy--;
        }
        dx++;
        placePoint(x, y, dx, dy, color, pix);
    }
}

void draw_part2(vector<Line> lines, vector<Point> points){
    vector<vector<vector<int>>> pix(SIZE, vector<vector<int>> (SIZE, vector<int> (3, 255)));
    vector<int> color {0, 0, 0};
    for(auto line : lines){
        drawLine(0, line.b * 800, 800, line.m * 800 + line.b * 800, color, pix);
    }
    for(auto p : points){
        drawCircle(p.x * 800, p.y * 800, 2, color, pix);
    }
    draw(pix);
}

vector<struct Line> part2_helper(Point p1, Point p2, Point p3, Point p4){
    auto m = slope(p1, p2);
    auto perp_m = -1 / m;
    auto perp_b = intercept(p3.x, p3.y, perp_m);
 
    auto x_i = x_intersect(m, intercept(p1.x, p1.y, m), perp_m, perp_b);

    auto l = distance(p3.x, p3.y, x_i, perp_m * x_i + perp_b);
    auto d = distance(p1.x, p1.y, p2.x, p2.y);

    auto del_x = abs(d / l) * abs(x_i - p3.x);
    auto new_x = p3.x + ((x_i - p3.x > 0) ? 1 : -1) * del_x;

    Point E = {new_x, new_x * perp_m + perp_b};
    auto edge_0_m = slope(E, p4);
    auto edge_0_b = intercept(E.x, E.y, edge_0_m);
    
    auto edge_1_m = -1 / edge_0_m;
    auto edge_1_b = intercept(p1.x, p1.y, edge_1_m);
    
    auto edge_2_m = edge_1_m;
    auto edge_2_b = intercept(p2.x, p2.y, edge_1_m);

    auto edge_3_m = edge_0_m;
    auto edge_3_b = intercept(p3.x, p3.y, edge_3_m);
    
    auto x_i1 = x_intersect(edge_0_m, edge_0_b, edge_1_m, edge_1_b);
    auto x_i2 = x_intersect(edge_0_m, edge_0_b, edge_2_m, edge_2_b);
    auto area = pow(distance(x_i1, edge_0_m * x_i1 + edge_0_b, x_i2, edge_0_m * x_i2 + edge_0_b), 2); 

    return vector<struct Line> { Line{edge_0_m, edge_0_b}, Line{edge_1_m, edge_1_b}, Line{edge_3_m, edge_3_b}, Line{edge_2_m, edge_2_b}, Line{area, 0.0}};
}
void part2(vector<struct Point> pts){
    auto p = pts[0];
    vector<vector<struct Line>> sq {};
    for(int i = 1; i < pts.size(); i++){
        auto m = slope(p, pts[i]);
        for(int i2 = 1; i2 < pts.size(); i2++){
            if(i2 != i){
                list<int> index {1,2,3};
                index.remove(i);
                index.remove(i2);
                sq.push_back(part2_helper(pts[0], pts[i], pts[i2], pts[index.front()]));
            }
        }
    }
    sort(sq.begin(), sq.end(), [](vector<struct Line> &a, vector<struct Line> const &b) {
    return a[4].m < b[4].m; 
    });

    ofstream ofs("output.txt");
    for(auto i = 0; i < 3; i++){
        ofs << pts[i] << " , "; 
    }
    ofs << pts[3] << endl;
    for(auto lines : sq){
        for(auto i = 0; i < 3; i++){
            ofs << lines[i] << " , "; 
        }
        ofs << lines[3] << " Area = " << lines[4].m << endl;
    }
    ofs.close();

    draw_part2(vector<Line>(sq[0].begin(), sq[0].end() - 1), pts);
}
bool isParen(char ch){
    return ch == '(' || ch == ')' || ch == ' ';
}
int main(){
    srand (time(NULL));
    vector<struct Point> pts {Point{my_random(), my_random()}, Point{my_random(), my_random()}, Point{my_random(), my_random()}};
    //part1(pts);

    ifstream infile;
    string points_str;
    infile.open("points.txt");
    getline(infile, points_str);
    replace(points_str.begin(), points_str.end(), ',', '\n');
    points_str.erase(remove_if(points_str.begin(), points_str.end(), isParen), points_str.end());
    istringstream f(points_str);
    string line, next;    
    vector<struct Point> pts2 {};
    while (getline(f, line)) {
        getline(f, next);
        pts2.push_back(Point{stod(line), stod(next)});
    }
    part2(pts2);

    return 0;
}