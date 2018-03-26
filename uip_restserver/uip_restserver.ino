#include <UIPEthernet.h> // Used for Ethernet
#include <ArduinoJson.h> //JSON parsing post data

/*
 * REST endpoints:
 * 
 * GET /lights/pipelight
 *  returns either "ON" or "OFF" depending on the state of the ledpin
 * GET /lights/pipelight/on
 *  sets the led light to on
 * GET /lights/pipelight/off 
 *  sets the led light to off
 * GET /lights/pipelight/brightness/<brightness>
 *  sets the brightness to the value in <brightness> (between 0 and 100, in percent)
 * GET /lights/pipelight/brightness
 *  returns the brightness of the light in percent
 * POST /lights/pipelight
 *  POST a JSON body with new parameters for the light brightness and state
 */

// **** ETHERNET SETTING ****
byte mac[] = { 0x54, 0x34, 0x41, 0x30, 0x30, 0x31 };
IPAddress ip(10,10,1,200);
EthernetServer server(80);

int pinsetting = HIGH;
int ledpin = 6; //CAN'T USE PIN 13 (builtin LED) - used by ENC28J60 board
int ledbrightness = 100;

void setup() {
//  pinMode(ledpin, OUTPUT);
  
  Serial.begin(9600);

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();

  Serial.print("IP Address: ");
  Serial.println(Ethernet.localIP());

  digitalWrite(ledpin, HIGH);
}

char linebuffer[60] = "";
char body[50];
enum flag {
  none = 0,
  getlight = 1, //returns ON or OFF
  postlight = 2, //body is JSON formatted, sets light state and brightness
  getlighton = 3, //returns ON, sets light to on
  getlightoff = 4, //returns OFF, sets light to off
  getlightbrightness = 5, //returns led brightness as a integer between 0 and 100 (percent)
  getlightbright = 6 //returns the set brightness, sets light to that brightness
} flags = none;
int contentlen = 0;
boolean currentLineIsBlank = true;
char c;

void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) 
  {  
    Serial.println("-> New Connection");
    linebuffer[0] = '\0';
    // an http request ends with a blank line
    currentLineIsBlank = true;
    flags = none;
    
    while (client.connected()) {
      if (client.available()) {
        c = client.read();

        if (c != '\r' && c != '\n') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
          stringappendchar(linebuffer, c);
        }

        if(c == '\n' && !currentLineIsBlank) {
          //linebuffer is a whole HTTP header line
          /*
           * lines that we want to catch: 
           * GET/POST and route
           * Content-Length
           */
           Serial.print("linebuffer: ");
           Serial.println(linebuffer);
          if(strstr(linebuffer, "GET /lights/pipelight")) {
            flags = getlight;
            Serial.println("rx GET");
          }
//          if(strstr(linebuffer, "POST /lights/pipelight")) {
//            flags = postlight;
//            Serial.println("rx POST");
//          }
          if(strstr(linebuffer, "GET /lights/pipelight/on")) {
            flags = getlighton;
            Serial.println("rx GET/on");
          }
          if(strstr(linebuffer, "GET /lights/pipelight/off")) {
            flags = getlightoff;
            Serial.println("rx GET/off");
          }
          if(strstr(linebuffer, "GET /lights/pipelight/brightness")) {
            flags = getlightbrightness;
            Serial.println("rx GET/brightness");
          }
          if(strstr(linebuffer, "GET /lights/pipelight/brightness/")) {
            flags = getlightbright;
            Serial.print("rx GET/brightness/");
            ledbrightness = atoi(linebuffer + 33);
            Serial.println(ledbrightness);
          }
//          if(strstr(linebuffer, "Content-Length: ")) {
//            //Content-Length : is 16 bytes long, integer will start at byte index 16 and end at strlen - 2 inclusive
//            //start reading content length
//            Serial.print("rx CONTENT-LENGTH: ");
//            contentlen = atoi(linebuffer + 16);
//            Serial.println(contentlen);
//          }
          linebuffer[0] = '\0';
        }
        
        if (c == '\n' && currentLineIsBlank) {
          //end of HTTP headers
          Serial.println("responding to request");
          switch(flags) {
            case getlight:
              if(pinsetting == HIGH)
                print200OK(client, "ON");
              else if(pinsetting == LOW)
                print200OK(client, "OFF");
              break;
//            case postlight:
//              //read the body
//              Serial.print("body:(");
//              Serial.print(contentlen);
//              Serial.println("):");
//              body[0] = '\0';
//              while(contentlen) {
//                c = client.read();
//                stringappendchar(body, c);
//                contentlen--;
//              }
//              Serial.println(body);
//              
//              //parse json data
//              parsejson(body);
//              
//              client.println("<html><title>Rx POST</title><body><h3>Hello World!</h3></body>");
//              break;
            case getlighton:
              print200OK(client, "<html><title>Rx GET/on</title><body><h3>Rx GET/on</h3></body>");
//              client.println("<html><title>Rx GET/on</title><body><h3>Rx GET/on</h3></body>");
              analogWrite(ledpin, map(ledbrightness, 0, 100, 0, 255));
              pinsetting = HIGH;
              break;
            case getlightoff:
              print200OK(client, "<html><title>Rx GET/off</title><body><h3>Rx GET/off</h3></body>");
//              client.println("<html><title>Rx GET/off</title><body><h3>Rx GET/off</h3></body>");
              analogWrite(ledpin, 0);
              pinsetting = LOW;
              break;
            case getlightbrightness:
              print200OKint(client, ledbrightness);
//              client.println(ledbrightness);
              break;
            case getlightbright:
              print200OKint(client, ledbrightness);
//              client.println(ledbrightness);
              analogWrite(ledpin, map(ledbrightness, 0, 100, 0, 255));
              break;
            case none:
              print200OK(client, "<html><title>Hello World!!</title><body><h3>none</h3></body>");
//              client.println("<html><title>Hello World!!</title><body><h3>Hello World!</h3></body>");
              break;
            default:
              print200OK(client, "<html><title>Hello World!!</title><body><h3>Hello World!</h3></body>");
//              client.println("<html><title>Hello World!!</title><body><h3>Hello World!</h3></body>");
          }
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
          linebuffer[0] = '\0';
        }
        
        
      } 
    }

    // give the web browser time to receive the data
    delay(10);

    // close the connection:
    client.stop();
    Serial.println("   Disconnected\n");
  }
}

//void parsejson(char *json) {
//  Serial.println("JSON string:");
//  Serial.println(json);
//  StaticJsonBuffer<50> jsonBuffer;
//  JsonObject& root = jsonBuffer.parseObject(json);
//  if (!root.success()) {
//    Serial.println("parseObject() failed");
//    return;
//  }
//  bool active = root["active"];
////  Serial.println(active);
//  int intensity = root["intensity"];
////  Serial.println(intensity);
//  if(active) {
//    digitalWrite(ledpin, HIGH);
//  } else {
//    digitalWrite(ledpin, LOW);
//  }
//}

/*
 * appends char to a string
 * assumes string is already null terminated
 */
void stringappendchar(char *string, char c) {
  int i;
  i = strlen(string); //pre append
//  Serial.print("stringlen :");
//  Serial.println(i);
  string[i+1] = '\0';
  string[i] = c;
}

void print200OK(EthernetClient client, char *toprint) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connnection: close");
  client.println();
  client.println(toprint);
}

void print200OKint(EthernetClient client, int toprint) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connnection: close");
  client.println();
  client.println(toprint);
}

