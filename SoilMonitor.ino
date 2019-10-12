#include <ESP8266WiFi.h>  //essa biblioteca já vem com a IDE. Portanto, não é preciso baixar nenhuma biblioteca adicional
 
//defines
#define SSID_REDE "Megaware"  //coloque aqui o nome da rede que se deseja conectar
#define SENHA_REDE "mamae1002"  //coloque aqui a senha da rede que se deseja conectar

#define    L1        169
#define    L2        340
#define    L3        511
#define    L4        682
#define    L5        853

int ValorRecuperado = 0x00;
float UmidadePercentual;
//constantes e variáveis globais
char EnderecoAPIThingSpeak[] = "api.thingspeak.com";
String ChaveEscritaThingSpeak = "KGFP6X4JQTJZFIAP";

WiFiClient client;
 
//prototypes
void FazConexaoWiFi(void);
void RealizaLeituraSensor(int level1, int level2, int level3, int level4, int level5);

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
  RealizaLeituraSensor(L1, L2, L3, L4, L5);                             //chama função que lê sensor de umidade
  delay(741);
}


void RealizaLeituraSensor(int level1, int level2, int level3, int level4, int level5)
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
    Serial.println(" Status: Solo totalmente umido");

 
  }

// ================================================================
  else if (ValorRecuperado > level1 && ValorRecuperado < level2)   //identifica nível de umidade 1
  {
    Serial.println(" Status: Umidade media");

 
  }
 
// ================================================================
  else if (ValorRecuperado > level2 && ValorRecuperado < level3)   //identifica nível de umidade 2
  {
    Serial.println(" Status: Umidade minima");


  }


// ================================================================
  else if (ValorRecuperado > level3 && ValorRecuperado < level4)   //identifica nível de umidade 3
  {
    Serial.println(" Status: Solo secando");

 
  }


// ================================================================
  else if (ValorRecuperado > level4 && ValorRecuperado < level5)   //identifica nível de umidade 4
  {
    Serial.println(" Status: Solo quase seco");


  }


// ================================================================
  else if (ValorRecuperado > level5 && ValorRecuperado < 1024)     //identifica nível de umidade 5
  {
    Serial.println(" Status: Solo seco");


  }
  
  
} //
 
