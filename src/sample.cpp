#include <iostream>

#include <fstream>

#include <opencv2/opencv.hpp>

#include <opencv2/dnn.hpp>

#include <opencv2/dnn/all_layers.hpp>

using namespace std;

using namespace cv;

int main(int argc, char **argv)

{

    dnn::Net model = dnn::readNetFromONNX("/home/euroicc/digits/model1.onnx");

    // load the image from disk

    Mat image = imread(argv[1]);

    // create blob from image

    Mat blob = dnn::blobFromImage(image, 1, Size(28, 28), Scalar(0, 0, 0), false, false);

    // set the input blob for the neural network

    model.setInput(blob);

    // forward pass the image blob through the model

    Mat outputs = model.forward();

    // Print predicted class.

    // put the class name text on top of the image

    imshow("Image", image);

    cout << outputs << endl;

    // imwrite("result_image.jpg", image);

    return 0;
}