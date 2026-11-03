#define UP_THRESHOLD 524
#define DOWN_THRESHOLD 500
#define JOYSTICK_ZERO 512
#define DEADBAND 12

#define SPEED_AXIS A2
#define TURN_AXIS A0
#define LIFT_AXIS A1

int speed = 0;
int turn = 0;
int lift = 0;

// Top left, top right, back right, back left
String NAMES[4];
int PWM[] = {1, 4, 7, 10};
int FORWARD[] = {2, 5, 8, 11};
int BACKWARD[] = {3, 6, 9, 12};
int power[4];
int scaled_power[4];

void setup() {
  // put your setup code here, to run once:
  NAMES[0] = "Top left";
  NAMES[1] = "Top right";
  NAMES[2] = "Back right";
  NAMES[3] = "Back left";

  pinMode(SPEED_AXIS, INPUT);
  pinMode(TURN_AXIS, INPUT);
  pinMode(LIFT_AXIS, INPUT);

  for (int i = 0; i < 4; i++) {
    pinMode(PWM[i], OUTPUT);
    pinMode(FORWARD[i], OUTPUT);
    pinMode(BACKWARD[i], OUTPUT);
  }

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  speed = -(analogRead(SPEED_AXIS) - JOYSTICK_ZERO);
  turn = analogRead(TURN_AXIS) - JOYSTICK_ZERO;
  lift = -(analogRead(LIFT_AXIS) - JOYSTICK_ZERO);

  if (abs(speed) <= DEADBAND) {
    speed = 0;
  }
  if (abs(turn) <= DEADBAND) {
    turn = 0;
  } 
  if (abs(lift) <= DEADBAND) {
    lift = 0;
  }
  
  Serial.print("Speed: ");
  Serial.print(speed);
  Serial.print(", Turn: ");
  Serial.print(turn);
  Serial.print(", Lift: ");
  Serial.println(lift);

  if ((abs(speed) + abs(turn) + abs(lift)) > JOYSTICK_ZERO) {
      power[0] = (speed + turn - lift)/(abs(speed) + abs(turn) + abs(lift))*255; // Top left, go clockwise
      power[1] = (speed - turn - lift)/(abs(speed) + abs(turn) + abs(lift))*255; 
      power[2] = (speed - turn + lift)/(abs(speed) + abs(turn) + abs(lift))*255;
      power[3] = (speed + turn + lift)/(abs(speed) + abs(turn) + abs(lift))*255;
  } else if ((abs(speed) + abs(turn) + abs(lift)) != 0) {
    power[0] = speed + turn - lift;
    power[1] = speed - turn - lift; 
    power[2] = speed - turn + lift;
    power[3] = speed + turn + lift;
  } else {
    for (int i = 0; i < 4; i++) {
      power[i] = 0;
    }
  }



  for (int i = 0; i < 4; i++) {
    // scaled_power[i] = map(abs(abs(power[i])), 0, 512, 0, 255); // Scales magnitude of the joystick reading from max of 512 to max of 255
    scaled_power[i] = map(abs(power[i]), 0, 512, 0, 255);
    analogWrite(PWM[i], scaled_power[i]);
    Serial.print(NAMES[i]);
    Serial.print("'s raw power: ");
    Serial.print(power[i]);
    Serial.print(", scaled power: ");
    Serial.print(scaled_power[i]);

    if (power[i] > DEADBAND) {
      digitalWrite(FORWARD[i], HIGH);
      digitalWrite(BACKWARD[i], LOW);

      Serial.println(", forward");
    } else if (power[i] < -DEADBAND) {
      digitalWrite(FORWARD[i], LOW);
      digitalWrite(BACKWARD[i], HIGH);

      Serial.println(", backward");
    } else {
      digitalWrite(FORWARD[i], LOW);
      digitalWrite(BACKWARD[i], LOW);

      Serial.println(", stationary");
    }
  }
}
