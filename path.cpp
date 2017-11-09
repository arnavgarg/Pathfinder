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
    // displays original image
    cv::namedWindow("original", cv::WINDOW_AUTOSIZE);
    cv::imshow("original", img);

    // runs thresholding on the image to isolate orange
    cv::inRange(img, cv::Scalar(115, 170, 240), cv::Scalar(190, 255, 255), img);

    // canny edge detection
    cv::blur(img, img, cv::Size(3, 3));
    cv::Canny(img, img, 50, 150);

    // creates an image to draw the lines on
    cv::Mat lineImg;
    cv::cvtColor(img, lineImg, cv::COLOR_GRAY2BGR);
    // finds all lines in image
    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(img, lines, 1, CV_PI/180, 25, 30, 10);
    for (int i = 0; i < lines.size(); i++) // drawing lines
    {
        cv::Vec4i l = lines[i];
        line(lineImg, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0,0,255), 3, CV_AA);
    }
    // displays img with detected lines
    cv::namedWindow("detected lines", cv::WINDOW_AUTOSIZE);
    cv::imshow("detected lines", lineImg);
    int size = lines.size();
    eliminateOutliers(lines);
    printf("%d %d\n", size, lines.size());

    cv::Mat lineImg2;
    cv::cvtColor(img, lineImg2, cv::COLOR_GRAY2BGR);
    for (int i = 0; i < lines.size(); i++) // drawing lines
    {
        cv::Vec4i l = lines[i];
        line(lineImg2, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(i*5+50,i*10+50, i*5+50), 3, CV_AA);
    }
    cv::namedWindow("path", cv::WINDOW_AUTOSIZE);
    cv::imshow("path", lineImg2);

    cv::waitKey(0);
    return 0;
}

