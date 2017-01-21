int axis_values[3];
int prev_motor_pos[4];
int curr_motor_pos[4];
int sensor_distance[4];
int velocity[2];
int motor_voltages[4];

setup(){
	//For debugging - serial data stream @ 9600 baud
	Serial.begin(9600);
	//Set up the output pins
}


loop() {
        while(){
		read_control_input()
		read_encoders()
		read_sensors()
        }
}
void read_control_input(int axis_values[]) {
	axis_values[0] = 0; // read left joystick X (forwards/backwards)
	axis_vlaues[1] = 0; // read left joystick Y (left/right)
	axis_values[2] = 0; // read right joystick X (rotation
}

void read_encoders(int prev_motor_pos[], int curr_motor_pos[]) {
        memcpy(prev_motor_pos, curr_motor_pos, 4);
	curr_motor_pos = 0;// read motor encoders
}

void read_sensors(int sensor_distance[]) {
	sensor_distance[0] = 0;// read sensor 1
        sensor_distance[1] = 0;// read sensor 2
        sensor_distance[2] = 0;// read sensor 3
        sensor_distance[3] = 0;// read sensor 4
}

void calculate_velocity(int curr_motor_pos[], int prev_motor_pos[]) {
int [] deltas = {curr_motor_pos[0] - prev_motor_pos[0],
                curr_motor_pos[1] - prev_motor_pos[1],
                curr_motor_pos[2] - prev_motor_pos[2],
                curr_motor_pos[3] - prev_motor_pos[3]
                }; 
        // calculate encoder deltas
	// calculate velocity
}

void set_motor_voltages(int axis[], int velocity[], int distances[], int motor_voltages[]) {
	// check for collisions (separate function ? )
	// calculate motor inputs
	
	// constrain voltages based on established constants
}
