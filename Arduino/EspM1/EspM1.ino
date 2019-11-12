#include <LiquidCrystal.h>
#include <DHTesp.h>
#include <Servo.h>
#include <ESP8266WiFi.h>

void print_dht(float,float);
void gira_motor();
void gira_motor2();
Servo servo;
int servoPin = 4;
int pos;
              //rs, enable, d4, d5, d6, d7
LiquidCrystal lcd(15, 13, 12, 14, 0, 2);
DHTesp dht;
int dhtPin = 16;

char estado = 0; // 0 fechado 1 aberto

// wifi setup
const char* ssid = "P30_IOT";
const char* password = "pucrs@2019";
const char* host = "192.168.30.126";
const int port = 31600;

void setup()
{
  Serial.begin(115200);
  dht.setup(dhtPin, DHTesp::DHT11); // Connect DHT sensor to GPIO 16
  lcd.begin(16, 2);
  servo.attach(servoPin);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Girando motor1");
  gira_motor();

  Serial.println("Girando motor2");
  gira_motor2();
  
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
  for(pos = 0; pos < 180; pos++){ //PARA "pos" IGUAL A 0, ENQUANTO "pos" MENOR QUE 180, INCREMENTA "pos"
    servo.write(pos); //ESCREVE O VALOR DA POSIÇÃO QUE O SERVO DEVE GIRAR
    delay(15); //INTERVALO DE 15 MILISSEGUNDOS
  }    
}

void gira_motor2()
{
  for(pos = 180; pos < 0; pos--){ //PARA "pos" IGUAL A 0, ENQUANTO "pos" MENOR QUE 180, INCREMENTA "pos"
    servo.write(pos); //ESCREVE O VALOR DA POSIÇÃO QUE O SERVO DEVE GIRAR
    delay(15); //INTERVALO DE 15 MILISSEGUNDOS
  }    
}
