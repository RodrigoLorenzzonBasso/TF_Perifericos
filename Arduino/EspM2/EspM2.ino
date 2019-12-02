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
LiquidCrystal lcd(15, 13, 12, 14, 16, 2);
int ldr_pin = A0;
int dimmer_pin = 5;

// wifi setup
const char* ssid = "P30_IOT";
const char* password = "pucrs@2019";

// mqtt setup
const char* mqtt_server = "10.30.157.153";
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

  luminosity = (1024 - ldr_raw) / 9;
  
  sprintf(str,"%5.2f",luminosity);
  client.publish("basso_luminosidade", str);

  sprintf(str,"Lumi %5.2f", luminosity);
  Serial.println(str);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(str);
  lcd.setCursor(0,1);
  lcd.print(espClient.localIP());

  delay(500);

}

// Functions

void setup_peripherals()
{
  Serial.println("Coloca o display");
  delay(5000);
  lcd.begin(16, 2);
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
    char *end = nullptr;
    long value = strtol((const char *) payload, &end, 10);
    Serial.print("Valor convertido:");
    Serial.println(value);
    // converter para AD 0 --- 1023
    float conv = (value*255.0)/100.0;
    int c = int(conv);
    analogWrite(dimmer_pin, c);
  }

}
