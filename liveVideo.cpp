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
#include <string>

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
	int x = -1, y = -1;

	if(meanX != 0){
		x = meanX/location.size();
	}

	if(meanY != 0){
		y = meanY/location.size();
	}

	return Point(x,y);
}

Scalar changeColor(Point pos, Scalar lineCol){
	if(pos.x >= 550){
		if(pos.y <= 155){
			lineCol = Scalar(0,0,255);	// red color
		}
		else if(pos.y >= 170 && pos.y <= 310){
			lineCol = Scalar(0,255,0); // green color
		}
		else if(pos.y >= 340 && pos.y <= 475){
			lineCol = Scalar(255,0,0);	// blue color
		}
	}

	return lineCol;
}

Mat drawL(int posX, int posY, int lastX, int lastY, Scalar lineCol, int thickness, Mat drawPanel){
	if(posX != -1 && posY != -1 && lastX != -1 && lastY != -1){
		if(posY > 0 && posY < 480){
			if(posX > 115 && posX < 540){
				line(drawPanel, Point(posX,posY), Point(lastX,lastY), lineCol, thickness);
			}
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
	Mat frame;

	// Open default camera with 0
	if(!cap.open(0)){
		cout << "Could not initialize capturing!!";
		return -1;
	}
	
	int width = cap.get(3);
	int height = cap.get(4);

	Point pos(width/2,height/2);

	// White panel
	Mat drawPanel(height, width, CV_8UC3, Scalar(255,255,255,0));	
	
	// Mat to concatenate frame and drawPanel
	Mat output(height, width*2, CV_8UC3, Scalar::all(0));

	Mat layout = imread("paint.png", -1);
	Mat mask; 
	tie(mask, layout) = getMask(layout);
	
	int close = 90; 	// at 30 FPS = 3 secs
	int clear = 90;		// at 30 FPS = 3 secs
	int thickness = 30;	// line thickness

	// Default color line -> blue
	Scalar lineCol;	
	lineCol  = Scalar(255,0,0);

	while(true){
		cap >> frame;
		
		// end of video frame
		if(frame.empty()) break;	

		// Flip image to draw according to movements
		flip(frame,frame,1);	

		// Decrease background noise
		medianBlur(frame, frame, 5);	

		// Keep object last position to draw line
		int lastX = pos.x;
		int lastY = pos.y;

		// Get mean coordinates of yellow object
		pos = objectCoords(frame, Scalar(20,100,100), Scalar(30,255,255));
		
		// Close windows or Clear DrawPanel
		if(pos.x > 0 && pos.x < 110){	
			if(pos.y > 0 && pos.y < 115){	// close windows
				close--;
				string c = "Closing window in: " + to_string(close/30+1);
				putText(frame, c, Point((width/2)-100, 30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0,0,0), 2);
				if(close == 0){				
					break;					
					close = 90;
				}
			}
			else if(pos.y > 135 && pos.y < 205){	// erase screen
				clear--;
				string c = "Erasing screen in: " + to_string(clear/30+1);
				putText(frame, c, Point((width/2)-100, 30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0,0,0), 2);
				if(clear == 0){	
					drawPanel.setTo(Scalar(255,255,255));	// white panel
					clear = 90;
				}
			}
		}
		else{
			close = 90;
			clear = 90;
		}

		// Line thickness
		if(pos.x > 20 && pos.x < 90){
			if(pos.y > 325 && pos.y < 390){
				thickness++;
			}
			else if(pos.y > 435 && pos.y < 460){
				thickness--;
				if(thickness <= 15){
					thickness = 15;
				}
			}
		}

		// line thickness
		putText(frame, to_string(thickness/15), Point(45,425), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,0,0), 2);

		lineCol	= changeColor(pos, lineCol);
		drawPanel = drawL(pos.x, pos.y, lastX, lastY, lineCol, thickness/15, drawPanel);

		layout.copyTo(frame, mask);

		frame.copyTo(output(Rect(0,0,width,height)));
		drawPanel.copyTo(output(Rect(width,0,width,height)));

		imshow("Live Video Paint", output);

		int c = waitKey(10);
		if(c == 27){
			break;	// stop capturing by pressing ESC
		}
		else if((char)c == 'R' || (char)c == 'r'){	// reset
			frame.setTo(Scalar(0,0,0));	// black frame	
			drawPanel.setTo(Scalar(255,255,255));	// white panel
		}

	}

	return 0;
}



