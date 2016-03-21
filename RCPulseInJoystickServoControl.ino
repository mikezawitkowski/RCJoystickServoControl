/*
RC PulseIn Joystick Servo Control
by Mike Zawitkowski
created on March 19, 2016
 
Enables an Arduino based robot car to be controlled directly iva
Frsky Taranis X9D Transmitter and X8R receiver.


CHANGES:
  3/19/2016 
    - Incorporated code into loop that was uncommented in tutorial
    - Swapped ch2/ch3 pins to correct incorrect mapping
    - changed serial monitor print behavior
    - Adapted to work with L293D motor controller [WORK IN PROGRESS]
   
  3/20/2016
    - Adapted to work with L293D motor controller [WORK IN PROGRESS]
    - Wiring both motor Enable triggers to a single PWM pin Out   
    - Changed move, turn variable names to avoid namespace conflict (move already exists)
    - Fixed error in channel mapping 
    - Added additional serial print statements for debugging
      
  TODO LIST:
    - Replace pulseIn with interrupts
    - Add the Taranis TX configuration info 
    - Include info on wiring with X8R and breadboard (fritzing?)
    - Cross-link with photos to an Instructables.com post
    - Incorporate autonomous mode
    - Run all channels into Arduino on a single wire

 CREDITS, THANKS, FURTHER READING:

  Attribution for the Sparkfun code, which served as a starting point:
    "RC Hobby Controllers and Arduino" by NPoole, May 22, 2012
    Retrieved on 3/19/2016 from: https://www.sparkfun.com/tutorials/348
   
    RC PulseIn Serial Read out
    By: Nick Poole
    SparkFun Electronics
    Date: 5
    License: CC-BY SA 3.0 - Creative commons share-alike 3.0
    use this code however you'd like, just keep this license and
    attribute. Let me know if you make hugely, awesome, great changes.


  To learn more about the L293D IC Dual H-bridge motor driver, check out:
    http://www.robotplatform.com/howto/L293/motor_driver_1.html
 

  I found this article very helpful to see the best approach options for PWM control:
    http://rcarduino.blogspot.com/2012/01/how-to-read-rc-receiver-with.html 
    
    
  Great stacikexchange post with lots of links, and discussion about getting all RX signals into a single input:
    http://robotics.stackexchange.com/questions/1207/read-multiple-channels-of-rx-tx-with-arduino
    


    http://blog.whatgeek.com.pt/2015/03/arduino-l293d-dc-motors-control/    

*/
 

// Channels are the RX in to the Arduino. These will be PWM pins
int ch1; // Left stick is on/off
int ch2; // Forward/Backward
int ch3; // Left/Right

int moveVal; // Forward/Back speed (formerly move but that appears to be already in the namespace)
int turnVal; // Turning Factor

// The L293D IC controls 2 motors, and for each has 2 control pins and 1 enable pin
// The enable pin will be referred to as pwm_a and pwm_b
int pwm_a = 3;  //PWM control for motor outputs 
int pwm_b = 5;  //PWM control for motor outputs

// direction control for motor outputs, non-PWM
int dir_a1 = 7;
int dir_a2 = 8;
int dir_b1 = 12;
int dir_b2 = 13;

void setup() {

  pinMode(9, INPUT); // Set our channel input pins as such
  pinMode(10, INPUT);
  pinMode(11, INPUT);
  
  Serial.begin(9600); // Pour a bowl of Serial (for debugging)

  pinMode(pwm_a, OUTPUT);  //Set control pins to be outputs
  pinMode(pwm_b, OUTPUT);

  pinMode(dir_a1, OUTPUT);
  pinMode(dir_a2, OUTPUT);
  pinMode(dir_b1, OUTPUT);
  pinMode(dir_b2, OUTPUT);

  analogWrite(pwm_a, 0);  
  analogWrite(pwm_b, 0);
}


void loop() {
  // Read pulse width of each channel
  ch1 = pulseIn(9, HIGH, 25000); // Left stick (engaged/disengaged)
  ch2 = pulseIn(11, HIGH, 25000); // Right stick up/down NOTE the swapped pins
  ch3 = pulseIn(10, HIGH, 25000); // Right stick left/right NOTE the swapped pins
  
  moveVal = map(ch2, 1000, 2000, -1000, 1000); //center over zero
  moveVal = constrain(moveVal, -255, 255);   //only pass values whose absolutes are
                                             //valid pwm values
  // Only run the loop when ch1 is Engaged
  if (ch1 > 1000) {
    if (moveVal > 0) {          // if moveVal is positive
//      Serial.println("Moving Forward");
      moveForward();  // then we move forward
    };  
    if (moveVal < 0) {          // if moveVal is negative
//      Serial.println("Moving Backward");
      moveBackward();          // we move backward
      moveVal = abs(moveVal);   // but now switch it to a positive int for the next step
    };

    /*Here we're determining whether a left or a right turn is being 
    executed*/
    turnVal = map(ch3,1000,2000,-500,500);
    turnVal = constrain(turnVal, -255, 255);

    /*This is where we do some mixing, by subtracting our "turn" 
    variable from the appropriate motor's speed we can execute
    a turn in either direction*/
    if (turnVal > 0) {
      analogWrite(pwm_b, moveVal - turnVal); 
      analogWrite(pwm_a, moveVal);
    };
    if (turnVal < 0) {
      turnVal = abs(turnVal); 
      analogWrite(pwm_a, moveVal - turnVal); 
      analogWrite(pwm_b, moveVal);
    };
  } else {
    fullStop();
  };
//  printReport();
                                                                                                                                                                                                                                                                                                                                                                                      
}

void moveForward() {
    digitalWrite(dir_a1, 1);   
    digitalWrite(dir_a2, 0);   
    
    digitalWrite(dir_b1, 1);   
    digitalWrite(dir_b2, 0);   
}

void moveBackward() {
    digitalWrite(dir_a1, 0);   
    digitalWrite(dir_a2, 1);   
    
    digitalWrite(dir_b1, 0);   
    digitalWrite(dir_b2, 1);   
}


void fullStop() {
    analogWrite(pwm_a, 0);
    analogWrite(pwm_b, 0);
    digitalWrite(dir_a1, 0);   
    digitalWrite(dir_a2, 0);     
    digitalWrite(dir_b1, 0);   
    digitalWrite(dir_b2, 0);   
}

// Easier to comment out serial out content if it's a single function
void printReport() {
  if (ch1 > 1000) {
    Serial.println("Left Switch: Engaged");
  }
  if (ch1 < 1000) {
    Serial.println("Left Switch: Disengaged");
  }

  Serial.print("Right Stick X:");
  Serial.println(map(ch3, 1000,2000,-500,500));
  
  Serial.print("Right Stick Y:");
  Serial.println(map(ch2, 1000,2000,-500,500));

//  Serial.print("\t\tturnVal:"); //Serial debugging stuff
//  Serial.print(turnVal);
//  
//  Serial.print("\t\tmoveVal - turnVal:"); //Serial debugging stuff
//  Serial.print(moveVal - turnVal);
//  Serial.print("\n\n\n"); //Serial debugging stuff
  
  delay(800);  // The delay is great for monitoring STDOUT; but awful for real motor behavior 
}



//void setSpeed(int speedValue){
//  analogWrite(E1, speedValue);
//  analogWrite(E2, speedValue);
//}

