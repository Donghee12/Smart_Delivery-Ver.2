#include <Servo.h>
#define espSerial Serial1  // Use Serial1 on MEGA (TX1=18, RX1=19)
#define TRIG_PIN1 6    // 초음파 센서 Trig 핀
#define ECHO_PIN1 7    // 초음파 센서 Echo 핀
#define SERVO1_PIN 10 // 첫 번째 차단봉 핀
#define SERVO2_PIN 11 // 두 번째 차단봉 핀 
#define SERVO3_PIN 12 // 세 번째 차단봉 핀
#define BUTTON1_PIN 2 // 시작 버튼 핀 (3번)
#define BUTTON2_PIN 3 // 정지 버튼 핀 (4번)
#define CONVEYOR_PIN1 5 // 컨베이어 벨트 제어 핀 1 (6번)
#define CONVEYOR_PIN2 4 // 컨베이어 벨트 제어 핀 2 (7번)
#define TRIG_PIN2 8    // 초음파 센서 Trig 핀
#define ECHO_PIN2 9   // 초음파 센서 Echo 핀
#define TRIG_PIN3 62    // 초음파 센서 Trig 핀 포트A8
#define ECHO_PIN3 63   // 초음파 센서 Echo 핀 포트A9

Servo servo1; // 첫 번째 차단봉 객체
Servo servo2; // 두 번째 차단봉 객체
Servo servo3; // 세 번째 차단봉 객체
long duration1, duration2, duration3;
int distance1, distance2, distance3;  //초음파센서 거리계산
char inputData; // 시리얼 입력 데이터
int servo1Angle = 180; // 첫 번째 차단봉 초기 각도
int servo2Angle = 180; // 두 번째 차단봉 초기 각도
int servo3Angle = 180; // 세 번째 차단봉 초기 각도

String dataFromPC= "";

//컨베이어 벨트 정지
void ar_stopConveyor() {
    analogWrite(CONVEYOR_PIN1, 0);
    analogWrite(CONVEYOR_PIN2, 0);
}

//컨베이어 벨트 속도 100로 설정
void ar_startConveyor() {
    analogWrite(CONVEYOR_PIN1, 100);
    analogWrite(CONVEYOR_PIN2, 0);
}
// 시리얼에서 데이터 읽는 함수 (불필요한 메시지 제거)
String readSerialData() {
  String receivedData = "";
  
  // 데이터를 한 문자씩 읽어서 줄바꿈 문자가 오면 종료
  while (espSerial.available()) {
    char c = espSerial.read();  // 하나의 문자 읽기
    if (c == '\n') {  // 줄바꿈 문자가 오면 데이터 끝
      break;
    }
    receivedData += c;  // 문자를 receivedData에 추가
    delay(2);  // 데이터 수신 간 딜레이 추가
  }
  return receivedData;
}

// AT 명령어 전송 함수 (불필요한 응답 제거)
void sendATCommand(String command, int timeout) {
  espSerial.print(command);
  long int time = millis();

  while (millis() - time < timeout) {
    while (espSerial.available()) {
      char c = espSerial.read();
      // 응답 출력 제거 (필요하면 주석 해제 가능)
      // Serial.print(c);
    }
  }
}
// 서버 종료
void stopServer() {
  sendATCommand("AT+CIPMUX=0\r\n", 2000);
  sendATCommand("AT+CIPSERVER=0\r\n", 2000);
  Serial.println("TCP Server stopped");
}

// 클라이언트로 데이터 전송
void sendDataToClient(String data) {
  String command = "AT+CIPSEND=0," + String(data.length()) + "\r\n";
  sendATCommand(command, 2000);
  delay(100);  // busy s... 방지
  espSerial.print(data);
  Serial.println("Sent to Client: " + data);
}
void sendDataToClient1(String data) {
  String command = "AT+CIPSEND=1," + String(data.length()) + "\r\n";
  sendATCommand(command, 2000);
  delay(100);  // busy s... 방지
  espSerial.print(data);
  Serial.println("Sent to Client: " + data);
}

