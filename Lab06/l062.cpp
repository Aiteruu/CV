#include <vector>
#include <iomanip>
#include <iostream> 
#include <fstream>  
#include <iterator> 
#include <cmath>
#include <algorithm> 
#include <string>
#include <limits>
#include <unordered_set>
using namespace std;

void show_img(vector<double> &img, string filename, int x_dim, int y_dim){
    ofstream ofs(filename);
    ofs << "P3" << endl << x_dim << ' ' << y_dim << endl << "255" << endl;
    for(auto v : img)
            ofs << (int) round(v) << " " << (int) round(v) << " " << (int) round(v) << " ";
}

void show_img(vector<vector<int>> &img, string filename, int x_dim, int y_dim){
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

void show_img_rgb(vector<vector<vector<int>>> &img, string filename, int x_dim, int y_dim){
    ofstream ofs(filename);
    ofs << "P3" << endl << x_dim << ' ' << y_dim << endl << "255" << endl;
    for(int y = 0; y < y_dim; y++)
        for(int x = 0; x < x_dim; x++)
            ofs << img[x][y][0] << " " << img[x][y][1] << " " << img[x][y][2] << " ";
    ofs.close();
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
    //show_img_debug(img, "imaged.ppm", x_dim, img.size()/x_dim);
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
    //cout << interval << endl;
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

void placePoint(int x, int y, int dx, int dy, vector<int> &color, vector<vector<vector<int>>> &pix, int x_dim, int y_dim){
    for(int i = 0; i < 8; i++){
        int dx_n = (i < 4 ? dx : dy) * (i % 4 < 2 ? 1 : -1);
        int dy_n = (i < 4 ? dy : dx) * (i % 2 == 0 ? 1 : -1);
        if(x + dx_n < x_dim && x + dx_n >= 0 && y + dy_n < y_dim && y + dy_n >= 0)
            pix[x + dx_n][y + dy_n] = color;
    }
}

void drawCircle(int x, int y, int r, vector<int> color, vector<vector<vector<int>>> &pix, int x_dim, int y_dim){
    int dx = 0, dy = r;
    int p = 3 - 2 * r;
    placePoint(x, y, dx, dy, color, pix, x_dim, y_dim);
    while(dx < dy){
        if(p < 0)
            p = p + 4 * dx + 6;
        else{
            p = p + 4 * (dx - dy) + 10;
            dy--;
        }
        dx++;
        placePoint(x, y, dx, dy, color, pix, x_dim, y_dim);
    }
}

void line(int x1, int x2, int y1, int y2, int x_dim, int y_dim, vector<vector<int>> &pix){
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
        if (drivingAxis && x >= 0 && x < x_dim && y >=0 && y < y_dim)
            pix[x][y] += 1;
        else if (drivingAxis && x >= 0 && x < y_dim && y >=0 && y < x_dim)
            pix[y][x] += 1;
        p += dir * 2 * dy;
        if(p > 0){
            y += dir;
            p -= 2 * dx;
        }
    }        
}

vector<vector<int>> vote(vector<vector<double>> &img, vector<double> &edges, int x_dim, int y_dim){
    vector<vector<int>> votes (x_dim, vector<int> (y_dim, 0));
    //cout << edges.size() << endl;
    for(int x = 0; x < x_dim; x++){
        for(int y = 0; y < y_dim; y++){
            if(edges[y * x_dim + x] > 0){
                double m = -1 * tan(img[x][y]);
                auto b = y - m * x;
                line(0, x_dim, b, m * x_dim + b, x_dim, y_dim, votes);
            }
        }
    }
    return votes;
}

vector<double> normalize_255(vector<double> img){
    vector<double> cp (img.begin(), img.end());

    sort(img.begin(), img.end());
    auto min = img[0];
    auto max = img[cp.size() - 1];
    //cout << max << endl;
    for(auto &v : cp){
        v = ( v - min ) / ( max - min ) * 255; 
    }
    return cp;
}
vector<double> and_concat(vector<double> &img1, vector<double> &img2){
    vector<double> out;
    for(int i = 0; i < img1.size(); i++){
        if(img2[i] > 0 && img1[i] > 0) out.push_back(255);
        else out.push_back(0);
    }
    return out;
}

int count_edge_pixels(vector<double> &edge_map, vector<vector<int>> &circles, int x, int r){
    auto count = 0;
    for(auto &i : circles[r-20]){
        if(x + i >= 0 && x + i < edge_map.size() && edge_map[x + i] > 10){
            count += 1;
        }
    }
    return count;
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
        file >> pt[0] >> pt[1] >> pt[2];
        greyscale.push_back((pt[0] + pt[1] + pt[2]) / 3);
    }

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

    double_threshold(flattened, 40, 65, x_dim);

    auto final_img = and_concat(flattened_cp, flattened);
    //show_img_debug(final_img, "imagef.ppm", x_dim, y_dim);
    auto votes = vote(theta, final_img, x_dim, y_dim);
    vector<double> flattened_votes;
    for(int y = 0; y < y_dim; y++)
        for(int x = 0; x < x_dim; x++)
            flattened_votes.push_back(votes[x][y]);
    //show_img(flattened_votes, "imagetest2.ppm", x_dim, y_dim);
    auto normalized_votes = normalize_255(flattened_votes);

    show_img(normalized_votes, "imagev.ppm", x_dim, y_dim);
    threshold(normalized_votes, 153);
    vector<vector<int>> reshape (x_dim, vector<int> (y_dim, 0));
    // for(int i = 0; i < normalized_votes.size(); i++){
    //     reshape[i%x_dim][i/x_dim] = normalized_votes[i];
    // }
    show_img(final_img, "debug_edge_map.ppm", x_dim, y_dim);
    vector<int> centers;
    unordered_set<int> duplicates;
    for(int i = 0; i < normalized_votes.size(); i++){
        if(normalized_votes[i] > 0){
            if(duplicates.count(i) == 0)
                centers.push_back(i);
            for(int x = -20; x < 20; x++){
                for(int y = -20; y < 20; y++){
                    duplicates.insert(i + x + y * x_dim);
                }
            }
        }
    }
    //cout << centers.size() << endl;
    vector<int> red {255, 0, 0};
    vector<vector<vector<int>>> pix(x_dim, vector<vector<int>> (y_dim, vector<int> (3, 255)));
    for(auto &v : centers){
        for(int r = 1; r <= 5; r++){
            auto x = v % x_dim;
            auto y = v / x_dim;
            //pix[x][y] = red;
            drawCircle(x, y, r, red, pix, x_dim, y_dim);
        }
    }
    show_img_rgb(pix, "imageCC.ppm", x_dim, y_dim);
    // vector<vector<vector<int>>> pix(x_dim, vector<vector<int>> (y_dim, vector<int> (3, 255)));
    // for(auto &v : centers){
    //     v = centers[centers.size() - 1];
    //     for(int r = 20; r <= 45; r+=5){
    //         auto x = v % x_dim;
    //         auto y = v / x_dim;
    //         pix[x][y] = red;
    //         drawCircle(x, y, r, red, pix, x_dim, y_dim);
    //     }
    //     break;
    // }
    show_img_rgb(pix, "image_debugger.ppm", x_dim, y_dim);
    int min_radius = 20;
    int max_radius = 40;
    vector<vector<int>> inside_circle;
    for(int r = 20; r < 45; r++){
        vector<int> temp(0);
        for(int x = -r; x <= r; x++){
            for(int y = -r; y <= r; y++){
                if( x * x + y * y <= r * r)
                    temp.push_back(y * x_dim + x);
            }
        }
        inside_circle.push_back(temp);
    }
    // vector<double> tester_circle (final_img.size());
    // for(auto &i : inside_circle[10]){
    //     tester_circle[centers[0] + i] = 255;
    // }
    // show_img(tester_circle, "fdsal.ppm", x_dim, y_dim);
    vector<int> coin_radii;
    int counter = 0;
    for(auto &v : centers){
        counter++;
        bool begin_count = false;
        auto init = count_edge_pixels(final_img, inside_circle, v, 20);
        for(int r=20; r < 45; r++){
            auto n = count_edge_pixels(final_img, inside_circle, v, r);
            if(begin_count && n - init < 10){
                coin_radii.push_back(r);
                break;
            }
            if(n - init > 10)
                begin_count = true;   
            init = n;
        }
        if(coin_radii.size() != counter){
            coin_radii.push_back(0);
        }
    }

    vector<double> coin_values;
    for(auto &v : coin_radii){
        if(v == 0)
            coin_values.push_back(0);
        else if(v <= 24)
            coin_values.push_back(10);
        else if(v <= 29)
            coin_values.push_back(1);
        else if(v <= 31)
            coin_values.push_back(5);
        else 
            coin_values.push_back(25);
    }
    vector<int> penny {255, 0, 0};
    vector<int> nickel {255, 0, 255};
    vector<int> dime {0, 0, 255};
    vector<int> quarter {0, 255, 0};
    vector<int> dollar {255, 255, 0};
    vector<vector<vector<int>>> pix2(x_dim, vector<vector<int>> (y_dim, vector<int> (3, 0)));
    double total_val = 0;
    for(int i = 0; i < centers.size(); i++){
        auto v = centers[i];
        auto r = coin_radii[i];
        auto c = coin_values[i];
        total_val += c;
        auto x = v % x_dim;
        auto y = v / x_dim;
        if(c == 1)
            drawCircle(x, y, r, penny, pix2, x_dim, y_dim);
        else if(c == 5)
            drawCircle(x, y, r, nickel, pix2, x_dim, y_dim);
        else if(c == 10)
            drawCircle(x, y, r, dime, pix2, x_dim, y_dim);
        else if(c == 25)
            drawCircle(x, y, r, quarter, pix2, x_dim, y_dim);
        else 
            drawCircle(x, y, r, dollar, pix2, x_dim, y_dim);
    }
    vector<int> white {255, 255, 255};
    for(int i = 0; i < final_img.size(); i++){
        if(final_img[i] > 0) pix2[i%x_dim][i/x_dim] = white;
    }
    show_img_rgb(pix2, "coins.ppm", x_dim, y_dim);
    cout << "Pennies: " << count(coin_values.begin(), coin_values.end(), 1);
    cout << ", Nickels: " << count(coin_values.begin(), coin_values.end(), 5);
    cout << ", Dimes: " << count(coin_values.begin(), coin_values.end(), 10);
    cout << ", Quarters: " << count(coin_values.begin(), coin_values.end(), 25);
    cout << ", Silver Dollars: " << count(coin_values.begin(), coin_values.end(), 100);
    cout << ", Total value: $" << total_val/100 << endl;

    ofstream ofs("results.txt");
    ofs << "Pennies: " << count(coin_values.begin(), coin_values.end(), 1);
    ofs << ", Nickels: " << count(coin_values.begin(), coin_values.end(), 5);
    ofs << ", Dimes: " << count(coin_values.begin(), coin_values.end(), 10);
    ofs << ", Quarters: " << count(coin_values.begin(), coin_values.end(), 25);
    ofs << ", Silver Dollars: " << count(coin_values.begin(), coin_values.end(), 100);
    ofs << ", Total value: $" << total_val/100 << endl;

}
int main(int argc, char** argv){
    part2();
    return 0;
}