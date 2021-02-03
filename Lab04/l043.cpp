#include <vector>
#include <iomanip>
#include <iostream> 
#include <fstream>  
#include <iterator> 
#include <cmath>
#include <algorithm> 
#include <string>
#include <limits>

#define DIM 2
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
    friend ostream & operator << (ostream &out, const Point& c) {
        out << setprecision(17) << c.features[0] << "  " << c.features[1] << endl;
        return out;
    }
};

class Node{
    public:
        Node *left, *right;
        bool orientation;
        double level, lower_bound, upper_bound, x, y;
        Node(){
            orientation = true;
            level = .5;
            lower_bound = 0;
            upper_bound = 1;
            x = 0;
            y = 0;
            left = NULL;
            right = NULL;
        }
        Node(bool o, double l, double lower, double upper, Point pt){
            orientation = o; //vertical == true, horizontal == false
            level = l;
            lower_bound = min(lower, upper);
            upper_bound = max(lower, upper);
            x = pt.features[0];
            y = pt.features[1];
            left = NULL;
            right = NULL;
        }
        double relative_pt_pos(Point p){ //returns pos if right or above, neg if left or below, 0 if not within bounds
            double x = p.features[0];
            double y = p.features[1];
            return (orientation ? x : y) - level;
        }
};

class KDTree{
    public:
        Node *head;
        vector<Node> node_list;
    KDTree(){
        head = NULL;
    }
    void insert(Point p){
        if(head == NULL){
            head = new Node(true, p.features[0], 0, 1, p);
            node_list.push_back(*head);
            return;
        }
        vector<double> bounds {0, 1, 0, 1}; //{lower y, upper y, lower x, upper x}

        Node *current = head;
        Node *prev;
        while(current != NULL){
            bool rel = current->relative_pt_pos(p) < 0; //true when left or below, false when right or above
            int orient = current->orientation;
            bounds[orient * 2 + rel] = current->level;
            prev = current;
            current = (rel ? current->left : current->right);
        }
        (prev->relative_pt_pos(p) < 0 ? prev->left : prev->right) = new Node (!prev->orientation, p.features[prev->orientation], bounds[(prev->orientation) * 2], bounds[(prev->orientation) * 2 + 1], p);
        node_list.push_back(*(prev->relative_pt_pos(p) < 0 ? prev->left : prev->right));
    }   
};

void drawLine(int x1, int x2, int y1, int y2, vector<int> color, vector<vector<vector<int>>> &pix){
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

vector<Point> input(){
    string s;
    cout << "Generate random points.txt?";
    cin >> s;
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    if(s == "yes"){
        cout << "Points saved to points.txt" << endl;
        return get_vector(10);
    }

    vector<Point> pts {};
    cout << "Reading from points.txt" << endl;
    ifstream file("points.txt");
    vector<double> pt (2);
    while(file){
        file >> pt[0] >> pt[1];
        Point c(pt);
        pts.push_back(c);
    }
    return pts;
}

void draw(KDTree tree){
    vector<vector<vector<int>>> pix(SIZE, vector<vector<int>> (SIZE, vector<int> (3, 255)));
    vector<int> blue {0, 0, 255};
    vector<int> red {255, 0, 0};
    vector<int> black {0, 0, 0};

    for(auto node : tree.node_list){
        if(node.orientation)
            drawLine(node.level * 800, node.level * 800, node.lower_bound * 800, node.upper_bound * 800, red, pix);
        else
            drawLine(node.lower_bound * 800, node.upper_bound * 800, node.level * 800, node.level * 800, blue, pix);
        drawCircle(node.x * 800, node.y * 800, 2, black, pix);
    }

    ofstream ofs("diagram.ppm");
    ofs << "P3" << endl << SIZE << ' ' << SIZE << endl << "255" << endl;
    for(int x = 0; x < SIZE; x++)
        for(int y = 0; y < SIZE; y++)
            ofs << pix[y][x][0] << " " << pix[y][x][1] << " " << pix[y][x][2] << " ";
    ofs.close();
}

void part3(){
    auto pts = input();

    auto kdtree = KDTree();
    for(auto pt : pts)
        kdtree.insert(pt);
    draw(kdtree);
    cout << "finished creating kd-tree and diagram.ppm" << endl;
}
int main(int argc, char** argv){
    part3();
    return 0;
}