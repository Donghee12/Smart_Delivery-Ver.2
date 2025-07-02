import pigpio
from time import sleep
import socket 

host = "192.168.4.1"
port = 1234             # 포트 번호

# 소켓 객체 생성
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# 서버의 IP와 포트에 바인드
server_socket.connect((host, port))
print("서버 연결 성공")


pi = pigpio.pi()

pins = [5, 6, 12, 13]  # Servo pins

# Store current angles (initialized with default values)
current_angles = [1500, 1090, 970, 550]


def smooth_move(target_angles, step=10, delay=0.01): # 각도를 10씩 증가
    """Smoothly transition from current to target angles"""
    global current_angles
    
    max_steps = max(abs(target_angles[i] - current_angles[i]) // step for i in range(3))

    # 1번 ~ 3번축 제어
    for _ in range(max_steps):
        for i in range(3):  # index[0] - index[2] => 1번~3번축
            if current_angles[i] < target_angles[i]:
                current_angles[i] += step
                if current_angles[i] > target_angles[i]:  # 초과 값 조정 
                    current_angles[i] = target_angles[i]
            elif current_angles[i] > target_angles[i]:
                current_angles[i] -= step
                if current_angles[i] < target_angles[i]:  # 미달 값 조정
                    current_angles[i] = target_angles[i]
            pi.set_servo_pulsewidth(pins[i], current_angles[i])
        sleep(delay)

    # 집게(4번축)를 나머지 축과 동시 제어 시 이동중에 집게가 닫히거나 열림으로 
    # 1~3번축 제어가 완료되면 (고정 자세) 집게를 제어하기
    gripper_index = 3  # index[3] => 4번 축
    sleep(0.3)
    while current_angles[gripper_index] != target_angles[gripper_index]:
        if current_angles[gripper_index] < target_angles[gripper_index]:
            current_angles[gripper_index] += step
            if current_angles[gripper_index] > target_angles[gripper_index]:
                current_angles[gripper_index] = target_angles[gripper_index]
        elif current_angles[gripper_index] > target_angles[gripper_index]:
            current_angles[gripper_index] -= step
            if current_angles[gripper_index] < target_angles[gripper_index]:
                current_angles[gripper_index] = target_angles[gripper_index]
        pi.set_servo_pulsewidth(pins[gripper_index], current_angles[gripper_index])
        sleep(delay)


def arm_default():
    smooth_move([1500, 1090, 970, 550])



######### LANE P ################## 
def arm_get_l1():
    smooth_move([1110, 1150, 850, 550])
    sleep(0.2)
    smooth_move([1110, 950, 850, 1060])
    sleep(0.2)
    smooth_move([1110, 1150, 850, 1060])


def arm_drop_l1_1():
    smooth_move([530, 1120, 680, 530])
    sleep(0.2)
    arm_default()
 
def arm_drop_l1_2():
    smooth_move([530, 880, 1060, 530])
    sleep(0.2)
    arm_default()

######### LANE C ##################
def arm_get_l2():
    smooth_move([1500, 1030, 710, 1060])
    sleep(0.2)
    smooth_move([1500, 1200, 710, 1060])
  

# Drop in Pallete for L2, 1st, 2nd
def arm_drop_l2_1():
    smooth_move([2370,1080,710,530])
    sleep(0.2)
    arm_default()

def arm_drop_l2_2():
    smooth_move([2400,930,1000,530])
    sleep(0.2)
    arm_default()
    
######### LANE J ##################
def arm_get_l3():
    smooth_move([1860,1110,880,550])
    sleep(0.2)
    smooth_move([1860,910,880,1100])
    sleep(0.2)
    smooth_move([1860, 910, 1080, 1100])
   

# Drop in Pallete for L2, 1st, 2nd
def arm_drop_l3_1():
    smooth_move([2090, 1200, 590, 1100])
    sleep(0.2)
    smooth_move([2090, 1120, 590, 550])
    sleep(0.2)
    arm_default()
   

def arm_drop_l3_2():
    smooth_move([2120, 1070, 820, 1100])
    sleep(0.2)
    smooth_move([2120, 970, 820, 550])
    sleep(0.2)
    arm_default()
    
    
def test():
    arm_default()
    while True:
        uchoice = input("[1] Default [2] GET L2 [3] Drop L2 [4] Drop L2-2 [6]: GET L1 [7] L1-1 [8] L1-2 ")
        if uchoice == '2':
            print("Getting L2..")
            arm_get_l2()

        elif uchoice == '3':
            print("Dropping L2..")
            arm_drop_l2_1()

        elif uchoice == '4':
            print("Dropping L2-2..")
            arm_drop_l2_2()
        
        elif uchoice == '5':
            print("Getting L1..")
            arm_get_l1()

        elif uchoice == '6':
            print("Dropping L1..")
            arm_drop_l1_1()

        elif uchoice == '7':
            print("Dropping L1-2..")
            arm_drop_l1_2()
            
        elif uchoice == '8':
            print("Getting L3..")
            arm_get_l3()

        elif uchoice == '9':
            print("Dropping L3..")
            arm_drop_l3_1()

        elif uchoice == '10':
            print("Dropping L3-2..")
            arm_drop_l3_2()
        else:
            print("Default Position..")
            arm_default()
   
def main():
    
    arm_default()
    P_flag = 1
    C_flag = 1
    J_flag = 1
    
    while True:
        data = server_socket.recv(1024).decode("utf-8")
        sleep(0.3)
        if data == 'P':
            print("Getting L3..")
            arm_get_l3()
            sleep(0.3)
            
            if P_flag == 1:
                print("Dropping L3..")
                arm_drop_l3_1()
                P_flag+= 1
            elif P_flag == 2:
                print("Dropping L3-2..")
                arm_drop_l3_2()
                P_flag = 1
                
            sleep(0.3)
            arm_default()
        
        elif data == 'C':
            print("Getting L2..")
            arm_get_l2()
            sleep(0.3)
            
            if C_flag == 1:
                print("Dropping L2..")
                arm_drop_l2_1()
                C_flag += 1
            elif C_flag == 2:
                print("Dropping L2-2..")
                arm_drop_l2_2()
                C_flag = 1
                
            sleep(0.3)
            arm_default()
       
        elif data == 'J':
            print("Getting L1..")
            arm_get_l1()
            sleep(0.3)
            
            if J_flag == 1:
                print("Dropping L1..")
                arm_drop_l1_1()
                J_flag+=1
            elif J_flag == 2:
                print("Dropping L1-2..")
                arm_drop_l1_2()
                J_flag = 1
            sleep(0.3)
            arm_default()
       
        else:
            print("Default Position..")
            arm_default()

main()
