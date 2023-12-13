#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <ezButton.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x20,16,2);

ezButton toggleSwitch(A1);

int laserPin = A0;
int laserAlign = 0;
String message = "";
String messageBuff = "";
String inputBuff = "";

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'},
};

byte colPins[COLS] = {9,8,7,6};
byte rowPins[ROWS] = {5,4,2,3};

Keypad keypad = Keypad(makeKeymap(keys), colPins, rowPins, ROWS, COLS);


int dotTime = 10;
int dashTime = 20;
int pauseTime = 20;
int charSwitchTime = 10;

bool ready2Send = false;

//Switch setup:
// constants won't change
const int BUTTON_PIN = 12; // Arduino pin connected to button's pin
const int PRESSOR_PIN = 13; //Arduino pin connected to second button's pin
const int SWITCH_PIN = A1;
const int SERVO1_PIN  = 10; // Arduino pin connected to servo motor's pin
const int SERVO2_PIN = 11;

Servo servo1; // create servo object to control a servo
Servo servo2;

int angle1 = 0;          // the current angle of servo motor
int angle2 = 0;
int lastButtonState;    // the previous state of button
int currentButtonState = 0; // the current state of button
int currentButton2State = 0; // reset button 2 current state
int currentSwitchState = 0;

int c = 0;

void setup() {
  lcd.init();
  Serial.begin(9600);
  pinMode(laserPin, OUTPUT);

  toggleSwitch.setDebounceTime(50);

  //Servo:
  pinMode(PRESSOR_PIN, INPUT_PULLUP); // set arduino pin to input pull-up mode
  pinMode(BUTTON_PIN, INPUT_PULLUP); // set arduino pin to input pull-up mode
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  //servo1.attach(SERVO1_PIN);           // attaches the servo on pin 9 to the servo object
  //servo2.attach(SERVO2_PIN);

  //servo1.write(angle1);
  //servo2.write(angle2);
  currentButtonState = digitalRead(BUTTON_PIN);
  currentButton2State = digitalRead(PRESSOR_PIN);
  currentSwitchState = digitalRead(SWITCH_PIN);

  lcd.backlight();
  lcd.print("Hello!");
  delay(2000);
  // servo2.attach(SERVO2_PIN);
  // servo2.write(94);
  // delay(100);
  // servo2.write(86);
  // delay(100);
  // servo2.detach();
  // delay(800);
}

