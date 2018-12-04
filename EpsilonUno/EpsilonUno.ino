#include <Chrono.h>
#include <LiquidCrystal.h> //申明1602液晶的函数库
#include <IRremote.h>

int Echo[2] = {A5, A2};
int Trig[2] = {A4, A3};

// Tracking Declare
Chrono myChrono;
long trackingTime = 0;
long preTime = 0;
int key = 1;
int minTime = 0;

//4数据口模式连线声明 LiquidCrystal(rs, rw, enable, d4, d5, d6, d7)
LiquidCrystal lcd(3, 4, 5, 6, 7, 8, 9);

// IR model changer
int model = -1;
int pinBuzzer = 13;
char _direction = '8';

/*
// IR controler declare
long unsigned int trackingMode[2] = {0x00FF6897, 0xC101E57B}; //按键0
long unsigned int  run_car[2] = {0x00FF18E7, 0x3D9AE3F7};     //按键2
long unsigned int  back_car[2] = {0x00FF4AB5, 0x1BC0157B};    //按键8
long unsigned int  left_car[2] = {0x00FF10EF, 0x8C22657B};    //按键4
long unsigned int  right_car[2] = {0x00FF5AA5, 0x0449E79F};   //按键6
long unsigned int  stop_car[2] = {0x00FF38C7, 0x488F3CBB};    /=/按键5
long unsigned int  left_turn[2] = {0x00FF30CF, 0x9716BE3F};   //按键1
long unsigned int  right_turn[2] = {0x00FF7A85, 0x6182021B};  //按键3
long unsigned int  plusSpeed[2] = {0x00FFA857, 0xA3C8EDDB};   //按键+
long unsigned int  subSpeed[2] = {0x00FFE01F, 0xF076C13B};    //按键-
long unsigned int  dance1[2] = {0x000FFA25, 0xE318261B};      //按键CH-
long unsigned int  dance2[2] = {0x00FF629D, 0x00511DBB};      //按键CH
*/


// long unsigned int trackingMode[2] = {0xa11, 0x8CE1E3FF}; // 跟踪模式
// long unsigned int remoteMode[2] = {0xa00, 0x5698051A}; // 遥控模式

long unsigned int changeMode[2] = {0xa11, 0x8CE1E3FF}; // 跟踪模式
long unsigned int  run_car[2] = {0xa02, 0xC69F7130};     //按键2
long unsigned int  back_car[6] = {0xa08,0xa07,0xa09,0xE779C90, 0xD2A35121, 0xE779C93};    //按键8
long unsigned int  left_car[2] = {0xa04, 0xAEA0D9DC};    //按键4
long unsigned int  right_car[2] = {0xa06, 0xD2A35120};   //按键6
long unsigned int  stop_car[2] = {0xa05, 0xAEA0D9DF};    //按键5
long unsigned int  left_turn[2] = {0xa01, 0x56980519};   //按键1
long unsigned int  right_turn[2] = {0xa03, 0xC69F7131};  //按键3


// Show Distance and Time
int RECV_PIN = 11;
IRrecv irrecv(RECV_PIN);
decode_results results;
long preDistance = 0;
long temp = 0;

int Distance_test(int i) // 量出前方距离
{

  digitalWrite(Trig[i], LOW); // 给触发脚低电平2μs
  delayMicroseconds(2);
  digitalWrite(Trig[i], HIGH); // 给触发脚高电平10μs，这里至少是10μs
  delayMicroseconds(10);
  digitalWrite(Trig[i], LOW);               // 持续给触发脚低电
  float Fdistance = pulseIn(Echo[i], HIGH); // 读取高电平时间(单位：微秒)
  Fdistance = Fdistance / 58;               //为什么除以58等于厘米，  Y米=（X秒*344）/2
  // X秒=（ 2*Y米）/344 ==》X秒=0.0058*Y米 ==》厘米=微秒/58
  return Fdistance;
}

void newtone(byte tonePin, int frequency, int duration)
{
  int period = 1000000L / frequency;
  int pulse = period / 2;
  for (long i = 0; i < duration * 1000L; i += period)
  {
    digitalWrite(tonePin, HIGH);
    delayMicroseconds(pulse);
    digitalWrite(tonePin, LOW);
    delayMicroseconds(pulse);
  }
}

