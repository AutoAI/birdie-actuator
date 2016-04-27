

int threshold = 20;
int power = 255;

int steeringPosition;
int STEERING_SENSE_PIN = 12;
int STEERING_DIRECTION_PIN = 7;
int STEERING_VELOCITY_PIN = 6;
bool steeringChanged = true;

int accelerator_position;
int ACCELERATOR_SENSE_PIN = 10;
int ACCELERATOR_DIRECTION_PIN = 2;
int ACCELERATOR_VELOCITY_PIN = 3;
bool acceleratorChanged = true;

int braking_position;
int BRAKING_SENSE_PIN = 8;
int BRAKING_VELOCITY_PIN = 4;
int BRAKING_DIRECTION_PIN = 5;
bool brakingChanged = true;

int safetypin = 8;

void setup() {
  Serial.begin(9600);
  pinMode(STEERING_VELOCITY_PIN, OUTPUT);
  pinMode(STEERING_DIRECTION_PIN, OUTPUT);
}

void loop() {
    Serial.println(1);

    analogWrite(STEERING_VELOCITY_PIN, 255 );
    digitalWrite(STEERING_DIRECTION_PIN, LOW);
    delay(4000);

    Serial.println(0);
    analogWrite(STEERING_VELOCITY_PIN, 255);
    digitalWrite(STEERING_DIRECTION_PIN, HIGH);
    delay(4000);

}

