#define BUFSIZE 512
#include <SPI.h>
#include <WiFi.h>
//#include <WiFiClient.h>
#include <WiFiServer.h>
// wifi details
char ssid[] = "NETGEAR63";
char password[] = "littlecello367";
String postBody;
String position;
String r;
String s;
int bk=0;
int cp;
char server[] = "54.78.246.30";
int port = 8081;
WiFiClient client;

// server details

int nodes[][3][4] {//{{nodes},{weight}{#nodes,origin,total weight}}
  {{4,7,8}, {3,5,20}, {3}}, //0
  {{6,7}, {1,3}, {2}}, //1
  {{3,7}, {3,5}, {2}}, //2
  {{2,6}, {3,12}, {2}}, //3
  {{0,6}, {3,12}, {2}}, //4
  {{6}, {5}, {1}}, //5
  {{1,3,4,5}, {1,12,12,5}, {4}}, //6
  {{0,1,2}, {5,3,5}, {3}}, //7
  {{0}, {20}, {1}}, //8
};

byte uturn = 0;

int motorRPWM = 39; // P2.6
int motorRPhase = 40; // P2.7
int motorLPWM = 37; // P2.4
int motorLPhase = 38; // P5.6

byte path[] = {8, 0, 4}; //{where it is, where it is going, where it came from}

byte AnalogValue = 0;
float strength = 0;
byte triggered = 0;
int DistancePin = A1;
int AnalogPin[5] = {A6,A9,A11,A13,A8}; //Analog pins P4.7, P4.4, P4.2, P4.0, P6.1 Brown, red, orange, yellow, green
// J3 -> 28, 26, 25, 24, 23

int threshold = 512;
int rpow = 236;
int lpow = 255;
// keep A8 free for tone O/P music

void connectToWiFi() {
  Serial.print("Connecting to network: ");
  Serial.print(ssid);
  Serial.flush();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    Serial.flush();
    delay(300);
  }

  Serial.println("Connected");
  //Serial.print("Obtaining IP address");
  Serial.flush();
  /*while (WiFi.localIP() == INADDR_NONE) {
    Serial.print(".");
    Serial.flush();
    delay(300);
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());*/
}  

bool connect() {
  Serial.println("Connecting to server");
  if (!client.connect(server, port)) {
    Serial.println("error connecting to server");
    return false;
  }
  Serial.println("Connected to server");
 return true;
}


// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(9600);
  delay(1000);
  connectToWiFi();
  postBody="position=";
  position="0";
  Serial.println(position);
  postBody += position;
  Serial.println(postBody);

  Serial.print(path[0]);
  Serial.print("\t");
  Serial.print(path[1]);
  Serial.print("\t");
  Serial.print(path[2]);
  Serial.println();
  delay(2000);
  scan();
  analogRead(DistancePin);
  digitalWrite(A0,0);
}

// read buffer size for HTTP response

String readResponse() {
  char buffer[BUFSIZE];
  memset(buffer, 0, BUFSIZE);
  client.readBytes(buffer, BUFSIZE);
  String response(buffer);
  return response;
}
  
int getStatusCode(String& response) {
  String code = response.substring(9, 12);
  return code.toInt();
}
    
String getResponseBody(String& response) {
  int split = response.indexOf("\r\n\r\n");
  String body = response.substring(split+4, response.length());
  body.trim();
  return body;
}

int destination() {
    connect();
    client.println("POST /api/arrived/5d00cd7c HTTP/1.1");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(postBody.length());
    client.println("Connection: close");
    client.println();

    // send post body
    client.println(postBody);
    r = readResponse();
    //getStatusCode(r);
    s = getResponseBody(r);
    Serial.println(s);
    client.stop();
    position = s;
    postBody = "position=";
    postBody += position;
    Serial.println(postBody);
}

void scan() {
  strength = 0;
  triggered = 0;
  //Serial.println(triggered);
  for (byte i=0;i<5;i++) {
    AnalogValue = AnalogValue << 1;
    if (analogRead(AnalogPin[i]) < threshold) {
      strength += i-2;
      AnalogValue += 1;
      triggered += 1;
    }
  }
}

