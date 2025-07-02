#include "detection.h"
#include "utils.h"
#include <iostream>

using namespace std;
using namespace cv;

bool processDamage(const vector<Detection>& detections) {
    for (const auto& det : detections) {
        if (getClassName(det.class_id) == "damage") {
            //cout << "[DAMAGE DETECTED] À§Ä¡: " << det.box << " ½Å·Úµµ: " << det.confidence << endl;
            return det.confidence > 0.8 ?  true :  false;
        }
    }
    return false;
}

bool processBOX(const vector<Detection>& detections) {
    for (const auto& det : detections) {
        if (getClassName(det.class_id) == "BOX") {
            //cout << "[QR CODE DETECTED] À§Ä¡: " << det.box << " ½Å·Úµµ: " << det.confidence << endl;
            return det.confidence > 0.8 ? true : false;
        }
    }
    return false;
}

void drawDetections(Mat& frame, const vector<Detection>& detections) {
    for (const auto& detection : detections)
    {
        Rect box = detection.box;
        Scalar color = Scalar(0, 255, 0);

        rectangle(frame, box, color, 2);
        string classString = getClassName(detection.class_id) + ' ' + to_string(detection.confidence).substr(0, 4);
        putText(frame, classString, Point(box.x, box.y - 10), FONT_HERSHEY_DUPLEX, 1, Scalar(255, 255, 255), 2);
    }
}
