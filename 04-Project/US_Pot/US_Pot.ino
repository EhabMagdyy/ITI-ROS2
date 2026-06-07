// Pin Definitions
const int trigPin = 9;
const int echoPin = 10;
const int potPin = A0;

// Variables
long duration;
int distance;
int potValue;

void setup() {
  // Ultrasonic pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Serial communication at 9600 baud
  Serial.begin(9600);
}

void loop() {
  // 1. Read Potentiometer (0 to 1023)
  potValue = analogRead(potPin);

  // 2. Trigger Ultrasonic Sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // 3. Calculate Distance
  duration = pulseIn(echoPin, HIGH);
  // Speed of sound is ~340m/s. Distance = (time * speed) / 2
  distance = duration * 0.034 / 2;

  // 4. Output to Serial Monitor
  Serial.print(potValue);
  Serial.print(" | ");
  Serial.print(distance);
  Serial.println();

  delay(100);
}