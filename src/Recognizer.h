#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>


using namespace cv;
using namespace cv::ml;
#define MAX_NUM_IMAGES    60000

typedef unsigned char BYTE;

class DigitRecognizer
{
public:
    DigitRecognizer();

    ~DigitRecognizer();

    bool train();

    int classify(Mat img);

private:
    Mat preprocessImage(Mat img);

private:
    Ptr<KNearest>    knn;
    int numRows, numCols, numImages;

};