/*
  SimpleMQTTClient.ino
  The purpose of this exemple is to illustrate a simple handling of MQTT and Wifi connection.
  Once it connects successfully to a Wifi network and a MQTT broker, it subscribe to a topic and send a message to it.
  It will also send a message delayed 5 seconds later.
*/

#include "EspMQTTClient.h"
#include <DHT.h>

#define LEDPIN 0     //定义IO0口控制继电器，进而控制其他一切开关型设备。 
#define DHTPIN 2      //定义DHT11模块连接管脚io2
#define DHTTYPE DHT11   // 使用DHT 11温度湿度模块 
DHT dht(DHTPIN, DHTTYPE);    //定义dht
float humi_read = 0, temp_read = 0;

EspMQTTClient client(
  "******",
  "*******",
  "******",  // MQTT Broker server ip
  "******",   // Can be omitted if not needed
  "******",   // Can be omitted if not needed
  "******",     // Client name that uniquely identify your device
  61613              // The MQTT port, default to 1883. this line can be omitted
);

void setup()
{
  Serial.begin(115200);
  pinMode(LEDPIN, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  digitalWrite(LEDPIN, LOW);           //定义io默认为高电平
  // Optionnal functionnalities of EspMQTTClient : 
  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  client.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overrited with enableHTTPWebUpdater("user", "password").
  client.enableLastWillMessage("TestClient/lastwill", "I am going offline");  // You can activate the retain flag by setting the third parameter to true
  dht.begin();
}


void onConnectionEstablished()
{
  // 订阅一个topic，可自行添加。
  client.subscribe("LED_STATUS", [](const String & payload) {
    //Serial.println(payload);
    if(payload.equals("1")){
        digitalWrite(LEDPIN, LOW);//开灯
        client.publish("LED_STATUS_control","1");
      }else {
        digitalWrite(LEDPIN, HIGH);//关灯
        client.publish("LED_STATUS_control","0");
      }
  });   
}

void loop()
{ 
  client.loop();  
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  char Temp[2],Humi[2];
  sprintf(Temp,"%f",t);
  sprintf(Humi,"%f",h);
  //data = strcat(Temp, Humi) ;
  if (isnan(h) || isnan(t))
  {
      Serial.println("Failed to read from DHT sensor!");
  }
  else
  {
      client.publish("temp", Temp);//每10秒获取一次温湿度发送。 
      client.publish("Humi", Humi);
  }
  delay(2000);
}
