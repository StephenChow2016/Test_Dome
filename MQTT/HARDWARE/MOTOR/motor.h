#ifndef __MOTOR_H__
#define __MOTOR_H__

#define MOTOR_IN1 PFout(0)
#define MOTOR_IN2 PFout(1)
#define MOTOR_IN3 PFout(2)
#define MOTOR_IN4 PFout(3)

extern void motor_init(void);
extern void motor_single_pwd(void);
extern void motor_single_rev(void);
extern void motor_double_pwd(void);
extern void motor_double_rev(void);
	
#endif
