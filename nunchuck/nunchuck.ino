/*
   WiiChuckDemo --

   2008 Tod E. Kurt, http://thingm.com/

*/

#include <Wire.h>
/*
 * Nunchuck functions  -- Talk to a Wii Nunchuck
 *
 * This library is from the Bionic Arduino course : 
 *                          http://todbot.com/blog/bionicarduino/
 *
 * 2007-11 Tod E. Kurt, http://todbot.com/blog/
 *
 * The Wii Nunchuck reading code originally from Windmeadow Labs
 *   http://www.windmeadow.com/node/42
 */

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
//#define Wire.write(x) Wire.send(x)
//#define Wire.read() Wire.receive()
#endif



static uint8_t nunchuck_buf[6];   // array to store nunchuck data,

// Uses port C (analog in) pins as power & ground for Nunchuck
static void nunchuck_setpowerpins()
{

    delay(100);  // wait for things to stabilize        
}

// initialize the I2C system, join the I2C bus,
// and tell the nunchuck we're talking to it
static void nunchuck_init()
{ 
    Wire.begin();                // join i2c bus as master
    Wire.beginTransmission(0x52);// transmit to device 0x52
#if (ARDUINO >= 100)
    Wire.write((uint8_t)0x40);// sends memory address
    Wire.write((uint8_t)0x00);// sends sent a zero.  
#else
    Wire.send((uint8_t)0x40);// sends memory address
    Wire.send((uint8_t)0x00);// sends sent a zero.  
#endif
    Wire.endTransmission();// stop transmitting
}

// Send a request for data to the nunchuck
// was "send_zero()"
static void nunchuck_send_request()
{
    Wire.beginTransmission(0x52);// transmit to device 0x52
#if (ARDUINO >= 100)
    Wire.write((uint8_t)0x00);// sends one byte
#else
    Wire.send((uint8_t)0x00);// sends one byte
#endif
    Wire.endTransmission();// stop transmitting
}

// Encode data to format that most wiimote drivers except
// only needed if you use one of the regular wiimote drivers
static char nunchuk_decode_byte (char x)
{
    x = (x ^ 0x17) + 0x17;
    return x;
}

// Receive data back from the nunchuck, 
// returns 1 on successful read. returns 0 on failure
static int nunchuck_get_data()
{
    int cnt=0;
    Wire.requestFrom (0x52, 6);// request data from nunchuck
    while (Wire.available ()) {
        // receive byte as an integer
#if (ARDUINO >= 100)
        nunchuck_buf[cnt] = nunchuk_decode_byte( Wire.read() );
#else
        nunchuck_buf[cnt] = nunchuk_decode_byte( Wire.receive() );
#endif
        cnt++;
    }
    nunchuck_send_request();  // send request for next data payload
    // If we recieved the 6 bytes, then go print them
    if (cnt >= 5) {
        return 1;   // success
    }
    return 0; //failure
}

// Print the input data we have recieved
// accel data is 10 bits long
// so we read 8 bits, then we have to add
// on the last 2 bits.  That is why I
// multiply them by 2 * 2
static void nunchuck_print_data()
{ 
    static int i=0;
    int joy_x_axis = nunchuck_buf[0];
    int joy_y_axis = nunchuck_buf[1];
    int accel_x_axis = nunchuck_buf[2]; // * 2 * 2; 
    int accel_y_axis = nunchuck_buf[3]; // * 2 * 2;
    int accel_z_axis = nunchuck_buf[4]; // * 2 * 2;

    int z_button = 0;
    int c_button = 0;

    // byte nunchuck_buf[5] contains bits for z and c buttons
    // it also contains the least significant bits for the accelerometer data
    // so we have to check each bit of byte outbuf[5]
    if ((nunchuck_buf[5] >> 0) & 1) 
        z_button = 1;
    if ((nunchuck_buf[5] >> 1) & 1)
        c_button = 1;

    if ((nunchuck_buf[5] >> 2) & 1) 
        accel_x_axis += 1;
    if ((nunchuck_buf[5] >> 3) & 1)
        accel_x_axis += 2;

    if ((nunchuck_buf[5] >> 4) & 1)
        accel_y_axis += 1;
    if ((nunchuck_buf[5] >> 5) & 1)
        accel_y_axis += 2;

    if ((nunchuck_buf[5] >> 6) & 1)
        accel_z_axis += 1;
    if ((nunchuck_buf[5] >> 7) & 1)
        accel_z_axis += 2;

    Serial.print(i,DEC);
    Serial.print("\t");

    Serial.print("joy:");
    Serial.print(joy_x_axis,DEC);
    Serial.print(",");
    Serial.print(joy_y_axis, DEC);
    Serial.print("  \t");

    Serial.print("acc:");
    Serial.print(accel_x_axis, DEC);
    Serial.print(",");
    Serial.print(accel_y_axis, DEC);
    Serial.print(",");
    Serial.print(accel_z_axis, DEC);
    Serial.print("\t");

    Serial.print("but:");
    Serial.print(z_button, DEC);
    Serial.print(",");
    Serial.print(c_button, DEC);

    Serial.print("\r\n");  // newline
    i++;
}

