#include <ESP8266WiFi.h>                                                // esp8266 library
#include "FirebaseESP8266.h"

int Rele = D1;
//defines
#define FIREBASE_HOST "https://soilmonitor-c8855.firebaseio.com"
#define FIREBASE_AUTH "Skau3AP0ZQG90aF90eLTkvvStrBwCL6qVKA7hVtU"
#define SSID_REDE ""  //coloque aqui o nome da rede que se deseja conectar
#define SENHA_REDE "" 

#define    L1        300
#define    L2        500

WiFiClient client;
FirebaseData firebaseData;

int ValorRecuperado = 0x00;
bool OperacaoStatusBomba;
String path = "/SoilMonitor_USJT";

void FazConexaoWiFi(void);
void VerificarStatusOperacoes(void);
void RealizaLeituraSensor(int level1, int level2);

//Função: faz a conexão WiFI
//Parâmetros: nenhum
//Retorno: nenhum
void FazConexaoWiFi(void)
{
    client.stop();
    Serial.println("Conectando-se à rede WiFi...");
    Serial.println();  
    delay(1000);
    WiFi.begin(SSID_REDE, SENHA_REDE);
 
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }
 
    Serial.println("");
    Serial.println("WiFi connectado com sucesso!");  
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.reconnectWiFi(true);
    Firebase.setReadTimeout(firebaseData, 600 * 60);
    Firebase.setwriteSizeLimit(firebaseData, "tiny");
    delay(100);
    Firebase.setString(firebaseData, path + "/Sensor/ConectadoIP", WiFi.localIP().toString());
    delay(300);
    Firebase.setBool(firebaseData, path + "/BombaSubmersivel/StatusOperacao" ,false);
    delay(300);
}

void setup() {
 Serial.begin(9600);
  FazConexaoWiFi();
  Serial.println("Soil Monitor (USJT)");
  pinMode(Rele, OUTPUT); 
  digitalWrite(Rele, HIGH); //Desliga rele 1
}

void loop() {

    VerificarStatusOperacoes();

    delay(1000);
    
    RealizaLeituraSensor(L1, L2);
    
    delay(1000);

    if(OperacaoStatusBomba){
        digitalWrite(Rele, LOW);
        delay(100);///Desliga rele 1
    }
    else {
      digitalWrite(Rele, HIGH);
      delay(100);//Desliga rele 1
    }
}

void VerificarStatusOperacoes() {
  
  if (Firebase.getBool(firebaseData, "/SoilMonitor_USJT/BombaSubmersivel/StatusOperacao")) {

        if (firebaseData.dataType() == "boolean") {
            OperacaoStatusBomba = firebaseData.boolData();
          }
        } else {
           Serial.println(firebaseData.errorReason());
      }
}

void RealizaLeituraSensor(int level1, int level2)
{
    //lê valor do sinal analógico e salva em ValorRecuperado
    ValorRecuperado = analogRead(0);
    delay(500);
    Serial.print((String)+  " Lendo Sensor: " + ValorRecuperado + " " );
    delay(500);

// ================================================================
  if (ValorRecuperado < level1)             //identifica nível de umidade 0
  {
    Serial.println(" Status: Sem previsão de Chuva");
    Firebase.setString(firebaseData, path + "/SensorChuva/Status", "Sem previsão de Chuva");
    delay(500);
  }
// ================================================================
  else if (ValorRecuperado <= level2 && ValorRecuperado >= level1)   //identifica nível de umidade 1
  {
    Serial.println(" Status: Chuva Moderada");
    Firebase.setString(firebaseData, path + "/SensorChuva/Status", " Chuva moderada");
    delay(500);
  }
// ================================================================
  else if (ValorRecuperado >= level2)   //identifica nível de umidade 2
  {
    Serial.println(" Status: Chuva Intensa");
    Firebase.setString(firebaseData, path + "/SensorChuva/Status", "Chuva Intensa");
    delay(500);
  }
}
