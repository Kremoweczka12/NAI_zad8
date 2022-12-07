#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>


bool compareContourAreas ( std::vector<cv::Point> contour1, std::vector<cv::Point> contour2 ) {
    double i = fabs( contourArea(cv::Mat(contour1)) );
    double j = fabs( contourArea(cv::Mat(contour2)) );
    return ( i < j );
}

int main( int argc, char** argv ) {
    bool capturing = true;
    cv::VideoCapture cap;
    // Question for you
//     C cap( "szukaj_zielonego.webm" );
    int deviceID = 0;             // 0 = open default camera
    int apiID = cv::CAP_ANY;      // 0 = autodetect default API
    // open selected camera using selected API
    cap.open(deviceID, apiID);
    if ( !cap.isOpened() ) {
        std::cerr << "error opening frames source" << std::endl;
        return -1;
    }
    std::cout << "Video size: " << cap.get( cv::CAP_PROP_FRAME_WIDTH )
    << "x" << cap.get( cv::CAP_PROP_FRAME_HEIGHT ) << std::endl;
    namedWindow("detected", cv::WINDOW_AUTOSIZE);
    //gfg 0 ,141, 87 - 15, 255, 255
    std::vector<int> lower = {0,141,87};
    std::vector<int> upper = {15,255,255};
    cv::createTrackbar("lh", "detected", &lower[0], 255);
    cv::createTrackbar("ls", "detected", &lower[1], 255);
    cv::createTrackbar("lv", "detected", &lower[2], 255);
    cv::createTrackbar("hh", "detected", &upper[0], 255);
    cv::createTrackbar("hs", "detected", &upper[1], 255);
    cv::createTrackbar("hv", "detected", &upper[2], 255);
    do {
        cv::Mat frame;
        if ( cap.read( frame ) ) {
            cv::flip(frame,frame,1);

            cv::Mat gray_image;
            GaussianBlur(frame, frame, cv::Size(3, 3), 0);
            cvtColor( frame, gray_image, cv::COLOR_BGR2HSV );
            cv::Mat imgThresholded;

//gfg 0 ,141, 87 - 15, 255, 255
            inRange(gray_image, cv::Scalar(lower[0], lower[1], lower[2]), cv::Scalar(upper[0], upper[1], upper[2]), imgThresholded);
            auto kernel = getStructuringElement(cv::MORPH_ELLIPSE,cv::Size{5,5});

            cv::Mat eroded;
            dilate(imgThresholded, eroded, kernel);
//            erode(eroded, eroded, kernel);

            cv::Mat canny_output;
            Canny( eroded, canny_output, 10, 255 );

            std::vector<std::vector<cv::Point> > contours;
            std::vector<cv::Vec4i> hierarchy;
            findContours(canny_output, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );

            std::sort(contours.begin(), contours.end(), compareContourAreas);
            std::vector<cv::Point> BiggestContour = contours[contours.size() - 1];
            std::vector<cv::Point> SecondBiggestContour = contours[contours.size() - 2];
            auto m = moments(BiggestContour, false);
            cv::Point p1 = {(int)(m.m10 / m.m00), (int)(m.m01 / m.m00)};
            auto m2 = moments(SecondBiggestContour, false);
            cv::Point p2 = {(int)(m2.m10 / m2.m00), (int)(m2.m01 / m2.m00)};

            line(frame, p1, p2, cv::Scalar(255, 0, 0),
                 10, cv::LINE_8);
            cv::imshow("zakresy", canny_output);
            cv::imshow("flipped",frame);



//            gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
//            ret,gray = cv2.threshold(gray,127,255,0)
//            gray2 = gray.copy()
//
//            contours, hier = cv2.findContours(gray,cv2.RETR_LIST,cv2.CHAIN_APPROX_SIMPLE)
//            for cnt in contours:
//            if 200<cv2.contourArea(cnt)<5000:
//            (x,y,w,h) = cv2.boundingRect(cnt)
//            cv2.rectangle(gray2,(x,y),(x+w,y+h),0,-1)

//            findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
//            cv::imshow( "Not-yet smart windown", frame );
        } else {
            // stream finished
            capturing = false;
        }
        //czekaj na klawisz, sprawdz czy to jest 'esc'
        if( (cv::waitKey( 1000.0/60.0 )&0x0ff) == 27 ) capturing = false;
    } while( capturing );
    return 0;
}