// returns zbutton state: 1=pressed, 0=notpressed
static int nunchuck_zbutton()
{
    return ((nunchuck_buf[5] >> 0) & 1) ? 0 : 1;  // voodoo
}

// returns zbutton state: 1=pressed, 0=notpressed
static int nunchuck_cbutton()
{
    return ((nunchuck_buf[5] >> 1) & 1) ? 0 : 1;  // voodoo
}

// returns value of x-axis joystick
static int nunchuck_joyx()
{
    return nunchuck_buf[0]; 
}

// returns value of y-axis joystick
static int nunchuck_joyy()
{
    return nunchuck_buf[1];
}

// returns value of x-axis accelerometer
static int nunchuck_accelx()
{
    return nunchuck_buf[2];   // FIXME: this leaves out 2-bits of the data
}

// returns value of y-axis accelerometer
static int nunchuck_accely()
{
    return nunchuck_buf[3];   // FIXME: this leaves out 2-bits of the data
}

// returns value of z-axis accelerometer
static int nunchuck_accelz()
{
    return nunchuck_buf[4];   // FIXME: this leaves out 2-bits of the data
}
int loop_cnt = 0;

byte accx, accy, zbut, cbut, joyx, joyy;
int ledPin = 13;




//Control Functions

float min_steering = 20;
float max_steering = 910;
float center_steering = max_steering - ((max_steering - min_steering)/2);
float lower_steering_size = center_steering - min_steering;
float upper_steering_size = max_steering - center_steering;


float max_gas = 900.0;        /// the maximum values for each actuator; we won't always have it going all the way to 1024
float min_gas = 0.0;
float gas_size = max_gas - min_gas;

float max_brake = 1024.0;
float min_brake = 0.0;
float brake_size = max_brake - min_brake;

float wiichuck_min = 30.0;
float wiichuck_max = 226.0;
float wiichuck_range = wiichuck_max - wiichuck_min;
float center = wiichuck_max - ((wiichuck_range)/2);

float deadzone_min = 128;
float deadzone_max = 136;

float lower_zone_size = deadzone_min - wiichuck_min;
float upper_zone_size = wiichuck_max - deadzone_max;

static int set_goal_steering(int raw_value) { //set steering actuator
  //return raw_value;
  float scalar_lower = (upper_steering_size)/lower_zone_size; //scalar for lower part
  float scalar_upper = (lower_steering_size)/upper_zone_size; //scaler for upper part

  if(raw_value > deadzone_min && raw_value < deadzone_max) { //deadzone
    return center_steering;              //if in deadzone, center steering
  }
  else if(raw_value <= deadzone_min) {            //if less than deadzone, go left
      return max_steering - ((raw_value - wiichuck_min) * scalar_lower);
  }
  else {                              //if more than deadzone, go right
      return min_steering + (center_steering - ((raw_value - deadzone_max) * scalar_upper)); //scale raw_value
  }
}

static int set_goal_accelerator(int raw_value) { //set accelerator actuator
  //return raw_value;
  float scalar = gas_size/upper_zone_size;        //sets scalar
  if(raw_value < deadzone_max) {               //if less than deadzone or mid value, we will be breaking, so gas is zero
    return min_gas;
  }                              //min is 135, so subtract 135, then multiple by scalar
    return min_gas + ((raw_value - deadzone_max) * scalar);
}

static int set_goal_brake(int raw_value, int zbut, int cbut){ //set brake actuator
  //return raw_value;
  float scalar = brake_size/lower_zone_size; //sets scalar

  if(zbut == 1 || cbut == 1){
    return min_brake;
  }
  
  if(raw_value > deadzone_min) { //if greater than deadzone or mid value, we will be gasing, so brake is max  
    return max_brake;
  }
  else {  //we assume the break is at full force when the actuator is at min length (min_brake), so we subtract the wii chuck value from the max_brake
    return ((raw_value - wiichuck_min) * scalar) + min_brake;
  }
}

int threshold = 30;
int power = 255;

