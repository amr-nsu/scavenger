#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace std;

int main()
{
    cv::namedWindow("Camera", cv::WINDOW_AUTOSIZE);
    cv::VideoCapture cap;
    cap.open(0);
    cv::Mat frame;
    for(;;)
    {
        cap >> frame;
        if (frame.empty()) break;
        cv::imshow("Camera", frame);
        if (cv::waitKey(33)>=0) break;
    }
    return 0;
}
