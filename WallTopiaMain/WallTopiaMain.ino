#include <Arduino.h>

#include <SoftwareSerial.h>
#include <Chrono.h>

// Instanciate a Chrono object.
Chrono mainTimer(Chrono::SECONDS );
Chrono puzzle1Timer(Chrono::SECONDS );
Chrono puzzle1PenaltyTimer(Chrono::SECONDS );

SoftwareSerial SoftSerial1(21, 20); // RX, TX
SoftwareSerial SoftSerial2(2, 3); // RX, TX

#define RS485Transmit    HIGH
#define RS485Receive     LOW

#define InterConnect1dir_pin 7
#define InterConnect2dir_pin 6
#define InterConnect3dir_pin 5


#define coFxSerial SoftSerial1
#define rpiSerial Serial1
#define interSerial1 Serial2
#define interSerial2 Serial3
#define interSerial3 SoftSerial2
#define debugSerial Serial


//PERIF 1-10 IN/OUT   OUT - YELLOW IN/ RED
#define PERIF_OUT_PIN1 52 //1
#define PERIF_IN_PIN1  53
#define PERIF_OUT_PIN2 50 //2
#define PERIF_IN_PIN2  51
#define PERIF_OUT_PIN3 48 //3
#define PERIF_IN_PIN3  49
#define PERIF_OUT_PIN4 46 //4
#define PERIF_IN_PIN54  47
#define PERIF_OUT_PIN5 44 //5
#define PERIF_IN_PIN5  45
#define PERIF_OUT_PIN6 42 //6
#define PERIF_IN_PIN6  43
#define PERIF_OUT_PIN7 40 //7
#define PERIF_IN_PIN7  41
#define PERIF_OUT_PIN8 38 //8
#define PERIF_IN_PIN8  39
#define PERIF_OUT_PIN9 36 //9
#define PERIF_IN_PIN9  37
#define PERIF_OUT_PIN10 34  //10
#define PERIF_IN_PIN10  35

#define RELAY_PIN1 A15
#define RELAY_PIN2 A14
#define RELAY_PIN3 A13
#define RELAY_PIN4 A12
#define RELAY_PIN5 A11
#define RELAY_PIN6 A10
#define RELAY_PIN7 A9
#define RELAY_PIN8 A8

//RPi COmmands
#define RESET_RPI_CMD "reset\n"
#define OFF_RPI_CMD "pioff\n"
#define SCRON_RPI_CMD "piscron\n"
#define SCROFF_RPI_CMD_RPI_CMD "piscroff\n"
#define REZ_RPI_CMD "rez,%d,%d,%d,%d,%d,%d\n"

String inputString1 = "";         // a string to hold incoming data
boolean stringComplete1 = false;  // whether the string is complete

unsigned int rezult[6] = {0, 0, 0, 0, 0, 0};
int stage = 0;
void setup() {
  // initialize serial:
  coFxSerial.begin(9600);
  rpiSerial.begin(9600);
  interSerial1.begin(9600);
  interSerial2.begin(9600);
  interSerial3.begin(9600);
  debugSerial.begin(9600);

  //   while (!debugSerial) {
  //    ; // wait for serial port to connect. Needed for native USB port only
  //  }


  inputString1.reserve(200);

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

  randomSeed(analogRead(7));
  debugSerial.println("Init Complete");



}

char command[50] = "";
void loop() {

  if (mainTimer.isRunning()) {
    if (mainTimer.hasPassed(60 * 60)) { //1 Hour?
      // OPEN DOOR
      // Stage .. 0
      // Restart the chronometer.
      mainTimer.restart();
    }
  }


  switch (stage)
  {
    case 0:
      mainTimer.start();

      //Enable ButtonStart1
      stage = 1;
      break;
    case 1:
      //Monitor ButtonStart1
      //stage = 2;
      puzzle1Timer.start();
      break;

    case 2:

      if (puzzle1Timer.isRunning())
      {
        if (puzzle1Timer.hasPassed(60 * 20)) //1 Hour?
        {


          if (puzzle1PenaltyTimer.isRunning())
          {
            if ( puzzle1PenaltyTimer.hasPassed(60)) //1min penalty
            {
              puzzle1PenaltyTimer.restart();
              rezult[0]--;
            }
          }
          else
          {
            puzzle1PenaltyTimer.start();
          }
        }
      }

      //If PuzzleIn True
      puzzle1Timer.restart();
      //stage=3;
      //Send Intermediate Rezults to Rpi
      //Резултат: получават 30 от общо 100те точки от Стаята.
      //По план, за пълни точки, тази стая трябва да се изиграе за 20 минути. За всяка минута над 20 се отнемат точки. //


      break;

    case 3:

      //Ennable ButtonStart2
      stage = 4;
      break;

    case 4:
      //Monitor ButtonStart1
      //Open Door2
      //stage=5;
      break;

    case 5:
      //Listen Serial Events
      //Game1 Steps    //max 10 points min 3points
      //Game2 Panda    //max 10 points min 3points
      //Game3 Hit    //max 10 points min 3points


      //Send Intermediate Rezults to Rpi
      //if finish once?
      //stage = 6;
      break;

    case 6:

      //Ennable ButtonStart3
      stage = 7;
      break;

    case 7:
      //Monitor ButtonStart3
      //Open Door3
      //stage = 8;
      break;


    case 8:
      stage = 9;
      break;


    case 9:
      stage = 10;
      break;
  }


  //CHECK 60min TIMER
  // if >60min open door , reset game

  // print the string when a newline arrives:
  rezult[0] = random(30);
  rezult[1] = random(10); //min3
  rezult[2] = random(10); //min3
  rezult[3] = random(10); //min3
  rezult[4] = random(20); //min5
  rezult[5] = random(20); //min5


sendResultToRPi();



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


void sendResultToRPi() {
  sprintf(command,
          REZ_RPI_CMD,
          rezult[0],
          rezult[1],
          rezult[2],
          rezult[3],
          rezult[4],
          rezult[5]
         );

  rpiSerial.print(command);
  debugSerial.println(command);
  command[100] = "";
  delay(4000);

}
