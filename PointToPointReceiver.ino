#include <LiquidCrystal_I2C.h> 

LiquidCrystal_I2C lcd(0x20,16,2);


int Detector = A3;
int blueLED = 12;
int yellowLED = 13;

int dotTime = 10;
int dashTime = 20;
int pauseTime = 20;
int charSwitchTime = 30;

String message = "";
String messageOld = "";
int change = 1;

int count = 0;
void setup ()
{
  lcd.init();
  Serial.begin(9600);
  //pinMode(Laser, OUTPUT); // define the digital output interface 12 feet
  pinMode(Detector, INPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
}
void loop () {
  lcd.backlight();
  if(message == "" && change == 1)
  {
    lcd.setCursor(0, 0);
    lcd.print("Waiting for a");
    lcd.setCursor(0, 1);
    lcd.print("message...");
    change = 0;
  }
  else if(message != "" && change == 1)
  {
    if(message != messageOld)
    {
      messageOld = message;
      lcd.clear();
    }
    if(message.length() <= 16)
    {
      lcd.setCursor(0, 0);
      lcd.print(message);
    }
    else
    {
      lcd.setCursor(0, 0);
      lcd.print(message.substring(0, 16));
      lcd.setCursor(0, 1);
      lcd.print(message.substring(16));
    }
    change = 0;
  }

  digitalWrite(blueLED, LOW);
  digitalWrite(yellowLED, LOW);
  
 //digitalWrite(Laser, HIGH); // open the laser head
 boolean val = digitalRead(Detector);
 //Serial.println(val);
 count = 0;
 if(val == 1)
 {
   digitalWrite(blueLED, HIGH);
   //Serial.println("begin");
 }
 while (val == 1)
 {
   count++;
   val = digitalRead(Detector);
   //Serial.println("Still going");
   delay(1);
 }
 if (count != 0)
 {
    digitalWrite(blueLED, LOW);
    Serial.println(count);

    if (count >= 490 && count <= 515)
    {
      Serial.println("Level 1 Achieved.");
      digitalWrite(yellowLED, HIGH);
      if(checkIndicSignal())
      {
        Serial.println("HIP HIP HOORAY!!!");
        String morse = receiveMorse();
        if(morse == "ERROR")
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Error receiving");
          lcd.setCursor(0, 1);
          lcd.print("last message.");
          for(int i = 0; i < 6; i++)
          {
            digitalWrite(yellowLED, HIGH);
            delay(500);
            digitalWrite(yellowLED, LOW);
            delay(500);
          }
        }
        else
        {
          digitalWrite(yellowLED, HIGH);
          Serial.println("Received morse code: " + morse);
          message = morseToStr(morse + "/");
          Serial.println(message);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Message has been");
          lcd.setCursor(0, 1);
          lcd.print("received!");
          delay(2000);
          lcd.clear();
          change = 1;       
        }
      }
    }
 }
}

bool checkIndicSignal()
{
  boolean val = digitalRead(Detector);
  count = 0;
  if(val == 0)
  {
    digitalWrite(blueLED, LOW);
    while(val == 0 && count < 516)
    {
      count++;
      val = digitalRead(Detector);
      delay(1);
    }
    //Serial.print("Delay: ");
    //Serial.println(count);
    if(count >= 490 && count <= 516)
    {
      Serial.println("Level 2 Achieved.");
      count = 0;
      digitalWrite(blueLED, HIGH);
      while(val == 1 && count < 516)
      {
        count++;
        val = digitalRead(Detector);
        delay(1);
      }
      if(count >= 490 && count <= 515)
      {
        Serial.println("Level 3 Achieved.");
        count = 0;
        digitalWrite(blueLED, LOW);
        while(val == 0 && count < 516)
        {
          count++;
          val = digitalRead(Detector);
          delay(1);
        }
        if(count >= 490 && count <= 515)
        {
          Serial.println("Level 4 Achieved.");
          count = 0;
          digitalWrite(blueLED, HIGH);
          while(val == 1 && count < 516)
          {
            count++;
            val = digitalRead(Detector);
            delay(1);
          }
          if(count >= 490 && count <= 515)
          {
            Serial.println("Level 5 Achieved.");
            count = 0;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Receiving a new");
            lcd.setCursor(0, 1);
            lcd.print("message.");
            digitalWrite(blueLED, LOW);
            while(val == 0 && count < 516)
            {
              count++;
              val = digitalRead(Detector);
              delay(1);
            }
            if(count >= 400 && count <= 515)
            {
              count = 0;
              Serial.println("INDICATOR RECEIVED");
              digitalWrite(blueLED, HIGH);              
              return true;
            }
          }
        }        
      }
    }
    else
    {
      //Serial.println("Level 1 failed. Count: " + count);
    }
  }
  count = 0;
  Serial.println("indicator FAILURE!!!!!");
  return false;
}

