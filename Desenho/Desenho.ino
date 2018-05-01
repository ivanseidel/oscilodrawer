#include <WiFi.h>
#include <ESPmDNS.h>
#include <PubSubClient.h>
#include <driver/dac.h>

#include "image.h"
#include "Matrix.h"

#define max(a, b) a > b ? a : b
#define min(a, b) a < b ? a : b
#define constrain(x, min, max) x < min ? min : (x > max ? max : x)

#define SMOOTHSTEP(x) ((x) * (x) * (3 - 2 * (x))) //SMOOTHSTEP expression.

const char* ssid = "Tenda Digital";
const char* password = "arduinos";
//const char* mqtt_server = "test.mosquitto.org";
const char* mqtt_server = "192.168.1.38";
//const char* mqtt_server = "10.11.0.101";
//const char* mqtt_server = "rpi-speak.local";

long int tempo_anterior = 0;
uint8_t raw_points[1500];
float XY_points[1500];
int XY_points_len = 0;



int j = 0;             //Just an Iterator.
int i = 0;             //Just another Iterator.
float A = 0.0;         //Input Min Value
float B = 255.0;       //Input Max Value
float C = 0.0;         //Input Min Value
float D = 255.0;       //Input Max Value
int N = 10;       //Input number of steps for transition
float X;               //final smoothstepped value
float v;               //smoothstep expression variable

#define axes 3

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

const int TAM_IMG = (sizeof(image));

// float Znear = -100;
// float Zfar = 100;
// float Q = Zfar / (Zfar - Znear);

// float w = 2 * Znear / 1000;
// float h = 2 * Znear / 1000;

Matrix4x4 projection = {
  {1, 0, 0, 0},
  {0, 1, 0, 0},
  {0, 0, 1, 0},
  {0, 0, 0.001, 1}
};
// {
//   {   w,  0,         0, 0},
//   {   0,  h,         0, 0},
//   {   0,  0,         Q, 1},
//   {   0,  0,-Q * Znear, 0}
// };

