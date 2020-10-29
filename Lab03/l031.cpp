#include <iostream> 
#include <list> 
#include <iterator> 
#include <cmath>
#include <iomanip>
#include <vector>
#include <fstream>
#define SIZE 800
using namespace std;

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
        out << fixed << setprecision(17) << c.x << "  " << c.y << endl;
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
    return pow(a.get_x() - b.get_x(), 2) + pow(a.get_y() - b.get_y(), 2); 
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

void draw(vector<vector<vector<int>>> &pix){
    ofstream ofs("output.ppm");
    ofs << "P3" << endl << SIZE << ' ' << SIZE << endl << "255" << endl;
    for(int x = 0; x < SIZE; x++)
        for(int y = 0; y < SIZE; y++)
            ofs << pix[y][x][0] << " " << pix[y][x][1] << " " << pix[y][x][2] << " ";
    ofs.close();
}

void part1(){
    srand (time(NULL));
    list<Point> pts{};
    for(int i = 0; i < 100; i++){
        pts.push_front(Point());
    }
    Point *pt1, *pt2;
    double min_distance = 2;
    
    for(auto iter = pts.begin(); iter != pts.end(); iter++){
        for(auto iter2 = next(pts.begin()); iter2 != pts.end(); iter2++){\
            if(iter != iter2){
                auto temp = distance(*iter, *iter2);
                if(temp < min_distance){
                    min_distance = temp;
                    pt1 = &(*iter);
                    pt2 = &(*iter2);
                } 
            }
        }
    }

    vector<vector<vector<int>>> pix(SIZE, vector<vector<int>> (SIZE, vector<int> (3, 255)));
    vector<int> color {0,0,0};
    vector<int> color2 {255,0,0};
    ofstream ofs("points.txt");
    for(auto pt :pts){
        ofs << pt;
        drawCircle(pt.get_x() * SIZE, pt.get_y() * SIZE, 2, color, pix);
    }
    ofs.close();
    drawCircle((*pt1).get_x() * SIZE, (*pt1).get_y() * SIZE, 2, color2, pix);
    drawCircle((*pt2).get_x() * SIZE, (*pt2).get_y() * SIZE, 2, color2, pix);
    draw(pix);


}

int main(){
    part1();
    return 0;
}