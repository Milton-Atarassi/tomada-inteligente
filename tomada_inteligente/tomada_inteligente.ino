//Tomada inteligente
//Projeto Integrador Oitavo Semestre
//Grupo SM2 Polo Sao Jose dos Campos
//Univesp

#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"
#include <EEPROM.h>
char ssid[15];
char password[15];

//id do script do Google Spreadsheet
const char *GScriptId = "AKfycbykjH5PVwp21Xv9VY3NDeWLVUQKP1fhSmaKzHvTS2RokKZzbG-K";

// Intervalo de envio de dados
const int dataPostDelay = 5000;

const char* host = "script.google.com";
const char* googleRedirHost = "script.googleusercontent.com";

const int httpsPort =  443;
HTTPSRedirect client(httpsPort);

//url para metodo get
String url = String("/macros/s/") + GScriptId + "/exec?";

const char* fingerprint = "F0 5C 74 77 3F 6B 25 D7 3B 66 4D 43 2F 7E BC 5B E9 28 86 AD";

// Entrada da leitura analogica
const int AnalogIn     = A0;

unsigned long prevMillis = millis();

//estados das tomadas ON/OFF
bool estado1 = false;
bool estado2 = false;
bool estado3 = false;


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

  Serial.print(String("Conectando em "));
  Serial.println(host);

  bool flag = false;
  for (int i = 0; i < 5; i++) {
    int retval = client.connect(host, httpsPort);
    if (retval == 1) {
      flag = true;
      break;
    }
    else
      Serial.println("Falha na conexao. Reconectando...");
  }

  //Status da conexao, 1 = Conectado, 0 nao conectado.
  Serial.println("Status da conexao: " + String(client.connected()));
  Serial.flush();

  if (!flag) {
    Serial.print("Nao foi possivel conectar em: ");
    Serial.println(host);
    Serial.println("Saindo...");
    Serial.flush();
    return;
  }

  // Dados serao enviados mesmo se o certificado nao for compativel
  if (client.verify(fingerprint, host)) {
    Serial.println("Certificado match.");
  } else {
    Serial.println("Certificado mis-match");
  }
}

// Metodo de envio dos dados e recebimento da resposta do servidor
void postData(String estado1, String estado2, String estado3, float corrente) {
  wifi_connect();
  if (!client.connected()) {
    Serial.println("Conectando ao servidor novamente...");
    client.connect(host, httpsPort);
  } else {
    String urlFinal = url + "tomada1=" + estado1 + "&tomada2=" + estado2 + "&tomada3=" + estado3 + "&corrente=" + String(corrente);
    client.printRedir(urlFinal, host, googleRedirHost);

    digitalWrite(2, LOW);
    delay(100);
    digitalWrite(2, HIGH);
  }
}

// Enviando os dados em intervalos definidos
void loop() {
  wifi_connect();

  unsigned long time = (millis() - prevMillis);
  if (time > dataPostDelay) {
    prevMillis = millis();

    if (client.isTomada1On() != NULL) {
      estado1 = client.isTomada1On();  //tomada 1
      estado2 = client.isTomada2On();  //tomada 2
      estado3 = client.isTomada3On();  //tomada 3
    }
      Serial.println(" estado1:" + String(estado1) + " estado2:" + String(estado2) + " estado3:" + String(estado3));
      if (!estado1) {
        digitalWrite(14, LOW);
      } else {
        digitalWrite(14, HIGH);
      }

      if (!estado2) {
        digitalWrite(12, LOW);
      } else {
        digitalWrite(12, HIGH);
      }

      if (!estado3) {
        digitalWrite(13, LOW);
      } else {
        digitalWrite(13, HIGH);
      }
    

    float corrente = correnteRMS();

    // Envio dos dados
    postData(String(estado1), String(estado2), String(estado3), corrente);
    Serial.print("enviado - tempo: ");
    Serial.println(time);
  }
  delay(100);
}

void wifi_connect() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  Serial.println("\nConectando ao wifi");
  Serial.flush();
  EEPROM.get(0, ssid);
  EEPROM.get(15, password);

  WiFi.begin(ssid, password);
  delay(4000);
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nTentando coneccao WPS ...");
    WiFi.beginWPSConfig();
    delay(3000);
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Conectado!");
      Serial.println(WiFi.SSID());
      Serial.println(" IP: ");
      Serial.println(WiFi.localIP());
      char id[15];
      WiFi.SSID().toCharArray(id, 15);
      char psd[15];
      WiFi.psk().toCharArray(psd, 15);
      //salvando os dados da conexao wifi
      EEPROM.put(0, id);
      EEPROM.put(15, psd);
    }
    else {
      Serial.println("Falha na coneccao!");
    }
  }
  else {
    Serial.println("Conectado!");
    Serial.println(WiFi.SSID());
    Serial.println(" IP: ");
    Serial.println(WiFi.localIP());
  }
}

float correnteRMS() {

  const unsigned long sampleTime = 100UL;  
  const unsigned long numSamples = 50UL;
  const unsigned long sampleInterval = sampleTime / numSamples;
  const int adc_zero = 510;
  unsigned long currentAcc = 0;
  unsigned int count = 0;
  unsigned long prevMillis = millis() - sampleInterval ;
  while (count < numSamples)
  {
    if (micros() - prevMillis >= sampleInterval)
    {
      int adc_raw = analogRead(AnalogIn) - adc_zero;
      currentAcc += (unsigned long)(adc_raw * adc_raw);
      ++count;
      prevMillis += sampleInterval;
      Serial.println(currentAcc);
    }
    delay(1);
  }

  float rms = sqrt((float)currentAcc / (float)numSamples) * (75.7576 / 1024.0);
  Serial.println("corrente: " + String(rms)+"numero de medidas"+count);
  return rms;
}
