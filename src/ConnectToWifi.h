
#include <Arduino.h>

#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <DNSServer.h>

#include <WiFiMulti.h>

class ConnectToWifi
{
public:
    ConnectToWifi(int show, int port, char *ipAdresse);
    ConnectToWifi();

    void Ssetup();
    void accespoint();
    char tradHEX(char c);
    String urlDecode(String str);
    void extractMDPSSID(int show);
    void setClient();
    void connectToServ();
    void lauchConneciton();

    // protected:
    WiFiServer server = WiFiServer(80);
    WiFiMulti WiFiMultit;

    const byte DNS_PORT = 53;
    IPAddress apIP = IPAddress(192,168,0,1); // The default
    DNSServer dnsServer;

    const char *ssid = "Caspa-PICO Connect";
    const char *password = "Caspa123";

    bool flag = 1;
    int countWrongWifi = 0;
    bool flagBadWIFI = 0;
    bool flagLoading =0;

    String IDwifi = "";
    String MDP = "";
    String ClientValue = "";

    int nWifi;
    String cWifi;
    char *char_arr;
};