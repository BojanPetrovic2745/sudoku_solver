#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/highgui/highgui_c.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
//#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <iostream>
#include <string>
/* Basic includes */
#include <stdio.h>
#include <cmath>
#include <fstream>
#include <sstream>

/* STL containers */
#include <set>
#include <vector>
#include <unordered_set>
#include <string>
#include <map>

#include "Recognizer.h"
#include "Functions.h"

using namespace cv;
using namespace std;

/* Mode of painting, to illustrate the process
 *  0 - Just the image
 *  1 - Preprocessed image
 *  2 - Biggest blob
 *  3 - Plot the corners
 *  4 - Ideal Sudoku box
 *  5 - Recognized numbers
 *  6 - Solved Sudoku
 */
int mode = 0;
//#define SAVE 0
#define TEST_MAIN 1

void showImage(std::string name, cv::Mat img);
cv::Mat findBiggestBlob(cv::Mat & matImage);

/* Funkcija za prepoznavanje brojeva pomocu Tesseract-a */
/*unsigned int recognize_digit(Mat& im,tesseract::TessBaseAPI& tess)
{
    tess.SetImage((uchar*)im.data, im.size().width, im.size().height, im.channels(), (int)im.step1());
    tess.Recognize(0);
    const char* out = tess.GetUTF8Text();
    if (out)
        if(out[0]=='1' or out[0]=='I' or out[0]=='i' or out[0]=='/' or out[0]=='|' or out[0]=='l' or out[0]=='t')
            return 1;
        else if(out[0]=='2')
            return 2;
        else if(out[0]=='3')
            return 3;
        else if(out[0]=='4')
            return 4;
        else if(out[0]=='5' or out[0]=='S' or out[0]=='s')
            return 5;
        else if(out[0]=='6')
            return 6;
        else if(out[0]=='7')
            return 7;
        else if(out[0]=='8')
            return 8;
        else if(out[0]=='9')
            return 9;
        else
            return 0;
    else
        return 0;
}
*/
int main(){

    /* Class to capture the webcam feed */
    VideoCapture capture(0);

    /* Create the window */
    string window_name = "Sudoku Solver";
    namedWindow(window_name, CV_WINDOW_KEEPRATIO);

	Mat sudoku;

    /* Start Tesseract OCR, we will use it to recognize digits */
    /*tesseract::TessBaseAPI tess;
    if (tess.Init("/opt/local/share/tessdata/", "eng")) 
    {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }*/

	while(1)
    {
#ifdef TEST
		cout << "\nEnter sudoku image name!" << endl << endl;
		cin >> name;

        /* Read input image */
		Mat sudoku_orig = imread(name, 0);
		Mat sudoku = imread(name, 0);
#endif
		
        capture.read(sudoku);

		if (sudoku.empty())
        {
			return -1;
        }
#ifdef SAVE
		imwrite("../../Izvestaj/Input_image.png",sudoku_orig);
#endif
		Size sz = sudoku.size();

        if (mode==0)
        {
            /* Show the result */
            imshow(window_name, sudoku);
        }
		
#ifdef TEST
		showImage("Input_image",sudoku);
#endif
        else
        {       
			/* To gray input photo */
        	cvtColor(sudoku, sudoku, CV_BGR2GRAY);

			Mat sudokuBox = sudoku.clone();
			if (sudokuBox.empty()) 
        	{
            	return -1;
        	}
		    /* To blur, thershold and invert input image 
            *  This will be used later, for extracting Sudoku grid */
		    GaussianBlur(sudoku, sudoku, Size(7,7), 0);
		    adaptiveThreshold(sudoku, sudoku, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 5, 2);
		    bitwise_not(sudoku, sudoku);
#ifdef TEST
		showImage("Prepared_input_image",sudoku);
#endif

#ifdef SAVE
		imwrite("../../Izvestaj/Prepared_input_image.png",sudoku);
#endif

		    /* Sudoku box bluring */
		    GaussianBlur(sudokuBox, sudokuBox, Size(11,11), 0);
#ifdef TEST
		showImage("Blured_input_image",sudokuBox);
#endif

#ifdef SAVE
		imwrite("../../Izvestaj/Blured_input_image.png",sudokuBox);
#endif

		    /* Sudoku box thershold */ 
		    adaptiveThreshold(sudokuBox, sudokuBox, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 5, 2);
#ifdef TEST
		showImage("Tresholded_input_image",sudokuBox);
#endif

#ifdef SAVE
		imwrite("../../Izvestaj/Tresholded_input_image.png",sudokuBox);
#endif
		    /* Sudoku box invert */
		    bitwise_not(sudokuBox, sudokuBox);
#ifdef TEST
		showImage("Inverted_input_image",sudokuBox);
#endif

#ifdef SAVE
		imwrite("../../Izvestaj/Inverted_input_image.png",sudokuBox);
#endif
		    /* Dilatation with + shape kernel */
		    Mat kernel = (Mat_<uchar>(3,3) << 0,1,0,1,1,1,0,1,0);
		    dilate(sudokuBox, sudokuBox, kernel);

            if (mode==1)
            {
                /* Show the result */
                imshow(window_name, sudokuBox);
            }
#ifdef TEST
		showImage("Dilatated_input_image",sudokuBox);
#endif

#ifdef SAVE
		imwrite("../../Izvestaj/Dilatated_input_image.png",sudokuBox);
#endif
            else
            {
                /* Biggest blob detection */
		        Mat sudokuBoxFin = findBiggestBlob(sudokuBox);
#ifdef TEST
		showImage("Bigest_blob_of_input_image", sudokuBoxFin );
#endif

#ifdef SAVE
		imwrite("../../Izvestaj/Bigest_blob_of_input_image.png",sudokuBoxFin);
#endif	
		        /* Erode, dilatate and thresholde for better extraction of Bigest blob */
		        erode(sudokuBoxFin, sudokuBox, kernel);
		        dilate(sudokuBox, sudokuBox, kernel);
		        adaptiveThreshold(sudokuBox, sudokuBoxFin, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 5, 2);

                if (mode==2)
                {
                    /* Show the result */
                    imshow(window_name, sudokuBox);
                }
#ifdef TEST
		        showImage("Biggest_blob_only", sudokuBoxFin );
#endif

#ifdef SAVE
		imwrite("../../Izvestaj/Biggest_blob_only.png",sudokuBoxFin);
#endif	
                else 
                {
		            /* Callibration */
		            Mat dst, dst_norm, dst_norm_scaled;
		            dst = Mat::zeros(sudokuBox.size(), CV_32FC1);

		            /* Harris corner detector parameters */
		            int blockSize = 5; /*bilo 3*/
		            int apertureSize = 5; /*bilo 3*/
		            double k = 0.05; /*bilo 0.08*/
		            int thresh = 200;
		            Point tl(sz.width/2,sz.height/2),tr(sz.width/2,sz.height/2),bl(sz.width/2,sz.height/2),br(sz.width/2,sz.height/2);

		            /* Detecting corners */
					//goodFeaturesToTrack(sudokuBox, dst, 4, 0.05, 15);
		            cornerHarris(sudokuBox, dst, blockSize, apertureSize, k, BORDER_DEFAULT);

		            /* Normalizing */
		            normalize(dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
#ifdef TEST
		showImage("Normalized corners",dst_norm);
#endif

#ifdef SAVE
		imwrite("../../Izvestaj/Normalized_corners.png",dst_norm);
#endif
                    /* Scaling */
		            convertScaleAbs(dst_norm, dst_norm_scaled);
#ifdef TEST
		showImage("Scaled normalized corners",dst_norm_scaled);
#endif

#ifdef SAVE
		imwrite("../../Izvestaj/Scaled_normalized_corners.png",dst_norm_scaled);
#endif
		            /* Drawing a circle around every corner */
		            for(int j = 0; j < dst_norm.rows ; j++)
                    {
		            	for(int i = 0; i < dst_norm.cols; i++)
		            	{
		            		if((int) dst_norm.at<float>(j,i) > thresh)
		            		{
		            			if((i < tl.x && j < sz.height/4) || (i < sz.width/4 && j < tl.y))
                                {
		            				tl.x = i;
		            				tl.y = j;
		            			}
		            			if((i > tr.x && j < sz.height/4) || (i > 3*sz.width/4 && j < tr.y))
                                {
		            				tr.x = i;
		            				tr.y = j;
		            			}
		            			if((i < bl.x && j > 3*sz.height/4) || (i < sz.width/4 && j > bl.y))
                                {
		            				bl.x = i;
		            				bl.y = j;
		            			}
		            			if((i > br.x && j > 3*sz.height/4) || (i > 3*sz.width/4 && j > br.y))
                                {
		            				br.x = i;
		            				br.y = j;
		            			}
		            			circle(dst_norm_scaled, Point( i, j ), 5,  Scalar(0), 2, 8, 0);
		            		}
		            	}
                    }
#ifdef TEST
        showImage("All corrners",dst_norm_scaled);
#endif

#ifdef SAVE
		imwrite("../../Izvestaj/All_corrners.png",dst_norm_scaled);
#endif
		            /* Drawing a circle around four corners */
		            circle(sudokuBoxFin, tl, 5,  Scalar(0), 2, 8, 0);
		            circle(sudokuBoxFin, tr, 5,  Scalar(0), 2, 8, 0);
		            circle(sudokuBoxFin, bl, 5,  Scalar(0), 2, 8, 0);
		            circle(sudokuBoxFin, br, 5,  Scalar(0), 2, 8, 0);
#ifdef TEST
        showImage("Outer corners detected", sudokuBoxFin);
#endif

#ifdef SAVE
		imwrite("../../Izvestaj/Outer_corrners.png",sudokuBoxFin);
#endif	
                    if (mode==3)
                    {
                        /* Plot the corners */
                        imshow(window_name, sudokuBoxFin);
                    }
                    else
                    {
                        /* Parametars for image extraction */
		                /* width, height */
		                int width = max(abs(tr.x-tl.x),abs(br.x-bl.x));
		                int height = max(abs(tr.y-br.y),abs(tl.y-bl.y));
		                /* line width */
		                int line_width = 2;
		                /* block width and height */
		                int blck_width = floor(width/3.);
		                int blck_height = floor(height/3.);
		                /* cell width and height */
		                int cell_width = floor(blck_width/3.);
		                int cell_height = floor(blck_height/3.);
		                /* wanted points */
		                Point tl_fin(0,0),tr_fin(width,0),bl_fin(0,height),br_fin(width,height);
		                Point2f pts1[4], pts2[4];
		                /* pts1 - current points in image */
		                pts1[0] = tl; pts1[1] = tr; pts1[2] = bl; pts1[3] = br;
		                /* pts2 - wanted points in image */
		                pts2[0] = tl_fin; pts2[1] = tr_fin; pts2[2] = bl_fin; pts2[3] = br_fin;

		                /* Perspective transform matrix */
		                Mat M = getPerspectiveTransform(pts1,pts2);


		                /* Extraction of grid */
		                warpPerspective(sudokuBoxFin, sudokuBoxFin,M,Size(width,height));
#ifdef TEST
        showImage("Extracted grid",sudokuBoxFin);
#endif

#ifdef SAVE
		imwrite("../../Izvestaj/Extracted_grid.png",sudokuBoxFin);
#endif
		                /* Extraction of grid with numbers */
                        Mat sudoku_ext;
                        warpPerspective(sudoku,sudoku_ext,M,Size(width,height));
#ifdef TEST
		showImage("Extracted numbers grid",sudoku_ext);
#endif

#ifdef SAVE
		imwrite("../../Izvestaj/Extracted_sudoku.png",sudoku_ext);
#endif
		                /* Drawing ideal grid */
		                Mat idealBox  = sudoku_ext.clone();
		                for(int i = 1; i<9; i++)
                        {
		                	int k = floor(((double)i)/3);
		                	line(idealBox, Point(i*cell_width+k*line_width, 0), Point(i*cell_width+k*line_width, height),CV_RGB(0,0,255));
		                	line(idealBox, Point(0, i*cell_height+k*line_width), Point(width, i*cell_height+k*line_width),CV_RGB(0,0,255));
		                }

						if (mode==4)
                    	{
                        	/* Show only Sudoku grid with cells on it */
                        	imshow(window_name, idealBox);
                    	}
#ifdef TEST
        showImage("Ideal box",idealBox);
#endif

#ifdef SAVE 
		imwrite("../../Izvestaj/Ideal_box.png",idealBox);
#endif
						else
						{
							// Digit recognizer train
		                	DigitRecognizer *dr = new DigitRecognizer();

		                	if(!dr->train())
                        	{
		                		exit(1);
                        	}
#ifdef TEST_MAIN
		cout << "Digit recognizer trained!" << endl;
#endif
		                	/* Loop for recognizing numbers */
		                	int rec[9][9];
		                	for (int i = 0; i < 9; i++)
                        	{	
		                		int p = floor(((double)i)/3);
		                		for(int j = 0; j < 9; j++)
                            	{
		                			int k = floor(((double)j)/3);
		                			int ri1 = i*cell_width+p*line_width;
		                			int ri2 = (i+1)*cell_width + p*line_width;
		                			int ci1 = j*cell_height+ k*line_width;
		                			int ci2 = (j+1)*cell_height + k*line_width;
		                			/* End casses */
		                			if(ci2 > height)	
                                	{
                                	    ci2 = height;
                                	}
		                			if(ri2 > width)
		                			{
                                		ri2 = width;
                                	}
		                			Range row(ri1,ri2), col(ci1 ,ci2);
		                			Mat cell = sudoku_ext(col,row);
#ifdef TEST_MAIN
		showImage("Extracted cell",cell);
#endif
				                	Scalar meanVal(mean(cell(Range(cell_height/4,3*cell_height/4),Range(cell_width/4, 3*cell_width/4))));
				                	if(meanVal[0] > 10)
                                	{
				                		int num =  dr->classify(cell);
#ifdef TEST_MAIN
		cout << "Num is " << num << endl;
#endif
				                		rec[j][i] = num;
				                	}
				                	else
                                	{
#ifdef TEST_MAIN
		cout << "Blank space!" <<  endl;
#endif
			            		    	rec[j][i] = 0;
		                			}
		                		}
		                	}	

							int rec_old[9][9];
							if(mode==5)
							{
								/* Print of recognized matrix */								
		                		for (int i = 0; i < 9; i++)
                        		{
									int p = floor(((double)i)/3);
		            	    		for( int j = 0; j < 9; j++)
                	            	{
										int k = floor(((double)j)/3);
										int ri1 = i*cell_width + p*line_width;
										int ci1 = j*cell_height + k*line_width;

										rec_old[i][j] = rec[i][j];
										/* Print Sudoku to console */
		            	    			cout << rec[i][j] << " ";
		        	        			if (j == 8)
		    	            			{
        	                        	    cout << endl;
    	                            	}

										if(rec[i][j] != 0)
										{
											Point text_pos(ri1+ri1/10, ci1+ci1/10);
											stringstream ss;
											ss << (int)rec[i][j];
											putText(idealBox, ss.str(), text_pos, CV_FONT_HERSHEY_DUPLEX, 1,
													Scalar(0,0,255), 1, 8);
										}
		                			}
                        		}
							}
							else
							{
		            	    	/* Solve sudoku */
		        	        	bool solved = SudokuSolver().solve(rec);
		    	            	if(solved)
        	                	{
			                		cout << "\nCongrats, your sudoku has been solved!" << endl;
			                		/* Print solved Sudoku */
		                			for (int i = 0; i < 9; i++)
                            		{
										int p = floor(((double)i)/3);
		            	    			for( int j = 0; j < 9; j++)
                	                	{
											int k = floor(((double)j)/3);
											int ri1 = i*cell_width + p*line_width;
											int ci1 = j*cell_height + k*line_width;
											/* Print solved Sudoku to console */
		                					cout << rec[i][j] << " ";
		                					if (j == 8)
		                					{
                            	        	    cout << endl;
                        	            	}

											/* Plot the numbers in the gaps using AR */
											if (rec_old[i][j] == 0)
											{
												Point text_pos(ri1+ri1/10, ci1+ci1/10);
												stringstream ss;
												ss << (int)rec[i][j];
												putText(idealBox, ss.str(), text_pos, CV_FONT_HERSHEY_DUPLEX, 1,
														Scalar(0,0,255), 1, 8);	
											}
		            	    			}
                	            	}
		    	            	}
			                	else
			                	{
	                            	cout << "\nThere is no solution to your sudoku!" << endl;
                        		}

		                		cout << "\nPress y/Y for continue! " << endl;
		                		char c;
		                		cin >> c;
		                		if ( c != 'y' && c != 'Y')
                        		{
                            		break;
                        		}
							}
						}
                    } 
                }
            }
	    }
    
    	/* Wait for a key */
    	char key = (char)waitKey(5);
    	switch (key)
    	{	
        	case '0':
            	mode = 0;
            	break;
        	case '1':
           		mode = 1;
            	break;
        	case '2':
            	mode = 2;
            	break;
        	case '3':
            	mode = 3;
            	break;
        	case '4':
        	    mode = 4;
        	    break;
        	case '5':
        	    mode = 5;
        	    break;
        	case '6':
        	    mode = 6;
        	    break;
        	case 27: //escape key
            	return 0;
        	default:
            	break;
    	}
	
	}


	return 0;
}

/*
 * Imshow with waitKey and destroying window
 */
void showImage(std::string name, Mat img){
	namedWindow(name,WINDOW_AUTOSIZE);
	imshow(name,img);

	waitKey(0);
	destroyWindow(name);
}

/*
 * Find biggest connected component (blob)
 */
cv::Mat findBiggestBlob(cv::Mat & matImage){
    int largest_area=0;
    int largest_contour_index=0;

    Size sz = matImage.size();

    Mat matZeros = Mat::zeros(sz, CV_8UC1);

    vector< vector<Point> > contours; // Vector for storing contour
    vector<Vec4i> hierarchy;

    findContours( matImage, contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE ); // Find the contours in the image

    for( unsigned i = 0; i < contours.size(); i++ ) {// iterate through each contour. 
        double a=contourArea( contours[i],false);  //  Find the area of contour
        if(a>largest_area){
            largest_area=a;
            largest_contour_index=i;                //Store the index of largest contour
            //bounding_rect=boundingRect(contours[i]); // Find the bounding rectangle for biggest contour
        }
    }

    drawContours( matZeros, contours, largest_contour_index, Scalar(255), CV_FILLED, 8, hierarchy ); // Draw the largest contour using previously stored index.
    return matZeros;
}