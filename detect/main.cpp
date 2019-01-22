#include <iostream>
#include <vector>
#include <sstream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

std::vector<Vec3f> detect(const Mat& frame_in, Mat& frame_out)
{
    cvtColor(frame_in, frame_out, COLOR_BGR2GRAY);
    GaussianBlur(frame_out, frame_out, cv::Size(5, 5), 0, 0);

    std::vector<Vec3f> objects;
    HoughCircles(frame_out, objects, HOUGH_GRADIENT, 2, frame_out.rows, 100, 100, 0, 100);
    return objects;
}

void paint(Mat& frame, const std::vector<Vec3f>& objects)
{
    Point object_center;
    const Scalar color(255, 255, 255);
    for(const auto& object : objects) {
        object_center = Point(cvRound(object[0]), cvRound(object[1]));
        Point object_size (cvRound(object[2]), cvRound(object[2]));
//        circle(frame, object_center, cvRound(object[2]), Scalar(255, 255, 255), 2);
        rectangle(frame, object_center - object_size, object_center + object_size, color);
        line(frame, Point(320, 240), object_center, color);  // отрезок до центра окружности
    }

    line(frame, Point(300, 240), Point(340, 240), color);
    line(frame, Point(320, 220), Point(320, 260), color);

    int degree = (object_center.x - 320) / 8.2;  //  расположение предмета в градусах
    std::ostringstream out;
    out << degree << " deg";
    putText(frame, out.str(), Point(300, 50), FONT_HERSHEY_DUPLEX, 1, color);
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
