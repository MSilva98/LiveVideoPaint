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
#include <tuple>

using namespace cv;
using namespace std;

// find and return mean coordinates of yellow object
Point objectCoords(Mat frame, Scalar lowerLimit, Scalar upperLimit){
	Mat frame_hsv;
	cvtColor(frame, frame_hsv, CV_BGR2HSV);
	Mat output;
	inRange(frame_hsv, lowerLimit, upperLimit, output);

	vector<Point> location;
	findNonZero(output, location);

	int meanX = 0, meanY = 0;
	for (int i = 0; i < location.size(); ++i)
	{
		meanX += location[i].x;
		meanY += location[i].y;	
	}
	int x = frame.size().width/2, y = frame.size().height/2;

	if(meanX != 0){
		x = meanX/location.size();
	}

	if(meanY != 0){
		y = meanY/location.size();
	}

	imshow("Threshold", output);

	return Point(x,y);
}

// mouse test
// Point pt(-1,-1);

// void mouse_callback(int  event, int  x, int  y, int  flag, void *param)
// {
//     if (event == EVENT_LBUTTONDOWN)
//     {
//         // Store point coordinates
//         pt.x = x;
//         pt.y = y;
//     }
// }

Mat drawL(int posX, int posY, int lastX, int lastY, Scalar lineCol, int thickness, Mat drawPanel){
	if(posX > 0 && posX < 110){	
		if(posY > 0 && posY < 115){	// close window
			printf("Close\n");
		}
		else if(posY > 135 && posY < 205){	// erase screen
			// frame.setTo(Scalar(0,0,0));	// black frame	
			// drawPanel.setTo(Scalar(255,255,255));	// white panel
			cout << "Screen cleared" << endl;
		}
	}
	else if(posX > 550){
		if(posY < 155){
			lineCol = Scalar(0,0,255);	// red color
			cout << "Red" << endl;
		}
		else if(posY > 170 && posY < 310){
			lineCol = Scalar(0,255,0); // green color
			cout << "Green" << endl;
		}
		else if(posY > 340 && posY < 475){
			lineCol = Scalar(255,0,0);	// blue color
			cout << "Blue" << endl;
		}
	}

	if(posY > 0 && posY < 480){
		if((posY > 220 && posX > 0 && posX < 540) || (posX > 120 && posX < 540)){
			line(drawPanel, Point(posX,posY), Point(lastX,lastY), lineCol, thickness);
		}
	}

	return drawPanel;
}

tuple<Mat, Mat> getMask(Mat matrix){
	vector<Mat> rgbLayer;
	split(matrix,rgbLayer); // seperate channels
    Mat cs[3] = { rgbLayer[0],rgbLayer[1],rgbLayer[2] };
    merge(cs,3,matrix);  	// glue together again
    Mat mask = rgbLayer[3];		// alpha channel used as mask
    return make_tuple(mask, matrix);
}

int main(int argc, char** argv){
	VideoCapture cap;
	Mat frame, scribbles;

	Scalar lineCol;	// default color line -> blue
	lineCol  = Scalar(255,0,0);

	// open default camera with 0
	if(!cap.open(0)){
		cout << "Could not initialize capturing!!";
		return -1;
	}

	// cout << cap.get(3);	width
	// cout << cap.get(4);	height
	
	Point pos(cap.get(3)/2,cap.get(4)/2);

	// white panel
	Mat drawPanel(cap.get(4), cap.get(3), CV_8UC4, Scalar(255,255,255,0));	

	Mat layout = imread("paint.png", -1);
	Mat mask; 
	tie(mask, layout) = getMask(layout);
	
    // namedWindow("Live Video Paint", 1);
    // setMouseCallback("Live Video Paint", mouse_callback);

	while(true){
		cap >> frame;
		
		if(frame.empty()) break;	// end of video frame

		medianBlur(frame, frame, 5);	// decrease background noise
		layout.copyTo(frame, mask);

		int lastX = pos.x;
		int lastY = pos.y;

		// get mean coordinates of yellow object
		pos = objectCoords(frame, Scalar(20,100,100), Scalar(30,255,255));
		
		drawPanel = drawL(pos.x, pos.y, lastX, lastY, lineCol, 2, drawPanel);

		imshow("Live Video Paint", frame);
		imshow("Draw Panel", drawPanel);

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



