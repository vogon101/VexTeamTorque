#include "main.c"

int ROBOT_ARM_STATE = 0;
int STATE_ARM_NONE = 0;
int STATE_ARM_HOLD = 1;
int STATE_ARM_MOVE = 2;

int ROBOT_POT_BASE = 0;
bool AUTONOMOUS_CODE = false;



void set_lift(int speed) {
	motor[LeftLift] = speed;
	motor[RightLiftUpper] = speed;
	motor[RightLiftLower] = speed;
}

int potValue = 0;
int MAX_POT_CHANGE = 7;

void calibrate_pot() {
	if(SensorValue[ForkTouch] == 1) nMotorEncoder[LeftLift] = 0;
}

int get_pot_raw() {
	calibrate_pot();
	int total = 0;
	for (int i = 0; i < 5; i++) {
		total += abs(nMotorEncoder[LeftLift]);
		sleep(1);
	}
	return abs(total/5);
}



void checkPot() {
	int newPVal = get_pot_raw();
	datalogAddValue(2, newPVal);
	if (newPVal - potValue > MAX_POT_CHANGE) {
		datalogAddValue(3, 100);
		potValue += MAX_POT_CHANGE;
	} else if (newPVal - potValue < -MAX_POT_CHANGE) {
		datalogAddValue(3, -100);
		potValue -= MAX_POT_CHANGE;
	} else {
		datalogAddValue(3, 50);
		potValue = newPVal;
	}
}



int get_pot() {
	checkPot();
	return potValue;
}

void control_drive_fw(int speed) {
	motor[DFrontLeft] = speed;
	motor[DFrontRight] = speed;
	motor[DBackLeft] = speed;
	motor[DBackRight] = speed;
	motor[DLateral] = 0;
}

void control_stop_drive() {
	motor[DFrontLeft] = 0;
	motor[DFrontRight] = 0;
	motor[DBackLeft] = 0;
	motor[DBackRight] = 0;
	motor[DLateral] = 0;
}

void control_stop_arm() {
	motor[LeftLift] = 0;
	motor[RightLiftUpper] = 0;
	motor[RightLiftLower] = 0;
}

void control_spin(bool clockwise) {
	if (clockwise){
		motor[DBackRight] = 127;
		motor[DFrontRight] = 127;
		motor[DBackLeft] = -127;
		motor[DFrontLeft] = -127;
		motor[DLateral] = 127;
	} else {
		motor[DBackRight] = -127;
		motor[DFrontRight] = -127;
		motor[DBackLeft] = 127;
		motor[DFrontLeft] = 127;
		motor[DLateral] = -127;
	}
	sleep(10);
}


bool control_check_emergency_stop () {
	if (AUTONOMOUS_CODE) return false;
	else if (vexRT[Btn7U] || vexRT[Btn7UXmtr2]) {
		SensorValue[LEDRed] = true;
		control_stop_arm();
		control_stop_drive();
		return true;
	}
	return false;
}

void control_throw() {

	set_lift(127);
	SensorValue[LEDGreen] = true;
	while (get_pot() < 200) {
		if (control_check_emergency_stop()){return;};
	}
	SensorValue[LEDAmber]= true;
	set_lift(0);
	sleep(100);
	set_lift(-20);
	sleep(100);
	set_lift(-40);
	sleep(100);
	while(get_pot() > 100) {
		if (control_check_emergency_stop()){return;}
	}
	set_lift(0);
	sleep(50);
	set_lift(30);
	sleep(100);
	set_lift(0);

}

void control_hold() {
	set_lift(35);
}

float ROBOT_ROTATIONS_PER_M = 1600;
void control_move_dist(int dist, int backwards = 1) {
		nMotorEncoder[DBackLeft] = 0;
		control_drive_fw(127 * backwards);
		while(nMotorEncoder[DBackLeft] > -dist * ROBOT_ROTATIONS_PER_M) {
			sleep(10);
			if (control_check_emergency_stop()){return;}
		}
		control_stop_drive();
		nMotorEncoder[DBackLeft] = 0;
}
