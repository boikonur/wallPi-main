#include <Arduino.h>

#include <SoftwareSerial.h>
#include <Chrono.h>
#include <Bounce2.h>

// Instanciate a Chrono object.
Chrono mainTimer(Chrono::SECONDS ); //60min timer
Chrono puzzle1Timer(Chrono::SECONDS ); //20min timer
Chrono puzzle1PenaltyTimer(Chrono::SECONDS ); //1min penalty timer

Chrono targetTimer(Chrono::SECONDS ); //3s


//////COMMON DEFINES
#define RS485Transmit    HIGH
#define RS485Receive     LOW
#define PRESSED LOW
#define RELEASED HIGH
////////////Serial Connections//////////////
SoftwareSerial SoftSerial1(21, 20); // RX, TX
SoftwareSerial SoftSerial2(2, 3); // RX, TX

#define coFxSerial SoftSerial1
#define rpiSerial Serial1
#define interSerial1 Serial2
#define interSerial2 Serial2
#define interSerial3 Serial3 //SoftSerial2
#define debugSerial Serial

#define InterConnect1dir_pin 7
#define InterConnect2dir_pin 6
#define InterConnect3dir_pin 5


/////////RELAYS/////////////
#define RELAY_PIN1 A15
#define RELAY_PIN2 A14
#define RELAY_PIN3 A13
#define RELAY_PIN4 A12
#define RELAY_PIN5 A11
#define RELAY_PIN6 A10
#define RELAY_PIN7 A9
#define RELAY_PIN8 A8

/////////PINS/////////////
#define MAIN_BUTTON_PIN1 8
#define MAIN_BUTTON_LED_PIN1 9
#define MAIN_BUTTON_PIN2 10
#define MAIN_BUTTON_LED_PIN2 11
#define MAIN_BUTTON_PIN3 30
#define MAIN_BUTTON_LED_PIN3 28

#define PUZZLE_GAME_INPUT_PIN 12

#define LASER_BUTTON_PIN1  A0 //START BUTTOn
#define LASER_BUTLED_PIN1  A3 //START BUTTOn LED
#define LASER_BUTTON_PIN2  A1 //BUTTON 1
#define LASER_BUTLED_PIN2  A4 //BUTTON 1 LED
#define LASER_BUTTON_PIN3  A2  //BUTTOn 2
#define LASER_BUTLED_PIN3  A5  //BUTTOn 2 LED
#define LASER_SENSOR_PIN 33
#define LASER_ENABLE_PIN  RELAY_PIN5

#define PISTOL_BUTTON_PIN1 31 //TODO
#define PISTOL_BUTLED_PIN1 29 //TODO
//PERIF 1-10 IN/OUT   OUT - YELLOW IN/ RED
#define PERIF_OUT_PIN1 53 //1
#define PERIF_IN_PIN1  52
#define PERIF_OUT_PIN2 51 //2
#define PERIF_IN_PIN2  50
#define PERIF_OUT_PIN3 49 //3
#define PERIF_IN_PIN3  48
#define PERIF_OUT_PIN4 47 //4
#define PERIF_IN_PIN54 46
#define PERIF_OUT_PIN5 45 //5
#define PERIF_IN_PIN5  44
#define PERIF_OUT_PIN6 43 //6
#define PERIF_IN_PIN6  42
#define PERIF_OUT_PIN7 41 //7
#define PERIF_IN_PIN7  40
#define PERIF_OUT_PIN8 39 //8
#define PERIF_IN_PIN8  38
#define PERIF_OUT_PIN9 37 //9
#define PERIF_IN_PIN9  36
#define PERIF_OUT_PIN10 35  //10
#define PERIF_IN_PIN10  34



//////EXTRA HEADER/////////////
#define  EXTRA_1_IN 22
#define  EXTRA_1_OUT 23
#define  EXTRA_2_IN 25
#define  EXTRA_2_OUT 24
#define  EXTRA_3_IN 26
#define  EXTRA_3_OUT 27

///////LIGHTS/////////////
#define LIGHTS1_PIN RELAY_PIN1 // same pin?
#define LIGHTS2_PIN RELAY_PIN1 // same pin?
#define LIGHTS3_PIN RELAY_PIN2
#define LIGHTS_UV_PIN RELAY_PIN3

