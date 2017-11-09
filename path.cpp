#include <iostream>
#include<algorithm>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

void eliminateOutliers(std::vector<cv::Vec4i> &lines);

class Rectangle
{
  private:
    double height;
    double width;
    cv::Point topLeft;

  public:
    Rectangle();
    void setValues(double inputh, double inputw, cv::Point inputp);
    double getHeight();
    double getWidth();
    cv::Point getPoint();
    cv::Point findMiddle();
};
Rectangle::Rectangle(){}
void Rectangle::setValues(double inputh, double inputw, cv::Point inputp)
{
  height = inputh;
  width = inputw;
  topLeft = inputp;
}
double Rectangle::getHeight()
{
  return height;
}
double Rectangle::getWidth()
{
  return width;
}
cv::Point Rectangle::getPoint()
{
  return topLeft;
}

cv::Point Rectangle::findMiddle()
{
  return cv::Point(topLeft.x + width/2, topLeft.y + height/2);
}


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

    double xvalues[(lines.size())*2];
    double yvalues[(lines.size())*2];

    double maxx, maxy, minx, miny;

    for (int i = 0; i < (lines.size())*2; i++)
    {
      xvalues[i] = (lines[i])[0];
      i++;
      xvalues[i] = (lines[i])[2];
    }
    for (int i = 0; i < (lines.size())*2; i++)
    {
      yvalues[i] = (lines[i])[1];
      i++;
      yvalues[i] = (lines[i])[3];
    }
    int xsize, ysize;
    xsize = sizeof(xvalues)/sizeof(xvalues[0]);
    ysize = sizeof(yvalues)/sizeof(yvalues[0]);
    maxx = *(std::max_element(xvalues, xvalues + xsize));
    minx = *(std::min_element(xvalues, xvalues + xsize));
    maxy = *(std::max_element(yvalues, yvalues + ysize));
    miny = *(std::min_element(yvalues, yvalues + ysize));


    //creates rectangle and find the middle point
    cv::Point topleft = cv::Point(minx, maxy);
    Rectangle rect;
    rect.setValues(maxy-miny, maxx-minx, topleft);
    cv::Point middle = rect.findMiddle();

    // eliminates the outliers and finds the confidence value
    double total = lines.size();
    eliminateOutliers(lines);
    double confidence = lines.size() / total;

    // outputs
    std::cout << middle.x << " ," << middle.y;
    std::cout << "confidence: " << confidence  << std::endl;
    return 0;
}

