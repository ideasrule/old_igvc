#include "ipc.h"
#include "Common.h"

#include "cv.h"
#include "highgui.h"

int main(int argc, char *argv[])
{
    cv::Mat im = cv::imread("file.png");
    cv::imshow("hello", im);
    cv::waitKey();

    return 0;
}