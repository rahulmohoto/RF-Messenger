#include <PS2Keyboard.h>
#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile
//data- = data
//data+ = clock
#include <LiquidCrystal.h>

const int DataPin = 1;
const int IRQpin =  2;
const int rs = 29, en = 28, d4 = 27, d5 = 26, d6 = 25, d7 = 24;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

PS2Keyboard keyboard;

RH_ASK driver;

void setup() {
  pinMode(4, INPUT_PULLUP);
  lcd.begin(16, 2);
  keyboard.begin(DataPin, IRQpin);
  lcd.print("Messenger v1.0");
  delay(2000);
  if (!driver.init())
         lcd.print("init failed");
}
char msg[17] ;
int staySending = 30;
unsigned long previousMillis=0;

void loop() {
  
  if(digitalRead(4)==LOW){
      gotMessage();
  }
  
  else{
  callForInput();
  lcd.clear();
  lcd.print("Sending Text");
  lcd.setCursor(0,1);
  unsigned long currentMillis = millis()/1000;
   while ((currentMillis - previousMillis) <= staySending) {
    sendMessage();
      //lcd.clear();
      //lcd.print(msg);
      //delay(10000);
      currentMillis = millis()/1000;    
   }
  previousMillis = currentMillis ;
}


}


void makeCursorBlink(int tm){
    lcd.cursor();
    delay(tm);
    lcd.noCursor();
    delay(tm);
  }

void sendMessage(){
  const char *char_msg = msg;

  driver.send((uint8_t *)char_msg, strlen(char_msg));
  driver.waitPacketSent();
  delay(200);
  }

void gotMessage(){
    lcd.clear();
    lcd.print("RECEIVED :");
    lcd.setCursor(0,1);
    uint8_t buf[16];
    uint8_t buflen = sizeof(buf);
    if (driver.recv(buf, &buflen)) // Non-blocking
    {
    lcd.print((char*)buf);
    delay(1000);
  // Message with a good checksum received, dump it.
  //driver.printBuffer("Got:", buf, buflen);
    }  
    delay(1000);  
 }

void callForInput(){
 lcd.clear();
 lcd.print("Input Message");
 lcd.setCursor(0, 1);
 int count=0;
 int currentPos=0;

 while(1){
 makeCursorBlink(150);
  
  if (keyboard.available()) {
    // read the next key
    char c = keyboard.read();   
    // check for some of the special keys
    if (c == PS2_ENTER) {
      break;
    } else if (c == PS2_TAB) {
      //do nothing
    } else if (c == PS2_ESC) {
      lcd.clear();
      lcd.print("Input Message");
      lcd.setCursor(0, 1);
      strcpy(msg," ");
      count=0;
      currentPos=0;
    } else if (c == PS2_PAGEDOWN) {
      //do nothing
    } else if (c == PS2_PAGEUP) {
      //do nothing
    } else if (c == PS2_LEFTARROW && currentPos!=0) {
      currentPos--;
      lcd.setCursor(currentPos,1);
      count=currentPos;
    } else if (c == PS2_RIGHTARROW && currentPos!=16) {
      currentPos++;
      lcd.setCursor(currentPos,1);
      count=currentPos;
    } else if (c == PS2_UPARROW) {
      //do nothing
    } else if (c == PS2_DOWNARROW) {
      //do nothing
    } else if (c == PS2_DELETE) {
      lcd.print(" ");
      //currentPos--;
      lcd.setCursor(currentPos,1);
      msg[currentPos]=' ';    //making delete function work
      //count=currentPos;
    } else {  
      // otherwise, just print all normal characters
      currentPos=count;
      lcd.setCursor(count,1);
      lcd.print(c);
      msg[currentPos]=c;
      count++;
    }
  }
 }  
  if(currentPos < 15){
      int pos=15-currentPos;
      for(int i=0 ; i <= pos ; i++){
            currentPos++;
            msg[currentPos]=' ';
        }
    }
    msg[16]='\0';
 
 }