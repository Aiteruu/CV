#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <cstdio>
#define SIZE 800
#define RGB false
using namespace std;

vector<vector<vector<int>>> pix(SIZE, vector<vector<int>> (SIZE, vector<int> (3, 0)));

void drawLine(int x1, int y1, int x2, int y2){
    vector<int> color {255,255,255};
    bool drivingAxis = abs(x1 - x2) > abs(y2 - y1); //True if x is the driving axis, False if y is the driving axis
    if(drivingAxis && x2 < x1){
        swap(x1, x2);
        swap(y1, y2);
    }
    else if(!drivingAxis && y2 < y1){
        swap(x1, x2);
        swap(y1, y2);
    }
    if(x1 == x2){ //vertical case
        for(int y = y1; y <= y2; y++)
            pix[x1][y] = color;
    }
    else if(y1 == y2){ //horizontal case
        for(int x = x1; x <= x2; x++)
            pix[x][y1] = color;
    }
    else{ //general case
        if(!drivingAxis){
            swap(x1, x2);
            swap(y1, y2);
        }
        int dx = x2 - x1, dy = y2 - y1;
        int v[] = {2 * dy - 2 * dx, 2 * dy};
        int p = 2 * dy - dx;
        for(int x = x1, y = y1; x<= x2; x++){
            drivingAxis ? pix[x][y] : pix[y][x] = color;
            if(p > 0){
                p += v[0];
                y++;
            }
            else
                p += v[1];
        }
    }    
};
void draw(){
    ofstream ofs("image.ppm", ios_base::out | ios_base::binary);
    ofs << "P6" << endl << SIZE << ' ' << SIZE << endl << "255" << endl;
 
    for(int x = 0; x < SIZE; x++)
        for(int y = 0; y < SIZE; y++)
            ofs << pix[x][y][0] << " " << pix[x][y][1] << " " << pix[x][y][2] << " ";
    ofs.close();
};

int main(){
    drawLine(rand()%800, rand()%800, rand()%800, rand()%800);
    draw();
    return 0; 
};