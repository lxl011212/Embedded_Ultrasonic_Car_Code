#include <Servo.h>

// create a servo object
Servo servo;

// declare pins
int int1 = 3;
int sound_p = 5;
int sensor_p = 6;
int data_p = 7;
int time_p = 8;
int servo_p = 9;
int g_led = 12;
int r_led = 13;

//declare variables
double distance;
double duration;

void setup()
{
  servo.attach(servo_p); // declare servo_p is connected to the servo object (sensor)

  Serial.begin(9600); // Sets the data rate in 9600 bits per second for serial data transmission

  // declare the pinmodes
  pinMode(sound_p, INPUT); // receives echo
  pinMode(sensor_p, OUTPUT); // send sound by the ultrasonic sensor
  pinMode(servo_p, OUTPUT); //control the servo(sensor position)
  pinMode(data_p, OUTPUT);
  pinMode(int1, OUTPUT); // moves the car by supplying power
  pinMode(time_p, OUTPUT);
  pinMode(r_led, OUTPUT);
  pinMode(g_led, OUTPUT);
}

void loop()
{
  double val {measure()}; // call the measure function, determines the distance
  servo.write(90); // set servo at 90 degree angle to measure front
  Serial.println(val); //Prints data to the serial port in double
  delay(150);

  if (val > 25) //if the distance is larger than 25 cm, the car drives with the green led on, indicating it's safe to drive forward
  {
    digitalWrite(g_led, HIGH);
    digitalWrite(r_led, LOW);

    digitalWrite(int1, LOW);
    move(data_p, time_p, B01010000); // move the car in forward direction
    digitalWrite(int1, HIGH);
  }

  if (val <= 25 && val > 15) //if the distance is between 25cm and 15 cm, the car drives with the both leds on, indicating it should pay attention now
  {
    digitalWrite(g_led, HIGH);
    digitalWrite(r_led, HIGH);

    digitalWrite(int1, LOW);
    move(data_p, time_p, B01010000); // move the car in forward direction
    digitalWrite(int1, HIGH);
  }

  if (val <= 15 ) //if the distance is less than 15 cm, the car stops with red led on, indicating it's dangerous to drive forward, then it finds a new direction to drive
  {
    digitalWrite(g_led, LOW);
    digitalWrite(r_led, HIGH);

    digitalWrite(int1, LOW);
    move(data_p, time_p, B00000000); // stop the car
    digitalWrite(int1, HIGH);
    delay(1500);
    digitalWrite(int1, LOW);
    move(data_p, time_p, B00101000); // first the car backs up
    digitalWrite(int1, HIGH);
    delay(1500);
    digitalWrite(int1, LOW);
    move(data_p, time_p, B00000000); // then again stop
    digitalWrite(int1, HIGH);
    delay(1500);

    drive(); // call drive function, finds a new direction and drive
    delay(1000);
  }
  servo.write(90); // reset servo at 90 degree angle for next detection on the front
}

//create a function to either move the car in a direction or stop the car if needed
void move(int dataPin, int clockPin, byte value)
{
  shiftOut(dataPin, clockPin, LSBFIRST, value);
}

// create a function to return the distance from the obstacle
double measure()
{
  digitalWrite(sensor_p, LOW);
  delayMicroseconds(15);
  digitalWrite(sensor_p, HIGH);
  delayMicroseconds(20);
  digitalWrite(sensor_p, LOW);

  duration = pulseIn(sound_p, HIGH); //reads how long it takes the high signal(sound) to come back
  distance = duration * 343 * 100 / 1000000 / 2; // distance is given back in cm
  return distance;
}

// create a function to move the car in the right direction when don't know where to go, use recursion to determine direction
void drive()
{
  double x;
  double y;
  servo.write(180); // set the servo in 180 degree, measures left side
  delay(1000);
  y = measure();
  servo.write(0); // set the servo in 0 degree, measures right side
  delay(1000);
  x = measure();

  // base case one
  if (x < y) //if the left side has more space, move to the left
  {
    digitalWrite(r_led, LOW);
    digitalWrite(g_led, LOW);

    digitalWrite(int1, LOW);
    move(data_p, time_p, B01000000); // move to the left side
    digitalWrite(int1, HIGH);
  }
  
  // base case two
  else if (x > y) //if the right side has more space, move to the right
  {
    digitalWrite(r_led, LOW);
    digitalWrite(g_led, LOW);

    digitalWrite(int1, LOW);
    move(data_p, time_p, B00010000); // move to the right side
    digitalWrite(int1, HIGH);
  }

  else //if both sides have the same distance, back up and further determine where to go
  {
    digitalWrite(r_led, LOW);
    digitalWrite(g_led, LOW);

    digitalWrite(int1, LOW);
    move(data_p, time_p, B00101000); // first backup the car
    digitalWrite(int1, HIGH);
    delay(3000); // wait for 3 seconds
    drive(); // use recursion to determine where to go after the back up, eventually it will reach one of the base cases
    delay(2000);
  }
}