void setup() {
    dataFromPC= "";
    Serial.begin(9600);          // 시리얼 통신 시작
    pinMode(TRIG_PIN1, OUTPUT);   // 초음파 센서 Trig 핀 설정
    pinMode(ECHO_PIN1, INPUT);    // 초음파 센서 Echo 핀 설정
    pinMode(TRIG_PIN2, OUTPUT);   // 초음파 센서 Trig 핀 설정
    pinMode(ECHO_PIN2, INPUT);    // 초음파 센서 Echo 핀 설정
    pinMode(TRIG_PIN3, OUTPUT);   // 초음파 센서 Trig 핀 설정
    pinMode(ECHO_PIN3, INPUT);    // 초음파 센서 Echo 핀 설정
    pinMode(BUTTON1_PIN, INPUT_PULLUP);  // 시작 버튼 핀 (풀업 저항 활성화)
    pinMode(BUTTON2_PIN, INPUT_PULLUP);  // 정지 버튼 핀 (풀업 저항 활성화)
    pinMode(CONVEYOR_PIN1, OUTPUT);  // 컨베이어 벨트 핀 1
    pinMode(CONVEYOR_PIN2, OUTPUT);  // 컨베이어 벨트 핀 2
    pinMode(24,OUTPUT);   // 초록 LED 설정
    pinMode(26,OUTPUT);   // 빨강 LED 설정
    servo1.attach(SERVO1_PIN);   // 첫 번째 차단봉 연결 
    servo2.attach(SERVO2_PIN);   // 두 번째 차단봉 연결
    servo3.attach(SERVO3_PIN);   // 세 번째 차단봉 연결
    servo1.write(servo1Angle);   // 첫 번째 차단봉 초기화 (180도)
    servo2.write(servo2Angle);   // 두 번째 차단봉 초기화 (180도)
    servo3.write(servo3Angle);   // 세 번째 차단봉 초기화 (180도)

    espSerial.begin(9600);  // ESP-01 시리얼 통신

    sendATCommand("ATE0\r\n", 1000);  // AT 응답 에코 끄기
    sendATCommand("AT+RST\r\n", 2000);  // ESP-01 리셋
    sendATCommand("AT+CWMODE=3\r\n", 2000);  // AP 모드 설정
    sendATCommand("AT+CWSAP=\"ESP8266-AP\",\"87654321\",5,3\r\n", 5000);  // Wi-Fi AP 설정

    delay(2000);  // AP 설정 후 대기

    sendATCommand("AT+CIPMUX=1\r\n", 2000);  // 멀티 연결 활성화
    sendATCommand("AT+CIPSERVER=1,1234\r\n", 5000);  // TCP 서버 시작
    delay(2000);
    Serial.println("TCP Server started on port 1234");
    digitalWrite(24,HIGH);         // LED 초록 ON
    ar_startConveyor();           // 컨베이어 동작
}

