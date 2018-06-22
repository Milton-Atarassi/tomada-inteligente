//Tomada inteligente
//Grupo SM2 Polo Sao Jose dos Campos
//Univesp

#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"
#include <EEPROM.h>
char ssid[15];
char password[15];

const char *GScriptId = "AKfycbykjH5PVwp21Xv9VY3NDeWLVUQKP1fhSmaKzHvTS2RokKZzbG-K"; // "AKfycbyb2Ij6O43uf3rOgkaiK6qJiv2iIeROw_5RU5pSzvqVVegdCBA";

// Push data on this interval
const int dataPostDelay = 5000;  // 15 minutes = 15 * 60 * 1000

const char* host = "script.google.com";
const char* googleRedirHost = "script.googleusercontent.com";

const int httpsPort =  443;
HTTPSRedirect client(httpsPort);

// Prepare the url (without the varying data)
String url = String("/macros/s/") + GScriptId + "/exec?";

const char* fingerprint = "F0 5C 74 77 3F 6B 25 D7 3B 66 4D 43 2F 7E BC 5B E9 28 86 AD";

// We will take analog input from A0 pin
const int AnalogIn     = A0;

unsigned long prevMillis = millis();

bool estado1, estado2, estado3;


void setup() {
  //LED
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);

  //tomada 1
  pinMode(14, OUTPUT);
  digitalWrite(14, LOW);

  //tomada 2
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);

  //tomada 3
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  EEPROM.begin(512);

  EEPROM.get(0, ssid);
  EEPROM.get(15, password);

  wifi_connect();

  Serial.print(String("Connecting to "));
  Serial.println(host);

  bool flag = false;
  for (int i = 0; i < 5; i++) {
    int retval = client.connect(host, httpsPort);
    if (retval == 1) {
      flag = true;
      break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }

  // Connection Status, 1 = Connected, 0 is not.
  Serial.println("Connection Status: " + String(client.connected()));
  Serial.flush();

  if (!flag) {
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    Serial.flush();
    return;
  }

  // Data will still be pushed even certification don't match.
  if (client.verify(fingerprint, host)) {
    Serial.println("Certificate match.");
  } else {
    Serial.println("Certificate mis-match");
  }
}

// This is the main method where data gets pushed to the Google sheet
void postData(String estado1, String estado2, String estado3, float corrente) {
  //  HTTPSRedirect client(httpsPort);
  if (!client.connected()) {
    Serial.println("Connecting to client again...");
    client.connect(host, httpsPort);
  } else {
    String urlFinal = url + "tomada1=" + estado1 + "&tomada2=" + estado2 + "&tomada3=" + estado3 + "&corrente=" + String(corrente);
    client.printRedir(urlFinal, host, googleRedirHost);

    digitalWrite(2, LOW);
    delay(100);
    digitalWrite(2, HIGH);
  }
}

// Continue pushing data at a given interval
void loop() {
  unsigned long time = (millis() - prevMillis);
  if (time > dataPostDelay) {
    prevMillis = millis();

    wifi_connect();

    int data = analogRead(AnalogIn);


    if (WiFi.status() == WL_CONNECTED && client.connected()) {
      estado1 = client.isTomada1On();  //tomada 1
      estado2 = client.isTomada2On();  //tomada 2
      estado3 = client.isTomada3On();  //tomada 3
    }

    if (estado1) {
      digitalWrite(14, HIGH);
    } else {
      digitalWrite(14, LOW);
    }

    if (estado2) {
      digitalWrite(12, HIGH);
    } else {
      digitalWrite(12, LOW);
    }

    if (estado3) {
      digitalWrite(13, HIGH);
    } else {
      digitalWrite(13, LOW);
    }

    // Post these information
    postData(String(estado1), String(estado2), String(estado3), data);
    Serial.print("enviado - tempo: ");
    Serial.println(time);
  }
  delay(100);
  //  delay (dataPostDelay);
}

void wifi_connect() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  Serial.println("Connecting to wifi");
  Serial.flush();

  WiFi.begin(ssid, password);

  // Long delay required especially soon after power on.
  delay(4000);
  // Check if WiFi is already connected and if not, begin the WPS process.
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nAttempting WPS connection ...");
    WiFi.beginWPSConfig();
    // Another long delay required.
    delay(3000);
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Connected!");
      Serial.println(WiFi.SSID());
      Serial.println(" IP address: ");
      Serial.println(WiFi.localIP());
      char id[15];
      WiFi.SSID().toCharArray(id, 15);
      char psd[15];
      WiFi.psk().toCharArray(psd, 15);

      EEPROM.put(0, id);
      EEPROM.put(15, psd);
    }
    else {
      Serial.println("Connection failed!");
    }
  }
  else {
    Serial.println("Connected!");
    Serial.println(WiFi.SSID());
    Serial.println(" IP address: ");
    Serial.println(WiFi.localIP());
  }
}