////////////DOOR Variables////////////////
#define DOOR1_ONTIME 4000
#define DOOR2_ONTIME 4000
#define DOOR3_ONTIME 4000
#define DOOR1_PIN RELAY_PIN7
#define DOOR2_PIN RELAY_PIN8
#define DOOR3_PIN RELAY_PIN6
unsigned long  prev_door1_time = 0;
unsigned long  prev_door2_time = 0;
unsigned long  prev_door3_time = 0;
bool door1Active = false;
bool door2Active = false;
bool door3Active = false;
bool enableDoor[3] = {0,0,0};

/////////DOJO GAME VARIABLES///////////////

//STARTA\n ->>>>>>>> STOPA%12222\n
String startGameA = "STARTA"; //"6002705323
String startGameB = "STARTB"; //6002705323
String startGameC = "STARTC";  //6002705323
String stopGameA = "STOPA"; //"6002705323
String stopGameB = "STOPB"; //6002705323
String stopGameC = "STOPC";  //6002705323

String inStrRpiSerial = "";
boolean inStrCompleteRpiSerial = false;
String inStrInterSerial1 = "";
boolean inStrCompleteInterSerial1 = false;
String inStrInterSerial2 = "";
boolean inStrCompleteInterSerial2 = false;

//RPi COmmands
#define LANG_BG_RPI_CMD "lang_bg\n"
#define LANG_EN_RPI_CMD "lang_en\n"
#define RESET_RPI_CMD "startgame\n"
#define OFF_RPI_CMD "pioff\n"
#define SCRON_RPI_CMD "piscron\n"
#define SCROFF_RPI_CMD_RPI_CMD "piscroff\n"
#define REZ_RPI_CMD "rez,%d,%d,%d,%d,%d,%d\n" //6 games

/////// MAIN GLOBAL VARIABLES/////////////

/*
GAME_NAME0_BG = u'Изпитание за наблюдателност'
GAME_NAME1_BG = u'Изпитание за сила'
GAME_NAME2_BG  = u'Изпитание за координация'
GAME_NAME3_BG = u'Изпитание за бързина'
GAME_NAME4_BG  = u'Изпитание за ловкост'
GAME_NAME5_BG  = u'Изпитание за точност'
*/
#define PUZZLE_GAME 0
#define HIT_GAME 2
#define STEPS_GAME 2
#define PANDA_GAME 3
#define LASER_GAME 4
#define PISTOL_GAME 5


int target_index=0;
int  targetPatern[10]={4,8,1,2,5,9,0,3,7,6};

unsigned int result[6] = {0, 0, 0, 0, 0, 0};
int stage = 0;
int prev_stage = 0;
int laser_stage=0;
int pistol_stage=0;



