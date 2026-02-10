#include <iostream>
#include <opencv2/opencv.hpp>

int main(int, char**){
    std::cout << "======== OpenCV Build Information ========" << std::endl;
    std::cout << cv::getBuildInformation() << std::endl;
    std::cout << "===========================================" << std::endl;
    std::cout << "Hello, hi imencv!\n";
}
