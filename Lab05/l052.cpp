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
void show_img_debug(vector<double> &img, string filename, int x_dim, int y_dim){
    ofstream ofs(filename);
    ofs << "P1" << endl << x_dim << ' ' << y_dim << endl << "255" << endl;
    for(auto v : img){
        if(v == 0) ofs << 1 << " ";
        else ofs << 0 << " ";
    }       
}

vector<vector<double>> ConvSobel(vector<vector<double>> &img){
    vector<vector<double>> conv_result (img.size() - 2, vector<double> (img[0].size() - 2, 0));
    vector<double> grad_x {-1, 0, 1, -2, 0, 2, -1, 0, 1};
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

vector<vector<double>> ConvSobelTheta(vector<vector<double>> &img){
    vector<vector<double>> conv_result (img.size() - 2, vector<double> (img[0].size() - 2, 0));
    vector<double> grad_x {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    vector<double> grad_y {1, 2, 1, 0, 0, 0, -1, -2, -1};
    for(int y = 0; y < conv_result[0].size(); y++){      
        for(int x = 0; x < conv_result.size(); x++){
            double g_x = 0;
            double g_y = 0;
            for(int i = 0; i < 9; i++){
                g_x += grad_x[i] * img[x + i % 3][y + i / 3];
                g_y += grad_y[i] * img[x + i % 3][y + i / 3];
            }
            conv_result[x][y] = (atan2(g_y, g_x));
            if(isnan(conv_result[x][y])) cout << "??" << endl;
        }
    }
    return conv_result;
}

void threshold(vector<double> &img, double thresh){
    for(auto &v : img){
        v = v > thresh ? 255 : 0;
    }
}

void area_fill(vector<double> &img, int i, bool init, int x_dim){
    if(i >= 0 && i < img.size() && (init || img[i] == 1)){
        img[i] = 2;
        area_fill(img, i + 1, false, x_dim);
        area_fill(img, i - 1, false, x_dim);
        area_fill(img, i + x_dim, false, x_dim);
        area_fill(img, i + x_dim + 1, false, x_dim);
        area_fill(img, i + x_dim - 1, false, x_dim);
        area_fill(img, i - x_dim, false, x_dim);
        area_fill(img, i - x_dim + 1, false, x_dim);
        area_fill(img, i - x_dim - 1, false, x_dim);
    }
}

void double_threshold(vector<double> &img, double lower, double upper, int x_dim){
    for(double &v : img){
        if(v < lower) v = 0;
        else if (v > upper) v = 2;
        else v = 1;
    }
    show_img_debug(img, "imaged.ppm", x_dim, img.size()/x_dim);
    for(int i = 0; i < img.size(); i++){
        if(img[i] == 2)
            area_fill(img, i, true, x_dim);
    }
    for(double &v : img){
        if(v == 2) v = 255;
        else v = 0;
    }
}

bool check_max(vector<double> &img, int i1, int i2, int i3){
    if(i2 >= 0 && i2 < img.size() && img[i1] <= img[i2])
        return false;
    if(i3 >= 0 && i3 < img.size() && img[i1] <= img[i3])
        return false;
    return true;
}

void non_max_suppression(vector<double> &img, vector<double> &img_theta, int x_dim){
    auto interval = atan(1);
    cout << interval << endl;
    for(int i = 0; i < img.size(); i++){
        auto theta = round(img_theta[i] / interval);
        //cout << img_theta[i] << " | " << theta << endl; 
        // if(theta == -2 || theta == 2)
        //     img[i] = check_max(img, i, i + x_dim, i - x_dim);
        // else if(theta == -1 || theta == 3)
        //     img[i] = check_max(img, i, i + x_dim + 1, i - x_dim - 1);
        // else if(theta == 0 || theta == -4 || theta == 4)
        //     img[i] = check_max(img, i, i + 1, i - 1);
        // else if(theta == 1 || theta == -3)
        //     img[i] = check_max(img, i, i + x_dim - 1, i - x_dim + 1);
        // else cout << theta << endl;
        //cout << img_theta[i] << " | " << theta << endl; 
        if(theta == -2 || theta == 2){
            img[i] = check_max(img, i, i + x_dim, i - x_dim);
            if(img[i]){
                if(i + x_dim < img.size()) img[i + x_dim] = 0;
                if(i - x_dim >= 0) img[i - x_dim] = 0;
            }
        }
        else if(theta == -1 || theta == 3){
            img[i] = check_max(img, i, i + x_dim + 1, i - x_dim - 1);
            if(img[i]){
                if(i + x_dim + 1 < img.size()) img[i + x_dim + 1] = 0;
                if(i - x_dim - 1 >= 0) img[i - x_dim - 1] = 0;
            }
        }
        else if(theta == 0 || theta == -4 || theta == 4){
            img[i] = check_max(img, i, i + 1, i - 1);
            if(img[i]){
                if(i + 1 < img.size()) img[i + 1] = 0;
                if(i - 1 >= 0) img[i - 1] = 0;
            }
        }
        else if(theta == 1 || theta == -3){
            img[i] = check_max(img, i, i + x_dim - 1, i - x_dim + 1);
            if(img[i]){
                if(i + x_dim - 1 < img.size()) img[i + x_dim - 1] = 0;
                if(i - x_dim + 1 >= 0)img[i - x_dim + 1] = 0;
            }
        }
        else cout << theta << endl;
    }

}

vector<double> and_concat(vector<double> &img1, vector<double> &img2){
    vector<double> out;
    for(int i = 0; i < img1.size(); i++){
        if(img2[i] > 0 && img1[i] > 0) out.push_back(255);
        else out.push_back(0);
    }
    return out;
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
    auto min = flat_cp[0];
    auto max = flat_cp[flat_cp.size() - 1];
    double sum = 0;
    for(auto &v : flattened){
        v = ( v - min ) / ( max - min ) * 255; 
        sum += v;
    }
    cout << sum / flattened.size() * 4 << endl;
    threshold(flattened, sum / flattened.size() * 4);
    show_img(flattened, "imagem.ppm", x_dim, y_dim);
}
void part2(){
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
    auto theta = ConvSobelTheta(img);

    vector<double> flattened;
    for(int y = 0; y < y_dim; y++)
        for(int x = 0; x < x_dim; x++)
            flattened.push_back(result[x][y]);

    vector<double> flattened_cp (flattened);
    vector<double> flat_cp (flattened);

    sort(flat_cp.begin(), flat_cp.end());
    auto min = flat_cp[0];
    auto max = flat_cp[flat_cp.size() - 1];
    for(auto &v : flattened){
        v = ( v - min ) / ( max - min ) * 255; 
    }
    vector<double> flattened_theta;
    for(int y = 0; y < y_dim; y++)
        for(int x = 0; x < x_dim; x++)
            flattened_theta.push_back(theta[x][y]);

    non_max_suppression(flattened_cp, flattened_theta, x_dim);
    show_img_debug(flattened_cp, "image1.ppm", x_dim, y_dim);

    double_threshold(flattened, 16, 23.5, x_dim);
    show_img_debug(flattened, "image2.ppm", x_dim, y_dim);

    auto final_img = and_concat(flattened_cp, flattened);
    show_img_debug(final_img, "imagef.ppm", x_dim, y_dim);
    
}
int main(int argc, char** argv){
    part2();
    return 0;
}