void setup() {
  // initialize serial:
  coFxSerial.begin(9600);
  rpiSerial.begin(9600);
  interSerial1.begin(9600);
  interSerial2.begin(9600);
  //interSerial3.begin(9600);
  debugSerial.begin(9600);

  //   while (!debugSerial) {
  //    ; // wait for serial port to connect. Needed for native USB port only
  //  }

  inStrRpiSerial.reserve(200);
  inStrInterSerial1.reserve(200);
  inStrInterSerial2.reserve(200);

  pinMode(13, OUTPUT);
  pinMode(InterConnect1dir_pin, OUTPUT);
  pinMode(InterConnect2dir_pin, OUTPUT);
  pinMode(InterConnect3dir_pin, OUTPUT);

  pinMode(RELAY_PIN1, OUTPUT);
  pinMode(RELAY_PIN2, OUTPUT);
  pinMode(RELAY_PIN3, OUTPUT);
  pinMode(RELAY_PIN4, OUTPUT);
  pinMode(RELAY_PIN5, OUTPUT);
  pinMode(RELAY_PIN6, OUTPUT);
  pinMode(RELAY_PIN7, OUTPUT);
  pinMode(RELAY_PIN8, OUTPUT);

  pinMode( MAIN_BUTTON_PIN1, INPUT_PULLUP);
  pinMode( MAIN_BUTTON_LED_PIN1, OUTPUT);
  pinMode( MAIN_BUTTON_PIN2 , INPUT_PULLUP);
  pinMode( MAIN_BUTTON_LED_PIN2, OUTPUT);
  pinMode( MAIN_BUTTON_PIN3 , INPUT_PULLUP);
  pinMode( MAIN_BUTTON_LED_PIN3, OUTPUT);

  pinMode( PUZZLE_GAME_INPUT_PIN, INPUT);

  pinMode(LASER_BUTTON_PIN1, INPUT_PULLUP);
  pinMode(LASER_BUTLED_PIN1, OUTPUT);
  pinMode(LASER_BUTTON_PIN2, INPUT_PULLUP);
  pinMode(LASER_BUTLED_PIN2, OUTPUT);
  pinMode(LASER_BUTTON_PIN3, INPUT_PULLUP);
  pinMode(LASER_BUTLED_PIN3, OUTPUT);
  pinMode(LASER_SENSOR_PIN, INPUT_PULLUP);

  pinMode(PISTOL_BUTTON_PIN1, INPUT_PULLUP);
  pinMode(PISTOL_BUTLED_PIN1, OUTPUT);

  pinMode( PERIF_OUT_PIN1, OUTPUT);
  pinMode( PERIF_IN_PIN1, INPUT);
  pinMode( PERIF_OUT_PIN2, OUTPUT);
  pinMode( PERIF_IN_PIN2, INPUT);
  pinMode( PERIF_OUT_PIN3, OUTPUT);
  pinMode( PERIF_IN_PIN3, INPUT);
  pinMode( PERIF_OUT_PIN4, OUTPUT);
  pinMode( PERIF_IN_PIN54, INPUT);
  pinMode( PERIF_OUT_PIN5, OUTPUT);
  pinMode( PERIF_IN_PIN5 , INPUT);
  pinMode( PERIF_OUT_PIN6, OUTPUT);
  pinMode( PERIF_IN_PIN6, INPUT);
  pinMode( PERIF_OUT_PIN7, OUTPUT);
  pinMode( PERIF_IN_PIN7 , INPUT);
  pinMode( PERIF_OUT_PIN8, OUTPUT);
  pinMode( PERIF_IN_PIN8 , INPUT);
  pinMode( PERIF_OUT_PIN9, OUTPUT);
  pinMode( PERIF_IN_PIN9 , INPUT);
  pinMode( PERIF_OUT_PIN10, OUTPUT);
  pinMode( PERIF_IN_PIN10 , INPUT);

  pinMode( EXTRA_1_IN , INPUT);
  pinMode( EXTRA_1_OUT , OUTPUT);
  pinMode( EXTRA_2_IN , INPUT);
  pinMode( EXTRA_2_OUT , OUTPUT);
  pinMode( EXTRA_3_IN , INPUT);
  pinMode( EXTRA_3_OUT, OUTPUT);


  //INIT OUTPUTS
  digitalWrite(InterConnect1dir_pin, RS485Receive);
  digitalWrite(InterConnect2dir_pin, RS485Receive);
  digitalWrite(InterConnect3dir_pin, RS485Receive);

  digitalWrite(RELAY_PIN1, LOW);
  digitalWrite(RELAY_PIN2, LOW);
  digitalWrite(RELAY_PIN3, LOW);
  digitalWrite(RELAY_PIN4, LOW);
  digitalWrite(RELAY_PIN5, LOW);
  digitalWrite(RELAY_PIN6, LOW);
  digitalWrite(RELAY_PIN7, LOW);
  digitalWrite(RELAY_PIN8, LOW);

  digitalWrite(MAIN_BUTTON_LED_PIN1, LOW);
  digitalWrite(MAIN_BUTTON_LED_PIN2, LOW);
  digitalWrite(MAIN_BUTTON_LED_PIN3, LOW);

  digitalWrite(LASER_BUTLED_PIN1, LOW);
  digitalWrite(LASER_BUTLED_PIN2, LOW);
  digitalWrite(LASER_BUTLED_PIN3, LOW);

  digitalWrite(PISTOL_BUTLED_PIN1, LOW);

  digitalWrite(PERIF_OUT_PIN1, LOW);
  digitalWrite(PERIF_OUT_PIN2, LOW);
  digitalWrite(PERIF_OUT_PIN3, LOW);
  digitalWrite(PERIF_OUT_PIN4, LOW);
  digitalWrite(PERIF_OUT_PIN5, LOW);
  digitalWrite(PERIF_OUT_PIN6, LOW);
  digitalWrite(PERIF_OUT_PIN7, LOW);
  digitalWrite(PERIF_OUT_PIN8, LOW);
  digitalWrite(PERIF_OUT_PIN9, LOW);
  digitalWrite(PERIF_OUT_PIN10, LOW);

  digitalWrite(EXTRA_1_OUT ,  LOW);
  digitalWrite(EXTRA_2_OUT, LOW);
  digitalWrite(EXTRA_3_OUT,  LOW);

  randomSeed(analogRead(7));

  debugSerial.println("Init Complete");

}






