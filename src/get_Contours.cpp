#include "../header/functions.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

void get_Contours(cv::Mat foreground_mask, cv::Mat frame) {

    std::vector<std::vector<cv::Point>> contours; //Outer vector - all detected contours; Inner vector - points belonging to contour
    //std::vector<cv::Vec4i> hierarchy; //Vector/array containing 4 intigers/relationships of contours [next, previous, first_child, parent]

    cv::findContours(foreground_mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE ); //RETR_EXTERNAL vs RETR_TREE tryout; CHAIN_APPROX_SIMPLE vs CHAIN_APPROX_NONE tryout
    //cv::drawContours(frame, contours, -1, cv::Scalar(255, 0, 255), 2);
    std::vector<std::vector<cv::Point>> conPoly(contours.size());
    std::vector<cv::Rect> boundRect(contours.size()); 

    for (int i = 0; i < contours.size(); i++) {

        double area = cv::contourArea(contours[i]);
        //std::cout << area << std::endl;
        
        //Bounding box
        if (area >= 100.0) {
            /* Provjera postoji li razlika između detektiranih kontura čovjeka i koze
            Lenght of contour peremiter or curve lenght*/ 
            double peri = cv::arcLength(contours[i], true);
            cv::approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
            //std::cout << conPoly[i].size() << std::endl;
            boundRect[i] = cv::boundingRect(conPoly[i]); // Find all edge points of contours
            cv::rectangle(frame, boundRect[i].tl(), boundRect[i].br(), cv::Scalar(0, 255,0), 4);
            
            //cv::drawContours(frame, contours, -1, cv::Scalar(255, 0, 255), 2);
        }
    }
}
