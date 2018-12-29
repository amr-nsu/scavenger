#include <iostream>
#include <vector>
#include <sstream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

std::vector<Vec3f> detect(const Mat& frame_in, Mat& frame_out)
{
    cvtColor(frame_in, frame_out, COLOR_BGR2GRAY);
    GaussianBlur(frame_out, frame_out, cv::Size(5, 5), 0.5, 0.5);

    std::vector<Vec3f> objects;
    HoughCircles(frame_out, objects, HOUGH_GRADIENT, 2, frame_out.rows / 5, 110, 165, 0, 100);
    return objects;
}

void paint(Mat& frame, const std::vector<Vec3f>& objects)
{
    Point r;
    for(const auto& object : objects) {
        r = Point(cvRound(object[0]), cvRound(object[1]));
        circle(frame, r, cvRound(object[2]), Scalar(255, 255, 255), 2);
    }

    line(frame, Point(300, 240), Point(340, 240), Scalar(255, 255, 255));
    line(frame, Point(320, 220), Point(320, 260), Scalar(255, 255, 255));
    line(frame, Point(320, 240), r, Scalar(255, 255, 255)); // отрезок до центра окружности

    int degree = (r.x - 320) / 8.2; //  расположение предмета в градусах
//    int w = frame.size[0];
    std::ostringstream out;
    out << degree << " dg";
    putText(frame, out.str(), Point(300, 50), FONT_HERSHEY_DUPLEX, 1, Scalar(255, 255, 255));
}

int main()
{
    constexpr int camera = 0;

    VideoCapture cap;
    if (!cap.open(camera)) {
        std::cerr << "Video device " << camera << " is not open" << std::endl;
        return -1;
    }

    while(true) {
        Mat frame;
        cap >> frame;
        if (frame.empty()) {
            break;
        }

        Mat frame_out;

        std::vector<Vec3f> objects = detect(frame, frame_out);
        paint(frame_out, objects);

        imshow("camera", frame);
        imshow("out", frame_out);

        if (waitKey(33) == /* ESC */ 27) {
            break;
        }
    }
}