void loop() {

doorHandle();
handleRpiInCmd();
gameTimer60();



if (stage != prev_stage)
 {
   prev_stage = stage;
   debugSerial.print("STAGE: ");
   debugSerial.println(stage);
 }

  switch (stage)
  {

    case -1:
      result[PUZZLE_GAME]=0;
      result[PANDA_GAME]=0;
      result[STEPS_GAME]=0;
      result[HIT_GAME]=0;
      result[LASER_GAME]=0;
      result[PISTOL_GAME]=0;
    break;

    case 0:
      mainTimer.start();

      changeMusic(0);
      enButtonStart(1);
      stage = 1;
      break;
    case 1:
    if(readButtonStart(1))
    {
      stage = 2;
      turnOnLights(1);   //Turn Lights ON
      changeMusic(1);
      puzzle1Timer.start();
      debugSerial.println("Game Started");
      rpiSerial.print(RESET_RPI_CMD);
      result[PUZZLE_GAME]=30; //Start with max points

    }
      break;

    case 2:

      if (puzzle1Timer.isRunning())
      {
        if (puzzle1Timer.hasPassed(60 * 20)) //20 min passed?
        {
          if (puzzle1PenaltyTimer.isRunning())
          {
            if ( puzzle1PenaltyTimer.hasPassed(60)) //1min penalty
            {
              result[PUZZLE_GAME]--;
              debugSerial.print("One Minute Penalty. Result is:");
              debugSerial.println(result[PUZZLE_GAME]);
              puzzle1PenaltyTimer.restart();
            }
          }
          else
          {
            puzzle1PenaltyTimer.start();
          }
        }
      }
      //If PuzzleIn True
      if(digitalRead(PUZZLE_GAME_INPUT_PIN) == HIGH)
      {
        puzzle1Timer.restart();
        stage=3;
        debugSerial.print("TIME:");
        debugSerial.println(getElapsed60());
        debugSerial.print("Puzzle result is:");
        debugSerial.println(result[PUZZLE_GAME]);
        sendResultToRPi(); //Send Intermediate results to Rpi
      }

    break;

    case 3:
      enButtonStart(2);
      debugSerial.println("Enable Room 2");
      stage = 4;
      break;

    case 4:
      //Monitor ButtonStart2
      if(readButtonStart(2))
      {
        debugSerial.println("Open Room 2");
        turnOnLights(2);
        changeMusic(2);
        unlockDoor(2); //Open Door2
        stage=5;
      }

      break;

    case 5:

    dojoGame();
    //sendResultToRPi();
      //Listen Serial Events
      //Game1 Steps    //max 10 points min 3points
      //Game2 Panda    //max 10 points min 3points
      //Game3 Hit    //max 10 points min 3points


      //Send Intermediate results to Rpi
      //if finish once?
      //stage = 6;
      break;

    case 6:

      enButtonStart(3);   //Ennable ButtonStart3
        debugSerial.print("TIME:");
        debugSerial.println(getElapsed60());
        debugSerial.println("Enable Room 3");
      stage = 7;
      break;

    case 7:

    if(readButtonStart(3))   //Monitor ButtonStart3
    {
      debugSerial.println("Open Room 3");
      turnOffLights(3);
      turnOffLights(4);
      changeMusic(3);
      unlockDoor(3); //Open Door3

      stage=8;
    }

    break;

case 8:
debugSerial.println("Enabling Lasers");
enableLasers(); //better start game in beginning
turnOffLights(4);
stage=8;
break;

case 9:

  if(laserGame()){
      stage = 10;
      debugSerial.print("TIME:");
      debugSerial.println(getElapsed60());
      debugSerial.print("Laser result is:");
      debugSerial.println(result[LASER_GAME]);
      sendResultToRPi(); //Send Intermediate results to Rpi

  }
      break;


    case 10:
    //switc lighs to UV
    debugSerial.println("Starting Target Practice");
    turnOffLights(3);
    turnOnLights(4);
    stage = 11;

    break;



    case 11:
      if(pistolGame())
        {
          stage=12;
          debugSerial.print("TIME:");
          debugSerial.println(getElapsed60());
          debugSerial.print("Pistol Game result is:");
          debugSerial.println(result[PISTOL_GAME]);
          sendResultToRPi(); //Send Intermediate results to Rpi
        }
    break;

    case 12:
      debugSerial.println("All Games are over");
      debugSerial.print("TOTAL RESULT: ");
      debugSerial.println( result[PUZZLE_GAME]+
                          result[PANDA_GAME]+
                          result[STEPS_GAME]+
                          result[HIT_GAME]+
                          result[LASER_GAME]+
                          result[PISTOL_GAME]
                        );
      stage=13;
    break;

    case 13:
    //do stuff
    break;



      case 14: //Decide to exit?
        enButtonStart(1);
        enButtonStart(3);

        if(readButtonStart(1)==HIGH) //FINISH NO
        {
            stage = 15;


        }
        if(readButtonStart(3)==HIGH) //FINISH YES
        {
            stage = 15;
        }

        break;



        case 15:
        unlockDoor(3);
        changeMusic(0);
          stage = 16;
          break;

        case 16:
        break;
  }


  //CHECK 60min TIMER
  // if >60min open door , reset game

  // print the string when a newline arrives:

  //////////TEST comm to Rpi
  // result[PUZZLE_GAME] = random(30);
  // result[PANDA_GAME] = random(10); //min3
  // result[STEPS_GAME] = random(10); //min3
  // result[HIT_GAME] = random(10); //min3
  // result[LASER_GAME] = random(20); //min5
  // result[PISTOL_GAME] = random(20); //min5
  // sendResultToRPi();





  //if (interSerial1.available()){
  //  debugSerial.write(Serial3.read());
  //}
  //
  //if (interSerial2.available()){
  //  debugSerial.write(Serial3.read());
  //}
  //
  //if (interSerial3.available()){
  //  debugSerial.write(Serial3.read());
  //}

}


