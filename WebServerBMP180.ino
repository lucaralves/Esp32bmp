#include <ArduinoJson.h>
#include <SFE_BMP180.h>
#include <WiFi.h>

SFE_BMP180 pressure;
double P = 0; 
double T = 0;
const char* ssid = "";
const char* password = "";
int LED = 2;
WiFiServer server(80);

void setup() {

  Serial.begin(115200);

  //Inicializa o servidor.
  Serial.println();
  Serial.print("Conectando-se a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi conectada.");
  Serial.println("Endereço de IP: ");
  Serial.println(WiFi.localIP());
 
  server.begin();

  //Inicializa BMP180.
  if (pressure.begin())
    Serial.println("BMP180 init success\n");
  else
  {
    Serial.println("BMP180 init fail");
    while(1); 
  }
}

void loop() {

  WiFiClient client = server.available();

  if (client) {
    Serial.println("New Client.\n");

    String currentLine = "";
    
    while (client.connected()) {     
      if (client.available()) {       
        char c = client.read();
        Serial.write(c);
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            
            DynamicJsonDocument doc(1024);
            
            doc["temperatura"] = T;
            doc["pressao"]   = P;

            serializeJson(doc, client);
            client.println();
            break;
          } 
          else {
            currentLine = "";
          }
        }         
        else if (c != '\r') {
          currentLine += c;
        }
        if (currentLine.endsWith("GET /temp")) {
          obtemTemp();

          Serial.print("\n\ntemperature: ");
          Serial.print(T,2);
          Serial.println(" deg C\n");
                   
        }
        if (currentLine.endsWith("GET /pressure")) {
          obtemTemp();
          obtemPress(T);
          
          Serial.print("\n\npressure: ");
          Serial.print(P,2);
          Serial.println(" mb\n");          
        }
      }
    }
    client.stop();
    Serial.println("Client Disconnected.");
  }

}

void obtemTemp() {

  char status;

  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);

    status = pressure.getTemperature(T);
    if (status != 0)
    {
      
    }
    else Serial.println("error retrieving temperature measurement");
  }
  else Serial.println("error starting temperature measurement");                  
  
}

void obtemPress(double T) {

  char status;

  status = pressure.startPressure(3);

  if (status != 0)
  {       
    delay(status);                         

    status = pressure.getPressure(P,T);
    if (status != 0)
    {         
    
    }
    else Serial.println("error retrieving pressure measurement");
  }
  else Serial.println("error starting pressure measurement");
}
