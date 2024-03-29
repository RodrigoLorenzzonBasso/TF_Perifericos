/*
*
* Publish temperatura
* Publish umidade
* Subscribe motor
*
*/

#include <LiquidCrystal.h>
#include <DHTesp.h>
#include <Servo.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <math.h>

void gira_motor();
void gira_motor2();
void setup_peripherals();
void setup_wifi();
void reconnect();
void callback(char* topic, byte* payload, unsigned int length);

Servo servo;
int servoPin = 4;

DHTesp dht;
int dhtPin = 16;
              //rs, enable, d4, d5, d6, d7
LiquidCrystal lcd(15, 13, 12, 14, 2, 0);

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
char humi[30];
char temp[30];
float humidity;
float temperature;
char str[50];
String local_ip;

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
  delay(dht.getMinimumSamplingPeriod());

  if(!client.connected())
  {
    reconnect();
  }
  client.loop();

  humidity = dht.getHumidity();
  temperature = dht.getTemperature();

  if(!isnan(temperature)){
    sprintf(str,"%03.2f",temperature);
    client.publish("basso_temperatura", str);
    sprintf(str,"%03.2f",humidity);
    client.publish("basso_umidade", str);

    sprintf(str,"%2.1f%%  %2.1fC",humidity,temperature);
    Serial.println(str);
  
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(str);
    lcd.setCursor(0,1);
    lcd.print(local_ip);
  }

  // subscribe motor está na função reconnect
  // fazer algo com o motor


}

// Functions

void gira_motor()
{
  int pos = 0;
  for(pos = 0; pos < 60; pos++){ //PARA "pos" IGUAL A 0, ENQUANTO "pos" MENOR QUE 180, INCREMENTA "pos"
    servo.write(pos); //ESCREVE O VALOR DA POSIÇÃO QUE O SERVO DEVE GIRAR
    delay(15); //INTERVALO DE 15 MILISSEGUNDOS
  }    
}

void gira_motor2()
{
  int pos = 60;
  for(pos = 60; pos < 120; pos++){ //PARA "pos" IGUAL A 0, ENQUANTO "pos" MENOR QUE 180, INCREMENTA "pos"
    servo.write(pos); //ESCREVE O VALOR DA POSIÇÃO QUE O SERVO DEVE GIRAR
    delay(15); //INTERVALO DE 15 MILISSEGUNDOS
  }    
}

void gira_motor3()
{
  int pos = 120;
  for(pos = 120; pos < 180; pos++){ //PARA "pos" IGUAL A 0, ENQUANTO "pos" MENOR QUE 180, INCREMENTA "pos"
    servo.write(pos); //ESCREVE O VALOR DA POSIÇÃO QUE O SERVO DEVE GIRAR
    delay(15); //INTERVALO DE 15 MILISSEGUNDOS
  }    
}

void setup_peripherals()
{
  dht.setup(dhtPin, DHTesp::DHT11); // Connect DHT sensor to GPIO 16
  servo.attach(servoPin);
  Serial.println("Coloque o display pfv");
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

  local_ip = String(WiFi.localIP());
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("LocalIP address: ");
  Serial.println(local_ip);
}

void reconnect() {
 // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("espm1", mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("basso_comum", "espm1");
      // ... and resubscribe
      client.subscribe("basso_motor");
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

  if(String(topic) == "basso_motor")
  {
    if((char)payload[0] == '1')
    {
      gira_motor();
    }
    else if((char)payload[0] == '2')
    {
      gira_motor2();
    }
    else if((char)payload[0] == '3')
    {
      gira_motor3();
    }
  }
}
