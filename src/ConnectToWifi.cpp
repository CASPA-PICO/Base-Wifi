#include "ConnectToWifi.h"

ConnectToWifi::ConnectToWifi(){}

void ConnectToWifi::Ssetup()
{
	WiFi.disconnect();	// clear wifi
	WiFi.mode(WIFI_OFF);

	Serial.begin(115200);

	Serial.println("setup");

	// Set WiFi to station mode and disconnect from an AP if it was previously connected
	WiFi.mode(WIFI_STA);
	WiFi.disconnect();
	delay(100);
	nWifi = WiFi.scanNetworks();

	Serial.println();
	Serial.println("Configuring access point...");

	WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

	// You can remove the password parameter if you want the AP to be open.
	WiFi.softAP(ssid, password);

	IPAddress myIP = WiFi.softAPIP();

	dnsServer.start(DNS_PORT, "*", apIP);

	Serial.print("AP IP address: ");
	Serial.println(myIP);
	server.begin();

	Serial.println("Server started");
}

void ConnectToWifi::accespoint()
{
	dnsServer.processNextRequest();
	WiFiClient client = server.available(); // listen for incoming clients

	if (client)
	{								   // if you get a client,
		Serial.println("New Client."); // print a message out the serial port
		String currentLine = "";	   // make a String to hold incoming data from the client
		ClientValue = "";
		while (client.connected())
		{ // loop while the client's connected
			if (client.available())
			{							// if there's bytes to read from the client,
				char c = client.read(); // read a byte, then
				Serial.write(c);		// print it out the serial monitor
				if (c == '\n')
				{ // if the byte is a newline character

					// if the current line is blank, you got two newline characters in a row.
					// that's the end of the client HTTP request, so send a response:
					if (currentLine.length() == 0)
					{
						// HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
						// and a content-type so the client knows what's coming, then a blank line:

						client.println("HTTP/1.1 200 OK");
						client.println("Content-type:text/html");
						client.println("Connection: close");
						client.println();

						// Display the HTML web page
						// Header
						client.println("<!DOCTYPE html><html>");
						client.println("<head><meta charset='UTF-8' name=\"webpage\" content=\"width=device-width, initial-scale=1\">");
						// CSS to the style
						client.println("<title>Caspa-PICO connexion</title>");
						client.println("<style>html { font-family: Tahoma; display: flex; flex-direction: column; justify-content:center; margin: 2px auto; text-align: center; align-items: center;}");
						client.println(".button { background-color: green; border: none; color: white; padding: 20px 48px;");
						client.println("text-decoration: none; font-size: 30px; margin: 8px; cursor: pointer; border-radius: 40px;}");
						client.println("fieldset{background-color: #eeeeee; border: none; border-radius: 28px; padding: 15px;  width:440px; min-width=440px;}");
						client.println("</style></head>");

						// Web Page 
						client.println("<body><h1>Caspa-PICO</h1><br><br><h2>Connexion des capteurs au wifi</h2>");
						if (flagLoading)
						{
							client.println("<h3><p style=\"color:green;\">Vérifier la connection sur l'écran</p></h3>");
							//client.println("<script>alert(\"Vérifier la connection sur l'écran, si il ne se connecte pas au bout d'une minute, veuillez rerentrée les donées\");</script>");
						}

						
						client.println();
						client.print("<fieldset>Voici les réseaux disponibles à proximité, choisissez le votre :<br>");
						client.print("<br><form action=\"/get\"> SSID: <select id=\"wifi\" name=\"inputSSID\">");

						//nWifi = WiFi.scanNetworks();
						for (int i = 0; i < nWifi; ++i)
						{
							// Print network found
							Serial.print(i + 1);
							Serial.print(": ");
							if(WiFi.SSID(i)!=NULL){
								Serial.println(WiFi.SSID(i));
								cWifi = WiFi.SSID(i);

								client.println("<option>");
								client.println(cWifi);
								client.println("</options>");
							}
						}

						client.println("</select>"); 
						client.println("</fieldset>"); 

						client.print("<br><br><fieldset>Entrez le mot de passe wifi ici :<br><br>");
						client.print("Mot de passe: <input type=\"password\" name=\"inputMDP\"></fieldset><br><br><input class=\"button\" type=\"submit\" value=\"Submit\"></form><br>");

						client.println("</body></html>");
						client.println();

						// break out of the while loop:
						break;
					}
					else
					{ // if you got a newline, then clear currentLine:
						currentLine = "";
					}
				}
				else if (c != '\r')
				{					  // if you got anything else but a carriage return character,
					currentLine += c; // add it to the end of the currentLine
				}

				if (currentLine.indexOf("get?inputSSID=") >= 0)
				{
					ClientValue = ClientValue + c;
					flagLoading=1;
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

char ConnectToWifi::tradHEX(char c)
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

String ConnectToWifi::urlDecode(String str)
{
	String temp;
	char charTransform;
	int len = str.length();

	for (int i = 0; i < len; i++)
	{
		if (str[i] == '%')
		{
			charTransform = tradHEX(str[i + 1]) * 16 + tradHEX(str[i + 2]);
			temp += charTransform;
			i = i + 2;
		}
		else
		{
			if (str[i] == '+')
				temp += ' ';
			else
				temp += str[i];
		}
	}
	return temp;
}

void ConnectToWifi::extractMDPSSID(int show)
{
	if (ClientValue != "")
	{
		MDP = "";
		IDwifi = "";
		size_t i = 1;
		while (ClientValue[i] != '\n' && ClientValue[i] != '\r' && !(ClientValue[i] == '&' && ClientValue[i + 1] == 'i' && ClientValue[i + 2] == 'n' && ClientValue[i + 3] == 'p' && ClientValue[i + 4] == 'u' && ClientValue[i + 5] == 't' && ClientValue[i + 6] == 'M' && ClientValue[i + 7] == 'D' && ClientValue[i + 8] == 'P' && ClientValue[i + 9] == '='))
		{
			IDwifi = IDwifi + ClientValue[i];
			i++;
		}
		i += 10;
		while (ClientValue[i] != '\n' && ClientValue[i] != '\r' && !(ClientValue[i] == ' ' && ClientValue[i + 1] == 'H' && ClientValue[i + 2] == 'T' && ClientValue[i + 3] == 'T' && ClientValue[i + 4] == 'P' && ClientValue[i + 5] == '/' && ClientValue[i + 6] == '1' && ClientValue[i + 7] == '.'))
		{
			MDP = MDP + ClientValue[i];
			i++;
		}
		if (show)
		{
			Serial.println("=====");
			Serial.println("ID et mdp:");
			Serial.println(IDwifi);
			Serial.println(MDP);
			Serial.println("=====");
		}
		flagBadWIFI = 0;
		flag = 1;
	}
	
}

void ConnectToWifi::setClient()
{
	delay(10);

	// decode
	IDwifi = urlDecode(IDwifi);
	MDP = urlDecode(MDP);
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
	WiFiMultit.addAP(BufID, BufMDP);

	Serial.println();
	Serial.println();
	Serial.print("Waiting for WiFi... ");

	while (WiFiMultit.run() != WL_CONNECTED)
	{
		Serial.print(".");
		delay(500);
		countWrongWifi++;
		if (countWrongWifi > 5)
		{
			countWrongWifi =0;
			MDP="";
			IDwifi="";
			ClientValue="";
			flag=0;
			flagBadWIFI=1;
			//Ssetup();
			return;
		}
	}
		

	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());

	delay(500);
}

void ConnectToWifi::connectToServ()
{
	flagLoading = 0;
	//    const uint16_t port = 80;
	//    const char * host = "192.168.1.1"; // ip or dns
	//    const uint16_t port = 1337;
	//    const char * host = "192.168.1.10"; // ip or dns
	const uint16_t port = 80;
	const char *host = "1.1.1.1"; // ip or dns

	Serial.print("Connecting to ");
	Serial.println(host);

	// Use WiFiClient class to create TCP Connexions
	WiFiClient client;

	if (!client.connect(host, port))
	{
		Serial.println("Connection failed.");
		Serial.println("Waiting 5 seconds before retrying...");
		delay(5000);
		return;
	}

	// This will send a request to the server
	// uncomment this line to send an arbitrary string to the server
	// client.print("Send this data to the server");
	// uncomment this line to send a basic document request to the server
	client.print("GET /index.html HTTP/1.1\n\n");

	int maxloops = 0;

	// wait for the server's reply to become available
	while (!client.available() && maxloops < 1000)
	{
		maxloops++;
		delay(1); // delay 1 msec
	}
	if (client.available() > 0)
	{
		// read back one line from the server
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

void ConnectToWifi::lauchConneciton()
{
	if (MDP == "" || IDwifi == "")
	{
		accespoint();
		extractMDPSSID(1);
	}
	else
	{
		if (flag && !flagBadWIFI)
		{
			setClient();
			flag = 0;
		}
		if (!flagBadWIFI) 
			connectToServ();
	}
}
