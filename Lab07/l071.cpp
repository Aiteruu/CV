#include<opencv2/opencv.hpp>
#include<iostream>
using namespace std;
using namespace cv;
int main()
{
    Mat src = imread("image.jpg");
    resize(src, src, Size(src.cols * 0.5, src.rows * 0.5), 0, 0, cv::INTER_LINEAR);
    Mat greyscale, edgemap;
    
    
    cvtColor(src, greyscale, cv::COLOR_BGR2GRAY); 
    medianBlur(greyscale, greyscale, 5);
 
    Canny(greyscale, edgemap, 70, 190, 3);

    vector<Vec3f> coins;
    vector<int> radii;

    HoughCircles(greyscale, coins, HOUGH_GRADIENT, 1, greyscale.rows/30, 140, 38, 35, greyscale.rows/20);

    for(auto coin : coins){
        Point center = Point(coin[0], coin[1]);
        int radius = coin[2];
        radii.push_back(radius);
        circle(src, center, radius, Scalar(255, 255, 0), 3, LINE_AA);
    }
    imwrite("imagec.jpg", src);
    
    sort(radii.begin(), radii.end());
    
    for(auto r: radii)
        cout << r << ", ";
    cout << endl;
    
    double r_dime = radii[0] + 2;
    double r_penny = 1.06365159129 * r_dime;
    double r_nickel = 1.18425460637 * r_dime;
    double r_quarter = 1.35455053043 * r_dime + 5;
    
    cout << r_dime << endl;
    cout << r_penny << endl;
    cout << r_nickel << endl;
    cout << r_quarter << endl;
    cout << endl; 
    
    vector<int> coin_values(5, 0);
    for(auto r : radii){
        if(r <= r_dime)
            coin_values[0]++;
        else if(r <= r_penny)
            coin_values[1]++;
        else if(r <= r_nickel)
            coin_values[2]++;
        else if(r <= r_quarter)
            coin_values[3]++;
        else
            coin_values[4]++;
    }
    double total_val = 10 * coin_values[0] + 1 * coin_values[1] + 5 * coin_values[2] + 25 * coin_values[3] + 100 * coin_values[4];
    cout << "Pennies: " << coin_values[1];
    cout << ", Nickels: " << coin_values[2];
    cout << ", Dimes: " << coin_values[0];
    cout << ", Quarters: " << coin_values[3];
    cout << ", Silver Dollars: " << coin_values[4];
    cout << ", Total value: $" << total_val/100 << endl;

    ofstream ofs("results.txt");
    ofs << "Pennies: " << coin_values[1];
    ofs << ", Nickels: " << coin_values[2];
    ofs << ", Dimes: " << coin_values[0];
    ofs << ", Quarters: " << coin_values[3];
    ofs << ", Silver Dollars: " << coin_values[4];
    ofs << ", Total value: $" << total_val/100 << endl;
    return 0;
}