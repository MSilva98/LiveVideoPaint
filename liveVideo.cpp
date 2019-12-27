/*
 *	Painting on Live Video from Webcam
 *
 *	Visual Computation - MIECT - UA - 2019/2020
 *
*/

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;


void clearScreen(Mat frame, Mat draw){
	frame.setTo(Scalar(0,0,0));	// black frame
	draw.setTo(Scalar(255,255,255));	// white panel
}

int main(int argc, char** argv){
	VideoCapture cap;

	// open default camera with 0
	if(!cap.open(0)){
		cout << "Could not initialize capturing!!";
		return -1;
	}

	// cout << cap.get(3);	width
	// cout << cap.get(4);	height

	Mat drawPanel = Mat::zeros(cap.get(4), cap.get(3), CV_8UC1);
	drawPanel = Scalar(255,255,255);	// white panel with video capture resolution

	Mat frame;

	while(true){
		cap >> frame;
		
		if(frame.empty()) break;	// end of video frame
		imshow("Live Video Paint", frame);
		imshow("Draw Panel", drawPanel);
		int c = waitKey(10);
		if(c == 27){
			break;	// stop capturing bye pressing ESC
		}

	}

	return 0;
}