String receiveMorse()
{
  String morse = "";
  boolean val = digitalRead(Detector);
  count = 0;
  bool indicatorExit = false;
  bool error = false;
  while(error != true && indicatorExit != true)
  {
    count = 0;
    digitalWrite(blueLED, HIGH);
    while(val == 1 && count <= 516)
    {
      count++;
      val = digitalRead(Detector);
      delay(1);    
    }
    digitalWrite(blueLED, LOW);
    Serial.print("ON DELAY: ");
    Serial.println(count);
    if(count >= dotTime - 5 && count <= dotTime + 5)
    {
      morse = morse + ".";
    }
    else if(count >= dashTime - 5 && count <= dashTime + 5)
    {
      morse = morse + "-";
    }
    else if(count >= 490 && count <= 515)
    {
      indicatorExit = true;
    }
    else
    {
      Serial.print("ERROR because of count ");
      Serial.println(count);
      error = true;
    }

    if(indicatorExit == true)
    {
      return morse;
    }

    if(indicatorExit != true && error != true)
    {
      count = 0;      
      while(val == 0 && count <= 516)
      {
        count++;
        val = digitalRead(Detector);
        delay(1);
      }
      Serial.print("OFF DELAY: ");
      Serial.println(count);
      if(count >= pauseTime - 5 && count <= pauseTime + 5)
      {
        morse = morse + "";
      }
      else if(count >= charSwitchTime - 5 && count <= charSwitchTime + 5)
      {
        morse = morse + "/";
      }
      else
      {
        error = true;
      }
    }

    if(error == true)
    {
      return "ERROR";
    }
  }
}

String morseToStr(String morse)
{
  String msg = "";
  String morseChar = "";
  int slashFreq = 0;
  int index [32];  
  for(int i = 0; i < morse.length(); i++)
  {
    if(morse.substring(i, i + 1) == "/")
    {
      String newChar = morseToChar(morseChar);
      if(newChar.length() != 1)
      {
        return newChar;
      }
      else
      {
        msg = msg + newChar;        
        morseChar = "";
      }
    }
    else
    {
      morseChar = morseChar + morse.substring(i, i + 1);
    }
  }
  
  return msg;
}

String morseToChar(String morse)
{
  if(morse == ".-")
  {
    return("A");
  }
  else if(morse == "-...")
  {
    return("B");
  }
  else if(morse == "-.-.")
  {
    return("C");
  }
  else if(morse == "-..")
  {
    return("D");
  }
  else if(morse == ".")
  {
    return("E");
  }
  else if(morse == "..-.")
  {
    return("F");
  }
  else if(morse == "--.")
  {
    return("G");
  }
  else if(morse == "....")
  {
    return("H");
  }
  else if(morse == "..")
  {
    return("I");
  }
  else if(morse == ".---")
  {
    return("J");
  }
  else if(morse == "-.-")
  {
    return("K");
  }
  else if(morse == ".-..")
  {
    return("L");
  }
  else if(morse == "--")
  {
    return("M");
  }
  else if(morse == "-.")
  {
    return("N");
  }
  else if(morse == "---")
  {
    return("O");
  }
  else if(morse == ".--.")
  {
    return("P");
  }
  else if(morse == "--.-")
  {
    return("Q");
  }
  else if(morse == ".-.")
  {
    return("R");
  }
  else if(morse == "...")
  {
    return("S");
  }
  else if(morse == "-")
  {
    return("T");
  }
  else if(morse == "..-")
  {
    return("U");
  }
  else if(morse == "...-")
  {
    return("V");
  }
  else if(morse == ".--")
  {
    return("W");
  }
  else if(morse == "-..-")
  {
    return("X");
  }
  else if(morse == "-.--")
  {
    return("Y");
  }
  else if(morse == "--..")
  {
    return("Z");
  }
  else if(morse == ".----")
  {
    return("1");
  }
  else if(morse == "..---")
  {
    return("2");
  }
  else if(morse == "...--")
  {
    return("3");
  }
  else if(morse == "....-")
  {
    return("4");
  }
  else if(morse == ".....")
  {
    return("5");
  }
  else if(morse == "-....")
  {
    return("6");
  }
  else if(morse == "--...")
  {
    return("7");
  }
  else if(morse == "---..")
  {
    return("8");
  }
  else if(morse == "----.")
  {
    return("9");
  }
  else if(morse == "-----")
  {
    return("0");
  }
  else if(morse == ".-.-")
  {
    return(" ");
  }
  else
  {
    return("ERROR: COULD NOT DECIPHER CODE: " + morse);
  }
}