int laserGame()
{
int res=0;


 switch(laser_stage)
 {
    case 0:
    disableLasers();

    if(digitalRead(LASER_BUTLED_PIN1)==PRESSED)
    {
      digitalWrite(LASER_BUTLED_PIN1, LOW);
      laser_stage=1;
      enableLasers();
      digitalWrite(LASER_BUTLED_PIN2, HIGH);
      digitalWrite(LASER_BUTLED_PIN3, HIGH);
    }
    break;
    case 1:

  if(digitalRead(LASER_BUTLED_PIN2)==PRESSED)
  {
     digitalWrite(LASER_BUTLED_PIN2, LOW);
  }else
    digitalWrite(LASER_BUTLED_PIN2, HIGH);

  if(digitalRead(LASER_BUTLED_PIN3)==PRESSED)
  {
     digitalWrite(LASER_BUTLED_PIN3, LOW);
  }else
    digitalWrite(LASER_BUTLED_PIN3, HIGH);

    if(digitalRead(LASER_SENSOR_PIN)==RELEASED)
    {
       debugSerial.println("Laser is crossed");
       result[4]--;

       delay(100);
        if (result[6] < 5)
        {
          laser_stage=0;
           return 0;
        }
    //
    //
    }


    break;
case 2:

 break;
case 3: break;

 }


return res;
} //end laser Game



int pistolGame()
{

int res=0;

if(targetTimer.hasPassed(3)) //change target on 3s?
{
disableTarget(targetPatern[target_index]);
  targetTimer.restart();
  target_index++;
}

if(readTarget(targetPatern[target_index]))
{
  disableTarget(targetPatern[target_index]);
    targetTimer.restart();
    target_index++;
    result[PISTOL_GAME]++;
}

enableTarget(targetPatern[target_index]);

if(target_index >=10)
{
  target_index=0;
  res=1;
}


return res;
}//end  pistol Game


void enableTarget(int num)
{
  switch(num)
  {
    case 0: digitalWrite( PERIF_OUT_PIN1, HIGH); break;
    case 1: digitalWrite( PERIF_OUT_PIN2, HIGH); break;
    case 2: digitalWrite( PERIF_OUT_PIN3, HIGH); break;
    case 3: digitalWrite( PERIF_OUT_PIN4, HIGH); break;
    case 4: digitalWrite( PERIF_OUT_PIN5, HIGH); break;
    case 5: digitalWrite( PERIF_OUT_PIN6, HIGH); break;
    case 6: digitalWrite( PERIF_OUT_PIN7, HIGH); break;
    case 7: digitalWrite( PERIF_OUT_PIN8, HIGH); break;
    case 8: digitalWrite( PERIF_OUT_PIN9, HIGH); break;
    case 9: digitalWrite( PERIF_OUT_PIN10, HIGH); break;
    default: debugSerial.println("Error. No such target.");
  }
}

