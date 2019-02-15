#include "mbed.h"

PwmOut servo0(D9);
PwmOut servo1(D10);
PwmOut servo2(D11);
PwmOut servo3(D12);
PwmOut servo4(D7);

PwmOut right_pwm(A1);
DigitalOut right_cmd(A0);
PwmOut left_pwm(A2);
DigitalOut left_cmd(A3);

DigitalOut led(LED1);

Serial device(USBTX, USBRX);


namespace
{
    double box_pose = 0.1;  // cup

    const double MOVE_VEL = 0.5;
    const double MOVE_TIME = 1.5;
    const double SMALL_MOVE_TIME = 0.5;
    const double ROTATE_VEL = 0.8;
    const double DEG_TO_SEC_GAIN = 0.015;
}

void robot_move(double left, double right)
{
    if (right >= 0) {
        right_cmd = 0;
        right_pwm = right;
    } else {
        right_cmd = 1;
        right_pwm = 1 + right;        
    }

    if (left >= 0) {
        left_cmd = 0;
        left_pwm = left;
    } else {
        left_cmd = 1;
        left_pwm = 1 + left;        
    }
}

void servo_position(PwmOut& servo, double position)
{
    servo.pulsewidth_us(500 + position * (2500 - 500));
}

void servo_move(PwmOut& servo, double from, double to)
{
    const double step = from < to? 0.01 : -0.01;
    double position = from;
    while (step > 0 && position < to || step < 0 && position > to) {
        servo_position(servo, position);
        position += step;
        wait(0.02);
    }
}

double get_grab_value(char type)
{
    if (type == '1') {  // cup
        return 0.3;
    }
    return 0.5;         // box
}

double get_box_pose(char type)
{
    if (type == '1') {  // cup
        return 0.1;
    }
    return 0.9;         // box    
}

void grab(char type='2')
{
    const double wait_time = 0.2;
        
    servo_move(servo1, 0.2, 0.1);
    wait(wait_time);
    
    servo_move(servo3, 0.9, get_grab_value(type));
    wait(wait_time);

    servo_move(servo0, 1.0, 0.2);
    wait(wait_time);
    
    servo_move(servo4, box_pose, get_box_pose(type));
    box_pose = get_box_pose(type);
    wait(wait_time);
    
    servo_move(servo1, 0.1, 0.7);
    wait(wait_time);
    
    servo_move(servo3, get_grab_value(type), 0.9);
    wait(wait_time);

    servo_move(servo1, 0.7, 0.2);
    wait(wait_time);

    servo_move(servo0, 0.2, 1.00);
    wait(wait_time);
}

void grab_up()
{
    const double wait_time = 0.2;
    
    servo_move(servo1, 0.2, 0.1);
    wait(wait_time);
    
    servo_move(servo3, 0.8, 0.5);
    wait(wait_time);

    servo_move(servo0, 1.0, 0.4);
    wait(wait_time);
    
    servo_move(servo1, 0.1, 0.4);
    wait(wait_time);
    
}

void grab_down()
{
    const double wait_time = 0.2;

    servo_move(servo1, 0.4, 0.1);
    wait(wait_time);   

    servo_move(servo0, 0.4, 1.0);
    wait(wait_time);
    
    servo_move(servo3, 0.5, 0.8);
    wait(wait_time);

    servo_move(servo1, 0.1, 0.2);
    wait(wait_time);
}

double angle_to_time(int angle)
{
    return DEG_TO_SEC_GAIN * abs(angle);
}

void move_to_object(int angle, double time = MOVE_TIME)
{
    if (angle > 0) {
        robot_move(ROTATE_VEL, -ROTATE_VEL);
    } else {
        robot_move(-ROTATE_VEL, ROTATE_VEL);
    }
    wait(angle_to_time(angle));
    
    robot_move(MOVE_VEL, MOVE_VEL);
    wait(time);
    
    robot_move(0, 0); 
}

void move_from_object(int angle, double time = MOVE_TIME)
{
    robot_move(-MOVE_VEL, -MOVE_VEL);
    wait(time);

    if (angle > 0) {
        robot_move(-ROTATE_VEL, ROTATE_VEL);
    } else {
        robot_move(ROTATE_VEL, -ROTATE_VEL);
    }
    wait(angle_to_time(angle));

    robot_move(0, 0); 
}

void serial_callback()
{
    char cmd = device.getc();
    switch(cmd) {
    case 'g': {  // move and grab
        int8_t angle = device.getc();
        char type = device.getc();
        move_to_object(angle);
        grab(type);
        move_from_object(angle);
        device.putc(cmd);
        break;
    }
    case 'v': {  // move not full distance and grab
        int8_t angle = device.getc();
        char type = device.getc();
        move_to_object(angle, MOVE_TIME - SMALL_MOVE_TIME);
        grab(type);
        move_from_object(angle);
        device.putc(cmd);
        break;
    }
    case 'x': {  // grab without moving
        int8_t angle = device.getc();
        char type = device.getc();
        wait(5);
        grab(type);
        wait(2);
        device.putc(cmd);
        break;
    }
    case 'z': {  // rotate to object without grabbing
        int8_t angle = device.getc();
        char type = device.getc();
        move_to_object(angle, 0);
        device.putc(cmd);
        break;
    }
    case 'c': {  // rotate and small move to object without grabbing
        int8_t angle = device.getc();
        char type = device.getc();
        move_to_object(angle, SMALL_MOVE_TIME);
        device.putc(cmd);
        break;
    }
    case 'f':
        robot_move(MOVE_VEL, MOVE_VEL);
        break;
    case 'b':
        robot_move(-MOVE_VEL, -MOVE_VEL);
        break;
    case 'r':
        robot_move(ROTATE_VEL, -ROTATE_VEL);
        break;
    case 'l':
        robot_move(-ROTATE_VEL, ROTATE_VEL);
        break;
    case 's':
        robot_move(0, 0);
        break;
    case 't':
        grab();
        break;
    case 'u':
        grab_up();
        break;
    case 'd':
        grab_down();
        break;
    }
}

void init() 
{   
    right_pwm.period_ms(1);
    left_pwm.period_ms(1);
    
    servo0.period_ms(20);
    servo1.period_ms(20);
    servo2.period_ms(20);
    servo3.period_ms(20);
    servo4.period_ms(20);
    
    servo_position(servo0, 1.0);
    servo_position(servo1, 0.2);
    servo_position(servo2, 0.5);
    servo_position(servo3, 0.9);
    servo_position(servo4, box_pose);
}

int main()
{  
    init();  
    
    device.baud(9600);
    device.attach(&serial_callback, Serial::RxIrq);
       
    while(true) {
        led = !led;
        wait(1);
    }
}
