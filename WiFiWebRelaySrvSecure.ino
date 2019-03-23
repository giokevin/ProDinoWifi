// WiFiWebRelaySrvSecure.ino
// Company: KMP Electronics Ltd, Bulgaria
// Web: http://kmpelectronics.eu/
// Supported boards:
//    KMP ProDino WiFi-ESP WROOM-02 (http://www.kmpelectronics.eu/en-us/products/prodinowifi-esp.aspx)
// Description:
//    Secure Web server relay manipulation example.
// Example link: http://www.kmpelectronics.eu/en-us/examples/dinowifiesp/wifiwebrelayserver.aspx
// Version: 1.0.0
// Date: 1-03.2019
// Authors: Plamen Kovandjiev <p.kovandiev@kmpelectronics.eu>, Giorgio Piccinini <gio.piccio@hotmail.it>

#include <KMPDinoWiFiESP.h>
#include <KMPCommon.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServerSecure.h>
#include <ESP8266mDNS.h>

BearSSL::ESP8266WebServerSecure server(443);

const char GREEN[] = "#90EE90"; // LightGreen
const char RED[] = "#FF4500"; // OrangeRed
const char* ssid = "";
const char* password = "";
const char* www_realm = "";
const char* www_username = "";
const char* www_password = "";
String authFailResponse = "Authentication Failed";

static const char serverCert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
// Your server certificate
-----END CERTIFICATE-----

)EOF";

static const char serverKey[] PROGMEM =  R"EOF(
-----BEGIN RSA PRIVATE KEY-----
// Your server key
-----END RSA PRIVATE KEY-----
)EOF";

void HandleRootPage()
{
 
  if (!server.authenticate(www_username, www_password))
  {
    return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
  }
  
  if (server.method() == HTTP_POST && (server.args() == 1 || server.args() == 2))
  {

    String argName = server.argName(0);
    char * x [] = { "r1", "r2", "r3", "r4"};
    char * s = new char[argName.length() + 1];
    strcpy(s, argName.c_str());

    int len = sizeof(x) / sizeof(x[0]);
    int i;
    String check = "FALSE";

    for (i = 0; i < len; ++i)
    {
      if (!strcmp(x[i], s))
      {
        check = "TRUE";
      }
    }
    
    if (argName.length() == 2 && check == "TRUE")
    {
      int relayNumber = CharToInt(argName[1]) - 1;
      Serial.print("Relay number: ");
      Serial.println(relayNumber);
      
      if (relayNumber >= 0 && relayNumber < RELAY_COUNT)
      {
        String argValue = server.arg(0);
        Serial.print("Argument value: ");
        Serial.println(argValue);

        if (argValue == W_ON || argValue == W_OFF)
        {
          KMPDinoWiFiESP.SetRelayState((uint8_t)relayNumber, argValue == W_ON);
        }
      }
    }
    else {

      server.send(200, "text/plain", "Parameters validation failed");

    }
  }

  server.send(200, TEXT_HTML, BuildPage());
 
}

void setup(void){
  
  Serial.begin(115200);
  KMPDinoWiFiESP.init();
  WiFi.begin(ssid, password);
  Serial.print("\n\r \n\rConnecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.setRSACert(new BearSSL::X509List(serverCert), new BearSSL::PrivateKey(serverKey));
  server.on("/", HandleRootPage);
  server.begin();
  
  Serial.println("HTTPS server started");
}

void loop(void){
  server.handleClient();
  MDNS.update();
}

String BuildPage()
{
  String page =
    "<html><head><title>"
    + String(KMP_ELECTRONICS_LTD)
    + " "
    + String(PRODINO_WIFI)
    + "</title></head>"
    + "<body><div style='text-align: center'>"
    + "<br><hr />"
    + "<h1 style = 'color: #0066FF;'>"
    + String(PRODINO_WIFI)
    + "</h1>"
    + "<hr /><br><br>"
    + ""
    + "<table border='1' width='300' cellpadding='5' cellspacing='0' align='center' style='text-align:center; font-size:large; font-family:Arial,Helvetica,sans-serif;'>";

  String rows = "";
  for (uint8_t i = 0; i < RELAY_COUNT; i++)
  {
    
    String relayNumber = String(i + 1);
    rows += "<tr><td>Relay " + relayNumber + "</td>";

    char* cellColor;
    char* cellStatus;
    char* nextRelayStatus;
    if (KMPDinoWiFiESP.GetRelayState(i))
    {
      cellColor = (char*)RED;
      cellStatus = (char*)W_ON;
      nextRelayStatus = (char*)W_OFF;
    }
    else
    {
      cellColor = (char*)GREEN;
      cellStatus = (char*)W_OFF;
      nextRelayStatus = (char*)W_ON;
    }

    rows += "<td bgcolor='" + String(cellColor) + "'>" + String(cellStatus) + "</td>";
    rows += "<td><input type='button' onclick='send(this)' name='r" + String(relayNumber) + "' value='" + String(nextRelayStatus) + "'/></td></tr>";
  }

  return page + rows
         + "</table></div>" +
         "<script>function send(e) {var XHR = new XMLHttpRequest();var formData = new FormData();formData.append(e.getAttribute('name'), e.getAttribute('value'));XHR.addEventListener('load', function(event) {document.location.reload()});XHR.open('POST', '/');XHR.send(formData);}" +
         "</script>" +
         "</body></html>";
}
