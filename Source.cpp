#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/tracking.hpp>

#include <iostream>
#include <chrono>

int main(int argc, char** argv)
{
	// variables
	cv::VideoCapture cap(0);
	if (cap.isOpened()==false) {
		std::cout << "Can't open camera in 0" << std::endl;
		return 0;
	}
	cv::Mat frame;
	cv::Rect2d roi;
	cv::Ptr<cv::Tracker> tracker = cv::TrackerKCF::create();
	int key;
	bool init = false;
	std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::high_resolution_clock::now();
	std::chrono::time_point<std::chrono::steady_clock> end;
	double fps;

	// loop
	while (true) {
		// read frame
		cap >> frame;
		if (frame.empty()) {
			std::cout << "Frame is empty" << std::endl;
			continue;
		}

		// process the frame if init is true
		if (init) {			
			if (tracker->update(frame, roi)) {
				// if update is succesful draw the rectangle
				cv::rectangle(frame, roi, cv::Scalar(0, 255, 0),2);
			}
		}

		// show result
		end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed_second = end - start;
		start = end;
		fps = pow(elapsed_second.count(), -1);
		std::cout << fps << std::endl;
		cv::putText(frame, "duration: "+std::to_string(elapsed_second.count()), cv::Point(15, 30), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 255));
		cv::putText(frame, "fps: "+std::to_string(fps), cv::Point(15, 60), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 255));
		cv::imshow("tracker", frame);

		// keyboard controls
		key = cv::waitKey(10);
		// quit
		if (key == 27) break;
		// select roi and start tracking
		else if (key == (int)'s') {
			// select roi
			roi = cv::selectROI("tracker", frame);
			// reset the tracker
			if (init) {
				tracker.release();
				tracker = cv::TrackerKCF::create();
			}
			// init tracker to start tracking
			tracker->init(frame, roi);
			init = true;
		}
	}
	// release camera resource
	// otherwise camera will stay on
	cap.release();

	return 0;
}