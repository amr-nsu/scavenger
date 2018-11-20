#include "mbed.h"
#include "Servo.h"

Serial serial(USBTX, USBRX, 9600);
Servo servo(PE_11);

int main()
{
    servo.calibrate(0.001, 90);
    while(1) {
        for(double pwm = 0; pwm < 0.5; pwm += 0.01) {
            servo.write(pwm);
            serial.printf("%f\n", pwm);
            wait(0.1);
        }
    }
}
