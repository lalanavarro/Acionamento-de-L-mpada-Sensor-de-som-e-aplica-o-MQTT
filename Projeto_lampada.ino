#include <ESP8266WiFi.h>
#include <PubSubClient.h>
 
const char* ssid = "******";
const char* password =  "*******";
const char* mqttServer = "hairdresser.cloudmqtt.com";
const int mqttPort =  15501;
const char* mqttUser = "lcflwrnr";
const char* mqttPassword = "TsfavFov39nP";
const char* mqttTopicSub ="lampada";   

#define psom    5
#define prele    4
boolean rele = HIGH;

WiFiClient espClient;
PubSubClient client(espClient);

 
 
void setup() {
 
  Serial.begin(115200);
  pinMode(psom, INPUT); //Coloca o pino do sensor de som como entrada
  pinMode(prele, OUTPUT);
  digitalWrite(prele, rele);
  
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    #ifdef DEBUG
    Serial.println("Conectando ao WiFi..");
    #endif
  }
  #ifdef DEBUG
  Serial.println("Conectado na rede WiFi");
  #endif
 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  while (!client.connected()) {
    #ifdef DEBUG
    Serial.println("Conectando ao Broker MQTT...");
    #endif
 
    if (client.connect("ESP32Client", mqttUser, mqttPassword )) {
      #ifdef DEBUG
      Serial.println("Conectado");  
      #endif
 
    } else {
      #ifdef DEBUG 
      Serial.print("falha estado  ");
      Serial.print(client.state());
      #endif
      delay(2000);
 
    }
  }

  //subscreve no tópico
  client.subscribe(mqttTopicSub);
 
}
 
void callback(char* topic, byte* payload, unsigned int length) {

  //armazena msg recebida em uma sring
  payload[length] = '\0';
  String strMSG = String((char*)payload);

  #ifdef DEBUG
  Serial.print("Mensagem chegou do tópico: ");
  Serial.println(topic);
  Serial.print("Mensagem:");
  Serial.print(strMSG);
  Serial.println();
  Serial.println("-----------------------");
  #endif

  //aciona saída conforme msg recebida 
  if (strMSG == "on"){ 
     rele = !rele;
     digitalWrite(prele, rele);   //coloca saída em LOW para ligar
  }
  
}

//função pra reconectar ao servido MQTT
void reconect() {
  //Enquanto estiver desconectado
  while (!client.connected()) {
    #ifdef DEBUG
    Serial.print("Tentando conectar ao servidor MQTT");
    #endif
     
    bool conectado = strlen(mqttUser) > 0 ?
                     client.connect("ESP32Client", mqttUser, mqttPassword) :
                     client.connect("ESP32Client");

    if(conectado) {
      #ifdef DEBUG
      Serial.println("Conectado!");
      #endif
      //subscreve no tópico
      client.subscribe(mqttTopicSub, 1); //nivel de qualidade: QoS 1
    } else {
      #ifdef DEBUG
      Serial.println("Falha durante a conexão.Code: ");
      Serial.println( String(client.state()).c_str());
      Serial.println("Tentando novamente em 10 s");
      #endif
      //Aguarda 10 segundos 
      delay(10000);
    }
  }
}
 
void loop() {
  int s = digitalRead(psom); //Verifica se a saida do sensor esta ativa
   if(s){ // Se houver captura de som
    rele = !rele; //muda o estado do relé, se estiver acessa devido ao mqtt, com a palma da mão ele apaga, vice e versa.
    digitalWrite(prele, rele); // Manda o valor da variavel para o rele
    delay(1000); // 
   }
  if (!client.connected()) {
    reconect();
  }
  client.loop();
}
