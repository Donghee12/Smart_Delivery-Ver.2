#pragma once
#define WIN32_LEAN_AND_MEAN  
#define NOMINMAX  
#include <windows.h> 
#include <opencv2/opencv.hpp>
#include "database.h"
using namespace cv;

//int scanQRCode(Mat& frame);
int scanQRCode(Mat& frame, sql::Connection* conn);