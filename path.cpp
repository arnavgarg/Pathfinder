#include <iostream>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

void eliminateOutliers(std::vector<cv::Vec4i> &lines);

int main(int argc, char* argv[1])
{
    // gets image from user input
    if (argc != 2)
    {
        std::cout << "usage: gate.out <image_path>\n" << std::endl;
        return -1;
    }
    cv::Mat img = cv::imread(argv[1], cv::IMREAD_COLOR);
    if (!img.data)
    {
        std::cout << "no image data found\n" << std::endl;
        return -1;
    }

    // runs thresholding on the image to isolate orange
    cv::inRange(img, cv::Scalar(115, 170, 240), cv::Scalar(190, 255, 255), img);

    // canny edge detection
    cv::blur(img, img, cv::Size(3, 3));
    cv::Canny(img, img, 50, 150);

    // finds all lines in image
    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(img, lines, 1, CV_PI/180, 25, 30, 10);

    // eliminates the outliers and finds the confidence value
    double total = lines.size();
    eliminateOutliers(lines);
    double confidence = lines.size() / total;

    // outputs
    std::cout << "confidence: " << confidence  << std::endl;
    return 0;
}

