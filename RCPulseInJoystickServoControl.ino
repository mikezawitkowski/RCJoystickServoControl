/*
 RC PulseIn Joystick Servo Control
 by Mike Zawitkowski
 created on March 19, 2016
 
 Enables an Arduino based robot car to be controlled directly iva
 Frsky Taranis X9D Transmitter and X8R receiver.


 Attribution for the Sparkfun code:
  "RC Hobby Controllers and Arduino" by NPoole, May 22, 2012
   Retrieved on 3/19/2016 from: https://www.sparkfun.com/tutorials/348
   
   RC PulseIn Serial Read out
   By: Nick Poole
   SparkFun Electronics
   Date: 5
   License: CC-BY SA 3.0 - Creative commons share-alike 3.0
   use this code however you'd like, just keep this license and
   attribute. Let me know if you make hugely, awesome, great changes.

 CHANGES:
   3/19/2016 
   - Incorporated code into loop that was uncommented in tutorial
   - Swapped ch2/ch3 pins to correct incorrect mapping
   - changed serial monitor print behavior
   - Adapted to work with L293D motor controller
   
 TODO LIST:
  - Add the Taranis TX configuration 
  - Include info on wiring with X8R and breadboard
  - Cross-link with photos to an Instructables.com post
  - Incorporate autonomous mode
  
 */

int ch1; // Here's where we'll keep our channel values
int ch2;
int ch3;

int move; // Forward/Back speed
int turn; // Turning Factor

int pwm_a = 3;  //PWM control for motor outputs 
int pwm_b = 11;  //PWM control for motor outputs
int dir_a = 12;  //direction control for motor outputs
int dir_b = 13;  //direction control for motor outputs

void setup() {

  pinMode(5, INPUT); // Set our input pins as such
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  
  Serial.begin(9600); // Pour a bowl of Serial (for debugging)

  pinMode(pwm_a, OUTPUT);  //Set control pins to be outputs
  pinMode(pwm_b, OUTPUT);
  pinMode(dir_a, OUTPUT);
  pinMode(dir_b, OUTPUT);

  analogWrite(pwm_a, 0);  
  analogWrite(pwm_b, 0);
}

// Easier to comment out serial out content if it's a single function
void printReport() {
  if (ch1>1000) {
    Serial.println("Left Switch: Engaged");
  }
  if (ch1<1000) {
    Serial.println("Left Switch: Disengaged");
  }

  Serial.print("Right Stick X:");
  Serial.println(map(ch3, 1000,2000,-500,500));
  
  Serial.print("Right Stick Y:");
  Serial.println(map(ch2, 1000,2000,-500,500));

  delay(1000);  // The delay is great for monitoring STDOUT; but awful for real motor behavior 
}


void loop() {
  // Read pulse width of each channel
  ch1 = pulseIn(5, HIGH, 25000); // Left stick (engaged/disengaged)
  ch2 = pulseIn(7, HIGH, 25000); // Right stick up/down NOTE the swapped pins
  ch3 = pulseIn(6, HIGH, 25000); // Right stick left/right NOTE the swapped pins
  
  printReport();
  
  move = map(ch2, 1000,2000, -500, 500); //center over zero
  move = constrain(move, -255, 255); //only pass values whose absolutes are
                                     //valid pwm values
  
  /*What we're doing here is determining whether we want to move
  forward or backward*/
  if (move>0) {
    digitalWrite(dir_a, 1);
    digitalWrite(dir_b, 1);
  };
  
  if (move<0) {
    digitalWrite(dir_a, 0);
    digitalWrite(dir_b, 0); 
    move=abs(move);
  };
  
  /*Here we're determining whether a left or a right turn is being 
  executed*/
  turn = map(ch1,1000,2000,-500,500);
  turn = constrain(turn, -255, 255);
  
  /*This is where we do some mixing, by subtracting our "turn" 
  variable from the appropriate motor's speed we can execute
  a turn in either direction*/
  if (turn>0) {
    analogWrite(pwm_b, move-turn); 
    analogWrite(pwm_a, move);
  };
  if (turn<0) {
    turn=abs(turn); 
    analogWrite(pwm_a, move-turn); 
    analogWrite(pwm_b, move);
  };
    
  Serial.print("move:");
  Serial.print(move); //Serial debugging stuff
  
  Serial.print("\t\tturn:"); //Serial debugging stuff
  Serial.print(turn);
  
  Serial.print("\t\tmove-turn:"); //Serial debugging stuff
  Serial.print(move-turn);
  Serial.print("\n\n\n"); //Serial debugging stuff

}


