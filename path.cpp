#include<iostream>
#include<vector>
#include<algorithm>
#include<opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>

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
    // displays original image
    cv::namedWindow("original", cv::WINDOW_NORMAL);
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

    std::cout << middle.x << " ," << middle.y;

    // displays img with detected lines
    cv::namedWindow("detected lines", cv::WINDOW_NORMAL);
    cv::imshow("detected lines", lineImg);

    cv::waitKey(0);
    return 0;
}
