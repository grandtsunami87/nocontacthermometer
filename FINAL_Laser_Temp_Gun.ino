//MN Maker
//Laser Temp Gun
//10.6.19

#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

float roomTemp; // ambient temperature
float objectTemp, stemp; // object temperature

int readcount = 0;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define echoPin 11 // Echo Pin
#define trigPin 10 // Trigger Pin

const int Laser_Pin=5;  //Laser Pin
int buttonState = 0; 
const int buttonPin = 2;     // the number of the pushbutton pin

// --------- Ultrasonic Sensor preparation
int maximumRange = 20; // Maximum range needed
int minimumRange = 10; // Minimum range needed
long duration, distance; // Duration used to calculate distance
int dtime;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
//Adafruit_SSD1306 display(-1);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
  Serial.println("Adafruit MLX90614 test"); 
  
  pinMode(Laser_Pin,OUTPUT);
  pinMode(buttonPin, INPUT);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  

  display.clearDisplay();
  display.setRotation(2);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20, 35);
  display.println("Initializing Temp");
  display.display();
  delay(1850);
  display.clearDisplay();

  mlx.begin(); 
  
}

void loop() {
  
  buttonState = digitalRead(buttonPin);
  Serial.println(buttonState);
  
  Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempC()); 
  Serial.print("*C\tObject = "); Serial.print(mlx.readObjectTempC()); Serial.println("*C");
  Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempF()); 
  Serial.print("*F\tObject = "); Serial.print(mlx.readObjectTempF()); Serial.println("*F");
  
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // turn LED on:
    digitalWrite(Laser_Pin, HIGH);
    
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    duration = pulseIn(echoPin, HIGH);
  //Calculate the distance (in cm) based on the speed of sound.
  distance= duration*0.034/2;

  // reading object and ambient  temperature
  objectTemp = 5.2 + mlx.readObjectTempC() ;  
  roomTemp = mlx.readAmbientTempC() ;  

  // print to Serial port
 Serial.println("Object:" + String(objectTemp) + ", Ambient:" + String(roomTemp));
 Serial.println(distance);
 
  // display on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,25);
  display.print("Dis:" + String(distance) + "cm");
  display.setCursor(65,25);
  display.print("Room:" + String(roomTemp).substring(0, 4)  + "C");
  display.display();
  display.setTextSize(2);
 display.setCursor(0, 0);
 if (distance > maximumRange) {
    display.print("GET CLOSER");
  }
  if (distance < minimumRange) {
    display.print("TOO CLOSE!");
  }
  if ((distance >= minimumRange) && (distance <= maximumRange)) 
   {
    if (readcount == 5) {   // after reading 5 consecutive time
      disptemp();
      
    } else {
      display.print("HOLD ON"); // when in range, ask user to hold position
      stemp = stemp + objectTemp;
      readcount++;
      dtime = 200;      // until approx. 5 x 200 ms = 1 sec
    }
  } else {     // if user is out of range, reset calculation
    dtime = 100;
    readcount = 0;
    stemp = 0;
  }
  display.display();
  delay(dtime);
  Serial.println("count  :"+String(readcount));
    
    
  } else {
    // turn LED off:
    digitalWrite(Laser_Pin, LOW);
  
    display.clearDisplay();
    display.setTextSize(2);  //Size 2 means each pixel is 12 width and 16 high
    display.setCursor(35, 10);
    display.print("-----");
    display.setCursor(105, 10);
    display.print("");
    display.setTextSize(2);
    display.setCursor(35, 36);
    display.print("-----");
    display.setCursor(105, 36);
    display.print("");
    display.display();
    
    
  }

  Serial.println();
  delay(500);
}

void disptemp()
 {
      objectTemp = stemp / 5;       // get the average reading of temp
      display.setTextSize(1);
      display.print("YOUR TEMP:");
      display.setTextSize(2);
      display.setCursor(60,5);
      display.print(String(objectTemp).substring(0, 4) + "C");
      display.display();
      readcount = 0;
      stemp = 0;
      if (objectTemp >= 37.5) {
        play_alert();
      } else {
        play_ok();
      }
      dtime = 5000; // wait for 5 seconds
 }

void play_ok() {  // play three sequential notes when object temperature is below 37.5C
  tone(3, 600, 1000);  // pin,frequency,duration
  delay(200);
  tone(3, 750, 500);
  delay(100);
  tone(3, 1000, 500);
  delay(200);
  noTone(3);
}

void play_alert() { // beep 3x when object temperature is >= 37.5C
  tone(3, 2000, 1000);
  delay(1000);
  tone(3, 3000, 1000);
  delay(1000);
  tone(3, 4000, 1000);
  delay(1000);
  noTone(3);
}
