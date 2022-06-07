#include "Recognizer.h"
#include <iostream>
#include <vector>
#include <opencv2/imgproc/imgproc.hpp>


//#define TEST 1

/*
 * Constructor
 */
DigitRecognizer::DigitRecognizer()
{
	this->knn = cv::ml::KNearest::create();
}

/*
 * Destructor
 */
DigitRecognizer::~DigitRecognizer()
{
	delete this->knn;
}


/*
 * Function that preprocess image, bluring, tresholding and inveting are first steps,
 * then it looks after bigest blob
 */
Mat DigitRecognizer::preprocessImage(Mat img)
{

	Mat grayImage,blurredImage,thresholdImage,contourImage,regionOfInterest;

	std::vector<std::vector<Point> > contours;

	grayImage = img;

	GaussianBlur(grayImage, blurredImage, Size(7,7), 0 );
	adaptiveThreshold(blurredImage, thresholdImage, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 5, 2);
	bitwise_not(thresholdImage,thresholdImage);
	thresholdImage.copyTo(contourImage);

	
	findContours(contourImage, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

	int idx = 0;
	size_t area = 0;
	for (size_t i = 0; i < contours.size(); i++){
		if (area < contours[i].size() ){
			idx = i;
			area = contours[i].size();
		}
	}

	Rect rec = boundingRect(contours[idx]);

	Mat outImage = thresholdImage(rec);

	resize(outImage,outImage, Size(this->numRows, this->numCols));

	return outImage;

}

/*
 * Function that train KNN recognizer, with data stored in images folder
 * every number has it's own image wich is preprocessed before storing 
 * it's value
 */
bool DigitRecognizer::train()
{
	this->numImages = 10;
	this->numRows = 20;
	this->numCols = 20;

	Mat img;
	const int classes = 10;
	int size = numRows*numCols;
	char pathToImages_sudoku[] = "images/sudoku";
	char file[255];

	Mat trainingVectors(numImages, size, CV_32F);
	Mat trainingClasses(numImages, 1, CV_32F);

	// Loop through all images, preproces them and train KNN instance
	for ( int j = 0; j < 3; j++) /*bilo j < 1*/
		for(int i = 1; i < classes; i++){
			sprintf(file, "%s%d%d.png", pathToImages_sudoku,j, i);
			img = imread(file, 0);
			if (!img.data){
				std::cout << "File " << file << " not found\n";
				exit(1);
			}
			Mat outfile;
			outfile = this->preprocessImage(img);
#ifdef TEST
			imshow("Image for train",outfile);
			waitKey(0);
#endif
			for (int n = 0; n < size; n++){

				trainingVectors.data[(i-1) * size + n] = outfile.data[n];
			}
			trainingClasses.data[i-1] = i;
			if ( j == 2 && i == 9) /*bilo j == 1 && i == 1*/
				break;
		}

	InputArray trainArray(trainingVectors);
	InputArray classArray(trainingClasses);
	Ptr<TrainData> trainingData = TrainData::create(trainArray, SampleTypes::ROW_SAMPLE,
			classArray);
	this->knn->setIsClassifier(0);
	this->knn->setAlgorithmType(KNearest::BRUTE_FORCE);
	this->knn->train(trainingData);

	return true;
}

/*
 * Function wich determines what number is input image, it returns 0 if it dosen't exists
 */
int DigitRecognizer::classify(cv::Mat image){
	Size sz = image.size();
	// if image is big, we resize it
	resize(image,image,Size(36,36));
	Mat sample2(1, 20*20, CV_32FC1);
	Mat sample2Out(1, 1, CV_32FC1);
	Mat grayImage  = image.clone();

	std::vector < std::vector<Point> > contours;

#ifdef TEST
	namedWindow("Prepared image for recognition",WINDOW_AUTOSIZE);
	imshow("Prepared image for recognition",grayImage);
	waitKey(0);
	destroyWindow("Prepared image for recognition");
#endif
	findContours(grayImage, contours, RETR_LIST, CHAIN_APPROX_NONE);
	// find bigest blob and then find nearest neighbour
	for (size_t i = 0; i < contours.size(); i++){
		std::vector < Point > cnt = contours[i];
		if (contourArea(cnt) >= 1){
			Rect rec = boundingRect(cnt);
			if (rec.height > 13 && rec.width > 5){
				Mat roi = image(rec);
#ifdef TEST
				imshow("Recognize::roi",roi);
				waitKey(0);
				destroyWindow("Recognize::roi");
#endif
				sz = roi.size();
				Mat stagedImage;
				resize(roi,stagedImage,Size(20,20));
#ifdef TEST
				imshow("Recognize::resized",stagedImage);
				waitKey(0);
				destroyWindow("resized");
#endif
				for (int n = 0; n < stagedImage.rows*stagedImage.cols; n++){
					sample2.data[n] = stagedImage.data[n];
				}

				Mat res;
				//knn->findNearest(Mat_<float>(sample2), 5, res);
				//std::cout << (int)res.at<float>(0) << std::endl;
				return knn->findNearest(Mat_<float>(sample2), 10, res); //1;
				// InputArray sampleArray(sample2);
				// OutputArray out(sample2Out);
				// // ToDo: need to read out this out array, not the return value
				// return this->knn->findNearest(sampleArray, 10, out);
			}

		}

	}
	return 0;
}