void disableTarget(int num)
{
  switch(num)
  {
    case 0: digitalWrite( PERIF_OUT_PIN1, LOW); break;
    case 1: digitalWrite( PERIF_OUT_PIN2, LOW); break;
    case 2: digitalWrite( PERIF_OUT_PIN3, LOW); break;
    case 3: digitalWrite( PERIF_OUT_PIN4, LOW); break;
    case 4: digitalWrite( PERIF_OUT_PIN5, LOW); break;
    case 5: digitalWrite( PERIF_OUT_PIN6, LOW); break;
    case 6: digitalWrite( PERIF_OUT_PIN7, LOW); break;
    case 7: digitalWrite( PERIF_OUT_PIN8, LOW); break;
    case 8: digitalWrite( PERIF_OUT_PIN9, LOW); break;
    case 9: digitalWrite( PERIF_OUT_PIN10, LOW); break;
    default: debugSerial.println("Error. No such target.");
  }
}

bool readTarget(int num)
{
  switch(num)
  {
    case 0: return digitalRead(PERIF_OUT_PIN1); break;
    case 1: return digitalRead(PERIF_OUT_PIN2); break;
    case 2: return digitalRead(PERIF_OUT_PIN3); break;
    case 3: return digitalRead(PERIF_OUT_PIN4); break;
    case 4: return digitalRead(PERIF_OUT_PIN5); break;
    case 5: return digitalRead(PERIF_OUT_PIN6); break;
    case 6: return digitalRead(PERIF_OUT_PIN7); break;
    case 7: return digitalRead(PERIF_OUT_PIN8); break;
    case 8: return digitalRead(PERIF_OUT_PIN9); break;
    case 9: return digitalRead(PERIF_OUT_PIN10); break;
    default: debugSerial.println("Error. No such target.");
  }
}

void enButtonStart( int num)
{
    switch(num){
      case 1:  digitalWrite(MAIN_BUTTON_LED_PIN1, HIGH);   break;
      case 2:  digitalWrite(MAIN_BUTTON_LED_PIN2, HIGH);   break;
      case 3:  digitalWrite(MAIN_BUTTON_LED_PIN3, HIGH);  break;
      default: debugSerial.println("Error Button");
    }

}

void disButtonStart( int num)
{
  switch(num){
    case 1:  digitalWrite(MAIN_BUTTON_LED_PIN1, LOW);   break;
    case 2:  digitalWrite(MAIN_BUTTON_LED_PIN2, LOW);   break;
    case 3:  digitalWrite(MAIN_BUTTON_LED_PIN3, LOW);  break;
    default: debugSerial.println("Error Button");
  }

}

bool readButtonStart( int num)
{
  switch(num){
    case 1:  return (digitalRead(MAIN_BUTTON_PIN1)==PRESSED);   break;
    case 2:  return (digitalRead(MAIN_BUTTON_PIN2)==PRESSED);   break;
    case 3:  return (digitalRead(MAIN_BUTTON_PIN3)==PRESSED);   break;
    default: debugSerial.println("Error Button");
  }
}




void doorHandle()
{
 unsigned long  now = millis();

  //Door 1 routine
  if (enableDoor[0])
  {
      if ((unsigned long)( millis() - prev_door1_time ) >= DOOR1_ONTIME)
      {
        prev_door1_time = millis();
        door1Active = !door1Active;
      }
  }
  else
  {
    door1Active = false;
  }
  //Door 2 routine
  if (enableDoor[1])
  {
      if ((unsigned long)( millis() - prev_door2_time ) >= DOOR2_ONTIME)
      {
        prev_door2_time = millis();
        door2Active = !door2Active;
      }
  }
  else
  {
    door2Active = false;
  }
  //Door 3 routine
  if (enableDoor[2])
  {
      if ((unsigned long)( millis() - prev_door3_time ) >= DOOR3_ONTIME)
      {
        prev_door3_time = millis();
        door3Active = !door3Active;
      }
  }
  else
  {
    door3Active = false;
  }


  //////////////////////update relays/////////////////
  digitalWrite(DOOR1_PIN, door1Active);
  digitalWrite(DOOR2_PIN, door2Active);
  digitalWrite(DOOR3_PIN, door3Active);
}


