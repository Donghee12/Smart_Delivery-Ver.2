#include "Qrcodegen.hpp"
#include "scanQRCode.h"
#include "database.h"

using qrcodegen::QrCode;
using namespace cv;
using namespace std;

int scanQRCode(Mat& frame, sql::Connection* conn)
{
    // 콘솔창에서 로그 생략 (INFO레벨의 로그 생략, WARNING과 ERROR만 표시됨)
    utils::logging::setLogLevel(utils::logging::LOG_LEVEL_WARNING);

    static QRCodeDetector detector;  // QR 코드 탐지기 객체 생성 (static으로 선언하여 상태 유지)
    static String lastQRCode = "";  // 마지막으로 인식된 QR 코드 정보 저장 (static으로 선언하여 상태 유지)

    // QR 코드 감지 (항상 화면에 표시)
    vector<Point> points;
    String info = detector.detectAndDecode(frame, points);  // qr코드 탐지 및 디코딩

    if (!info.empty() && info != lastQRCode)  // QR 코드가 발견되고, 마지막 QR 코드와 다를 경우
    {
        polylines(frame, points, true, Scalar(0, 0, 255), 7);  // qr코드의 외곽선 출력
        cout << "QR 코드 검출 성공: " << info << endl;

        int qr_order_num = stoi(info);
        if (db_checkOrderExists(conn, qr_order_num))  // DB에서 주문번호 확인
        {
            lastQRCode = info;  // 새로운 QR 코드가 인식되었으므로 `lastQRCode`를 업데이트
            return qr_order_num;  // QR 코드가 정상적으로 인식된 경우 주문 번호 반환
        }
        else
        {
            cout << "주문 번호 " << qr_order_num << " 이 DB에 없습니다." << endl;
        }
    }

    return 0;  // QR 코드가 발견되지 않거나 인식된 QR 코드가 이전 QR 코드와 동일한 경우
}
