#include "../header/functions.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <fstream>
#include <cstdlib>

void get_Contours(cv::Mat foreground_mask, cv::Mat frame) {
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(foreground_mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<std::vector<cv::Point>> conPoly(contours.size());
    std::vector<cv::Rect> boundRect(contours.size());

    for (int i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);

        // Koristimo veći prag (npr. 1500) da AI ne provjerava svaku sitnu bubu ili sjenu
        if (area >= 1500.0) {
            double peri = cv::arcLength(contours[i], true);
            cv::approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
            boundRect[i] = cv::boundingRect(conPoly[i]);

            // --- AI PROVJERA ---
            // 1. Izreži detektirani pokret iz originalnog okvira
            cv::Mat crop = frame(boundRect[i]);
            
            // 2. Spremi ga privremeno (u build/Release/)
            cv::imwrite("temp_crop.jpg", crop);

            // 3. Pozovi Python (blokirajući poziv, ispis spremi u res.txt)
            // Napomena: "python" mora biti u PATH-u, inače stavi punu putanju do python.exe
            system("python yolo_check.py temp_crop.jpg > res.txt");

            // 4. Pročitaj što je Python ispisao
            std::ifstream resFile("res.txt");
            std::string label;
            resFile >> label;

            // 5. Boja i tekst ovisno o rezultatu
            cv::Scalar color;
            std::string display_text;

            if (label == "PERSON") {
                color = cv::Scalar(0, 0, 255); // Crveno za čovjeka
                display_text = "ALARM: COVJEK";
            } else {
                color = cv::Scalar(0, 255, 0); // Zeleno za ostalo (koze)
                display_text = "POKRET";
            }

            // Crtanje pravokutnika i teksta
            cv::rectangle(frame, boundRect[i].tl(), boundRect[i].br(), color, 4);
            cv::putText(frame, display_text, cv::Point(boundRect[i].x, boundRect[i].y - 10), 
                        cv::FONT_HERSHEY_SIMPLEX, 0.7, color, 2);
        }
    }
}