void loop() 
{
  //Transmission:
  lcd.backlight();
  if(laserAlign == 0)
  {
    lcd.setCursor(0, 0);
    lcd.print("Press A to Align");
    lcd.setCursor(0, 1);
    lcd.print(" Press B to Msg");
  }
  
  char key = keypad.getKey();

  if(key){
    Serial.println(key);

    if(key == 'A'){
      laserAlign = 1;
      digitalWrite(laserPin, HIGH);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" Aligning Laser");
      lcd.setCursor(0, 1);
      lcd.print("Press A to Leave");
      key = ' ';
      while(laserAlign == 1)
      {
        toggleSwitch.loop();

        lastButtonState = currentButtonState;
        currentButtonState = digitalRead(BUTTON_PIN);
        currentButton2State = digitalRead(PRESSOR_PIN);
        currentSwitchState = digitalRead(SWITCH_PIN);

        if(currentButtonState == HIGH) 
        {
          Serial.println("The button is pressed");//right button
          if(toggleSwitch.getState() == LOW)
          {
            Serial.println("The switch is off");
            servo1.attach(SERVO1_PIN);
            servo2.detach();
            servo1.write(86);
          }
          else//VERTICAL
          {
            servo1.detach();
            servo2.attach(SERVO2_PIN);
            servo2.write(89 - 1);//c);
            delay(25);
            servo2.detach();
            delay(25);
          }      
        }
        else if(currentButton2State == HIGH)//left button
        {
          Serial.println("The button2 is pressed");
          if(toggleSwitch.getState() == LOW)
          {
            servo1.attach(SERVO1_PIN);
            servo2.detach();
            servo1.write(93);
          }
          else//VERTICAL
          {
            servo1.detach();
            servo2.attach(SERVO2_PIN);
            servo2.write(91 + 0);//c);
            delay(25);
            servo2.detach();
            delay(25);
          }
          //angle1=88;
          //angle2=88;
        }
        else
        {
          Serial.println("No buttons pressed");
          //angle1=90;
          //angle2=90;
          servo1.detach();
          servo2.detach();
        }
        if(key == '7')
        {
          if(c == 0)
          {
            c = 1;
          }
          else
          {
            c = 0;
          }
        }
        if(key == '#')
        {
          servo1.detach();
          servo2.detach();

          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(" Laser aligned?");
          lcd.setCursor(0, 1);
          lcd.print("* = no. # = yes.");
          
          key = ' ';

          while(key != '*' && key != '#')
          {
            key = keypad.getKey();
          }
          if(key == '#')
          {
            laserAlign = 0;
          }
        }
        key = keypad.getKey();
        Serial.println(key);
        if(key == 'A')
        {
          digitalWrite(laserPin, LOW);
          lcd.clear();
          laserAlign = 0;
        }
      }
    }

    if(key == 'B'){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Press Numbers to");
      lcd.setCursor(0, 1);
      lcd.print("Type Message.");
      laserAlign = 0;
      digitalWrite(laserPin, LOW);
      message = "";
      String character = "";
      while(!ready2Send){
        //Serial.println("while has been entered");
        if(key != 'D' && !isblank(key)){
          if(key == '*'){
            message = message + character;
            character = "";
            Serial.println("Message: " + message);
          }
          else if(key == 'A')
          {
            message = inputBuff;
            character = "";

            lcd.clear();
            lcd.setCursor(0, 0);
            if((message + character).length() <= 16){
              lcd.print(message + character);
            }
            else{
              lcd.print(message.substring(0,16));
              lcd.setCursor(0, 1);
              lcd.print(message.substring(16) + character);
            }
          }
          else if(key == 'B' && (message + character).length() != 0)
          {
            if(character.length() > 0)
            {
              character = "";
            }
            else
            {
              message = message.substring(0, message.length() - 1);
            }

            lcd.clear();
            lcd.setCursor(0, 0);
            if((message + character).length() <= 16){
              lcd.print(message + character);
            }
            else{
              lcd.print(message.substring(0,16));
              lcd.setCursor(0, 1);
              lcd.print(message.substring(16) + character);
            }
          }
          else if(key == 'C' && (message + character).length() != 0)
          {
            message = "";
            character = "";

            lcd.clear();
            lcd.setCursor(0, 0);
            if((message + character).length() <= 16){
              lcd.print(message + character);
            }
            else{
              lcd.print(message.substring(0,16));
              lcd.setCursor(0, 1);
              lcd.print(message.substring(16) + character);
            }
          }
          else if(key == '#')
          {
            if(message.length() + character.length() != 0)
            {
              if(character.length() == 1)
              {
                message = message + character;
              }
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Processing.");
              delay(250);
              lcd.print(".");
              delay(250);
              lcd.print(".");
              delay(250);
              ready2Send = true;
              // lcd.clear();
              // lcd.setCursor(0, 0);
              // lcd.print(" Ready to send?");
              // lcd.setCursor(0, 1);
              // lcd.print("* = no. # = yes.");

              // key = ' ';

              // while(key != '*' && key != '#')
              // {
              //   key = keypad.getKey();
              // }
              // if(key == '#')
              // {
              //   ready2Send = true;
              // }
              // else
              // {
              //   lcd.clear();
              //   lcd.setCursor(0, 0);
              //   if((message + character).length() <= 16){
              //     lcd.print(message + character);
              //   }
              //   else{
              //     lcd.print(message.substring(0,16));
              //     lcd.setCursor(0, 1);
              //     lcd.print(message.substring(16) + character);
              //   }
              // }
            }
          }
          else{
            if((message + character).length() <= 32)
            {
              character = getChar(key, character);
              //Serial.println("Char: " + character + "   Length: " + message.length());
            }
            else
            {
              character = "";
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.println("Exceeded maximum");
              lcd.setCursor(0, 1);
              lcd.println("of 32 chars.");
              delay(2000);
            }
          }
          
          //Serial.println("test");
        }
        lcd.setCursor(0, 0);
        if((message + character) != messageBuff)
        {
          if((message + character).length() <= 16){
            if((message + character).length() == 0)
            {
              lcd.print("Press numbers to");
              lcd.setCursor(0, 1);
              lcd.print("type message.");
            }
            else
            {
              lcd.clear();
              lcd.print(message + character);
            }
          }
          else{
            lcd.clear();
            lcd.print(message.substring(0,16));
            lcd.setCursor(0, 1);
            lcd.print(message.substring(16) + character);
          }

          messageBuff = message + character;
        }
        
        key = keypad.getKey();
      }
      inputBuff = message;
      ready2Send = false;
      Serial.println("Message: " + message);
      String morse = msg2Morse(message);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("The message is");
      lcd.setCursor(0, 1);
      lcd.print("being sent.");
      sendMorseMsg(morse);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("The message has");
      lcd.setCursor(0, 1);
      lcd.print("been sent.");
      delay(2000);

      // lcd.clear();
      // lcd.setCursor(0, 0);
      // lcd.print(" Send another?");
      // lcd.setCursor(0, 1);
      // lcd.print("* = no. # = yes.");

      // key = " ";

      // while(key != '*' && key != '#')
      // {
      //   key = keypad.getKey();
      // }
      // if(key == '*')
      // {
      //   lcd.clear();
      //   lcd.setCursor(0, 0);
      //   lcd.print("Thank you.");
      //   lcd.setCursor(0, 1);
      //   lcd.print("Please pwr off.");
      //   while(true)
      //   {

      //   }
      // }
    }
  }
}


