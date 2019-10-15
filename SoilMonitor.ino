#include <ESP8266WiFi.h>  //essa biblioteca já vem com a IDE. Portanto, não é preciso baixar nenhuma biblioteca adicional
#include "DHT.h"
#define DHT_DATA_PIN 2
#define DHTTYPE DHT11

//defines
#define SSID_REDE ""  //coloque aqui o nome da rede que se deseja conectar
#define SENHA_REDE ""  //coloque aqui a senha da rede que se deseja conectar

#define    L1        310
#define    L2        580
#define    L3        850

int ValorRecuperado = 0x00;
float UmidadePercentual;
//constantes e variáveis globais
DHT dht(DHT_DATA_PIN, DHTTYPE);


WiFiClient client;
 
//prototypes
void FazConexaoWiFi(void);
void RealizaLeituraSensor(int level1, int level2, int level3);

float FazLeituraUmidade(void);

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
    dht.begin();
    delay(1000);
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
  RealizaLeituraSensor(L1, L2, L3);                             //chama função que lê sensor de umidade
  float umidade = dht.readHumidity();
  //Leitura de temperatura
  float temperaturaCelsius = dht.readTemperature();
  float temperaturaFahrenheit = (dht.readTemperature()*1.8) + 32;
   Serial.print("Temperatura Celsius: ");
   Serial.print(temperaturaCelsius);
   Serial.print("Temperatura Fahrenheit: ");
   Serial.print(temperaturaFahrenheit);
   Serial.print(" Umidade: ");
   Serial.println(umidade);
  delay(741);
}


void RealizaLeituraSensor(int level1, int level2, int level3)
{

  ValorRecuperado = analogRead(0);                                 //lê valor do sinal analógico e salva em ValorRecuperado
  Serial.println("");
  Serial.print("Lendo Sensor: ");                                  //imprime no Serial Monitor
  Serial.print(ValorRecuperado);                                         //imprime no Serial Monitor
  UmidadePercentual = 100 * ((978-(float)ValorRecuperado) / 978);
   Serial.print("[Umidade Percentual] ");
   Serial.print(UmidadePercentual);
   Serial.println("%");
 
// ================================================================
  if (ValorRecuperado > 0 && ValorRecuperado < level1)             //identifica nível de umidade 0
  {
    Serial.println(" Status: Umidade Alta");

 
  }

// ================================================================
  else if (ValorRecuperado > level1 && ValorRecuperado < level2)   //identifica nível de umidade 1
  {
    Serial.println(" Status: Umidade Média");

 
  }
 
// ================================================================
  else if (ValorRecuperado > level2 && ValorRecuperado < level3)   //identifica nível de umidade 2
  {
    Serial.println(" Status: Umidade Baixa");


  }


// ================================================================
  else if (ValorRecuperado > level3 && ValorRecuperado < 1024)     //identifica nível de umidade 3
  {
    Serial.println(" Status: Solo seco");


  }
  
  
} //
 