void loop() {

    // 초음파 신호 발생
    digitalWrite(TRIG_PIN1, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN1, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN1, LOW);
    duration1 = pulseIn(ECHO_PIN1, HIGH);
    distance1 = duration1 * 0.034 / 2;
    
    // 두 번째 초음파 센서 거리 측정
    digitalWrite(TRIG_PIN2, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN2, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN2, LOW);
    duration2 = pulseIn(ECHO_PIN2, HIGH);
    distance2 = duration2 * 0.034 / 2;

    //세번째 초음파 센서 거리 측정
    digitalWrite(TRIG_PIN3, LOW);      
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN3, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN3, LOW);
    duration3 = pulseIn(ECHO_PIN3, HIGH);
    distance3 = duration3 * 0.034 / 2;
  
    
    // 시리얼 입력 확인
    if (espSerial.available())
    {
      //dataFromPC에 읽은 데이터(PC로부터 받은 데이터) 저장
      dataFromPC = readSerialData();  // 데이터 읽기
      Serial.println(dataFromPC);

      // PC로부터 받은 데이터가 "P"
      if (dataFromPC == "+IPD,0,1:P\0") // 받은 데이터가 "P"
      {
        Serial.println("Received: P");
        if (servo1Angle == 180) {
          for (int i = 0; i < 60; i++) {
            servo1Angle-=2;
            servo1.write(servo1Angle);   //차단봉 각도 설정
            delay(5);              // 각도 변경 후 5ms 딜레이
          }
          delay(500);
        } 
      }
      // PC로부터 받은 데이터가 "C"
      else if (dataFromPC == "+IPD,0,1:C\0")
      {
        Serial.println("Received: C"); // 받은 데이터가 "C"
        if (servo2Angle == 180) {
          for (int i = 0; i < 55; i++) {
            servo2Angle-=2;
            servo2.write(servo2Angle);   // 차단봉 각도 설정
            delay(5);              // 각도 변경 후 5ms 딜레이
          }
        delay(500);
        }
      }
      // PC로부터 받은 데이터가 "J"
      else if (dataFromPC == "+IPD,0,1:J\0")
      {
        Serial.println("Received: J"); // 받은 데이터가 "J" 표시
        if (servo3Angle == 180) {
          for (int i = 0; i < 60; i++) {
            servo3Angle-=2;
            servo3.write(servo3Angle);   // 차단봉 각도 설정
            delay(5);              // 각도 변경 후 5ms 딜레이
          }
        delay(500);
        }
      }
      // PC로부터 받은 데이터가 BOX 또는 dmg 일때
      else if (dataFromPC == "+IPD,0,3:BOX\0" || dataFromPC == "+IPD,0,3:dmg\0")
      {
        digitalWrite(24,LOW); // LED 빨강 OFF 
        digitalWrite(26,HIGH);// LED 초록 ON
      }
    }
    // AND 조건 체크
    //(거리가 6cm 이하 && 차단봉 60도 상태일 때)
    if ((distance1 <= 6 && distance1 > 0)  && servo1Angle == 60) {
      delay(700);
        for (int i = 0; i < 60; i++) {
            servo1Angle+=2;
            servo1.write(servo1Angle); //1번 차단봉 닫음
            delay(12);
        }
        sendDataToClient1("P"); // 라즈베리파이 "P" 전송
        digitalWrite(24,HIGH);  // LED 초록 ON
        digitalWrite(26,LOW);   // LED 빨강 OFF

    }
    // (거리가 6cm 이하 && 차단봉 60도 상태일 때)
    if ((distance2 <= 6 && distance2 > 0) && servo2Angle == 70) {
        for (int i = 0; i < 55; i++) {
            servo2Angle+=2;
            servo2.write(servo2Angle);  //2번 차단봉 닫음
            delay(12);
        }
        sendDataToClient1("C"); // 라즈베리파이 "C" 전송
        digitalWrite(24,HIGH);  // LED 초록 ON
        digitalWrite(26,LOW);   // LED 빨강 OFF
    }
    // (거리가 6cm 이하 && 차단봉 60도 상태일 때)
    if ((distance3 <= 6 && distance3 > 0) && servo3Angle == 60) {
        for (int i = 0; i < 60; i++) {
            servo3Angle+=2;
            servo3.write(servo3Angle);  //3번 차단봉 닫음
            delay(12);
        }
        sendDataToClient1("J"); // 라즈베리파이 "J" 전송
        digitalWrite(24,HIGH);  // LED 초록 ON
        digitalWrite(26,LOW);   // LED 빨강 OFF
    }

    // 3번쨰 초음파 인식
    else if(distance3 <= 6 && distance3 > 0)
    {
      delay(1000);
      digitalWrite(24,HIGH);  // LED 초록 ON
      digitalWrite(26,LOW);   // LED 빨강 OFF
    }

    // 버튼 상태 확인 (시작 버튼 눌렀을 때)
    if (digitalRead(BUTTON1_PIN) == HIGH) {
        ar_startConveyor(); //컨베이어 벨트 시작
    }

    // 버튼 상태 확인 (정지 버튼 눌렀을 때)
    if (digitalRead(BUTTON2_PIN) == HIGH) {
        ar_stopConveyor(); //컨베이어 벨트 멈춤
    }

}