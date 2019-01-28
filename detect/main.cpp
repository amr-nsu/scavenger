#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <optional>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

enum class Type {
    PLASTIC_CAP, GLASS_BOTTLE, PAPER_BOX
};

void send_to_board(int angle)
{
    std::fstream board("/dev/ttyACM0");
    board << 'g' << static_cast<int8_t>(angle) << std::flush;
    std::cout << "request: g" << angle << std::endl;
    char response;
    board >> response;
    std::cout << "response: " << response << std::endl;
}

std::vector<Vec3f> detect(const Mat& frame_in, Mat& frame_out)
{
    cvtColor(frame_in, frame_out, COLOR_BGR2GRAY);
    GaussianBlur(frame_out, frame_out, cv::Size(5, 5), 0, 0);

    std::vector<Vec3f> objects;
    HoughCircles(frame_out, objects, HOUGH_GRADIENT, 2, frame_out.rows, 100, 100, 0, 100);
    return objects;
}

std::optional<int> paint(Mat& frame, const std::vector<Vec3f>& objects, const Type type)
{
    Point object_center;
    const Scalar color(255, 255, 255);

    line(frame, Point(300, 240), Point(340, 240), color);
    line(frame, Point(320, 220), Point(320, 260), color);

    for(const auto& object : objects) {
        object_center = Point(cvRound(object[0]), cvRound(object[1]));
        Point object_size (cvRound(object[2]), cvRound(object[2]));
        rectangle(frame, object_center - object_size, object_center + object_size, color, 2);
        line(frame, Point(320, 240), object_center, color);  // отрезок до центра

        int degree = (object_center.x - 320) / 8.2;  //  расположение предмета в градусах
        std::ostringstream out;
        out << degree << " DEG";
        if (type == Type::PLASTIC_CAP) {
            out << " PLASTIC CAP";
        }
        putText(frame, out.str(), Point(150, 50), FONT_HERSHEY_DUPLEX, 1, color);
        return degree;
    }
    return std::nullopt;
}

int main()
{
    constexpr int camera = 0;

    VideoCapture cap;
    if (!cap.open(camera)) {
        std::cerr << "Video device " << camera << " is not open" << std::endl;
        return -1;
    }

    namedWindow("detect");

    while(true) {
        Mat frame;
        cap >> frame;
        if (frame.empty()) {
            break;
        }

        Mat frame_out;

        std::vector<Vec3f> objects = detect(frame, frame_out);
        auto result = paint(frame_out, objects, Type::PLASTIC_CAP);

//        imshow("camera", frame);
        imshow("detect", frame_out);

 	if (result) {
            if (waitKey(500) == /* ESC */ 27) {
                break;
            }
            send_to_board(*result);
	    for(int i=0; i<50; i++) {
 	       cap >> frame;
	    }
	}

        if (waitKey(33) == /* ESC */ 27) {
            break;
        }
    }
}
