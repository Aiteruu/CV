#include <vector>
#include <iomanip>
#include <iostream> 
#include <fstream>  
#include <iterator> 
#include <cmath>
#include <algorithm> 
#define K 5
#define DIM 2
#define NPOINTS 100
#define SIZE 800
using namespace std;

class Point{
    public:
    vector<double> features;
    Point(){
        for(int i = 0; i < DIM; i++)
            features.push_back(random_point());  
    }
    Point(vector<double> a){
        features = a;
    }
    static double random_point(){
        return (double)rand()/(double)RAND_MAX;
    }
    double distance(Point &a){
        double sum = 0;
        for(int i = 0; i < DIM; i++)
            sum += (features[i] - a.features[i]) * (features[i] - a.features[i]);
        return sqrt(sum);
    }
    bool operator==(const Point &other) const{ 
        return features == other.features;
    }
    void print(){
        for(auto &feature : features){
            cout << feature << " ";
        }
        cout << endl;
    }
};

Point centroid(vector<Point> &pts){
    vector<double> sum(pts[0].features.size());
    for(auto &pt : pts){
        for(int i = 0; i < pts[0].features.size(); i++){
            sum[i] += pt.features[i];
        }
    }
    for(auto &val : sum)
        val /= pts.size();
    return Point(sum);
}

vector<Point> new_centroids(vector<vector<Point>> clusters){
    vector<Point> centroids;
    for(auto cluster : clusters)
        centroids.push_back(centroid(cluster)); 
    return centroids;
}

int find_closest(vector<Point> &centroids, Point pt){
    int min_index = 0;
    double min_dist = 2;
    for(int i = 0; i < centroids.size(); i++){
        auto new_dist = pt.distance(centroids[i]);
        if(new_dist < min_dist){
            min_dist = new_dist;
            min_index = i;
        }
    }
    return min_index;
}

vector<vector<Point>> cluster(vector<Point> &centroids, vector<Point> &pts){
    vector<vector<Point>> clusters(centroids.size());
    for(auto &pt : pts)
        clusters[find_closest(centroids, pt)].push_back(pt);
    return clusters;
}

vector<Point> kmeans(vector<Point> &pts){
    vector<vector<Point>> centroids;
    centroids.push_back(vector<Point>(pts.begin(), pts.begin() + K));
    
    while(true){
        centroids.push_back(new_centroids(cluster(centroids[centroids.size() - 1], pts)));
        if(is_permutation(centroids[centroids.size() - 1].begin(), centroids[centroids.size() - 1].end(), centroids[centroids.size() - 2].begin()))
            return centroids[centroids.size() - 1];
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

void draw(vector<Point> &centroids, vector<Point> &pts){
    vector<vector<int>> colors {{5, 101, 179}, {92, 153, 110}, {108, 239, 221}, {217, 187, 249}, {198, 54, 47}};
    auto black = vector<int> {0, 0, 0};
    // for(auto &pt : centroids)
    //     colors.push_back(vector<int> {(rand()%255 + 200) / 2, (rand()%255 + 200) / 2, (rand()%255 + 200) / 2});

    vector<vector<vector<int>>> pix(SIZE, vector<vector<int>> (SIZE, vector<int> (3, 255)));

    for(auto pt : pts)
        drawCircle(pt.features[0] * 800, pt.features[1] * 800, 2, colors[find_closest(centroids, pt)], pix);
    for(auto pt : centroids)
        drawCircle(pt.features[0] * 800, pt.features[1] * 800, 3, black, pix);

    ofstream ofs("clusters.ppm");
    ofs << "P3" << endl << SIZE << ' ' << SIZE << endl << "255" << endl;
    for(int x = 0; x < SIZE; x++)
        for(int y = 0; y < SIZE; y++)
            ofs << pix[y][x][0] << " " << pix[y][x][1] << " " << pix[y][x][2] << " ";
    ofs.close();
}
void part1(){
    srand (time(NULL));
    vector<Point> pts;
    for(int i = 0; i < NPOINTS; i++)
        pts.push_back(Point());

    auto centroids = kmeans(pts);

    // for(auto &centroid : centroids)
    //     centroid.print();
    draw(centroids, pts);
}
int main(int argc, char** argv){
    part1();
    return 0;
}   