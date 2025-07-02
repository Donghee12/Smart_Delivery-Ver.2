#pragma once
#ifndef DETECTION_H
#define DETECTION_H

#include <opencv2/opencv.hpp>
#include "inference.h"
#include "utils.h"  

void drawDetections(cv::Mat& frame, const std::vector<Detection>& detections);
bool processDamage(const std::vector<Detection>& detections);
bool processBOX(const std::vector<Detection>& detections);

#endif // DETECTION_H