void unlockDoor(int num)
{
  switch(num){
    case 1:    enableDoor[0]=true;
    case 2:    enableDoor[1]=true; break;
    case 3:    enableDoor[2]=true; break;
    default: debugSerial.println("No Such door");
  }

}

void lockDoor(int num)
{
  switch(num){
    case 1:    enableDoor[0]=false;
    case 2:    enableDoor[1]=false; break;
    case 3:    enableDoor[2]=false; break;
    default: debugSerial.println("No Such door");
  }
}


void turnOnLights(int num)
{
  switch(num){
    case 1:   digitalWrite(LIGHTS1_PIN, HIGH); break;
    case 2:   digitalWrite(LIGHTS2_PIN, HIGH); break;
    case 3:   digitalWrite(LIGHTS3_PIN, HIGH); break;
    case 4:   digitalWrite(LIGHTS_UV_PIN, HIGH); break;
    default: debugSerial.println("No Such Light");
  }

}

void turnOffLights(int num)
{
  switch(num){
    case 1:   digitalWrite(LIGHTS1_PIN, LOW); break;
    case 2:   digitalWrite(LIGHTS2_PIN, LOW); break;
    case 3:   digitalWrite(LIGHTS3_PIN, LOW); break;
    case 4:   digitalWrite(LIGHTS_UV_PIN, LOW); break;
    default: debugSerial.println("No Such Light");
  }

}

void enableLasers()
{
digitalWrite(LASER_ENABLE_PIN, HIGH);
debugSerial.println("Lasers ON");
}

void disableLasers()
{
digitalWrite(LASER_ENABLE_PIN, LOW);
debugSerial.println("Lasers OFF");
}


void changeMusic(int  track)
{

  switch(track)
  {
    case 0:   coFxSerial.print("fxstop\n"); break;
    case 1:   coFxSerial.print("fxtrack1\n"); break;
    case 2:   coFxSerial.print("fxtrack2\n"); break;
    case 3:   coFxSerial.print("fxtrack3\n"); break;
    case 4:   coFxSerial.print("fxtrack4\n"); break;
    case 5:   coFxSerial.print("fxtrack5\n"); break;
    case 6:   coFxSerial.print("fxtrack6\n"); break;
    case 7:   coFxSerial.print("fxtrack7\n"); break;
    default: debugSerial.println("No Such CoFX command");
  }
}

unsigned long hidden_combination_timer1 = 0;
unsigned long hidden_combination_timer2 = 0;
unsigned long hidden_combination_timer3 = 0;


#define HIDDEN_HOLD_TIME 15000



void hiddenButtonControl()
{
  unsigned long cur_time = millis();

// HOLD 1 and 3 to turnoff RPI
  if (readButtonStart(1) == PRESSED && readButtonStart(2) == RELEASED && readButtonStart(3) == PRESSED)
  {
    if ((unsigned long)(cur_time - hidden_combination_timer1) >= HIDDEN_HOLD_TIME)
    {
      hidden_combination_timer1 = cur_time;
      turnOffRpi();
    }
  }
  else
  {
    hidden_combination_timer1 = cur_time;
  }

// HOLD 1 and 2 to change language
  if (readButtonStart(1) == PRESSED && readButtonStart(2) == PRESSED && readButtonStart(3) == RELEASED)
  {
    if ((unsigned long)(cur_time - hidden_combination_timer2) >= HIDDEN_HOLD_TIME)
    {
      hidden_combination_timer2 = cur_time;
      changeRpiLang();
    }
  }
  else
  {
    hidden_combination_timer2 = cur_time;
  }

  // HOLD 1 and 2 to change language
  if (readButtonStart(1) == RELEASED && readButtonStart(2) == PRESSED && readButtonStart(3) == PRESSED)
    {
      if ((unsigned long)(cur_time - hidden_combination_timer3) >= HIDDEN_HOLD_TIME)
      {
        hidden_combination_timer3 = cur_time;
        //TODO some hidden function
      }
    }
    else
    {
      hidden_combination_timer3 = cur_time;
    }


}

