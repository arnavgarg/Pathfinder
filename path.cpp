#include<iostream>

#include<opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>

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
    cv::namedWindow("original", cv::WINDOW_NORMAL);
    cv::imshow("original", src);

    // runs thresholding on the image to isolate orange
    cv::Mat thresh;
    cv::inRange(src, cv::Scalar(115, 170, 240), cv::Scalar(190, 255, 255), thresh);
    cv::namedWindow("threshold", cv::WINDOW_NORMAL);
    cv::imshow("threshold", thresh);

    // canny edge detection
    cv::Mat canny;
    cv::blur(thresh, canny, cv::Size(3, 3));
    cv::Canny(canny, canny, 50, 150);

    cv::namedWindow("canny", cv::WINDOW_NORMAL);
    cv::imshow("canny", canny);

    cv::Mat lineImg;
    cv::cvtColor(canny, lineImg, cv::COLOR_GRAY2BGR);
    // finds all lines in image
    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(canny, lines, 1, CV_PI/180, 25, 30, 10);
    for (int i = 0; i < lines.size(); i++) // drawing lines
    {
        cv::Vec4i l = lines[i];
        line(lineImg, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0,0,255), 3, CV_AA);
    }
    cv::namedWindow("all lines", cv::WINDOW_NORMAL);
    cv::imshow("all lines", lineImg);

    cv::waitKey(0);
    return 0;
}
