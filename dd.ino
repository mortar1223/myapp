/*  NETPIE ESP8266 basic sample                            */
/*  More information visit : https://netpie.io             */

#include <ESP8266WiFi.h>
#include <MicroGear.h>
#include <time.h>

const char* ssid     = "Mortar 2G";
const char* password = "1236547890";

#define APPID   "IotProjectGroup2"
#define KEY     "Nd84hROnwsmttms"
#define SECRET  "2g5zD4Wzqe1SdI4vnJ33pZHb6"
#define ALIAS   "Mortar"
#define CLIENT1  "doraemon"
#define CLIENT2  "Board"

/*
 * Led 1       =  D1
 * Led 2       =  D2
 * Led 3       =  D3
 * Button      =  D4
 * Buzzer      =  D5
 * Led Warning =  D8
 */

byte state = LOW;
String Led1 = String(digitalRead(D1));
String Led2 = String(digitalRead(D2));
String Led3 = String(digitalRead(D3));
String message = "Led1 = " + Led1 + " Led2 = " + Led2  + " Led3 = " + Led3;

WiFiClient client;

int timer = 0;
MicroGear microgear(client);

/* If a new message arrives, do this */
void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
  Serial.print("Incoming message --> ");
  msg[msglen] = '\0';
  Serial.println((char *)msg);
  String smsg((char*)msg);
  if (smsg == "led1off") {
    digitalWrite(D1, LOW);
    actionSound(); actionSound(); actionSound();
  } else if (smsg == "led1on") {
    digitalWrite(D1, HIGH);
    actionSound();
  } else if (smsg == "led2off") {
    digitalWrite(D2, LOW);
    actionSound(); actionSound(); actionSound();
  } else if (smsg == "led2on") {
    digitalWrite(D2, HIGH);
    actionSound();
  } else if (smsg == "led3off") {
    digitalWrite(D3, LOW);
    actionSound(); actionSound(); actionSound();
  } else if (smsg == "led3on") {
    digitalWrite(D3, HIGH);
    actionSound();
  } else if (smsg == "ledAllon") {
    digitalWrite(D1, HIGH);
    digitalWrite(D2, HIGH);
    digitalWrite(D3, HIGH);
    actionSound();
  } else if (smsg == "ledAlloff") {
    digitalWrite(D1, LOW);
    digitalWrite(D2, LOW);
    digitalWrite(D3, LOW);
    actionSound(); actionSound(); actionSound(); actionSound(); actionSound(); actionSound();
  } else if (smsg == "check") {
    setLedState();
    microgear.chat(CLIENT1, message);
  } else if (smsg == "freeboardcheck") {
    setLedState();
    microgear.chat(CLIENT2, message);
  }


}

void onFoundgear(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.print("Found new member --> ");
  for (int i = 0; i < msglen; i++)
    Serial.print((char)msg[i]);
  Serial.println();
}

void onLostgear(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.print("Lost member --> ");
  for (int i = 0; i < msglen; i++)
    Serial.print((char)msg[i]);
  Serial.println();
}

/* When a microgear is connected, do this */
void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE...");
  /* Set the alias of this microgear ALIAS */
  microgear.setAlias(ALIAS);
  microgear.subscribe("/led");
  microgear.chat(CLIENT1, message);
  microgear.chat(CLIENT2, message);
}


void setup() {
  /* Add Event listeners */
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);

  //Warning OUTPUT
  pinMode(D5, OUTPUT);
  pinMode(D8, OUTPUT);
  
  /* Call onMsghandler() when new message arraives */
  microgear.on(MESSAGE, onMsghandler);

  /* Call onFoundgear() when new gear appear */
  microgear.on(PRESENT, onFoundgear);

  /* Call onLostgear() when some gear goes offline */
  microgear.on(ABSENT, onLostgear);

  /* Call onConnected() when NETPIE connection is established */
  microgear.on(CONNECTED, onConnected);

  Serial.begin(115200);
  Serial.println("Starting...");

  /* Initial WIFI, this is just a basic method to configure WIFI on ESP8266.                       */
  /* You may want to use other method that is more complicated, but provide better user experience */
  if (WiFi.begin(ssid, password)) {
    while (WiFi.status() != WL_CONNECTED) {
      digitalWrite(D8, HIGH);
      delay(50);
      digitalWrite(D8, LOW);
      delay(500);
      Serial.print(".");
    }
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  /* Initial with KEY, SECRET and also set the ALIAS here */
  microgear.init(KEY, SECRET, ALIAS);

  /* connect to NETPIE to a specific APPID */
  microgear.connect(APPID);

    /* Call time from server */
  configTime(0, 0, "th.pool.ntp.org"); //ดึงเวลาจาก Server
  Serial.println("\nLoading time");
  while (!time(nullptr)) {
    Serial.print("*");
    delay(1000);
  }
}
int led = 0;
void loop() {
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
//  Serial.println(String(ctime(&now)));
  if(p_tm->tm_hour == 11 && p_tm->tm_min == 0 && p_tm->tm_sec == 0){
     digitalWrite(D1,HIGH);
     delay(1000);
  } else if (p_tm->tm_hour == 23 && p_tm->tm_min == 0 && p_tm->tm_sec == 0){
    digitalWrite(D1,LOW);
     delay(1000);
  }
  
  byte button = digitalRead(D4);
  if (button == LOW && state == LOW) {
    state = HIGH;
    digitalWrite(D1, state);
    digitalWrite(D2, state);
    digitalWrite(D3, state);
    setLedState();
    microgear.chat(CLIENT1, message);
    microgear.chat(CLIENT2, message);
    actionSound();
    Serial.println("LED Turned ON");
    delay(200);
  } else if (button == LOW && state == HIGH) {
    state = LOW;
    digitalWrite(D1, state);
    digitalWrite(D2, state);
    digitalWrite(D3, state);
    setLedState();
    microgear.chat(CLIENT1, message);
    microgear.chat(CLIENT2, message);
    actionSound();actionSound();actionSound();
    Serial.println("LED Turned OFF");
    delay(200);
  }

  /* To check if the microgear is still connected */
  if (microgear.connected()) {
    Serial.println("connected");

    /* Call this method regularly otherwise the connection may be lost */
    microgear.loop();

    if (timer >= 1000) {
      Serial.println("Publish...");

      /* Chat with the microgear named ALIAS which is myself */
      microgear.chat(ALIAS, "Hello");
      timer = 0;
    }
    else timer += 100;
  }
  else {
    digitalWrite(D8, HIGH);
    delay(50);
    digitalWrite(D8, LOW);
    Serial.println("connection lost, reconnect...");
    if (timer >= 5000) {
      microgear.connect(APPID);
      timer = 0;
    }
    else timer += 100;

  }
  delay(100);


}

void actionSound() {
  digitalWrite(D5, HIGH);
  delay(100);
  digitalWrite(D5, LOW);
}
void setLedState() {
  Led1 = String(digitalRead(D1));
    Led2 = String(digitalRead(D2));
    Led3 = String(digitalRead(D3));
    message = "Led1 = " + Led1 + " Led2 = " + Led2  + " Led3 = " + Led3;
}
