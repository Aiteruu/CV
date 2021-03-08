#include <vector>
#include <iomanip>
#include <iostream> 
#include <fstream>  
#include <iterator> 
#include <cmath>
#include <algorithm> 
#include <string>
#include <limits>

using namespace std;

void show_img(vector<double> &img, string filename, int x_dim, int y_dim){
    ofstream ofs(filename);
    ofs << "P3" << endl << x_dim << ' ' << y_dim << endl << "255" << endl;
    for(auto v : img)
            ofs << (int) round(v) << " " << (int) round(v) << " " << (int) round(v) << " ";
}

void show_img(vector<vector<double>> &img, string filename, int x_dim, int y_dim){
    ofstream ofs(filename);
    ofs << "P3" << endl << x_dim << ' ' << y_dim << endl << "255" << endl;
    for(int y = 0; y < y_dim; y++)
        for(int x = 0; x < x_dim; x++)
            ofs << (int) round(img[x][y]) << " " << (int) round(img[x][y]) << " " << (int) round(img[x][y]) << " ";
}
vector<vector<double>> ConvSobel(vector<vector<double>> &img){
    vector<vector<double>> conv_result (img.size() - 2, vector<double> (img[0].size() - 2, 0));
    vector<double> grad_x {1, 0, 1, -2, 0, 2, -1, 0, 1};
    vector<double> grad_y {1, 2, 1, 0, 0, 0, -1, -2, -1};
    for(int y = 0; y < conv_result[0].size(); y++){      
        for(int x = 0; x < conv_result.size(); x++){
            double g_x = 0;
            double g_y = 0;
            for(int i = 0; i < 9; i++){
                g_x += grad_x[i] * img[x + i % 3][y + i / 3];
                g_y += grad_y[i] * img[x + i % 3][y + i / 3];
            }
            conv_result[x][y] = (pow(g_x * g_x + g_y * g_y, .5));
        }
    }
    return conv_result;
}

void threshold(vector<double> &img, double thresh){
    for(auto &v : img){
        v = v > thresh ? 255 : 0;
    }
}
void part1(){ 
    ifstream file("image.ppm");
    string str;
    file >> str;
    int x_dim, y_dim, color_size;
    file >> x_dim;
    file >> y_dim;
    file >> color_size;
    vector<double> greyscale;
    vector<double> pt(3);
    int padding = 1;

    for(int i = 0; i < x_dim * y_dim; i++){
        //cout << i << endl;
        file >> pt[0] >> pt[1] >> pt[2];
        greyscale.push_back((pt[0] + pt[1] + pt[2]) / 3);
    }
    show_img(greyscale, "imageg.ppm", x_dim, y_dim);
    vector<vector<double>> img (x_dim + 2 * padding, vector<double> (y_dim + 2 * padding, 0));
    for(int i = 0; i < greyscale.size(); i++){
        img[i % x_dim + 1][i / x_dim + 1] = greyscale[i];
    }
    auto result = ConvSobel(img);
    vector<double> flattened;
    for(int y = 0; y < y_dim; y++)
        for(int x = 0; x < x_dim; x++)
            flattened.push_back(result[x][y]);
    vector<double> flat_cp (flattened);
    sort(flat_cp.begin(), flat_cp.end());
    threshold(flattened, flat_cp[flat_cp.size() / 2] * 1.33);
    show_img(result, "imagem.ppm", x_dim, y_dim);
}
int main(int argc, char** argv){
    part1();
    return 0;
}