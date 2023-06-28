#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <base64.h>
#include <string.h>


#define Unknown_Mode          1
#define Send_Mode             2
#define Receive_Mode          3

//A.hamid 
#define SERIAL_MONITOR        0 //set to 0 if you want to disable priting to serial monitor

/* WiFi settings */
//const char* ssid = "Omar's phone";//A.hamid 
//const char* password = "12345689";

const char* ssid = "WEE024C8";//A.hamid 
const char* password = "l4061095";

/* MQTT Broker settings */
const char* mqtt_server = "6acf025f42a74ff5b4b455a12145644c.s2.eu.hivemq.cloud";
const char* mqtt_username = "TEST_GRAD";
const char* mqtt_password = "TEST_GRAD_1p"; 
const int mqtt_port = 8883;

/* User Info */
const char* user_ID = "ESP8266_Client";
const char* Subscribed_Topic ="outTopic";
const char* TOPIC_TX= "C2V_topic";

/***************************************/

const String V_ID                   = "A12";    //A.hamid 

const char* V_ID_key                = "id";//A.hamid 
const char* COLLISION_WARNING_key   = "collision warning";//A.hamid 
const char* ROUTE_key               = "ROUTE";//A.hamid 
 

/***************************************/

#if SERIAL_MONITOR              //A.hamid disable printing 
#define LOG(x) Serial.println(x)
#else
#define LOG(x) //x
#endif

WiFiClientSecure espClient;
PubSubClient client(espClient);

StaticJsonDocument<200> doc;

char DataByte_Received_UART;
int Flag_Mode = Unknown_Mode;


const char IRG_Root_X1 [] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFnjCCA4YCCQDm7/FV5GARUDANBgkqhkiG9w0BAQ0FADCBqTELMAkGA1UEBhMC
REUxEDAOBgNVBAgMB0JhdmFyaWExETAPBgNVBAcMCEVybGFuZ2VuMRcwFQYDVQQK
DA5Tb2Z0YmxhZGUgR21iSDEZMBcGA1UECwwQd3d3LnNvZnRibGFkZS5kZTEcMBoG
A1UEAwwTU29mdGJsYWRlIFJvb3QgQ0EgMTEjMCEGCSqGSIb3DQEJARYUY29udGFj
dEBzb2Z0YmxhZGUuZGUwHhcNMjMwMzE2MDc0NjM2WhcNMjMwNzE0MDc0NjM2WjB4
MQswCQYDVQQGEwJERTEMMAoGA1UECAwDTlJXMREwDwYDVQQHDAhFcmxhbmdlbjES
MBAGA1UECgwJU29mdGJsYWRlMRIwEAYDVQQDDAlTb2Z0YmxhZGUxIDAeBgkqhkiG
9w0BCQEWEW5pa29Ac29mdGJsYWRlLmRlMIICIjANBgkqhkiG9w0BAQEFAAOCAg8A
MIICCgKCAgEAqUE9p71j0zzCIQJlacOrA/k7EvNO98jdVUS77opS2ONUg6mwWZ8H
QJrJbce00xN8RPTPfKG4gmpWYY9JEIl9U4baYGAjMALiEaInxODZ8TF1IqyKdmyL
9Q7t+Fd+goGZy1rZUeLDxU40OXZ0okJu1vc5lrXkVDQtscPeEvB/HORsV9+9LM0v
hUIx+S+SmF0CCh/oNWlmIW5lL0kViD49YC8jaULLzEgljh4rV3rQwKp40MBmFJkt
E3eMa3vfjHwDTlsTY84gLhAY5gYPzlTJPCXFrMlmiSGc5/tl3TTyT1+TJAEP3pGJ
ktRbEmlQi5FwOaGgSfbUj+5Sq1t+ZLp0stmB2wYh0izN/zklTB9FJMN93o/dbqVm
TS4ygxiUD3wuxL/yQryMoSHkPgZQfEEd4fHc22eetCrBwJRgs4wP528WfZ/Kup8o
SpAXexJlptFfiLY00XzXNxDWfzwEKBbJ5JUnNRIvzZ2Pul1ZYZ+/FKjjLUG7hmBW
nkkmUcu3758bbCRccwqiIlLcQrzx48TnOkiIpEoIEPKpBFkwpURLYMyrMfK9CSFO
l28/xYqf7WNdywABCmqCIiQIyvoIHDvYF3HR3fcEJ/4kqAA+Zt0A6X3uK2Bvsr9h
LuwaZnKPC2r3ghvqKIvTzvztj7isL4RRTrkf/0hZ/Xg5MIxsZCsRZ3cCAwEAATAN
BgkqhkiG9w0BAQ0FAAOCAgEAdfO9yG50D23lUX/YrsE2j42jwBbVvqHK9cfQe+4o
DTAw+c70WB0J/ZDuQGAcQjTkBt1VYj+7g/WLQV6C4SmUfeTLwzkpzUk8g1jwzQz+
VfL/hib2lSN6+OdA+cdrbzmRG0RZvfTyHZqMHKtexTZJlOVcoZ6duCd2v3rC6RgS
swqCtQ2alZpDydWdmFciJgQN4v/O2auznqIpGs9XgBgsvQl6PTW3gs/3lFsfiHlN
9Dbfq0kC0HdLSyfPvZoyavFDSMY2JLbnCx9s0DzHwXov+RJSqbkdPiDcj+cMtSvo
4BwgLVBQYIn1cM6J2eICdC/2igvELf6Fxzju/8DIrnUVf8jrZS2J7drgtI76hw5r
06HH3s07XxL9Ff/7HuYwciWdw0IwE5pki/Icp7IY7e5+FbtxpsDm7nK7K8yYNKjZ
kC2lCi0iAMTPvtK9Z5lySl5T1o2c3v91QSY7kOOXNcb66crHvyzUzpZ5GoHMb1+K
y80Ix9YrImb5Nva0manycccq+NU6NRAh/dCEL7i9tIiRHgk88IdspUsiAn8dKecj
IxKoQkD7yEBLEqTNcs4aKID9SagNaSUVJuUJDreo3UaANj6WkbJHr/cqU8eE0c2p
ybbnHfpQeK4EPN1hJ3zcZk0ANaXXuasDNEPBK3r/9heHNv3MePVksqLLkwf+NV10
UM8=
-----END CERTIFICATE-----

)EOF";



