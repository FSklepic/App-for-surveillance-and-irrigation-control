#include "../header/functions.h"
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <iostream>



int main() {
    // relative path from project root (workspace folder)
    std::string file_path = "resources/Test_data3.mp4";

        
    cv::VideoCapture cap(file_path);
    if (!cap.isOpened()) {
        std::cerr << "ERROR: cannot open file: " << file_path << "\n";
        return -1;
    }

    /* Using smart pointer allows for automatic deletion of data upon overwritte (i.e. when last known data goes out of scope)
    which prevents data leaking and allows shared managment using reference counter*/ 
    /*cv::BackgroundSubratctor is an OpenCv abstract class that allows use of different methods such as apply.
    In this case backround is a smart pointer to a BackgroundSubratctor object that automaticlly handels memory*/
    /*cv::createBackgroundSubtractorMOG2 (int history = 500, double varThreshold = 16, bool	detectShadows = true )
    history -> 500 - 1000 (how fast does model learn backround)
    varThreshold -> 16 - 25 (how different a pixel must be to count as foreground)
    detectShadows -> Shadow pixels are marked as 127 in mask (foreground is 255). Decreases a speed a bit*/
    cv::Ptr<cv::BackgroundSubtractorMOG2> background; 
    background = cv::createBackgroundSubtractorMOG2(500, 20, true);	// Interchangable values

    cv::Mat frame, foreground_mask, background_image, kernel;

    cv::namedWindow("Frame", cv::WINDOW_NORMAL);
    cv::namedWindow("Background", cv::WINDOW_NORMAL);
    cv::resizeWindow("Frame", 640, 480);
    cv::resizeWindow("Background", 640, 480);

    while (true) {
        if (!cap.read(frame)) {
            std::cout << "No more frames or cannot read frame\n";
            break;
        }
        if (frame.empty()) break;

        background->apply(frame, foreground_mask); // works continuously, retains history, possible to add learning rate
        
        //background->getBackgroundImage(background_image);
        
        //Morphology (clean motion mask)
        cv::threshold(foreground_mask, foreground_mask, 0, 255, cv::THRESH_BINARY); //Transform shadows to white pixels (x,y > 0 = 255; else 0)        
        cv::medianBlur(foreground_mask, foreground_mask, 3); //Removes noise
        
        kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)); // kernel size 5 - 11; Rect - general purpuse; Ellipse smoother shapes; cross - directional filtering; Size - interchangable
        cv::morphologyEx(foreground_mask, foreground_mask, cv::MORPH_OPEN, kernel, cv::Point(-1, -1), 1); //Erosion (shrinks white regions) -> dilation (expands white regions) => Removes small white dots and thin connections
        cv::morphologyEx(foreground_mask, foreground_mask, cv::MORPH_CLOSE, kernel, cv::Point(-1,-1), 1); //Dilation -> erosion => fills small holes or gaps in objects

        //Get contours
        get_Contours(foreground_mask, frame);


        cv::imshow("Frame", frame);
        cv::imshow("Background", foreground_mask);
        //cv::imshow("Backgrounds", background_image);

        if (cv::waitKey(10) >= 0) break;
    }

    cap.release();
    background.release();
    return 0;
}
