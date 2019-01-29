#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

struct Object
{
    enum class Type
    {
        NO_OBJECT, PLASTIC_CUP, PAPER_BOX
    };

    Type    type = Type::NO_OBJECT;
    Point   p1;
    Point   p2;
    int     angleDeg;
};

void sendToBoard(int angle)
{
    std::fstream board("/dev/ttyACM0");
    board << 'g' << static_cast<int8_t>(angle) << std::flush;
    std::cout << "request: g" << angle << std::endl;
    char response;
    board >> response;
    std::cout << "response: " << response << std::endl;
}

double angle( Point pt1, Point pt2, Point pt0 )
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

void detectCup(const Mat& frame, Object& object)
{
    if (object.type != Object::Type::NO_OBJECT) {
        return;
    }

    std::vector<Vec3f> objects;
    HoughCircles(frame, objects, HOUGH_GRADIENT, 2, frame.rows, 100, 100, 0, 100);
    if (!objects.empty()) {
        Vec3f firstObject = objects.front();
        Point objectCenter = Point(cvRound(firstObject[0]), cvRound(firstObject[1]));
        Point objectSize (cvRound(firstObject[2]), cvRound(firstObject[2]));

        object.type = Object::Type::PLASTIC_CUP;
        object.p1 = objectCenter - objectSize;
        object.p2 = objectCenter + objectSize;
    }
}

void detectBox(const Mat& frame, Object& object)
{
    if (object.type != Object::Type::NO_OBJECT) {
        return;
    }

    Mat cannyOutput;
    const int thresh = 40;

    Canny(frame, cannyOutput, thresh, thresh * 2, 3);
    dilate(cannyOutput, cannyOutput, Mat(), Point(-1, -1));

//    imshow("canny", cannyOutput);

    std::vector<std::vector<Point> > contours;
    findContours(cannyOutput, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

    std::vector<Point> approx;
    for(const auto & contour : contours)
    {
        approxPolyDP(contour, approx, arcLength(contour, true)*0.02, true);
        if( approx.size() == 4 &&
                fabs(contourArea(approx)) > 1000 &&
                isContourConvex(approx)) {
            double maxCosine = 0;
            for( int j = 2; j < 5; j++ )
            {
                double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                maxCosine = std::max(maxCosine, cosine);
            }
            if(maxCosine < 0.3 ) {
                Point p1 = approx[0], p2 = approx[0];
                for (const auto& p: approx) {
                    if (p.x < p1.x) {
                        p1.x = p.x;
                    }
                    if (p.y < p1.y) {
                        p1.y = p.y;
                    }
                    if (p.x > p2.x) {
                        p2.x = p.x;
                    }
                    if (p.y > p2.y) {
                        p2.y = p.y;
                    }
                }
                object.type = Object::Type::PAPER_BOX;
                object.p1 = p1;
                object.p2 = p2;
                return;
            }
        }
    }
}

void calculateAngle(Object& object)
{
    if (object.type != Object::Type::NO_OBJECT) {
        object.angleDeg = ((object.p1.x + object.p2.x) / 2 - 320) / 8.2;
    }
}

void paintHUD(Mat& frame, const Object& object)
{
    const Scalar color(255, 255, 255);

    line(frame, Point(300, 240), Point(340, 240), color);
    line(frame, Point(320, 220), Point(320, 260), color);

    if (object.type != Object::Type::NO_OBJECT) {
        rectangle(frame, object.p1, object.p2, color, 2);
        std::ostringstream out;
        out << object.angleDeg << " DEG";
        if (object.type == Object::Type::PLASTIC_CUP) {
            out << " PLASTIC CAP";
        } else if (object.type == Object::Type::PAPER_BOX) {
            out << " PAPER BOX";
        }
        putText(frame, out.str(), Point(150, 50), FONT_HERSHEY_DUPLEX, 1, color);
    }
}

int main()
{
    constexpr int camera = 0;

    VideoCapture cap;
    cap.set(CV_CAP_PROP_BUFFERSIZE, 3);
    if (!cap.open(camera)) {
        std::cerr << "Video device " << camera << " is not open" << std::endl;
        return -1;
    }

    while(true) {
        Mat frame, frameGray;
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "No frames" << std::endl;
            return -2;
        }

        cvtColor(frame, frameGray, COLOR_BGR2GRAY);
        GaussianBlur(frameGray, frameGray, cv::Size(5, 5), 0, 0);

        Object object;

        detectBox(frameGray, object);
        detectCup(frameGray, object);
        calculateAngle(object);
        paintHUD(frameGray, object);

        imshow("detect", frameGray);

        if (waitKey(33) == /* ESC */ 27) {
            break;
        }

        if (object.type != Object::Type::NO_OBJECT) {
            if (waitKey(100) == /* ESC */ 27) {
                break;
            }
            sendToBoard(object.angleDeg);
            for(int i=0; i<3; i++) {
               cap >> frame;
            }
        }
    }
}
