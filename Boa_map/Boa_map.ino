#include "pitches.h"
#include "SPI.h"
#include "OneMsTaskTimer.h"
#include "LCD_SharpBoosterPack_SPI.h"

// Variables
LCD_SharpBoosterPack_SPI myScreen(SHARP_128);
uint8_t myOrientation = 0;
uint16_t myCount = 0;
int x[2];

#define LCD_VERTICAL_MAX    myScreen.getSize()
#define LCD_HORIZONTAL_MAX  myScreen.getSize()
int Music = 8; //4.5
int bpm = 200;
int notelen = 60000/(bpm*4); //bar = 16

// notes in the melody:
int melody[] = {
  //1
  NOTE_B4,0,//2
  0,NOTE_AS4,//2
  NOTE_B4,0,//2
  0,//1
  //5
  NOTE_B4,0,NOTE_B4,NOTE_GS4,0,//5
  0,NOTE_FS5,0,NOTE_FS5,0,NOTE_DS5,//6
  NOTE_B4,0,NOTE_B4,NOTE_GS4,0,//5
  0,NOTE_E5,0,NOTE_E5,0,NOTE_DS5,//6
  //5 again
  NOTE_B4,0,NOTE_B4,NOTE_GS4,0,//5
  0,NOTE_FS5,0,NOTE_FS5,0,NOTE_DS5,//6
  NOTE_B4,0,NOTE_B4,NOTE_GS4,0,//5
  0,NOTE_E5,0,NOTE_E5,0,NOTE_DS5,//6
  //9
  NOTE_B4,0,NOTE_B4,NOTE_GS4,0,NOTE_B4,0,NOTE_B4,//8
  0,NOTE_AS4,0,NOTE_FS4,NOTE_FS5,0,NOTE_FS5,0,NOTE_DS5,//9
  NOTE_B4,0,NOTE_B4,NOTE_GS4,0,NOTE_B4,0,NOTE_B4,//8
  0,NOTE_AS4,0,NOTE_FS4,NOTE_E5,0,NOTE_E5,0,NOTE_DS5,//9
  //13
  NOTE_B4,0,NOTE_B4,NOTE_GS4,0,NOTE_B4,0,NOTE_B4,//8
  0,NOTE_AS4,0,NOTE_FS4,NOTE_FS5,0,NOTE_FS5,0,NOTE_DS5,//9
  NOTE_B4,0,NOTE_B4,NOTE_GS4,0,NOTE_B4,0,NOTE_B4,//8
  0,NOTE_AS4,0,NOTE_FS4,NOTE_E5,0,NOTE_E5,0,NOTE_DS5,//9
  //13 again
  NOTE_B4,0,NOTE_B4,NOTE_GS4,0,NOTE_B4,0,NOTE_B4,//8
  0,NOTE_AS4,0,NOTE_FS4,NOTE_FS5,0,NOTE_FS5,0,NOTE_DS5,//9
  NOTE_B4,0,NOTE_B4,NOTE_GS4,0,NOTE_B4,0,NOTE_B4,//8
  0,NOTE_AS4,0,NOTE_FS4,NOTE_E5,0,NOTE_E5,0,NOTE_DS5,//9
  //17
  NOTE_B4//1
};
//semi quavers, 4 in a crotchet, 16 in a bar
int noteDurations[] = {
  //1
  4,12,
  14,2,
  4,12,
  16,
  //5
  2,2,2,2,8,
  8,2,1,2,1,2,
  2,2,2,2,8,
  8,2,1,2,1,2,
  //5 again
  2,2,2,2,8,
  8,2,1,2,1,2,
  2,2,2,2,8,
  8,2,1,2,1,2,
  //9
  2,2,2,2,2,2,2,2,
  2,2,2,2,2,1,2,1,2,
  2,2,2,2,2,2,2,2,
  2,2,2,2,2,1,2,1,2,
  //13
  2,2,2,2,2,2,2,2,
  2,2,2,2,2,1,2,1,2,
  2,2,2,2,2,2,2,2,
  2,2,2,2,2,1,2,1,2,
  //13 again
  2,2,2,2,2,2,2,2,
  2,2,2,2,2,1,2,1,2,
  2,2,2,2,2,2,2,2,
  2,2,2,2,2,1,2,1,2,
  //16
  12
};

void setup() {
  Serial.begin(9600);

    myScreen.begin();
    myScreen.clearBuffer();

    myScreen.setFont(1);
    // iterate over the notes of the melody:
  
}

void screen()
{
    /*for (uint8_t i = 0; i < 4; i++)
    {
        myScreen.text(60, 60, String(i));
        myScreen.flush();
        delay(200);
    }*/
    uint8_t j=0;
    j = (digitalRead(A5)<<2) + (digitalRead(A4)<<1) + digitalRead(A3);
    myScreen.clearBuffer();
    if(digitalRead(A0)!=1){
      myScreen.text(60, 10, String(j));
      for (uint8_t i = 10; i < LCD_HORIZONTAL_MAX - 10; i++){
          myScreen.setXY(i, 40, 1);
          myScreen.setXY(i, 100, 1);
          if(i>=10&&i<=50){
            myScreen.setXY(i, 57, 1);
            myScreen.setXY(i, 83, 1);
          }
          else{
            myScreen.setXY(i, 70, 1);
          }
      }
      for (uint8_t i = 40; i < LCD_VERTICAL_MAX - 27; i++)
      {
          myScreen.setXY(LCD_HORIZONTAL_MAX - 10, i, 1);
          if(i>57&&i<83){
            myScreen.setXY(50, i, 1);
          }
          else{
            myScreen.setXY(10, i, 1);
          }
      }
      switch(j){
        case 0:   
          x[0]= 90;  
          x[1]= 100;
          break;
        case 1:   
          x[0]= 90;  
          x[1]= 70;  
          break;
        case 2:   
          x[0]= 90;  
          x[1]= 40;
          break;
        case 3:   
          x[0]= 50;  
          x[1]= 40;
          break;
        case 4:   
          x[0]= 50;  
          x[1]= 100;
          break;
        case 5:   
          x[0]= 10;  
          x[1]= 70;
          break;
        case 6:   
          x[0]= 50;  
          x[1]= 70;
          break;
        case 7:   
          x[0]= 118;  
          x[1]= 70;
          break;
      }
      myScreen.setXY(x[0],x[1],1);
      for (uint8_t i = 1; i <=5; i++){
         myScreen.setXY(x[0]+i,x[1]+i,1);
         myScreen.setXY(x[0]-i,x[1]+i,1);
         myScreen.setXY(x[0]+i,x[1]-i,1);
         myScreen.setXY(x[0]-i,x[1]-i,1);
      }  
    }
    else{
       for (uint8_t i = 10; i < 50; i++){
          myScreen.setXY(54,i,1);
          myScreen.setXY(74,i,1);
       }
       for (uint8_t i = 70; i <= 90; i++){
          myScreen.setXY(44,i,1);
          myScreen.setXY(84,i,1);
       }
       for (uint8_t i = 44; i < 84; i++){
          myScreen.setXY(i,90,1);
       }
    }
    myScreen.flush();
    
}

void loop() {
  for (int thisNote = 0; thisNote < (sizeof(melody)/sizeof(int))/*154*/; thisNote++) {
    int noteDuration = noteDurations[thisNote]*notelen;
    tone(Music, melody[thisNote],noteDuration);
    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    unsigned long int timer = millis() + pauseBetweenNotes;
    while (timer > millis()) {
      screen();
    }
    noTone(Music);
  }
}
