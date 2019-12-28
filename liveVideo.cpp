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

// detect object given lower color limit and upper color limit
Mat detectObject(Mat frame, Scalar lowerLimit, Scalar upperLimit){
	Mat frame_hsv;
	cvtColor(frame, frame_hsv, CV_BGR2HSV);
	Mat output;
	inRange(frame_hsv, lowerLimit, upperLimit, output);
	return output;
}

int main(int argc, char** argv){
	VideoCapture cap;
	Mat frame, scribbles;
	int posX = 0, posY = 0;

	// open default camera with 0
	if(!cap.open(0)){
		cout << "Could not initialize capturing!!";
		return -1;
	}

	// cout << cap.get(3);	width
	// cout << cap.get(4);	height

	// white panel
	Mat drawPanel(cap.get(4), cap.get(3), CV_8UC3, Scalar(255,255,255));
	
	Mat layout = imread("paint.png", -1);

	vector<Mat> rgbLayer;
	split(layout,rgbLayer); // seperate channels
    Mat cs[3] = { rgbLayer[0],rgbLayer[1],rgbLayer[2] };
    merge(cs,3,layout);  	// glue together again
    Mat mask = rgbLayer[3];		// png's alpha channel used as mask

	while(true){
		cap >> frame;
		
		if(frame.empty()) break;	// end of video frame
		
		layout.copyTo(frame, mask);
		imshow("Live Video Paint", frame);
		// imshow("Draw Panel", drawPanel);

		// Img threshold -> tracked color white, rest -> black
		Mat object = detectObject(frame, Scalar(20,100,100), Scalar(30,255,255));
		imshow("Threshold", object);
		vector<Point> location;
		findNonZero(object, location);
		int meanX = 0, meanY = 0;
		for (int i = 0; i < location.size(); ++i)
		{
			meanX += location[i].x;
			meanY += location[i].y;	
		}

		int lastX = posX;
		int lastY = posY;

		if(meanX != 0){
			posX = meanX/location.size();
			posY = meanY/location.size();
		}
		
		printf("x:%d, y:%d\n",posX,posY );

		int c = waitKey(10);
		if(c == 27){
			break;	// stop capturing bye pressing ESC
		}
		else if((char)c == 'R' || (char)c == 'r'){	// reset
			frame.setTo(Scalar(0,0,0));	// black frame	
			drawPanel.setTo(Scalar(255,255,255));	// white panel
		}

	}

	return 0;
}



