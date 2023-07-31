#define BLYNK_PRINT Serial
#include <SimpleTimer.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SoftwareSerial.h>

#define BLYNK_TEMPLATE_ID "TMPL6TJ7EHyJq"
#define BLYNK_TEMPLATE_NAME "WWTP TRIAL 1"
#define BLYNK_AUTH_TOKEN "CAz1TEZLZJeXGC4CF_7XNCfobW4uugRY"

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "insyaallah";
char pass[] = "279september";

int dataSwitch;

SimpleTimer timer;

String myString;

SoftwareSerial espSerial(D7,D8);

double rtdsTank1, rtdsTank5, rtempTank1, rtempTank5, rjumlahProses, rphTank1, rphTank5;

void myTimerEvent(){
  Blynk.virtualWrite(V3,millis()/1000);
}

void setup() {
  Serial.begin(115200);
  espSerial.begin(115200);
  Blynk.begin(auth,ssid,pass,"blynk.cloud",80);
  timer.setInterval(1000L,tdsTank1);
  timer.setInterval(1000L,tdsTank5);
  timer.setInterval(1000L,tempTank1);
  timer.setInterval(1000L,tempTank5);
  timer.setInterval(1000L,phTank1);
  timer.setInterval(1000L,phTank5);
  timer.setInterval(1000L,jumlahProses);
}

void loop() {
  if(espSerial.available() == 0){
    Blynk.run();
    timer.run();
  }
  if(espSerial.available() > 0){
    char rdata = espSerial.read();
    Serial.print(rdata);
    myString = myString + rdata;
    if(rdata == '\n'){
      String l = getValue(myString,',',0);
      String m = getValue(myString,',',1);
      String n = getValue(myString,',',2);
      String o = getValue(myString,',',3);
      String p = getValue(myString,',',4);
      String q = getValue(myString,',',5);
      String r = getValue(myString,',',6);


      rtdsTank1 = l.toDouble();
      rtdsTank5 = m.toDouble();
      rtempTank1 = n.toDouble();
      rtempTank5 = o.toDouble();
      rphTank1 = p.toDouble();
      rphTank5 = q.toDouble();
      rjumlahProses = r.toDouble();
      myString = "";
    }
  }
}

void tdsTank1(){
  double sdata = rtdsTank1;
  Blynk.virtualWrite(V20, sdata);
}

void tdsTank5(){
  double sdata = rtdsTank5;
  Blynk.virtualWrite(V21, sdata);
}

void tempTank1(){
  double sdata = rtempTank1;
  Blynk.virtualWrite(V24, sdata);
}

void tempTank5(){
  double sdata = rtempTank5;
  Blynk.virtualWrite(V23, sdata);
}

void phTank1(){
  double sdata = rphTank1;
  Blynk.virtualWrite(V25, sdata);
}

void phTank5(){
  double sdata = rphTank5;
  Blynk.virtualWrite(V26, sdata);
}

void jumlahProses(){
  double sdata = rjumlahProses;
  Blynk.virtualWrite(V5, sdata);
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

/*BLYNK_WRITE(V10){
  dataSwitch = param.asInt();
  Serial.print(dataSwitch);
}*/