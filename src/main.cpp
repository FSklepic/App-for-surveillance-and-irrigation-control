#include <opencv2/videoio.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp> //Includes IntelliSense
#include <iostream>


int main() {

    std::string file_path = "Resource_Files/Test_data/Test_data1.mp4";
    
    cv::VideoCapture cap(file_path);
    cv::Mat frame;

    if (!cap.isOpened()) {
        std::cerr << "ERROR, No captcure file detected\n";
        return -1;
    }

    while(true) {
        cap.read(frame);

        if (frame.empty()) {
            std::cout << "There are no more frames to display.\n";
            break;
        }

        cv::imshow("Frame", frame);
        if (cv::waitKey(5) >= 0) break;
    }

    return 0;
}