void convertToWorld(Matrix4x4 mat) {
  // Serial.println();
  // Serial.println();
  // Serial.println("matrixes: ");
  // printMatrix(mat);
  // float rot_y_s = sin(rot_y);
  // uint8_t x, y, outX, outY;
  Vector4 in = {0, 0, 0, 1};
  Vector4 out = {0, 0, 0, 1};
  Vector4 world = {0, 0, 0, 1};

  // multiply4x4(mat, projection, mat);

  for (int i = 0; i < XY_points_len; i += axes) {
    in[0] = (raw_points[i + 0] - 128.0) * 0.8;
    in[1] = (raw_points[i + 1] - 128.0) * 0.8;
    in[2] = (raw_points[i + 2] - 128.0) * 0.8;

    if (raw_points[i + 0] == 0 && raw_points[i + 1] == 0) {
      out[0] = 0;
      out[1] = 0;
      out[2] = 0;
    } else {
      multiply4x4Point(mat, in, world);
      multiply4x4Point(projection, world, out);


      // Serial.println();
      // Serial.print("point #");
      // Serial.println((int)(i / 2));
      // printVector4(in);
      // printVector4(world);
      // printVector4(out);
      out[0] = out[0] + 128.0;
      out[1] = out[1] + 128.0;
      out[2] = out[2] + 128.0;
      // printVector4(out);

      // if (out[0] > 255 || out[0] < 0 || out[1] < 0 || out[1] > 255) {
      //   out[0] = 0;
      //   out[1] = 0;
      // }
      // out = (x - 128) * rot_y_s + 128;
      // outY = y;
    }

    
    XY_points[i + 0] = out[0];
    XY_points[i + 1] = out[1];
  }
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  static int var=0;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  memcpy(raw_points, payload, length);
  XY_points_len = length;

  //convertToWorld();
  //Serial.println(XYZ_points);
  Serial.println(length);

  //var++;

  /*for(int i =0; i<(length/3); i++)
  {
    var = XYZ_points[i][0];
    Serial.println(var);
  }*/

  //Serial.println(var);
  
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    uint64_t chipid = ESP.getEfuseMac(); // MAC address of ESP32
    uint16_t chip = (uint16_t)(chipid>>32);
    char clientid[25];
    //snprintf(clientid,25,"WIFI-Display-%04X",chip);
    if (client.connect(clientid)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("Say", "-t 'hello world'");
      // ... and resubscribe
      client.subscribe("frame");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setOutput(uint8_t x, uint8_t y) {
  dac_out_voltage(DAC_CHANNEL_1, constrain(x, 0, 255)); // chanel 1 X
  dac_out_voltage(DAC_CHANNEL_2, constrain(y, 0, 255)); //channel 2 Y
}

void keepClient() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void setup()
{
  Serial.begin(230400);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(DAC_CHANNEL_1, OUTPUT);
  pinMode(DAC_CHANNEL_2, OUTPUT);

  // Setup projection matrix
  // Vector4 tLeft = {-128, -128, 0, 1};
  // Vector4 tLeftNeg = {128, 128, 0, 1};
  // translate(projection, tLeft, projection);
  // setProjectionMatrix(90, 0.1, 200, projection);
  // setFrustumMatrix(-128, 128, -128, 128, 0.1, 128, projection);
  // projection = {
  //   {1, 0, 0, 0},
  //   {0, 1, 0, 0},
  //   {0, 0, 1, 0},
  //   {0, 0,10, 1}
  // };
  // translate(projection, tLeftNeg, projection);

}

int frame = 0;
void loop() 
{

  double tick = 0;
  uint8_t x_out;
  uint8_t y_out = 0;
  int k,z;

  keepClient();
/*  while(1) {

    tick = tick + 1;
    double t = tick / 100.0;

    uint8_t x_out = (sin(t) + 1) * 100 + 10;
    uint8_t y_out = (cos(t) + 1) * 100 + 10;

    setOutput(x_out, y_out);
  }
*/


  // Vector4 trans1 = {0, 0, (millis() % 2000 ) / 25.0, 1};
  // Vector4 trans1 = {0, millis() / 500.0, 0, 1};
  // Vector4 trans2 = { 128,  128, 0, 1};

  // Serial.print("matrixes: ");
  // printMatrix(mvm);
  // multiply4x4(mvm, projection, mvm);
  // if (XY_points_len < 100) {
  Matrix4x4 mvm = Matrix4x4Identity;
  float xRot = analogRead(34) / 2000.0;
  float yRot = analogRead(35) / 2000.0;
  float zRot = analogRead(32) / 2000.0;
  rotateX(mvm, xRot, mvm);
  rotateY(mvm, yRot, mvm);
  rotateZ(mvm, zRot, mvm);

  convertToWorld(mvm);
  
  // Serial.print("34: ");
  // translate(mvm, trans1, mvm);
  // translate(mvm, trans2, mvm);

  // printMatrix(mvm);
  frame++;

  if (frame % 50 == 0) {
    char vals[3] = {xRot, yRot, zRot};
    client.publish("rot", vals, 3);
  }

  for(k=0; ((k + axes) < XY_points_len); k += axes)
  {
    float fromX = XY_points[k + 0];
    float fromY = XY_points[k + 1];
    // float fromZ = XY_points[k + 4];
    float toX = XY_points[k + axes + 0];
    float toY = XY_points[k + axes + 1];
    // float toZ = XY_points[k + 4];

    if ((toX == 0 && toY == 0) || (fromX == 0 && fromY == 0)) {
      k += axes;
      continue;
    }

    N = min(max(abs(fromX - toX), abs(fromY - toY)) / 5 + 1, 50);
    
    for (j = 0; j < N; j++)
    {
      v = (float) j / N;

      float vInv = (1.0 - v);
      x_out = (fromX * v) + (toX * vInv);
      y_out = (fromY  * v) + (toY * vInv);

      dac_out_voltage(DAC_CHANNEL_1, x_out);
      dac_out_voltage(DAC_CHANNEL_2, -y_out);
    }
  }

    /*while(1)
    {
      dac_out_voltage(DAC_CHANNEL_1, 0); // chanel 1 X
      dac_out_voltage(DAC_CHANNEL_2, 0);
    }
*/
  /*if((millis() - tempo_anterior) >= 1000)
  {
    tempo_anterior = millis();
   client.publish("display1", "-t 'samuel_test'");
  }
  */
} 
