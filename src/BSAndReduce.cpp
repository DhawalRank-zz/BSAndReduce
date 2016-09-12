//opencv includes
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
//C++ includes
#include <iostream>
#include <sstream>
using namespace cv;
using namespace std;
// Global variables
Mat frame; //current frame
Mat fgMaskMOG2; //fg mask fg mask generated by MOG2 method
Ptr<BackgroundSubtractor> pMOG2; //MOG2 Background subtractor
int keyboard; //input from keyboard
void help();
void processVideo(char* videoFilename);
Mat processColors(Mat& frame);
void help()
{
    cout
    << "--------------------------------------------------------------------------" << endl
    << "This program shows how to use background subtraction methods and reduce it to 16 colors using Kmeans Clustering."  << endl
    << "You can process videos (-vid) for the backgroung subtraction."             << endl
                                                                                    << endl
    << "Usage:"                                                                     << endl
    << "./BSAndReduce <video filename>"                     << endl
    << "for example: ./BCAndReduce video.avi"                                       << endl
    << endl;
}
int main(int argc, char* argv[])
{
    //print help information
    help();
    //check for the input parameter correctness
    if(argc != 2) {
        cerr <<"Incorret input list" << endl;
        help();
        cerr <<"exiting..." << endl;
        return EXIT_FAILURE;
    }
    //create GUI windows
    namedWindow("Frame");
    namedWindow("Final Output");
    //create Background Subtractor objects
    pMOG2 = createBackgroundSubtractorMOG2(); //MOG2 approach
    if(argv[1]) {
        //video is valid
        processVideo(argv[1]);
    }
    else {
        //error in reading input parameters
        cerr <<"Please, check the input parameters." << endl;
        help();
        cerr <<"Exiting..." << endl;
        return EXIT_FAILURE;
    }
    //destroy GUI windows
    destroyAllWindows();
    return EXIT_SUCCESS;
}
void processVideo(char* videoFilename) {
    //create the capture object
    VideoCapture capture(1);
    if(!capture.isOpened()){
        //error in opening the video input
        cerr << "Unable to open video file: " << videoFilename << endl;
        exit(EXIT_FAILURE);
    }
    //read input data. ESC or 'q' for quitting
    while( (char)keyboard != 'q' && (char)keyboard != 27 ){
        //read the current frame
        if(!capture.read(frame)) {
            cerr << "Unable to read next frame." << endl;
            cerr << "Exiting..." << endl;
            exit(EXIT_FAILURE);
        }
        //update the background model
        pMOG2->apply(frame, fgMaskMOG2);
        //reduce the colors
        Mat reducedFrame = processColors(fgMaskMOG2);
        imshow("Frame", frame);
        imshow("Final Output", reducedFrame);
        //get the input from the keyboard
        keyboard = waitKey( 30 );
    }
    //delete capture object
    capture.release();
}
Mat processColors(Mat& fgMaskMOG2)
{
	  Mat colVec = fgMaskMOG2.reshape(1, fgMaskMOG2.rows*fgMaskMOG2.cols); // change to a Nx3 column vector as class kmeans works with Nx3 vector
	  Mat colVecD, bestLabels, centers, clustered;
	  int attempts = 5;
	  int K = 16;				//No. of clusters or No. of colors in output
	  double eps = 0.001;
	  colVec.convertTo(colVecD, CV_32FC3, 1.0/255.0); // convert to floating point (required format for kmeans)
	  kmeans(colVecD, K, bestLabels, TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER,
			  attempts, eps), attempts, KMEANS_PP_CENTERS, centers);		//use kmeans to generate clusters. value of K determines number of clusters (in our case no. of colors in final output
	  int colors[K];
	  for(int i=0; i<K; i++) {
	  colors[i] = 255/(i+1);						//generate K colors in order to predict final colors on reduced image
	  }
	  clustered = Mat(fgMaskMOG2.rows, fgMaskMOG2.cols, CV_8U);	//initialize final image
	  for(int i=0; i<fgMaskMOG2.cols*fgMaskMOG2.rows; i++) {
		  clustered.at<uchar>(i/fgMaskMOG2.cols, i%fgMaskMOG2.cols) = (uchar)(colors[bestLabels.at<int>(0,i)]);			//assign estimated color to each cluster point generating quantized final image
	  }
	  return clustered;
}
