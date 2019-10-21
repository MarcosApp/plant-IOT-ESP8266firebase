#include <ESP8266WiFi.h>                                                // esp8266 library
#include "FirebaseESP8266.h"
#include "DHT.h"
#define DHT_DATA_PIN 2
#define DHTTYPE DHT11

//defines
#define FIREBASE_HOST "https://soilmonitor-c8855.firebaseio.com"
#define FIREBASE_AUTH "Skau3AP0ZQG90aF90eLTkvvStrBwCL6qVKA7hVtU"
#define SSID_REDE " "  //coloque aqui o nome da rede que se deseja conectar
#define SENHA_REDE " "  //coloque aqui a senha da rede que se deseja conectar

#define    L1        370
#define    L2        600
#define    L3        1000

int ValorRecuperado = 0x00;
bool OperacaoSensor;
float UmidadePorcentual;
//constantes e variáveis globais
DHT dht(DHT_DATA_PIN, DHTTYPE);

WiFiClient client;
FirebaseData firebaseData;

//prototypes
void FazConexaoWiFi(void);
void RealizaLeituraSensor(int level1, int level2, int level3);

float FazLeituraUmidade(void);

String path = "/SoilMonitor_USJT";

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
    Firebase.setReadTimeout(firebaseData, 500 * 60);
    Firebase.setwriteSizeLimit(firebaseData, "tiny");
    delay(100);
    dht.begin();
    Firebase.setString(firebaseData, path + "/Sensor/ConectadoIP", WiFi.localIP().toString());
    delay(300);
    Firebase.setBool(firebaseData, path + "/BombaSubmersivel/StatusOperacao" ,false);
    delay(300);
}

void setup()
{  
    Serial.begin(9600);
    FazConexaoWiFi();
    Serial.println("Soil Monitor (USJT)");
}
 
//loop principal
void loop()
{
  
      if (Firebase.getBool(firebaseData, "/SoilMonitor_USJT/Sensor/StatusOperacao")) {

        if (firebaseData.dataType() == "boolean") {
            OperacaoSensor = firebaseData.boolData();
          }
        } else {
           Serial.println(firebaseData.errorReason());
      }
      
      if(OperacaoSensor) {  
        
        RealizaLeituraSensor(L1, L2, L3);                             //chama função que lê sensor de umidade
    
        float umidade = dht.readHumidity();
    
        Firebase.setDouble(firebaseData, path + "/TemperaturaAmbiente/Umidade", umidade);
        //Leitura de temperatura
    
        float temperaturaCelsius = dht.readTemperature();
    
        Firebase.setDouble(firebaseData, path + "/TemperaturaAmbiente/TemperaturaCelsius", temperaturaCelsius);
    
        float temperaturaFahrenheit = (dht.readTemperature()*1.8) + 32;
    
        Firebase.setDouble(firebaseData, path + "/TemperaturaAmbiente/TemperaturaFahrenheit", temperaturaFahrenheit);
    
        delay(100);
      }
}

void RealizaLeituraSensor(int level1, int level2, int level3)
{
    //lê valor do sinal analógico e salva em ValorRecuperado
    ValorRecuperado = analogRead(0);
    
   if(ValorRecuperado != 1024) {
      
      Serial.print((String)+  " Lendo Sensor: " + ValorRecuperado + " " ); //imprime no Serial Monitor
      
      float convert = 1.0 - (((float)ValorRecuperado) / 1024.0);
      
      UmidadePorcentual = (int) ((convert) * 100.0);
      
      Serial.println((String)+ " Umidade Percentual " +UmidadePorcentual+ "%" );
      
      Firebase.setDouble(firebaseData, path + "/Sensor/Umidade", UmidadePorcentual);
   
   } else {
      
      Serial.println("Sensor Desconectado");
      
      Firebase.setDouble(firebaseData, path + "/Sensor/Umidade", 0);
      
      Firebase.setString(firebaseData, path + "/Sensor/Status", "Sensor Desconectado");
   }
   
   delay(100); // Espera 5 segundos

// ================================================================
  if (ValorRecuperado < 1000 && ValorRecuperado < level1)             //identifica nível de umidade 0
  {
    Serial.println(" Status: Umidade  Alta");
    Firebase.setString(firebaseData, path + "/Sensor/Status", "Umidade  Alta");
  }
// ================================================================
  else if (ValorRecuperado >= level1 && ValorRecuperado < level2)   //identifica nível de umidade 1
  {
    Serial.println(" Status: Umidade Média");
    Firebase.setString(firebaseData, path + "/Sensor/Status", "Umidade  Média");
  }
// ================================================================
  else if (ValorRecuperado >= level2 && ValorRecuperado < level3)   //identifica nível de umidade 2
  {
    Serial.println(" Status: Umidade Baixa");
    Firebase.setString(firebaseData, path + "/Sensor/Status", "Umidade  Baixa");
  }
// ================================================================
  else if (ValorRecuperado > level3 && ValorRecuperado < 1024)     //identifica nível de umidade 3
  {
    Serial.println(" Status: Solo seco");
    Firebase.setString(firebaseData, path + "/Sensor/Status", "Solo seco");
  }
}
 
