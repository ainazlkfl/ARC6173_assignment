
// Programme by: ainamyzlkfl
// Edited on 14/12/2024
// Description: Project for a smart waste system management.

#define halftrig 16 //ultrasonic sensor for half-bin checking
#define halfecho 17
#define fulltrig 18 //ultrasonic sensor for full-bin checking
#define fullecho 19
#define bLedPin 21 //empty bin indicator
#define yLedPin 22 // half bin indicator
#define rLedPin 23 // full bin indicator

#define SOUND_SPEED 0.034

long duration1, duration2;
float distance1, distance2;

void setup()
{
  pinMode(halftrig, OUTPUT);
  pinMode(halfecho, INPUT);
  pinMode(fulltrig, OUTPUT);
  pinMode(fullecho, INPUT);
  pinMode(bLedPin, OUTPUT);
  pinMode(yLedPin, OUTPUT);
  pinMode(rLedPin, OUTPUT);
  Serial.begin(115200);
}

void loop()
{
	// read sensors input
	duration1 = pulseIn(halfecho, HIGH);
	duration2 = pulseIn(fullecho, HIGH);
	  
  // Calculate the distance
  distance1 = duration1 * SOUND_SPEED;
  distance2 = duration2 * SOUND_SPEED;
   
	// Blue LED on means bin is empty
	if(distance1 >= 300 && distance2 >= 300)
	{
		digitalWrite(bLedPin, HIGH);
		digitalWrite(rLedPin, LOW);
		digitalWrite(rLedPin, LOW);
	}
	// Yellow LED on means half full
	else if((distance1 <= 300 && distance2 >= 300)
	{
		digitalWrite(bLedPin, LOW);
		digitalWrite(rLedPin, HIGH);
		digitalWrite(rLedPin, LOW);
	}
	// Red LED on means bin is full
	else if((distance1 <= 300 && distance2 <= 300)
	{
		digitalWrite(bLedPin, LOW);
		digitalWrite(rLedPin, LOW);
		digitalWrite(rLedPin, HIGH);
	}
	delay(500);
}

// Note: 
// Need to connect to wifi
// Connect with blynk 
// Coding for control with blynk app