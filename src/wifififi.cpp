
#include "ConnectToWifi.h"


/*
#include <Arduino.h>

#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream> 

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

#include <WiFiMulti.h>

WiFiMulti WiFiMulti;


const char *ssid = "Caspa-PICO connect";
const char *password = "Caspa123";

int flag=1;

String IDwifi = "";
String MDP = "";
String ClientValue = "";

int nWifi;
String cWifi;
char* char_arr;

WiFiServer server(80);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  nWifi = WiFi.scanNetworks();

  Serial.println();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  Serial.println("Server started");

    
}


void accespoint(){
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    ClientValue= "";
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();                       
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta meta charset='UTF-8' name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println("</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>Caspa-PICO</h1><br><br><h2>Connection des capteurs au wifi</h2>");
            
            client.println();
            client.print("Voicie les réseaux disponibles à proximité, choisissez le votre :<br>");
            client.print("<br><form action=\"/get\"> SSID: <select id=\"wifi\" name=\"input1\">");

            for (int i = 0; i < nWifi; ++i) {
              // Print SSID and RSSI for each network found
              Serial.print(i + 1);
              Serial.print(": ");
              Serial.println(WiFi.SSID(i));
              cWifi = WiFi.SSID(i);
              
              client.println("<option>");
              client.println(cWifi);    
              client.println("</options>");
            }

            client.println("</select>");//<input type=\"submit\" value=\"Submit\"></form>");

            client.print("<br><br>Entrez le mot de passe wifi ici :<br><br>");
            client.print("Mot de passe: <input type=\"text\" name=\"input2\"><br><br><input class=\"button\" type=\"submit\" value=\"Submit\"></form><br>");

            //client.print("<form action=\"/get\">input1: <input type=\"text\" name=\"input1\"><input type=\"submit\" value=\"Submit\"></form><br>");
            //client.print("<form action=\"/get\">input2: <input type=\"text\" name=\"input2\"><input type=\"submit\" value=\"Submit\"></form><br>");

            // The HTTP response ends with another blank line:
            client.println("</body></html>");
            client.println();
            
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
        
        if (currentLine.indexOf("get?input1=") >=0){
          ClientValue=ClientValue+c;
        }
        
      }
      
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
    Serial.println("");
    
    Serial.println("value return=");
    Serial.println(ClientValue);
    Serial.println("=====");
    Serial.println("");

  }
}

char tradHEX(char c)
{
    if ('0' <= c && c <= '9')
    	return c - '0';
    else if ('a' <= c && c <= 'f')
    	return c - 'a' + 10;
    else if ('A' <= c && c <= 'F')
    	return c - 'A' + 10;
    else
		return c;
}

String urlDecode(String str){
    String temp;
    char charTransform;
    int len = str.length();
    Serial.println("urldec: ");

    for (int i=0; i < len; i++){
        if(str[i] == '%'){
			charTransform= tradHEX(str[i+1])*16 + tradHEX(str[i+2]);
			temp += charTransform;
            i = i + 2;
        }else{
            if(str[i] == '+')
                temp += ' ';
            else
                temp += str[i];
        }
    }
    return temp;
}

void extractMDPSSID(int show){
  if (ClientValue!="")
  {
    MDP="";
    IDwifi="";
    size_t i = 1;
    while (ClientValue[i] !='\n' && !(ClientValue[i]=='&' && ClientValue[i+1]=='i' && ClientValue[i+2]=='n' && ClientValue[i+3]=='p' 
    && ClientValue[i+4]=='u' && ClientValue[i+5]=='t' && ClientValue[i+6]=='2' && ClientValue[i+7]=='='))
    {
      IDwifi=IDwifi + ClientValue[i]; 
      i++;  
    }
    i+=8;
    while (ClientValue[i] != '\n' && !(ClientValue[i]==' ' && ClientValue[i+1]=='H' && ClientValue[i+2]=='T' && ClientValue[i+3]=='T' 
    && ClientValue[i+4]=='P' && ClientValue[i+5]=='/' && ClientValue[i+6]=='1' && ClientValue[i+7]=='.'))
    {
      MDP=MDP + ClientValue[i]; 
      i++;
    }
    if (show )
    {
      Serial.println("=====");
      Serial.println("ID et mdp:");
      Serial.println(IDwifi);
      Serial.println(MDP);
      Serial.println("=====");
    }
    
  }
}

void setClient(){
  delay(10);

	//decode
  IDwifi=urlDecode(IDwifi);
  MDP=urlDecode(MDP);
  	Serial.println("=====");
	Serial.println("ID et mdp DECODE:");
	Serial.println(IDwifi);
	Serial.println(MDP);
	Serial.println("=====");

  char BufID[50];
  IDwifi.toCharArray(BufID, 50);
  char BufMDP[50];
  MDP.toCharArray(BufMDP, 50);
  	Serial.println(BufMDP);
  	Serial.println("=====");
	Serial.println("BUFFER ID et mdp:");
	Serial.println(IDwifi);
	Serial.println(MDP);
	Serial.println("=====");
  // We start by connecting to a WiFi network
  WiFiMulti.addAP(BufID, BufMDP);

  Serial.println();
  Serial.println();
  Serial.print("Waiting for WiFi... ");

  while(WiFiMulti.run() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(500);
}

void connectToServ(){
  //    const uint16_t port = 80;
  //    const char * host = "192.168.1.1"; // ip or dns
  //    const uint16_t port = 1337;
  //    const char * host = "192.168.1.10"; // ip or dns
  const uint16_t port = 80;
  const char * host = "1.1.1.1"; // ip or dns

  Serial.print("Connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;

  if (!client.connect(host, port)) {
    Serial.println("Connection failed.");
    Serial.println("Waiting 5 seconds before retrying...");
    delay(5000);
    return;
  }

  // This will send a request to the server
  //uncomment this line to send an arbitrary string to the server
  //client.print("Send this data to the server");
  //uncomment this line to send a basic document request to the server
  client.print("GET /index.html HTTP/1.1\n\n");

  int maxloops = 0;

  //wait for the server's reply to become available
  while (!client.available() && maxloops < 1000)
  {
    maxloops++;
    delay(1); //delay 1 msec
  }
  if (client.available() > 0)
  {
    //read back one line from the server
    String line = client.readStringUntil('\r');
    Serial.println(line);
  }
  else
  {
    Serial.println("client.available() timed out ");
  }

  Serial.println("Closing connection.");
  client.stop();

  Serial.println("Waiting 5 seconds before restarting...");
  delay(5000);
}

*/
ConnectToWifi Test;

void setup(){
  Test.Ssetup();
  
}

void loop() {
  Test.lauchConneciton();

}
