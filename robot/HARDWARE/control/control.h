#ifndef __CONTROL_H
#define __CONTROL_H
#include "sys.h"
#define PI 3.14159265
void EXTI9_5_IRQHandler(void);
int balance_UP(float Angle, float Mechanical_balance, float Gyro);
int velocity(int encoder_left, int encoder_right);
int turn(int encoder_left, int encoder_right, float gyro);
#endif