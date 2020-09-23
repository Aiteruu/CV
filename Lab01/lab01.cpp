#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <cstdio>
#include <time.h>
#define SIZE 800
#define RGB false
using namespace std;

vector<vector<vector<int>>> pix(SIZE, vector<vector<int>> (SIZE, vector<int> (3, 255)));

void drawLine(int x1, int y1, int x2, int y2, vector<int> &color){
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
            drivingAxis ? pix[x][y] = color : pix[y][x] = color;
        p += dir * 2 * dy;
        if(p > 0){
            y += dir;
            p -= 2 * dx;
        }
    }
        
}
void placePoint(int x, int y, int dx, int dy, vector<int> color){
    for(int i = 0; i < 8; i++){
        int dx_n = (i < 4 ? dx : dy) * (i % 4 < 2 ? 1 : -1);
        int dy_n = (i < 4 ? dy : dx) * (i % 2 == 0 ? 1 : -1);
        if(x + dx_n < SIZE && x + dx_n >= 0 && y + dy_n < SIZE && y + dy_n >= 0)
            pix[x + dx_n][y + dy_n] = color;
    }
}
void drawCircle(int x, int y, int r, vector<int> color){
    int dx = 0, dy = r;
    int p = 3 - 2 * r;
    placePoint(x, y, dx, dy, color);
    while(dx < dy){
        if(p < 0)
            p = p + 4 * dx + 6;
        else{
            p = p + 4 * (dx - dy) + 10;
            dy--;
        }
        dx++;
        placePoint(x, y, dx, dy, color);
    }
}

void draw(){
    ofstream ofs("image.ppm");
    ofs << "P3" << endl << SIZE << ' ' << SIZE << endl << "255" << endl;
    for(int x = 0; x < SIZE; x++)
        for(int y = 0; y < SIZE; y++)
            ofs << pix[y][x][0] << " " << pix[y][x][1] << " " << pix[y][x][2] << " ";
    ofs.close();
}

double dist(int x1, int y1, int x2, int y2){
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

vector<double> incircle(vector<double> p, vector<double> lengths){
    vector<double> v = {0.0, 0.0, 0.0};
    double perimeter = 0;
    for(auto& x : lengths)
        perimeter += x;
    for(int i = 0; i < 3; i ++){ //no list comprehensions :(
        v[0] += p[i * 2] * lengths[(i + 1) % 3];
        v[1] += p[i * 2 + 1] * lengths[(i + 1) % 3];
    }
    v[0] /= perimeter;
    v[1] /= perimeter;
    auto s = perimeter / 2;
    v[2] = sqrt( s * (s - lengths[0]) * (s - lengths[1]) * (s - lengths[2]) ) / s;
    return v;
}

vector<double> circumcircle(vector<double> p, vector<double> lengths, double r){
    vector<double> v = {0.0, 0.0, 0.0};
    double perimeter = 0;
    vector<double> p1 {(p[0] + p[2])/2.0, (p[1] + p[3])/2.0};
    vector<double> p2 {(p[0] + p[4])/2.0, (p[1] + p[5])/2.0};
    double  a1 = -(p[0] - p[2]),
            b1 = p[3] - p[1],
            c1 = a1 * p1[0] + b1 * p1[1],

            a2 = -(p[0] - p[4]),
            b2 = p[5] - p[1],
            c2 = a2 * p2[0] + b2 * p2[1],
            det = a1 * b2 - a2 * b1;
    v[0] = (b2 * c1 - b1 * c2) / det;
    v[1] = (a1 * c2 - a2 * c1) / det;
    for(auto& x : lengths)
        perimeter += x;
    v[2] = lengths[0] * lengths[1] * lengths[2] / ( 4 * r * perimeter / 2);
    return v;
}

int main(int argc, char** argv){
    srand (time(NULL));
    vector<int> color {rand()%255, rand()%255, rand()%255};
    vector<double> p(6);
    for(int i = 0; i < 6; i++)
        p[i] = double(rand()%SIZE);
    vector<double> lengths {0, 0, 0};
    for(int i = 0; i < 6; i += 2)
        lengths[i/2] = (dist(p[i], p[i + 1], p[(i + 2) % 6], p[(i+3) % 6]));

    for(int i = 0; i < 6; i += 2)
        drawLine(int(p[i]), int(p[i + 1]), int(p[(i + 2) % 6]), int(p[(i+3) % 6]), color);

    //incircle
    auto in = incircle(p, lengths);
    drawCircle(round(in[0]), round(in[1]), round(in[2]), vector<int> {rand()%255, rand()%255, rand()%255});
    //circumcircle
    auto circum = circumcircle(p, lengths, in[2]);
    drawCircle(round(circum[0]), round(circum[1]), round(circum[2]), vector<int> {rand()%255, rand()%255, rand()%255});
    //9-point
    vector<double> midpoint(6);
    for(int i = 0; i < 6; i ++)
        midpoint[i] = (p[i] + p[(i + 2) % 6]) / 2;

    auto ninepoint = circumcircle(midpoint, lengths, in[2]);
    drawCircle(round(ninepoint[0]), round(ninepoint[1]), round(circum[2]/2), vector<int> {rand()%255, rand()%255, rand()%255});
    //euler line
    double slope = (ninepoint[1] - circum[1]) / (ninepoint[0] - circum[0]);
    double y_int = ninepoint[1] - slope * ninepoint[0];
    drawLine(0, round(y_int), SIZE, round(slope * SIZE + y_int), vector<int> {rand()%255, rand()%255, rand()%255});
    
    draw();
    cout << "done" << endl;
    return 0; 
}