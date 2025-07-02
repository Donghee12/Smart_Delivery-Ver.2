#include "Qrcodegen.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include "database.h"
#include "scanQRCode.h"
#include "detection.h"
#include <string.h>
#include "conn.h"

using qrcodegen::QrCode;
using namespace cv;
using namespace std;
extern OrderDetails orderData;


int main()
{
    system("color f0");
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);
    char ar_input_data;

    int a = 1;
    int flag = 0;  // QR 코드 인식 상태 (0: 인식 안 됨, >0: 인식 성공)
    bool detect_damage = false;
    bool detect_box = false;

    Inference inf("best.onnx", Size(640, 640), "model.txt", true);
    loadClassNames("model.txt");

    Mat img_frame;
    VideoCapture cap(0);
    cap.set(CAP_PROP_EXPOSURE, -6);
    cap.set(CAP_PROP_FOCUS, 255);
    Connection* conn = db_connect_to(db_config);
    conn->setSchema("test_db");
    int frame_counter = 0;

    flag = 0;  // 매번 프레임 시작 시 flag 초기화
    detect_damage = false;
    detect_box = false;
    while (1)
    {

        cap.read(img_frame);
        if (img_frame.empty())
            return -1;

        /*YOLO 객체 인식 코드 */

        vector<Detection> detections = inf.runInference(img_frame);

        drawDetections(img_frame, detections);  // YOLO 객체 검출 표시
        
   
        detect_damage = processDamage(detections);
        detect_box = processBOX(detections);


        if (detect_box)
        {
            dataToSend = "BOX";
            flag = scanQRCode(img_frame, conn);  // QR 코드 인식 시도

            if (detect_damage)
            {
                dataToSend = "dmg";
            }
        }
        else {
            continue;
        }
        
        dataToSend = "";
        
        if (flag > 0)  // QR 코드 인식 성공 시
        {
            printOrderData(conn, flag);

            if(!detect_damage)
            { 
                dataToSend = orderData.addr[0];
            }
            // 여기서 DB에서 해당 제품 OK로 변경 
            cout << "QR 코드 인식 성공: 주문 번호 " << flag << endl;
            flag = 0;
        }

        else if (flag < 0)  // QR 코드 인식 실패 (유효하지 않음)
        {
            cout << "QR 코드 인식 실패 (오류)" << endl;
            dataToSend = "dmg";
            break;  // 실패 후 탈출 (다시 시도하지 않음)
        }


        imshow("VideoFeed", img_frame);
        if (waitKey(1) == 27)  // ESC 키를 눌러 종료
            break;

    }

    cap.release();

    return 0;
}