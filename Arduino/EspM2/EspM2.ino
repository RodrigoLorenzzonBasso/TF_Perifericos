/*
*
* Publish luminosidade
* Subscribe dimmer
*
*/

#include <LiquidCrystal.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

void setup_peripherals();
void setup_wifi();
void reconnect();
void callback(char* topic, byte* payload, unsigned int length);

              //rs, enable, d4, d5, d6, d7
LiquidCrystal lcd(15, 13, 12, 14, 0, 2);
int ldr_pin = A0

// wifi setup
const char* ssid = "P30_IOT";
const char* password = "pucrs@2019";

// mqtt setup
const char* mqtt_server = "10.30.152.111";
const char* mqtt_user = "embarcados";
const char* mqtt_pass = "embarcados";

// connecting wifi with mqtt
WiFiClient espClient;
PubSubClient client(espClient);

// vars
char str[50];
int ldr_raw;
float luminosity;

void setup()
{
  Serial.begin(115200);

  setup_peripherals();
  setup_wifi();
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop()
{
  if(!client.connected())
  {
    reconnect();
  }
  client.loop();

  ldr_raw = analogRead(ldr_pin);

  // TODO: converter ldr_raw para valor em lux

  sprintf(str,"%f",luminosity);
  client.publish("basso_luminosidade", str);

  // subscribe dimmer está na função reconnect

  sprintf(str,"Lumi %4.1f", luminosity);
  Serial.println(str);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(str);
  lcd.setCursor(0,1);
  lcd.print(espClient.localIP());

}

// Functions

void setup_peripherals()
{
  lcd.begin(16, 2);
  // falta setup do ldr e dimmer da luminaria
}

void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("LocalIP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
 // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("espm2", mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("basso_comum", "espm2");
      // ... and resubscribe
      client.subscribe("basso_dimmer");
    } 
    else 
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println("");

  if(String(topic) == "basso_dimmer")
  {
    // faz algo no dimmer
  }

}
