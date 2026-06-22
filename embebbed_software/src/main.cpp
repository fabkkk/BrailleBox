#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pca9685 = Adafruit_PWMServoDriver(0x40);

#define SERVOMIN 80
#define SERVOMAX 600

const uint8_t servos[] = {0, 1, 2, 3, 4, 5};
const int NUM_SERVOS = sizeof(servos) / sizeof(servos[0]);

void moverServo(uint8_t canal, int angulo)
{
    int pwm = map(angulo, 0, 180, SERVOMIN, SERVOMAX);
    pca9685.setPWM(canal, 0, pwm);
}

void setup()
{
    Serial.begin(115200);

    Wire.begin(5, 6);

    pca9685.begin();
    pca9685.setPWMFreq(50);
    
    delay(1000);

    // Coloca todos em 0°
    for (int i = 0; i < NUM_SERVOS; i++)
    {
        moverServo(servos[i], 0);
    }

    delay(1000);

    // Move um servo por vez
    for (int i = 0; i < NUM_SERVOS; i++)
    {
        Serial.print("Movendo servo ");
        Serial.println(servos[i]);

        // 0° -> 180°
        for (int angulo = 0; angulo <= 180; angulo++)
        {
            moverServo(servos[i], angulo);
            delay(15);
        }

        delay(500);

        // 180° -> 0°
        for (int angulo = 180; angulo >= 0; angulo--)
        {
            moverServo(servos[i], angulo);
            delay(15);
        }

        delay(500);
    }

    Serial.println("Sequencia concluida.");
}

void loop()
{
    // Nada aqui
}