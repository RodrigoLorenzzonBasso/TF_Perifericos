#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

SoftwareSerial s(14,12,false); // RX, TX

// wifi setup
const char* ssid = "P30_IOT";
const char* password = "pucrs@2019";
int port = 31600;
WiFiServer server(port);

void setup() {
  Serial.begin(115200);
  s.begin(38400);
  
  Serial.println(WiFi.localIP());
  WiFi.begin(ssid, password);
  server.begin();
}

void loop() {

  ///////////// recepcao do cortex /////////////

  String comando = ";";

  char c = 'x';

  if(s.available() > 0)
  {
     c = s.read();
     Serial.print("Recebido do Cortex: ")
     Serial.println(c);
  } 

  if(c == 'a')
    comando = "abrir;";
  else if(c == 'f')
    comando = "fechar;";
  else
    comando = ";";

  //////////////////////////////////////////////

  String line = "x";
  char connected = 'n';

  ////////////////// Wifi //////////////////////
  // Check if module is still connected to WiFi.
  if (WiFi.status() != WL_CONNECTED) {
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
  }

  WiFiClient client = server.available();

  if(client)
  {
    Serial.println("Client connected.");

    connected = 's';

    client.print(comando);

    while(client.connected())
    {
      if(client.available())
      {
        line = client.readStringUntil(';');
        Serial.print("Recebido do Cliente: ");
        Serial.println(line);
      }
    }

    Serial.println("Client disconnected.");
    client.stop();
  }
  //////////////////////////////////////////////

  ///////////// reenvio para o cortex //////////

  if(line != "x")
  {
    line += " ";
    line += connected;
    Serial.print("Enviando para o Cortex: ");
    Serial.println(line);
    s.print(line);
  }
  
  //////////////////////////////////////////////
  

}
