#include<Servo.h>
#include<Ethernet.h>
#include<SPI.h>
#include<SD.h>

//Pannelli
Servo SolarPanel;
int PanelPin=2;
int PanelRotate=A0;
int PanelValue=0;
int Voltage=A1;
int VoltageValue=0;
int Current=A2;
int CurrentValue=0;
int Power=0;
int PanelTemp=A3;
int PanelTempValue=0;

//Garage
Servo GarageMotor;
const int GarageButton=3;
 int OldGarageState=1;
 int NewGarageState=0;
 int GaragePin=5;
 int AbilGarage=0;

 //Garden
 int GardenLedSX=9;
int GardenLedC=14;
int GardenLedDX=15;
const int GardenButton=16;
int OldGardenState=1;
int NewGardenState=0;
int AbilGarden=0;

//Floor 1
const int Floor1Button=17;
int OldFloor1State=1;
int NewFloor1State=0;
int AbilFloor1=0;
int LedFloor1=18;
int TempFloor1=A4;
int Floor1Value=0;
int DoorAlarm=19;
int DoorState=0;
int Win11Alarm=20;
int Win11State=0;
int Win12Alarm=21;
int Win12State=0;
int FanFloor1=22;

//Floor 2
const int Floor2Button=23;
int OldFloor2State=1;
int NewFloor2State=0;
int AbilFloor2=0;
int LedFloor2=24;
int TempFloor2=A5;
int Floor2Value=0;
int Win21Alarm=25;
int Win21State=0;
int Win22Alarm=26;
int Win22State=0;
int PeltierPin=27;
int AirConditioning=28;

//Alarm
const int AlarmButton=29;
int OldAlarmState=1;
int NewAlarmState=0;
int AbilAlarm=0;
int AlarmNoise=30;

//Misura
double Celsius1;
double Tmin1=1000;
double Tmax1=0; 
double Celsius2;
double Tmin2=1000;
double Tmax2=0;
double CelsiusPanel;
double TminPanel=1000;
double TmaxPanel=0;
double R1=10000.0;
double V_IN=5.0;
double A=1.189148e-3;
double B=2.34125e-4;
double C=8.76741e-8;
double K=6;

//Ethernet
byte mac[]= {0x90,0xA2,0xDA,0x0F,0x1A,0x0E};
byte ip[] = {192,168,80,20};
EthernetServer shttp(80);
const int chipSelect = 4;
void setup() 
{
//PannelloSolare
  SolarPanel.attach(PanelPin);

   //Garage
  GarageMotor.attach(GaragePin);
  pinMode(GarageButton, INPUT);

  //Garden
   pinMode(GardenLedSX, OUTPUT);
 pinMode(GardenLedC, OUTPUT);
 pinMode(GardenLedDX, OUTPUT);
 pinMode(GardenButton,INPUT);

 //Floor 1
 pinMode(LedFloor1, OUTPUT);
 pinMode(Floor1Button, INPUT);
 pinMode(DoorAlarm, INPUT);
 pinMode(Win11Alarm, INPUT);
 pinMode(Win12Alarm, INPUT);
 pinMode(FanFloor1, OUTPUT);

 //Floor 2
 pinMode(LedFloor2, OUTPUT);
 pinMode(Floor2Button, INPUT);
 pinMode(Win21Alarm, INPUT);
 pinMode(Win22Alarm, INPUT);
 //pinMode(PeltierPin, OUTPUT);
//pinMode(AirConditioning, OUTPUT);

//Alarm
pinMode(AlarmButton, INPUT);
pinMode(AlarmNoise, OUTPUT);

 //Ethernet
 Ethernet.begin(mac,ip);
shttp.begin();
pinMode(10, OUTPUT);
if (!SD.begin(chipSelect)) {
return;
}
Serial.begin(9600);
}

