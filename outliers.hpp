#ifndef OUTLIERS
#define OUTLIERS

#include <opencv2/core.hpp>

class node
{
public:
    int line;
    int group;
    node* next;

    node(int line) : line(line) {}
};

void eliminateOutliers(std::vector<cv::Vec4i> &lines);

#endif