void setup()
{
    
  Serial.begin(4800);

  preTime = millis();

  irrecv.enableIRIn(); // Start the receiver

  lcd.begin(16, 2); //初始化1602液晶工作模式
                    //定义1602液晶显示范围为2行16列字符

  pinMode(Echo[0], INPUT);  // 定义超声波输入脚
  pinMode(Trig[0], OUTPUT); // 定义超声波输出脚
  pinMode(Echo[1], INPUT);  // 定义超声波输入脚
  pinMode(Trig[1], OUTPUT); // 定义超声波输出脚

  pinMode(pinBuzzer, OUTPUT);

  lcd.setCursor(4, 0); //光标定位到第一行
  lcd.print("EpsilonX");
  lcd.setCursor(4, 1); //光标定位到第一行
  lcd.print("Welcome!");
 
}

void loop()
{

  /************************************************************************
模式更改
************************************************************************/

  if (irrecv.decode(&results))
  {
    temp = results.value;
    if (temp == changeMode[0] || temp == changeMode[1]){
      model *= -1;
    

      // 三声 追踪模式
      if (model == 1)
      {
        changeMode[0] = 0xa11;
        changeMode[1] = 0x8CE1E3FF;
        newtone(pinBuzzer, 300, 100);
        delay(100);
        newtone(pinBuzzer, 300, 100);
        delay(200);
        newtone(pinBuzzer, 300, 200);
        lcd.clear();
      }

      // 两声 遥控模式
      else
      {
        changeMode[0] = 0xa00;
        changeMode[1] = 0x5698051A;
        _direction = '8';
        newtone(pinBuzzer, 500, 100);
        delay(200);
        newtone(pinBuzzer, 500, 200);
      }
      }
    
    irrecv.resume(); // Receive the next value
  }
  else
  {
    temp = 0;
  }

  /************************************************************************
跟踪模式
************************************************************************/

  if (model == 1)
  {

    if (temp == -1 && key == 1)
    {
      preTime = millis();
      key = 0;
    }
    else if (temp == -1 && key == 0)
    {
      trackingTime += millis() - preTime;
      preTime = millis();
    }
    /************************************************************************
    // LcdShow Begin
    ************************************************************************/
    if (trackingTime / 1000 == 30)
    {
      minTime += 1;
      trackingTime -= 30000;
    }

    lcd.home(); //光标定位到第一行
    lcd.print("Dist: ");
    lcd.print((trackingTime / 1000 + minTime * 60) * 16.18);
    lcd.print("cm");

    lcd.setCursor(0, 2); //光标定位到第二行
    lcd.print("Time: ");

    if (minTime > 1)
    {
      lcd.print(minTime / 2);
      lcd.print("min ");
    }
    lcd.print((minTime % 2) * 30 + trackingTime / 1000);
    lcd.print("s");

    // LcdShow End

    int leftBarrier = Distance_test(0);
    int rightBarrier = Distance_test(1);
    /*
       if((leftBarrier < 40 && leftBarrier > 20)|| (rightBarrier < 40 && rightBarrier > 20)){
        Serial.write('2);  // Slow
        Serial.print('2);  // Slow
      }
  */

  /*
    if (leftBarrier < 14 || rightBarrier < 14)
    {
      Serial.print('2'); // Circle
    }
    
    else
    {
      Serial.print('3'); // Circle
    }
    */

    Serial.print('3'); // Circle
  }

  

  /************************************************************************
遥控模式
************************************************************************/

  else
  {
    key = 1;

    if (temp == run_car[0] || temp == run_car[1])
    {
      _direction = '7';
    }
    else if (temp == stop_car[0] || temp == stop_car[1])
    {
      _direction = '8';
    }
    else if (temp == back_car[0] || temp == back_car[1] || temp == back_car[2] || temp == back_car[3] || temp == back_car[4] || temp == back_car[5])
    {
      _direction = '6';
    }
    else if (temp == left_car[0] || temp == left_car[1])
    {
      _direction = '5';
    }
    else if (temp == right_car[0] || temp == right_car[1])
    {
      _direction = '4';
    }
    else if (temp == left_turn[0] || temp == left_turn[1])
    {
      _direction = '9';
    }
    else if (temp == right_turn[0] || temp == right_turn[1])
    {
      _direction = 'a';
    }

    Serial.print(_direction);

    
  }
}