void loop() {
  //Pannello Solare
PanelValue=analogRead(PanelRotate);
VoltageValue=analogRead(Voltage);
CurrentValue=analogRead(Current);
SolarPanel.write(PanelValue);

//Garage
NewGarageState=digitalRead(GarageButton);
if(NewGarageState==LOW && OldGarageState==HIGH)
{
  AbilGarage=1-AbilGarage;
}
OldGarageState=NewGarageState;
if(AbilGarage==1)
{
  GarageMotor.write(200);
}
else if(AbilGarage==0)
{
  GarageMotor.write(-100);
}

//Garden
NewGardenState=digitalRead(GardenButton);
if(NewGardenState==LOW && OldGardenState==HIGH)
{
  AbilGarden=1-AbilGarden;
}
OldGardenState=NewGardenState;
if(AbilGarden==1)
{
  digitalWrite(GardenLedSX,HIGH);
  digitalWrite(GardenLedC,HIGH);
  digitalWrite(GardenLedDX,HIGH);
}
else
{
  digitalWrite(GardenLedSX,LOW);
  digitalWrite(GardenLedC,LOW);
  digitalWrite(GardenLedDX,LOW); 
}

//Floor 1
NewFloor1State=digitalRead(Floor1Button);
DoorState=digitalRead(DoorAlarm);
Win11State=digitalRead(Win11Alarm);
Win12State=digitalRead(Win12Alarm);
if(NewFloor1State==LOW && OldFloor1State==HIGH)
{
  AbilFloor1=1-AbilFloor1;
}
OldFloor1State=NewFloor1State;
if(AbilFloor1==1)
{
  digitalWrite(LedFloor1,HIGH);
}
else
{
  digitalWrite(LedFloor1,LOW);
}

if(AbilFloor1==1)
{
  digitalWrite(LedFloor1,HIGH);
}
else
{
  digitalWrite(LedFloor1,LOW);
}

//Floor 2
NewFloor2State=digitalRead(Floor2Button);
Win21State=digitalRead(Win21Alarm);
Win22State=digitalRead(Win22Alarm);
if(NewFloor2State==LOW && OldFloor2State==HIGH)
{
  AbilFloor2=1-AbilFloor2;
}
OldFloor2State=NewFloor2State;
if(AbilFloor2==1)
{
  digitalWrite(LedFloor2,HIGH);
}
else if(AbilFloor2==0)
{
  digitalWrite(LedFloor2,LOW);
}

MisuraFloor1();
MisuraFloor2();
MisuraPanel();

//Alarm
NewAlarmState=digitalRead(AlarmButton);
if(NewAlarmState==LOW && OldAlarmState==HIGH)
{
  AbilAlarm=1-AbilAlarm;
}
OldAlarmState=NewAlarmState;

if(Win11State==HIGH   && AbilAlarm==1)
{
tone(AlarmNoise,100,100);
  delay(500);
  noTone(AlarmNoise);  
}
else if(Win11State==LOW || AbilAlarm==0)
{
digitalWrite(AlarmNoise, LOW);
}
if(Win12State==HIGH && AbilAlarm==1)
{
tone(AlarmNoise,100,100);
  delay(500);
  noTone(AlarmNoise);  
}
else if(Win12State==LOW || AbilAlarm==0)
{
digitalWrite(AlarmNoise, LOW);
}
if(DoorState==HIGH && AbilAlarm==1)
{
tone(AlarmNoise,100,100);
  delay(500);
  noTone(AlarmNoise);  
}
else if(DoorState==LOW || AbilAlarm==0)
{
digitalWrite(AlarmNoise, LOW);
}
if(Win21State==HIGH && AbilAlarm==1)
{
tone(AlarmNoise,100,100);
  delay(500);
  noTone(AlarmNoise);  
}
else if(Win21State==LOW || AbilAlarm==0)
{
digitalWrite(AlarmNoise, LOW);
}
if(Win22State==HIGH && AbilAlarm==1)
{
tone(AlarmNoise,1,100);
  delay(500);
  noTone(AlarmNoise);  
}
else if(Win22State==LOW || AbilAlarm==0)
{
  digitalWrite(AlarmNoise, LOW);
}
EthernetClient client = shttp.available(); // in ascolto
if (client) {
while(client.connected() ){
while(client.available() ) { // arrivato carattere
char c= client.read(); }
client.println("HTTP/1.1 200 OK");
client.println("Content-Type: text/html");
client.println("Connection: close");
client.println();
File dataFile = SD.open("indexen1.htm");
if (dataFile) {
while (dataFile.available()) {
client.write(dataFile.read());
//istruzioni
}
dataFile.close();
} // end if
  client.stop();
}
} // if
}


double SteinhartHart(double R)
  {
    double logR=log(R);
    double logR3=logR*logR*logR;
   return 1.0/(A+B*logR+C*logR3);
  }

  void MisuraFloor1()
{
  double adc_raw=analogRead(TempFloor1);
  delay(10);
  adc_raw=adc_raw+analogRead(TempFloor1);
  delay(10);
  adc_raw=(adc_raw+analogRead(TempFloor1))/3;
  double V=adc_raw/1024*V_IN;
  double R_th=(R1*V)/(V_IN-V);
  double kelvin=SteinhartHart(R_th)-V*V/(K*R_th);
  Celsius1=kelvin-273.15;
  
  if(Celsius1<=Tmin1)
  Tmin1=Celsius1;
  
  if(Tmax1<=Celsius1)
  Tmax1=Celsius1;
  
  if(Celsius1>=15 && Celsius1<=25)
 { 
  
  digitalWrite(FanFloor1,HIGH);
 
    }
    else
    {
  digitalWrite(FanFloor1,LOW);
    }   
}

void MisuraFloor2()
{
  double adc_raw=analogRead(TempFloor2);
  delay(10);
  adc_raw=adc_raw+analogRead(TempFloor2);
  delay(10);
  adc_raw=(adc_raw+analogRead(TempFloor2))/3;
  double V=adc_raw/1024*V_IN;
  double R_th=(R1*V)/(V_IN-V);
  double kelvin=SteinhartHart(R_th)-V*V/(K*R_th);
  Celsius2=kelvin-273.15;
  
   if(Celsius2<=Tmin2)
  Tmin2=Celsius2;
  
  if(Tmax2<=Celsius2)
  Tmax2=Celsius2;
  
  if( Celsius2>=15 && Celsius2<=25 )
 { 
  digitalWrite(AirConditioning, HIGH);
  digitalWrite(PeltierPin, HIGH);

    }
 else
 { 
  digitalWrite(AirConditioning, LOW);
  digitalWrite(PeltierPin, LOW);

    }
}

void MisuraPanel()
{
  double adc_raw=analogRead(PanelTemp);
  delay(10);
  adc_raw=adc_raw+analogRead(PanelTemp);
  delay(10);
  adc_raw=(adc_raw+analogRead(PanelTemp))/3;
  double V=adc_raw/1024*V_IN;
  double R_th=(R1*V)/(V_IN-V);
  double kelvin=SteinhartHart(R_th)-V*V/(K*R_th);
  CelsiusPanel=kelvin-273.15;
  
   if(CelsiusPanel<=TminPanel)
  TminPanel=CelsiusPanel;
  
  if(TmaxPanel<=CelsiusPanel)
  TmaxPanel=CelsiusPanel;
  

}
