#include "mbed.h"
#include "Servo.h"
#include "LCD_DISCO_L476VG.h"

LCD_DISCO_L476VG lcd;
DigitalOut led_green(LED1);

Servo servo1(PE_8);
Servo servo2(PA_0);
Servo servo3(PE_11);

Serial serial(USBTX, USBRX, 9600);

void servo_move(Servo& servo, double from, double to)
{
    const double step = from < to? 0.01 : -0.01;
    double position = from;
    while (step > 0 && position < to || step < 0 && position > to) {
        servo.write(position);
        position += step;
        wait(0.03);
    }
}

void grab_cargo()
{
    lcd.Clear();
    lcd.DisplayString((uint8_t *)"GRAB");
    servo_move(servo1, 1.0, 0.4);
}

void up_and_down()
{
    servo_move(servo2, 0.0, 0.5);
    wait(1);
    servo_move(servo3, 0.4, 0.0);
    wait(5);
    servo_move(servo3, 0.0, 0.4);
    wait(1);
    servo_move(servo2, 0.5, 0);
    wait(0.5);
}

void release_cargo()
{
    lcd.Clear();
    lcd.DisplayString((uint8_t *)"RELEASE");
    servo_move(servo1, 0.4, 1.0);
}

void show_prompt()
{
    uint8_t title[] = "ROBOT BY LIZA PRISHLYAK";
    lcd.Clear();
    lcd.ScrollSentence(title, 1, 200);
}

int main()
{
    servo3.calibrate(0.001, 90);

    servo1.write(1.0);
    servo2.write(0.0);
    servo3.write(0.4);

    show_prompt();

    while(1) {
        grab_cargo();
        wait(1);
        up_and_down();
        release_cargo();
        wait(5);
    }
}