int steeringPosition;
int STEERING_SENSE_PIN = 1;
int STEERING_DIRECTION_PIN = 7;
int STEERING_VELOCITY_PIN = 6;
bool steeringChanged = true;

int ACCELERATOR_DIRECTION_PIN = 10;
int ACCELERATOR_VELOCITY_PIN = 9;
bool acceleratorChanged = true;

int braking_position;
int BRAKING_SENSE_PIN = 0;
int BRAKING_VELOCITY_PIN = 5;
int BRAKING_DIRECTION_PIN = 4;
bool brakingChanged = true;

int safetypin = 8;



void set_steering(int goal) {
    steeringPosition = analogRead(STEERING_SENSE_PIN);
     
    if (steeringPosition < goal - threshold) { // always move toward the value
            digitalWrite(STEERING_DIRECTION_PIN, LOW);
            analogWrite(STEERING_VELOCITY_PIN, power);
            Serial.println("Moving from <");
    } else if (steeringPosition > goal + threshold) {
            digitalWrite(STEERING_DIRECTION_PIN, HIGH);
            analogWrite(STEERING_VELOCITY_PIN, power);
            Serial.println("Moving from >");
    } else {
            analogWrite(STEERING_VELOCITY_PIN, 0);
            steeringChanged = false;
            Serial.println("Flag is now false");
    }   
}

void set_accelerator(int goal) {
            
            analogWrite(ACCELERATOR_VELOCITY_PIN, floor(goal/4));
            acceleratorChanged = false; 
    }

void set_brake(int goal) {
    braking_position = analogRead(BRAKING_SENSE_PIN);
            
      if (braking_position < goal - threshold) { // always move toward the value
            digitalWrite(BRAKING_DIRECTION_PIN, LOW);
            analogWrite(BRAKING_VELOCITY_PIN, power);
    } else if (braking_position > goal + threshold) {
            digitalWrite(BRAKING_DIRECTION_PIN, HIGH);
            analogWrite(BRAKING_VELOCITY_PIN, power);
    } else {
            analogWrite(BRAKING_VELOCITY_PIN, 0);
            brakingChanged = false; 
    }
}

void setup()
{
  Serial.begin(19200);
  nunchuck_setpowerpins();
  nunchuck_init(); // send the initilization handshake

pinMode(STEERING_VELOCITY_PIN, OUTPUT);
pinMode(STEERING_DIRECTION_PIN, OUTPUT);
pinMode(BRAKING_VELOCITY_PIN, OUTPUT);
pinMode(BRAKING_DIRECTION_PIN, OUTPUT);
pinMode(ACCELERATOR_VELOCITY_PIN, OUTPUT);
pinMode(ACCELERATOR_DIRECTION_PIN, OUTPUT);
  Serial.print("WiiChuckDemo ready\n");
}

void loop() {
  if ( loop_cnt > 10 ) { // every 100 msecs get new data
    loop_cnt = 0;

    digitalWrite(safetypin, HIGH);
    
    nunchuck_get_data();

    accx  = nunchuck_accelx(); // ranges from approx 70 - 182
    accy  = nunchuck_accely(); // ranges from approx 65 - 173
    zbut = nunchuck_zbutton();
    cbut = nunchuck_cbutton();
    joyx = nunchuck_joyx();
    joyy = nunchuck_joyy();
    
    Serial.print(braking_position);
    
    //Serial.print("accx: "); Serial.print((byte)accx, DEC);
    //Serial.print("\taccy: "); Serial.print((byte)accy, DEC);
    //Serial.print("\tjoyx: "); Serial.print((byte)joyx, DEC);
   // Serial.print("\tjoyy: "); Serial.print((byte)joyy, DEC);
    //Serial.print("\tzbut: "); Serial.print((byte)zbut, DEC);
    //Serial.print("\tcbut: "); Serial.println((byte)cbut, DEC); 
    Serial.print("Steering: "); Serial.print(set_goal_steering(joyx));
    Serial.print("\tAccelerator: "); Serial.print(set_goal_accelerator(joyy));
    Serial.print("\tBrake: "); Serial.println(set_goal_brake(joyy, zbut, cbut));

    
    steeringChanged = true;
    acceleratorChanged = true;
    brakingChanged = true;

    if (steeringChanged) {
        set_steering(set_goal_steering(joyx));
    }

    if (acceleratorChanged) {
        set_accelerator(set_goal_accelerator(joyy));
    }

    if (brakingChanged) {
        set_brake(set_goal_brake(joyy, zbut, cbut));
    }
  }
  loop_cnt++;
  delay(1);
}

