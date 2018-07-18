// Smart car with navigation and following.

#include <Stepper.h>
#include <AccelStepper.h>
#include <MultiStepper.h>
#include <IRremote.h>
#include <CppList.h>

// IR receiver declare
bool _initialized = false;
IRrecv **all_rcv;

int rcv_count = 4;

// IR navigation declare
int btnBuffer[3] = {1, 1, 1};
int circleCount = 0;
int k = 1;
char _direction = 's';
char detect = '8';

// Motor steppers decalre
AccelStepper stepper1(AccelStepper::FULL4WIRE, 46, 47, 48, 49);
AccelStepper stepper2(AccelStepper::FULL4WIRE, 50, 51, 52, 53);
MultiStepper steppers;

// Position controller
long positions[2] = {0, 0}; // Array of desired stepper positions

int trackingDistance = 0;
int preDistance = 0;

void setup()
{
  Serial.begin(4800);
  
  // IR receiver initialized
  if (!_initialized)
  {
    all_rcv = (IRrecv **)malloc(rcv_count * sizeof(int));
    all_rcv[0] = new IRrecv(22);
    all_rcv[1] = new IRrecv(23);
    all_rcv[2] = new IRrecv(24);
    all_rcv[3] = new IRrecv(25);
    /*
      all_rcv[4] = new IRrecv(6);
      all_rcv[5] = new IRrecv(7);
      all_rcv[6] = new IRrecv(8);
      all_rcv[7] = new IRrecv(9);
    */

    for (int i = 0; i < rcv_count; ++i)
    {
      all_rcv[i]->enableIRIn();

      _initialized = true;
    }
  }
  // set the speed of the motor to 50 RPMs
  // Configure each stepper
  stepper1.setMaxSpeed(200);
  stepper2.setMaxSpeed(200);

  stepper1.setSpeed(200);
  stepper2.setSpeed(200);

  // Then give them to MultiStepper to manage
  steppers.addStepper(stepper1);
  steppers.addStepper(stepper2);

  // Move to a different coordinate
  positions[0] = 0;
  positions[1] = 0;

}

void loop()
{

  if (Serial.available()) {  //检测串口是否有可以读入的数据
    detect = Serial.read();     
    while(Serial.read() >= 0){}  
   }

   Serial.print(detect);
 
  switch(detect){
  case '0':
  {
    _direction = 'C';  
    break;
  }
  case '1':
  {
    _direction = 'y';  
    break;
  }
  case '2':
  {
    _direction = 'z';  
    break;
  }
  case '4':
  {
    _direction = 'r';  
    break;
  }
  case '5':
  {
    _direction = 'l';  
    break;
  }
  case '6':
  {
    _direction = 't';  
    break;
  }
  case '7':
  {
    _direction = 'g';  
    break;
  }
  case '8':
  {
    _direction = 's';  
    break;
  }
  case '9':
  {
    _direction = 'L';  
    break;
  }

  case 'a':
  {
    _direction = 'R';  
    break;
  }
  case 'b':
  {
    _direction = 'D';  
    break;
  }
  case 'c':
  {
    _direction = 'F';  
    break;
  }

  case '3':
  {

  _direction = 's';  
    
  for (int i = 0; i < rcv_count; ++i)
  {

    if (btnBuffer[i] < 0)
      btnBuffer[i] += 1;

    decode_results results;
    if (all_rcv[i]->decode(&results))
    {

      int btn =  results.value;
      if (i != 3 && btn == -1 && btnBuffer[i] > -1)
      {
        btnBuffer[i] -= 1;
      }

      all_rcv[i] -> resume();
    }
  }

/*
  // Magic Code
  for (int i = 0; i < 3; i++)
  {
    Serial.println(btnBuffer[i]);
  }
*/

  if (btnBuffer[0] == 0 && btnBuffer[1] == 0 && btnBuffer[2] == 0)
  {
    _direction = 's';
  }
  else if (btnBuffer[0] < 0 && btnBuffer[1] < 0 && btnBuffer[2] < 0)
  {
    _direction = 'g';
  }
  else if (btnBuffer[0] == 0 && btnBuffer[1] < 0 && btnBuffer[2] < 0)
  {
    _direction = 'l'; // radious small
  }
  else if (btnBuffer[0] == 0 && btnBuffer[1] == 0 && btnBuffer[2] < 0)
  {
    _direction = 'L'; // radious big
  }
  else if (btnBuffer[0] < 0 && btnBuffer[1] < 0 && btnBuffer[2] == 0)
  {
    _direction = 'r'; // radious small
  }
  else if (btnBuffer[0] < 0 && btnBuffer[1] == 0 && btnBuffer[2] == 0)
  {
    _direction = 'R'; // radious big
  }
  break;
  }
 
  default:
    break;
  }

  switch (_direction)
  {
    case 's':
      break;
    case 'g':
      {

        steppers.moveTo(positions);
        steppers.runSpeedToPosition(); // Blocks until all are in position

        positions[0] += 80;
        positions[1] -= 80;
        
        trackingDistance += 8;
        break;
      }

    case 't':
      {
        steppers.moveTo(positions);
        steppers.runSpeedToPosition(); // Blocks until all are in position

        positions[0] += -80;
        positions[1] -= -80;
        
        trackingDistance += 8;
        break;
      }

    case 'l':
      {
        steppers.moveTo(positions);
        steppers.runSpeedToPosition(); // Blocks until all are in position

        positions[0] += 85;
        positions[1] -= 115;

        trackingDistance += 10;
        break;
      }

    case 'r':
      {
        steppers.moveTo(positions);
        steppers.runSpeedToPosition(); // Blocks until all are in position

        positions[0] += 115;
        positions[1] -= 85;
        
        trackingDistance += 10;
        break;
      }

    case 'L':
      {
        steppers.moveTo(positions);
        steppers.runSpeedToPosition(); // Blocks until all are in position

        positions[0] += 30;
        positions[1] -= 70;

        trackingDistance += 5;
        break;
      }

    case 'R':
      {
        steppers.moveTo(positions);
        steppers.runSpeedToPosition(); // Blocks until all are in position

        positions[0] += 70;
        positions[1] -= 30;

        trackingDistance += 5;
        break;
      }

    case 'C':
      {

        steppers.moveTo(positions);
        steppers.runSpeedToPosition(); // Blocks until all are in position

        positions[0] += 100;
        positions[1] -= -100;

        trackingDistance += 0;
        break;
      }

      case 'z': // 左转弯避障
      {

        steppers.moveTo(positions);
        steppers.runSpeedToPosition(); // Blocks until all are in position

        positions[0] += 90;
        positions[1] -= -110;

        trackingDistance += 10;
        break;
      }

      case 'y': // 右转弯避障
      {

        steppers.moveTo(positions);
        steppers.runSpeedToPosition(); // Blocks until all are in position

        positions[0] += 110;
        positions[1] -= -90;

        trackingDistance += 10;
        break;
      }  
      case 'D': // 预设舞蹈1
      {

        steppers.moveTo(positions);
        steppers.runSpeedToPosition(); // Blocks until all are in position

        positions[0] += 110;
        positions[1] -= -90;

        trackingDistance += 10;
        break;
      }  
      case 'F': // 预设舞蹈2
      {

        steppers.moveTo(positions);
        steppers.runSpeedToPosition(); // Blocks until all are in position

        positions[0] += 90;
        positions[1] -= -110;

        trackingDistance += 10;
        break;
      }  

    default:
      break;
  }

  
}
