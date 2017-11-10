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
    cv::Mat src = cv::imread(argv[1], cv::IMREAD_COLOR);
    if (!src.data)
    {
        std::cout << "no image data found\n" << std::endl;
        return -1;
    }
    cv::Mat img = src.clone();

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

    // use the max and min bounds to find the middle
    int maxx = 0, minx = INT_MAX;
    int maxy = 0, miny = INT_MAX;
    for (int i = 0; i < lines.size(); i++)
    {
        maxx = std::max(std::max(lines[i][0], lines[i][2]), maxx);
        minx = std::min(std::min(lines[i][0], lines[i][2]), minx);
        maxy = std::max(std::max(lines[i][1], lines[i][3]), maxy);
        miny = std::min(std::min(lines[i][1], lines[i][3]), miny);
    }
    cv::Point middle = cv::Point((maxx+minx)/2, (maxy+miny)/2);

    cv::circle(src, middle, 4, cv::Scalar(255,0,0),-1);
    cv::namedWindow("path", CV_WINDOW_AUTOSIZE);
    cv::imshow("path", src);

    // outputs
    std::cout << "path center: (" << middle.x << ", " << middle.y << ")" << std::endl;
    std::cout << "confidence: " << confidence  << std::endl;

    cv::waitKey(0);
    return 0;
}

