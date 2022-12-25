//nút chủ giao tiếp với webserver và mạng NODE = 2
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include "painlessMesh.h"
#include <Arduino_JSON.h>
#include "IPAddress.h"
#include <ESPAsyncWebServer.h>  //webserver
#include <AsyncTCP.h>
#include "SPIFFS.h"

//Khai bao chan
#define DHTTYPE DHT11
#define DHTPIN 21


// MESH Details
#define   MESH_PREFIX     "Khoi_Group" //name for your MESH
#define   MESH_PASSWORD   "hoilamgi12@" //password for your MESH
#define   MESH_PORT       5555 //default port
//
//#define   STATION_SSID     "Hang2.4G"
//#define   STATION_PASSWORD "0948315735"

#define HOSTNAME "HTTP_BRIDGE"

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
IPAddress myIP(0, 0, 0, 0);
IPAddress myAPIP(0, 0, 0, 0);
//Khai bao bien sensor
DHT dht(DHTPIN, DHTTYPE);
//Thay đổi cái nodeNumber này theo cái node từng vị trí
int nodeNumber = 2;
// Cai dat SPIFFS
void initFS() {
  if (!SPIFFS.begin()) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  else {
    Serial.println("SPIFFS mounted successfully");
  }
}
//String to send to other nodes with sensor readings
String readings;
String receiveCall;
Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain
String getReadings(); // Prototype for sending sensor readings

//Create tasks: to send messages and get readings;
Task taskSendMessage(TASK_SECOND * 5 , TASK_FOREVER, &sendMessage);

String getReadings ()
{
  JSONVar jsonReadings;
  jsonReadings["node"] = nodeNumber;
  jsonReadings["temp"] = dht.readTemperature();
  jsonReadings["hum"] = dht.readHumidity();

  readings = JSON.stringify(jsonReadings);
  return readings;
}

void sendMessage () {
  String msg = getReadings();
  mesh.sendBroadcast(msg); // gửi tới các nút khác trong mạng chuỗi JSON
}

//Init DHT
// void initDHT(){
//     dht.begin();
//    if (isnan() || isnan(temp) ) {
//     Serial.println("Failed to read from DHT sensor!");
//     while (1);
//   }
// }

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
  receiveCall = msg.c_str();  // biến tạm để nhận Json từ các nút khác
  JSONVar myObject = JSON.parse(msg.c_str());
  int node = myObject["node"];
  double temp = myObject["temp"];
  double hum = myObject["hum"];

  Serial.print("Node: ");
  Serial.println(node);
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" C");
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.println(" %");


}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void setup() {
  Serial.begin(115200);
  initFS();
  // initDHT();
  dht.begin();

  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION  ); // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA,10);
//  mesh.stationManual(STATION_SSID, STATION_PASSWORD);
//  mesh.setHostname(HOSTNAME);
  mesh.onReceive(&receivedCallback);
  myAPIP = IPAddress(mesh.getAPIP());
  Serial.println("My AP IP is " + myAPIP.toString());
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  //mesh.setRoot(true);
  // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes
  mesh.setContainsRoot(true);

  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();

  //server ROOT
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });
  server.on("/upload", HTTP_GET, [](AsyncWebServerRequest * request) {      // Giao tiếp qua url : /upload
    request->send_P(200, "application/json", getReadings().c_str());
  });
  server.on("/upload1", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "application/json", receiveCall.c_str());
  });
  server.serveStatic("/", SPIFFS, "/");
  server.begin();
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
  if (myIP != getlocalIP()) {
    myIP = getlocalIP();
    Serial.println("My IP is " + myIP.toString());
  }
}

IPAddress getlocalIP() {
  return IPAddress(mesh.getStationIP());
}