void turnOffRpi()
{
    debugSerial.println("Turning OFF Raspberry");
    rpiSerial.print(OFF_RPI_CMD);
    turnOffLights(1);
    turnOffLights(2);
    turnOffLights(3);
    turnOffLights(4);
}

void changeRpiLang()
{
    rpiSerial.print(LANG_BG_RPI_CMD);
    rpiSerial.print(LANG_EN_RPI_CMD);
}


void sendResultToRPi() {
  char command[100] = "";
  sprintf(command,
          REZ_RPI_CMD,
          result[0],
          result[1],
          result[2],
          result[3],
          result[4],
          result[5]
         );
  rpiSerial.print(command);
  debugSerial.println(command);
  delay(4000);

}


bool dojoGame()
{
  //serialEvent2();

  if (inStrCompleteInterSerial1)
  {
    debugSerial.println(inStrInterSerial1);

    if  (inStrInterSerial1 == startGameA )
    {
        debugSerial.println("Starting Game A");
    }
    if  (inStrInterSerial1 == startGameB)
    {
          debugSerial.println("Starting Game B");
    }

    if (inStrInterSerial1.startsWith(stopGameA)) {
          debugSerial.print("Stopping Game A. ");
          debugSerial.print("Result is: ");
          result[HIT_GAME]= inStrInterSerial1.substring(6).toInt();
          debugSerial.println( result[HIT_GAME]);
    }

    if (inStrInterSerial1.startsWith(stopGameB)) {
            debugSerial.print("Stopping Game B. ");
            debugSerial.print("Result is: ");
            result[PANDA_GAME]=  inStrInterSerial1.substring(6).toInt();
            debugSerial.println(result[PANDA_GAME] );
    }

    inStrInterSerial1 = "";
    inStrCompleteInterSerial1 = false;
  }

  if (inStrCompleteInterSerial2)
  {

    if  (inStrInterSerial2 == startGameC)
    {
          debugSerial.println("Starting Game C");
    }

    if (inStrInterSerial2.startsWith(stopGameC)) {
          debugSerial.print("Stopping Game C. ");
          debugSerial.print("Result is: ");
          result[STEPS_GAME]= inStrInterSerial2.substring(6).toInt();
          debugSerial.println(result[STEPS_GAME]);
    }
    inStrInterSerial2 = "";
    inStrCompleteInterSerial2 = false;
    // return false;
}

  return false;
}

void handleRpiInCmd()
{
  if (inStrCompleteRpiSerial)
  {
    debugSerial.print("RPi:");
    debugSerial.println(inStrRpiSerial);
        // if(inStrRpiSerial == stopGameC)
        //      debugSerial.println("Executing Something?");
    inStrRpiSerial = "";
    inStrCompleteRpiSerial = false;
  }
}


unsigned long getElapsed60(){

  return mainTimer.elapsed();
}

void gameTimer60()
{
  if (mainTimer.isRunning())
   {

    if (mainTimer.hasPassed(60 * 60)) //1 Hour?
     {
      unlockDoor(1); // OPEN DOOR
      stage=0;   // Stage .. 0
      mainTimer.restart();      // Restart the chronometer.
      debugSerial.println("60 min are over...");
    }
  }
}


void serialEvent1() //RPi Serial
{
  while (rpiSerial.available())
  {
    // get the new byte:
    char inChar = (char)rpiSerial.read();

    if ((inChar == '\n') )
    {
      if (inStrRpiSerial.length() > 0)
      {
        inStrCompleteRpiSerial = true;
        break;
      }
    }
    else
    {
      inStrRpiSerial += inChar;
    }

  }
}


void serialEvent2() //interSerial1    Puzzle A and puzzle B
{
  while (interSerial1.available())
  {
    // get the new byte:
    char inChar = (char)interSerial1.read();

    if (inChar == '\n')
    {
      if (inStrInterSerial1.length() > 0)
      {
        inStrCompleteInterSerial1 = true;
        break;
      }
    }
    else
    {
      inStrInterSerial1 += inChar;
    }

  }
}

void serialEvent3() //interSerial2  puzzle C
{
  while (interSerial2.available())
  {
    // get the new byte:
    char inChar = (char)interSerial2.read();

    if (inChar == '\n')
    {
      if (inStrInterSerial2.length() > 0)
      {
        inStrCompleteInterSerial2 = true;
        break;
      }
    }
    else
    {
      inStrInterSerial2 += inChar;
    }

  }
}
