#include <LiquidCrystal.h>
#include <DHTesp.h>
#include <Servo.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

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
LiquidCrystal lcd(15, 13, 12, 14, 0, 2);

char estado = 0; // 0 fechado 1 aberto

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

  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();

  char temp[30];
  sprintf(temp,"Temperatura %2.1fC",temperature);

  char humi[30];
  sprintf(humi,"Umidade %2.1f%%",humidity);

  Serial.println(humi);
  Serial.println(temp);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(temp);
  lcd.setCursor(0,1);
  lcd.print(humi);

  ////////////////// Wifi //////////////////////

  WiFiClient server;
  
  if (!server.connect(host, port)) {
    Serial.println("connection failed");
    return;
  }

  // connected

  String line = server.readStringUntil(';');
  Serial.print("Recebido do Servidor: ");
  Serial.println(line);

  char str[30];
  sprintf(str,"%02.1f %02.1f;",temperature,humidity);

  server.print(str);

  if(line == "abrir")
  {
     if(estado == 0)
     {
       gira_motor();
       estado = 1;
     } 
  }
  else if(line == "fechar")
  {
    if(estado == 1)
    {
      gira_motor2();
      estado = 0; 
     }
  }
  
  ////////////////// Wifi //////////////////////
}

// Functions

void gira_motor()
{
  int pos = 0;
  for(pos = 0; pos < 180; pos++){ //PARA "pos" IGUAL A 0, ENQUANTO "pos" MENOR QUE 180, INCREMENTA "pos"
    servo.write(pos); //ESCREVE O VALOR DA POSIÇÃO QUE O SERVO DEVE GIRAR
    delay(15); //INTERVALO DE 15 MILISSEGUNDOS
  }    
}

void gira_motor2()
{
  int pos = 0;
  for(pos = 180; pos < 0; pos--){ //PARA "pos" IGUAL A 0, ENQUANTO "pos" MENOR QUE 180, INCREMENTA "pos"
    servo.write(pos); //ESCREVE O VALOR DA POSIÇÃO QUE O SERVO DEVE GIRAR
    delay(15); //INTERVALO DE 15 MILISSEGUNDOS
  }    
}

void setup_peripherals()
{
  dht.setup(dhtPin, DHTesp::DHT11); // Connect DHT sensor to GPIO 16
  servo.attach(servoPin);
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
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("EspM1",mqtt_user,mqtt_pass)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("temperatura", "hello world");
      // ... and resubscribe
      client.subscribe("umidade");
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
}