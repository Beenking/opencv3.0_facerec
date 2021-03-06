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
#include <list>
#include <numeric>

#include "opencv2/core.hpp"
#include "opencv2/face.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace cv;
using namespace cv::face;
using namespace std;

const int LIST_BUFFER = 30;

static void lockComputer()
{
	system("\"rundll32.exe user32.dll,LockWorkStation\"");
}

static void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator = ';') {
	std::ifstream file(filename.c_str(), ifstream::in);
	if (!file) {
		string error_message = "No valid input file was given, please check the given filename.";
		CV_Error(Error::StsBadArg, error_message);
	}
	string line, path, classlabel;
	while (getline(file, line)) {
		stringstream liness(line);
		getline(liness, path, separator);
		getline(liness, classlabel);
		if(!path.empty() && !classlabel.empty()) {
			images.push_back(imread(path, 0));
			labels.push_back(atoi(classlabel.c_str()));
		}
	}
}

int main(int argc, const char *argv[]) {

	// Check for valid command line arguments, print usage
	// if no arguments were given.
	if (argc < 5) {
		cout << "\t\t Please test and learn. Donnt use for commerial purpose."<< endl;
		cout << "\t Any question about the exe. feel free email me bestheart@qq.com." << endl;
		cout << endl;
		cout << "usage: " << argv[0] << " </path/to/haar_cascade> </path/to/csv.txt> <admin face id> <device id> </path/to/face_model.xml>" << endl;
		cout << "\t </path/to/haar_cascade> -- Path to the Haar Cascade for face detection." << endl;
		cout << "\t </path/to/csv.txt> -- Path to the CSV file with the face database." << endl;
		cout << "\t <face id> -- The face id to identify who you are." << endl;
		cout << "\t <device id> -- The webcam device id to grab frames from." << endl;
		cout << "\t </path/to/face_mdoel.xml> -- This is an optional arg. Traing when no chosed. Otherwise just load it." << endl << endl;
		cout << "\t for exampe: face_recognition.exe data/haarcascade_frontalface_default.xml data/csv.txt 110 0 (data/face_model.xml)" << endl << endl;
		exit(1);
	}

	// Get the path to your CSV:
	string fn_haar = string(argv[1]);
	string fn_csv = string(argv[2]);
	int admin_face_id = atoi(argv[3]);
	int deviceId = atoi(argv[4]);

	// These vectors hold the images and corresponding labels:
	vector<Mat> images;
	vector<int> labels;

	// Get the height from the first image. We'll need this
	// later in code to reshape the images to their original
	// size AND we need to reshape incoming faces to this size:
	int im_width = 92;
	int im_height = 112;

	// Create a FaceRecognizer and train it on the given images:
	Ptr<FaceRecognizer> model = createFisherFaceRecognizer();
	// Load a exsiting model file. Ohterwise, traing an saving it.
	if(argc==5){
		cout << " Training now...Just wait a moment..." << endl;
		// Read in the data (fails if no valid input filename is given, but you'll get an error message):
		try {
			read_csv(fn_csv, images, labels);
		} catch (cv::Exception& e) {
			cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << endl;
			// nothing more we can do
			exit(1);
		}
		model->train(images, labels);
		cout << " Training completed" << endl;
		model->save("face_model.xml");
		cout << " Face model has been saved in face_model.xml" << endl;
	}else{
		model->load(argv[5]);
	}

	// That's it for learning the Face Recognition model. You now
	// need to create the classifier for the task of Face Detection.
	// We are going to use the haar cascade you have specified in the
	// command line arguments:
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
	Mat frame;
	list<int> list_face_id;
	// recognize every frame:
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
		// recognize every face:
		int prediction = -1; // prediction id
		for(int i = 0; i < faces.size(); i++) {
			// Process face by face:
			Rect face_i = faces[i];
			// Crop the face from the image. So simple with OpenCV C++:
			Mat face = gray(face_i);
			// Resizing the face is necessary for Eigenfaces and Fisherfaces. You can easily
			// verify this, by reading through the face recognition tutorial coming with OpenCV.
			// Resizing IS NOT NEEDED for Local Binary Patterns Histograms, so preparing the
			// input data really depends on the algorithm used.
			//
			// I strongly encourage you to play around with the algorithms. See which work best
			// in your scenario, LBPH should always be a contender for robust face recognition.
			//
			// Since I am showing the Fisherfaces algorithm here, I also show how to resize the
			// face you have just found:
			Mat face_resized;
			cv::resize(face, face_resized, Size(im_width, im_height), 1.0, 1.0, INTER_CUBIC);
			// Now perform the prediction, see how easy that is:
			//int prediction = model->predict(face_resized);
			double confidence;
			model->predict(face_resized, prediction, confidence);
			// push result of confidence to list:
			list_face_id.push_back(prediction);
			cout << confidence <<endl;
			// And finally write all we've found out to the original image!
			// First of all draw a green rectangle around the detected face:
			rectangle(original, face_i, Scalar(0, 255,0), 1);
			// Create the text we will annotate the box with:
			string box_text = format("Prediction = %d", prediction);
			// Calculate the position for annotated text (make sure we don't
			// put illegal values in there):
			int pos_x = std::max(face_i.tl().x - 10, 0);
			int pos_y = std::max(face_i.tl().y - 10, 0);
			// And now put it into the image:
			putText(original, box_text, Point(pos_x, pos_y), FONT_HERSHEY_PLAIN, 1.0, Scalar(0,255,0), 2.0);
		}
		if(prediction==-1){
			list_face_id.push_back(prediction);
		}
		// keep 10 face id in list.
		while (list_face_id.size() > LIST_BUFFER){
			list_face_id.pop_front();
			cout << list_face_id.size() << endl;
		}
		// dicide whether lock computer or not
		auto iter = list_face_id.begin();
		if( list_face_id.size()==LIST_BUFFER ){
			int fit_num = 0;
			while(iter!=list_face_id.end()){
				if ( *iter == admin_face_id )
					fit_num++;
				iter++;
			}
			if( fit_num < 5 ){
				lockComputer();
			}
		}

		// Show the result:
		imshow("face_recognizer", original);
		// And display it:
		char key = (char) waitKey(1);
		// Exit this loop on escape:
		if(key == 27)
			break;
	}
	return 0;
}