/* Connect ESP8266 To Wifi Network */
void setup_wifi(void) 
{
  /* Delay 10 Milli Seconds */
  delay(10);
  
  /* Set Wifi In Station Mode */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    /* Tring To reconnect To Wifi Network In 500 Milli Seconds */
    LOG("failed WiFI");
    delay(500);
  }
  LOG("connected to WiFI");
}


/* Blocking Reconnect To MQTT Broker in case it can't connect */
void Client_Reconnect_To_MQTT_Broker(void) 
{
  /* Loop until we're reconnected */ 
  while (!client.connected()) 
  {
    /* Attempting to connect */
    if (client.connect(user_ID, mqtt_username, mqtt_password)) 
    {
      /* Subscribe the topics here */
      LOG("connected succesfully");
      client.subscribe(TOPIX_TX);
    } 
    else 
    {
      /* Trying Again In 5 Seconds */
      LOG("failed to connect retrying");
      delay(500);
    }
  }
}


/* CallBack When receiveng New Messages To Subscribed Topics */
void callback(char* topic, byte* payload, unsigned int length) 
{
  String Incomming_Message = "";

  if ((Flag_Mode == Unknown_Mode) || (Flag_Mode == Send_Mode))
  {
    Flag_Mode = Receive_Mode;
  }
  else
  {
    /* Send The Incomming Message To UART In case Of Receive Mode */
    Incomming_Message.clear();

    Flag_Mode = Receive_Mode;
  
    for (int i = 0; i < length; i++) 
    {
      Incomming_Message += (char)payload[i];  
    }
    
  // Deserialize the JSON document
  //A.hamid recieve msg and parseit as a json
  DeserializationError error = deserializeJson(doc, Incomming_Message); 

  // Test if parsing succeeds.
  if (error) {
    LOG(F("deserializeJson() failed: "));
    LOG(error.f_str());
    return;
  }
//A.hamid store important variables in variables
  const String msg_v_id = doc[V_ID_key];
  int collision_flag = doc[COLLISION_WARNING_key];
  String route_direction = doc[ROUTE_key];
//route_direction = doc[ROUTE_key];
//  const char* r = "r";
//  const char* R = "R";
  LOG(COLLISION_WARNING_key);
  LOG(collision_flag);
  LOG(ROUTE_key);
  LOG(route_direction);

  if(msg_v_id !=V_ID ){//A.hamid if ids missmaatch skip msg
//    LOG("miss matching id");
//    LOG(msg_v_id);
//    LOG(V_ID);
    
//    Serial.print();
  }else{
      if(collision_flag){
        // Serial.print("Warning");
        Serial.print("W");

        }else{
          // Serial.print("Safe");
          Serial.print("N");

          }
        
      if(route_direction == "r"|| route_direction == "R" ){
      //  Serial.print("Right");
       Serial.print("R");

// Serial.print(1);
        
        }else if(route_direction == "l" || route_direction == "L" ){
        //  Serial.print("Left");
         Serial.print("L");

// Serial.print(2);
          }else{
          //  Serial.print("Straight");
            Serial.print("S");
  
// Serial.print(0);
            }
//      Serial.println(route_direction);
    
    }

    
  }
}


/* void setup */
void setup() 
{

  /* Set Baud Rate */
  Serial.begin(115200);


  setup_wifi();
  
  
  espClient.setInsecure();
  /*if (espClient.setFingerprint(FingerPrint))
  {
    pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  }*/

  /*setClock();
  X509List cert(IRG_Root_X1);
  espClient.setTrustAnchors(&cert);
  */

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}


/* void loop */
void loop() 
{



  if (!client.connected()) 
  {
    /* Set Flag Mode */
    Flag_Mode = Unknown_Mode;

    Client_Reconnect_To_MQTT_Broker();
  }
  
  client.loop();

  /* Initalize String Buffer To Hold The Published Message once */
  String Published_Message_Buffer = "";   

  if (Serial.available() > 0)
  {
    delay(1);   

    /* Buffer The Inputs While Still Receving New Char */
//    do
//    {
//      DataByte_Received_UART = Serial.read();
//
//      Published_Message_Buffer += DataByte_Received_UART;
//      
//      delayMicroseconds(50);
//            
//    } while (Serial.available() > 0);
 Serial.setTimeout(100); // Set timeout to 100 milliseconds

    /* Buffer the inputs */
    char buffer[500];
    int bytesRead = Serial.readBytes(buffer, 500);

    if (bytesRead > 0) {
      /* Copy buffer to Published_Message_Buffer */
      Published_Message_Buffer = String(buffer);
    }
 }

//while (Serial.available()==0){};
//Published_Message_Buffer=Serial.readString();


  /* Send The Message While The Buffer Isn't Empty */
  if (!Published_Message_Buffer.isEmpty())
  {
    /* Set Flag Mode */
    Flag_Mode = Send_Mode;
    client.publish(Subscribed_Topic, Published_Message_Buffer.c_str(),true);
    
    //Serial.println(Published_Message_Buffer);

    /* Clear The Buffer To Hold A New Message */
    Published_Message_Buffer.clear();
  }
}