String getChar(char num, String currentChar){
  if(num == '1'){
    currentChar = "1";
  }
  else if(num == '2'){
    if(currentChar == "A"){
      currentChar = "B";
    }
    else if(currentChar == "B"){
      currentChar = "C";
    }
    else if(currentChar == "C"){
      currentChar = "2";
    }
    else{
      currentChar = "A";
    }
  }
  else if(num == '3'){
    if(currentChar == "D"){
      currentChar = "E";
    }
    else if(currentChar == "E"){
      currentChar = "F";
    }
    else if(currentChar == "F"){
      currentChar = "3";
    }
    else{
      currentChar = "D";
    }
  }
  else if(num == '4'){
    if(currentChar == "G"){
      currentChar = "H";
    }
    else if(currentChar == "H"){
      currentChar = "I";
    }
    else if(currentChar == "I"){
      currentChar = "4";
    }
    else{
      currentChar = "G";
    }
  }
  else if(num == '5'){
    if(currentChar == "J"){
      currentChar = "K";
    }
    else if(currentChar == "K"){
      currentChar = "L";
    }
    else if(currentChar == "L"){
      currentChar = "5";
    }
    else{
      currentChar = "J";
    }
  }
  else if(num == '6'){
    if(currentChar == "M"){
      currentChar = "N";
    }
    else if(currentChar == "N"){
      currentChar = "O";
    }
    else if(currentChar == "O"){
      currentChar = "6";
    }
    else{
      currentChar = "M";
    }
  }
  else if(num == '7'){
    if(currentChar == "P"){
      currentChar = "Q";
    }
    else if(currentChar == "Q"){
      currentChar = "R";
    }
    else if(currentChar == "R"){
      currentChar = "S";
    }
    else if(currentChar == "S"){
      currentChar = "7";
    }
    else{
      currentChar = "P";
    }
  }
  else if(num == '8'){
    if(currentChar == "T"){
      currentChar = "U";
    }
    else if(currentChar == "U"){
      currentChar = "V";
    }
    else if(currentChar == "V"){
      currentChar = "8";
    }
    else{
      currentChar = "T";
    }
  }
  else if(num == '9'){
    if(currentChar == "W"){
      currentChar = "X";
    }
    else if(currentChar == "X"){
      currentChar = "Y";
    }
    else if(currentChar == "Y"){
      currentChar = "Z";
    }
    else if(currentChar == "Z"){
      currentChar = "9";
    }
    else{
      currentChar = "W";
    }
  }
  else if(num == '0'){
    if(currentChar == "_"){
      currentChar = "0";
    }
    else{
      currentChar = "_";
    }
  }

  return currentChar;
}

