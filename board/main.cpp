#include "mbed.h"

PwmOut servo0(D9);
PwmOut servo1(D10);
PwmOut servo2(D11);
PwmOut servo3(D12);

PwmOut right_pwm(A1);
DigitalOut right_cmd(A0);
PwmOut left_pwm(A2);
DigitalOut left_cmd(A3);

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
        left_pwm = right;
    } else {
        left_cmd = 1;
        left_pwm = 1 + right;        
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

void grab()
{
    const double wait_time = 0.2;
    
    servo_move(servo1, 0.2, 0.1);
    wait(wait_time);
    
    servo_move(servo3, 0.8, 0.5);
    wait(wait_time);

    servo_move(servo0, 1.0, 0.2);
    wait(wait_time);
    
    servo_move(servo1, 0.1, 0.6);
    wait(wait_time);
    
    servo_move(servo3, 0.5, 0.8);
    wait(wait_time);

    servo_move(servo1, 0.6, 0.2);
    wait(wait_time);

    servo_move(servo0, 0.2, 1.00);
    wait(wait_time);
}

void move_demo()
{
    const double wait_time = 2;

    robot_move(0.5, 0.5);
    wait(wait_time);
    
    robot_move(0.5, -0.5);
    wait(wait_time);

    robot_move(-0.5, -0.5);
    wait(wait_time);      
}

void init()
{
    right_pwm.period_ms(1);
    
    servo0.period_ms(20);
    servo1.period_ms(20);
    servo2.period_ms(20);
    servo3.period_ms(20);
    
    servo_position(servo0, 1.0);
    servo_position(servo1, 0.2);
    servo_position(servo2, 0.5);
    servo_position(servo3, 0.8);
}

int main()
{  
    init();  
    grab();
     
    while(true) {
        move_demo();
    }
}