void follow_line () {
  //float strength = 0;
  //byte triggered = 0;
  int v = 0;
  while (triggered < 4) {
    scan();
    //Serial.println(triggered);
    if (triggered>0) {
      strength = strength/triggered;
      float a = 3.864134282;
      float b = 30.92128522;
      float c = 97.27165564;
      float d = 138.9864345;
      v = a*strength*strength*strength+b*strength*strength+c*strength+d;
      if (v > 0) {
        digitalWrite(motorLPhase, HIGH); //forward
      }
      else {
        digitalWrite(motorLPhase, LOW); //backward
      }
      analogWrite(motorLPWM, abs(v)); // set speed of motor
      a = -3.58812469;
      b = 28.71262199;
      c = -90.32368024;
      d = 129.0588321;
      v = a*strength*strength*strength+b*strength*strength+c*strength+d;
      if (v > 0) {
        digitalWrite(motorRPhase, HIGH); //forward
      }
      else {
        digitalWrite(motorRPhase, LOW); //backward
      }
      analogWrite(motorRPWM, abs(v)); // set speed of motor
    }
  }
  if (triggered >= 3) {
    analogWrite(motorLPWM, 0); // set speed of motor
    analogWrite(motorRPWM, 0); // set speed of motor
  }
  //Serial.println(triggered);
}

void clear_line() {
  triggered = 5;
  digitalWrite(motorLPhase, HIGH); //forward
  digitalWrite(motorRPhase, HIGH); //forward
  analogWrite(motorLPWM, lpow); // set speed of motor
  analogWrite(motorRPWM, rpow); // set speed of motor
  while (triggered >= 2) {
    scan();
  }
}

void turn(byte dir) {
  triggered = 5;
  digitalWrite(motorLPhase, HIGH); //forward
  digitalWrite(motorRPhase, HIGH); //forward
  if (dir == 0) {
    Serial.print("38.89855072");
    Serial.print("\t");
    Serial.print(rpow);
    digitalWrite(motorLPhase, LOW); //forward
    digitalWrite(motorRPhase, HIGH); //forward
    //analogWrite(motorLPWM, 38.89855072); // set speed of motor
    analogWrite(motorLPWM, lpow); // set speed of motor
    //analogWrite(motorLPWM, 0); // set speed of motor
    analogWrite(motorRPWM, rpow); // set speed of motor
  }
  else {
    Serial.print(lpow);
    Serial.print("\t");
    Serial.print("40.65217391");
    digitalWrite(motorLPhase, HIGH); //forward
    digitalWrite(motorRPhase, LOW); //forward
    analogWrite(motorLPWM, lpow); // set speed of motor
    //analogWrite(motorRPWM, 40.65217391); // set speed of motor
    //analogWrite(motorRPWM, 0); // set speed of motor
    analogWrite(motorRPWM, rpow); // set speed of motor
  }
  Serial.println();
  while (triggered >= 2) {
    scan();
  }
}

void rotate(byte dir=1) {
  triggered = 5;
  if (dir == 1) {
    digitalWrite(motorLPhase, HIGH); //forward
    digitalWrite(motorRPhase, LOW); //forward
  }
  else {
    digitalWrite(motorLPhase, LOW); //forward
    digitalWrite(motorRPhase, HIGH); //forward
  }
  //analogWrite(motorLPWM, 255); // set speed of motor
  //analogWrite(motorRPWM, 255); // set speed of motor
  analogWrite(motorLPWM, lpow/2); // set speed of motor
  analogWrite(motorRPWM, rpow/2); // set speed of motor
  while (triggered >= 1) {
    scan();
  }
  while (analogRead(AnalogPin[2]) > threshold) {
    triggered = 0;
  }
}

void travel() {
  scan();
  if (triggered > 1) {
    //180 check
    if (path[1] == path[2]) {
      rotate();
    }
    //junction turn
    if ((path[2] == 1) || (path[1] == 1)) {
      int dir = 0;
      if (path[1] == 1) {
        dir = ((path[2]>>0)&1)^((path[2]>>1)&1)^((path[2]>>2)&1);
        if (dir == 1) {
          turn(1);
        }
        else {
          turn(0);
        }
      }
      else {
        dir = ((path[1]>>0)&1)^((path[1]>>1)&1)^((path[1]>>2)&1);
        if (dir == 1) {
          turn(0);
        }
        else {
          turn(1);
        }
      }
    }
    //normal clear
    else {
      clear_line();
    }
  }
  //follow until at node
  follow_line();
}