String msg2Morse(String msg)
{
  String morse = "";
  if(msg.length() == 0)
  {
    return "";
  }
  else
  {
    for(int i = 0; i < msg.length(); i++)
    {
      Serial.println("Getting conversion for letter " + msg.substring(i, i + 1));
      morse = morse + letter2Morse(msg.substring(i, i + 1)) + "/";
    }
  }
  if(msg.substring(0,1) == "C"){
    Serial.println("this is awkward...");
  }
  Serial.println("Morse translation: " + morse.substring(0, morse.length() - 1));
  return morse.substring(0, morse.length() - 1);
}

String letter2Morse(String letter)
{
  if(letter == "A")
  {
    return(".-");
  }
  else if(letter == "B")
  {
    return("-...");
  }
  else if(letter == "C")
  {
    return("-.-.");
  }
  else if(letter == "D")
  {
    return("-..");
  }
  else if(letter == "E")
  {
    return(".");
  }
  else if(letter == "F")
  {
    return("..-.");
  }
  else if(letter == "G")
  {
    return("--.");
  }
  else if(letter == "H")
  {
    return("....");
  }
  else if(letter == "I")
  {
    return("..");
  }
  else if(letter == "J")
  {
    return(".---");
  }
  else if(letter == "K")
  {
    return("-.-");
  }
  else if(letter == "L")
  {
    return(".-..");
  }
  else if(letter == "M")
  {
    return("--");
  }
  else if(letter == "N")
  {
    return("-.");
  }
  else if(letter == "O")
  {
    return("---");
  }
  else if(letter == "P")
  {
    return(".--.");
  }
  else if(letter == "Q")
  {
    return("--.-");
  }
  else if(letter == "R")
  {
    return(".-.");
  }
  else if(letter == "S")
  {
    return("...");
  }
  else if(letter == "T")
  {
    return("-");
  }
  else if(letter == "U")
  {
    return("..-");
  }
  else if(letter == "V")
  {
    return("...-");
  }
  else if(letter == "W")
  {
    return(".--");
  }
  else if(letter == "X")
  {
    return("-..-");
  }
  else if(letter == "Y")
  {
    return("-.--");
  }
  else if(letter == "Z")
  {
    return("--..");
  }
  else if(letter == "1")
  {
    return(".----");
  }
  else if(letter == "2")
  {
    return("..---");
  }
  else if(letter == "3")
  {
    return("...--");
  }
  else if(letter == "4")
  {
    return("....-");
  }
  else if(letter == "5")
  {
    return(".....");
  }
  else if(letter == "6")
  {
    return("-....");
  }
  else if(letter == "7")
  {
    return("--...");
  }
  else if(letter == "8")
  {
    return("---..");
  }
  else if(letter == "9")
  {
    return("----.");
  }
  else if(letter == "0")
  {
    return("-----");
  }
  else if(letter == "_")
  {
    return(".-.-");
  }
}

void sendMorseMsg(String morse)
{
  indicatorSequence();
  for(int i = 0; i < morse.length(); i++)
  {
    if(morse.substring(i, i + 1) == ".")
    {
      dot();
      delay(pauseTime);
    }
    else if(morse.substring(i, i + 1) == "-")
    {
      dash();
      delay(pauseTime);
    } 
    else if(morse.substring(i, i + 1) == "/")
    {
      charSwitch();
    }
  }
  digitalWrite(laserPin, HIGH);
  delay(500);
  digitalWrite(laserPin, LOW);
}

void dot()
{
  digitalWrite(laserPin, HIGH);
  delay(dotTime);
  digitalWrite(laserPin, LOW);
}

void dash()
{
  digitalWrite(laserPin, HIGH);
  delay(dashTime);
  digitalWrite(laserPin, LOW);
}

void pause()
{
  delay(pauseTime);
}

void charSwitch()
{
  delay(charSwitchTime);
}

void indicatorSequence()
{
  digitalWrite(laserPin, HIGH);
  delay(500);
  digitalWrite(laserPin, LOW);
  delay(500);
  digitalWrite(laserPin, HIGH);
  delay(500);
  digitalWrite(laserPin, LOW);
  delay(500);
  digitalWrite(laserPin, HIGH);
  delay(500);
  digitalWrite(laserPin, LOW);
  delay(500);
}