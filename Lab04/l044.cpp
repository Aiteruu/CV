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
#define K 5
using namespace std;

class Point{
    private:
    vector<double> features;
    bool valid;
    public:
    Point(){
        for(int i = 0; i < DIM; i++)
            features.push_back(random_point());  
        valid = true;
    }
    Point(double x, double y){
        features.push_back(x);
        features.push_back(y);
        valid = true;
    }
    Point(vector<double> a){
        features = a;
        valid = true;
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
    vector<double> get_features(){
        return features;
    }
    double x(){
        return features[0];
    }
    double y(){
        return features[1];
    }
    void set_valid(bool b){
        valid = b;
    }
    bool get_valid(){
        return valid;
    }
};

class Node{
    private:
        Node *left, *right;
        bool orientation;
        double level, lower_bound, upper_bound, x, y;
        vector<double> bounds;
        vector<Point> centroids;
    public:
        Node(){
            orientation = true;
            level = .5;
            lower_bound = 0;
            upper_bound = 1;
            bounds = {0, 1, 0, 1};
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
            bounds = {0, 1, 0, 1};
            x = pt.get_features()[0];
            y = pt.get_features()[1];
            left = NULL;
            right = NULL;
        }
        Node(bool o, double l, double lower, double upper, vector<double> bound, Point pt){
            orientation = o; //vertical == true, horizontal == false
            level = l;
            lower_bound = min(lower, upper);
            upper_bound = max(lower, upper);
            bounds = {0, 1, 0, 1};
            x = pt.get_features()[0];
            y = pt.get_features()[1];
            left = NULL;
            right = NULL;
            bounds = bound;
        }
        double relative_pt_pos(Point p){ //returns pos if right or above, neg if left or below, 0 if not within bounds
            double x = p.get_features()[0];
            double y = p.get_features()[1];
            return (orientation ? x : y) - level;
        }
        bool get_orientation(){
            return orientation;
        }
        double get_level(){
            return level;
        }
        Node *get_left(){
            return left;
        }
        Node *get_right(){
            return right;
        }
        vector<Point> get_bounds(){
            return vector<Point>{ Point{bounds[0], bounds[2]}, Point{bounds[0], bounds[3]}, Point{bounds[1], bounds[2]}, Point{bounds[1], bounds[3]} };
        }
        void set_right(Node * n){
            right = n;
        }
        void set_left(Node * n){
            left = n;
        }
        void set_centroids(vector<Point> c){
            centroids = c;
        }
        vector<Point> get_centroids(){
            return centroids;
        }
        Point get_point(){
            return Point{x, y};
        }
};

class KDTree{
    private:
        Node *head;
        vector<Node*> node_list;
    public:
        KDTree(){
            head = NULL;
        }
        void insert(Point p){
            if(head == NULL){
                head = new Node(true, p.get_features()[0], 0, 1, p);
                node_list.push_back(head);
                return;
            }
            vector<double> bounds {0, 1, 0, 1}; //{lower y, upper y, lower x, upper x}

            Node *current = head;
            Node *prev;
            while(current != NULL){
                bool rel = current->relative_pt_pos(p) < 0; //true when left or below, false when right or above
                int orient = current->get_orientation();
                bounds[orient * 2 + rel] = current->get_level();
                prev = current;
                current = (rel ? current->get_left() : current->get_right());
            }
            Node *n = new Node (!prev->get_orientation(), p.get_features()[prev->get_orientation()], bounds[(prev->get_orientation()) * 2], bounds[(prev->get_orientation()) * 2 + 1], bounds, p);
            if(prev->relative_pt_pos(p) < 0)
                prev->set_left(n);
            else
                prev->set_right(n);
            node_list.push_back((prev->relative_pt_pos(p) < 0 ? prev->get_left() : prev->get_right()));
        }
        bool dominate(vector<Point> centroids, Node node, int i, int j){ //returns true if i dominates j
            auto c1 = centroids[i];
            auto c2 = centroids[j];
            auto a = 1;
            auto m = -1 * (c1.x() - c2.x()) / (c1.y() - c2.y());
            auto b = -1 * m;
            auto x = (c1.x() + c2.x()) * .5;
            auto y = (c1.y() + c2.y()) * .5;
            auto c = -1 * y + m * x;
            
            auto ref = a * c1.y() + b * c1.x() + c;
            for(auto pt : node.get_bounds()){
                auto eval = a * pt.y() + b * pt.x() + c;
                if(eval * ref < 0)
                    return false;
            }
            return true;
            // i dominates j if all corners of hyper-rectangle are closer to i
            // for(auto pt : node.get_bounds()){
            //     if(pt.distance(c1) > pt.distance(c2))
            //         return false;
            // }
            // return true;
        }
        void eliminate_centroids(vector<Point> centroids){
            prune(head, centroids);
        }
        void prune(Node *node, vector<Point> centroids){
            if(node == NULL)
                return;

            for(int i = 0; i < centroids.size(); i++){
                for(int j = 0; j < centroids.size(); j++){
                    if(i != j)
                        if(dominate(centroids, *node, i, j)){
                            centroids[j].set_valid(false);
                        }
                }
            }
            node->set_centroids(centroids);
            prune(node->get_left(), centroids);
            prune(node->get_right(), centroids);
        }
        vector<Node*> get_list(){
            return node_list;
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

Point centroid(vector<Point> &pts){
    if(pts.size() == 0)
        return Point(vector<double>{0,0,0});
    vector<double> sum(pts[0].get_features().size());
    for(auto &pt : pts){
        for(int i = 0; i < pts[0].get_features().size(); i++){
            sum[i] += pt.get_features()[i];
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

int find_closest(vector<Point> centroids, Point pt){
    int min_index = 0;
    double min_dist = 10000;
    for(int i = 0; i < centroids.size(); i++){
        auto new_dist = pt.distance(centroids[i]);
        if(new_dist < min_dist){
            min_dist = new_dist;
            min_index = i;
        }

    }
    return min_index;
}

void print_vector(vector<Point> a){
    for(auto c : a){
        cout << c;
    }
    cout << endl;
}
vector<vector<Point>> cluster(vector<Point> &centroids, KDTree tree){
    //print_vector(centroids);
    vector<vector<Point>> clusters(centroids.size());
    tree.eliminate_centroids(centroids);
    for(auto node : tree.get_list()){
        clusters[find_closest(node->get_centroids(), node->get_point())].push_back(node->get_point());
    }
    return clusters;
}

vector<Point> kmeans(vector<Point> &pts){
    vector<vector<Point>> centroids;
    centroids.push_back(vector<Point>(pts.begin(), pts.begin() + K));
    auto kdtree = KDTree();
    for(auto pt : pts)
        kdtree.insert(pt);
    while(true){
        centroids.push_back(new_centroids(cluster(centroids[centroids.size() - 1], kdtree)));
        if(is_permutation(centroids[centroids.size() - 1].begin(), centroids[centroids.size() - 1].end(), centroids[centroids.size() - 2].begin()))
            return centroids[centroids.size() - 1];
    }
}

void draw(vector<Point> &centroids, vector<Point> &pts){
    vector<vector<int>> colors {{5, 101, 179}, {92, 153, 110}, {108, 239, 221}, {217, 187, 249}, {198, 54, 47}};
    auto black = vector<int> {0, 0, 0};
    // for(auto &pt : centroids)
    //     colors.push_back(vector<int> {(rand()%255 + 200) / 2, (rand()%255 + 200) / 2, (rand()%255 + 200) / 2});

    vector<vector<vector<int>>> pix(SIZE, vector<vector<int>> (SIZE, vector<int> (3, 255)));

    for(auto pt : pts)
        drawCircle(pt.x() * 800, pt.y() * 800, 2, colors[find_closest(centroids, pt)], pix);
    for(auto pt : centroids)
        drawCircle(pt.x() * 800, pt.y() * 800, 3, black, pix);

    ofstream ofs("clusters.ppm");
    ofs << "P3" << endl << SIZE << ' ' << SIZE << endl << "255" << endl;
    for(int x = 0; x < SIZE; x++)
        for(int y = 0; y < SIZE; y++)
            ofs << pix[y][x][0] << " " << pix[y][x][1] << " " << pix[y][x][2] << " ";
    ofs.close();
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
        return get_vector(50);
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

void part4(){
    auto pts = input();
    auto centroids = kmeans(pts);
    draw(centroids, pts);
    cout << "finished kmeans with kd-tree and clusters.ppm" << endl;
}
int main(int argc, char** argv){
    part4();
    return 0;
}