void bfs(int start, int target) {
  nodes[target][2][1] = -1;
  nodes[target][2][2] = 0;
  byte passed = 0;
  byte queuelen = 1;
  int queue[] = {target,-1,-1,-1,-1,-1,-1,-1,-1,-1};
  int marked[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
  while (queuelen > 0) {
    marked[passed] = queue[0]; //mark node
    //add new nodes to queue
    for (byte i=0;i<nodes[queue[0]][2][0];i++) {
      byte duplicate = 0;
      byte k = queuelen;
      for (byte j=0;j<passed;j++) {
        if (marked[j] == nodes[queue[0]][0][i]) {
          duplicate = 1;
          j = passed;
        }
      }
      for (byte j=0;j<queuelen;j++) {
        if (queue[j] == nodes[queue[0]][0][i]) {
          byte turn = 0;
          if (queue[0]==path[2]){
            turn = uturn;
          }
          if (nodes[queue[j]][2][2] > (nodes[queue[0]][2][2]+nodes[queue[0]][1][i]+turn)) {
            k = j;
            queuelen--; //incremented later, so decremented to keep same length
          }
          else {
            duplicate = 1;
          }
          j = queuelen;
        }
      }
      if (duplicate == 0) {
        //normal
        /*
        queue[queuelen] = nodes[queue[0]][0][i];
        //queueorigin[queuelen] = queue[0];
        nodes[nodes[queue[0]][0][i]][2][1] = queue[0];
        queuelen++;
        */
        //weighted
        //path length of indexed node = path lengh of current node+weight of indexed node
        byte turn = 0;
        if (queue[0]==path[2]){
          turn = uturn;
        }
        nodes[nodes[queue[0]][0][i]][2][2] = nodes[queue[0]][2][2]+nodes[queue[0]][1][i]+turn;
        //origin of indexed node = current node
        nodes[nodes[queue[0]][0][i]][2][1] = queue[0];
        while (nodes[queue[k-1]][2][2] > nodes[nodes[queue[0]][0][i]][2][2]) {
          queue[k] = queue[k-1];
          k--;
        }
        queue[k] = nodes[queue[0]][0][i];
        queuelen++;
      }
    }
    //remove node from queue
    for (byte j=0;j<queuelen;j++) {
      queue[j] = queue[j+1];
    }
    queuelen--;
    passed++;
  }
  byte route[passed] = {};
  route[0] = start;
  byte i = 0;
  //Serial.print(target);
  //Serial.print("\t");
  while (route[i] != target) {
    Serial.print(route[i]);
    Serial.print("(");
    Serial.print(nodes[route[i]][2][1]); 
    Serial.print(")");
    Serial.print("(");
    Serial.print(nodes[route[i]][2][2]);
    Serial.print(")");
    Serial.print("\t");
    route[i+1] = nodes[route[i]][2][1];
    i++;
    path[1] = route[i];
    Serial.print("\tgoing to ");
    Serial.print(path[1]);
    Serial.print("\tcame from ");
    Serial.print(path[2]);
    Serial.println();
    travel();
    //delay(500);
    path[2] = path[0]; //set where it came from, to where it is
    if (path[2] == 8) {
      path[2] = path[1]^0b100;
    }
    path[0] = path[1]; //set where it is to where it is going
  }
  Serial.println(route[i]);
}

void forwardsensor() {
  int i=0;
  float average =0;
  int halt[] = {0,0,0,0,0}; 
  while(average < 375) {
    halt[i] = analogRead(DistancePin);

    digitalWrite(motorLPhase, HIGH); //forward
    digitalWrite(motorRPhase, HIGH); //forward
    analogWrite(motorLPWM, (lpow-6)); // set speed of motor
    analogWrite(motorRPWM, rpow); // set speed of motor

    average = (halt[0]+halt[1]+halt[2]+halt[3]+halt[4])/5;
    Serial.println(average);
    i = (i+1)%5;
  }
  Serial.print("Stop"); // This prints the actual       analog reading from the sensors
  analogWrite(motorLPWM, 0); // set speed of motor
  analogWrite(motorRPWM, 0); // set speed of motor
}

// the loop routine runs over and over again continuously:
void loop() {
  //my code
  //forwardsensor();
  //delay(2000);
  ///*
  destination();
  if(position=="1"||position=="3"||position=="5"||position=="7"){
    digitalWrite(A3,1);
  }
  else{
    digitalWrite(A3,0);
  }
  if(position=="2"||position=="3"||position=="6"||position=="7"){
    digitalWrite(A4,1);
  }
  else{
    digitalWrite(A4,0);
  }
  if(position=="4"||position=="5"||position=="6"||position=="7"){
    digitalWrite(A5,1);
  }
  else{
    digitalWrite(A5,0);
  }
  if(position!="undefined"){
    cp=position.toInt();
    Serial.println(cp);
    if (cp==5) {
      bfs(path[0], 1);
      bfs(path[0], 6);
      forwardsensor();
    }
    else {
      bfs(path[0], cp);
    }
  }
  else{
    digitalWrite(A0,1);
    while (1) {}
  }
}
