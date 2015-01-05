/*
* Copyright (c) 2011. Philipp Wagner <bytefish[at]gmx[dot]de>.
* Released to public domain under terms of the BSD Simplified license.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the
*     documentation and/or other materials provided with the distribution.
*   * Neither the name of the organization nor the names of its contributors
*     may be used to endorse or promote products derived from this software
*     without specific prior written permission.
*
*   See <http://www.opensource.org/licenses/bsd-license>
*/

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect.hpp"

#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

int main(int argc, const char *argv[]) {
	// Check for valid command line arguments, print usage
	// if no arguments were given.
	if (argc != 5) {
		cout << "usage: " << argv[0] << " </path/to/haar_cascade> </path/to/face_filder> </path/to/device id>" << endl;
		cout << "\t </path/to/haar_cascade> -- Path to the Haar Cascade for face detection." << endl;
		cout << "\t </path/to/folder> -- Path to the face folder that you want to retain the faces picture." << endl;
		cout << "\t <device id> -- The webcam device id to grab frames from." << endl;
		exit(1);
	}
	// Get the path:
	string fn_haar = string(argv[1]);
	string face_lib = string(argv[2]);
	int face_id = atoi(argv[3]);
	int deviceId = atoi(argv[4]);

	// ofstream
	ofstream output("../data/face_cap.txt");

	CascadeClassifier haar_cascade;
	haar_cascade.load(fn_haar);
	// Get a handle to the Video device:
	VideoCapture cap(deviceId);
	// Check if we can use this device at all:
	if(!cap.isOpened()) {
		cerr << "Capture Device ID " << deviceId << "cannot be opened." << endl;
		return -1;
	}
	// Holds the current frame from the Video device:
	int num = 0;  // the captured face numbers
	Mat frame;
	for(;;) {
		cap >> frame;
		// Clone the current frame:
		Mat original = frame.clone();
		// Convert the current frame to grayscale:
		Mat gray;
		cvtColor(original, gray, COLOR_BGR2GRAY);

		// Find the faces in the frame:
		vector< Rect_<int> > faces;
		haar_cascade.detectMultiScale(gray, faces, 1.1, 8, 0, Size(70,70));
		// At this point you have the position of the faces in
		// faces. Now we'll get the faces, make a prediction and
		// annotate it in the video. Cool or what?
		for(int i = 0; i < faces.size(); i++) {
			// Process face by face:
			Rect face_i = faces[i];
			// Crop the face from the image. So simple with OpenCV C++:
			Mat face = gray(face_i);

			Mat face_resized;
			cv::resize(face, face_resized, Size(92, 112), 1.0, 1.0, INTER_CUBIC);

			// Store the faces in your computer
			char c[4];
			itoa(num,c,10);
			string s = face_lib + (string)c + ".jpg";
			imwrite(s, face_resized);
			cout << "Captruing the #" << num << "face" << endl;
			output << s << ";" << face_id << endl;
			num++;

			// First of all draw a green rectangle around the detected face:
			rectangle(original, face_i, Scalar(0, 255,0), 1);
		}
		// Show the result:
		imshow("face_recognizer", original);
		// And display it:
		char key = (char) 
			waitKey(300);
		// Exit this loop on escape:
		if(key == 27)
			break;
	}
	return 0;
}