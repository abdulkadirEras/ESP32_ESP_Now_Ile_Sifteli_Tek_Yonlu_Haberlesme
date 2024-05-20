#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

#include <esp_now.h>
#include <WiFi.h>

#define wificonnect_led  2 //yesil
// mesaj yapısı oluşturuluyor. aynı mesaj yapısı alıcı da oluşturulmalıdır
typedef struct mesajYapisi 
{
  char metin[32];
  int tamSayi;
  float ondalikli;
  bool boolen;
} mesajYapisi;

//mesajYapisi ndan Verilerim adında değişken oluşturuluyor
mesajYapisi Verilerim;


uint8_t masterMacAddress[] = {0xc0, 0x49, 0xeF, 0xd3, 0x91, 0x24};//Kullanacağınız ESP32 Vericinin MAC adresini girin. BURADAKİ ÖRNEKTİR DENEMEYİN!
// PMK(Primary Master Key) and LMK(Local Master Key) keys. 16 byte olmalı
static const char* PMK_KEY_STR = "pmkSifre16Bytolm";//örnek bir şifre
static const char* LMK_KEY_STR = "lmkSifre16Bytolm";//örnek bir şifre

// Veri hangi ESP den geldiyse onun MAC adresini yazdırır
void MACEkranaYaz(const uint8_t * mac_addr)
{
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
}

// ne zaman veri gelirse gireceği callback fonksiyonu
void VeriAlindi(const uint8_t * mac, const uint8_t *gelenVeri, int VeriUzunlugu) 
{

  Serial.print("Verinin geldigi MAC adresi: ");
  MACEkranaYaz(mac);

  memcpy(&Verilerim, gelenVeri, sizeof(Verilerim));
  Serial.print("Bytes received: ");
  Serial.println(VeriUzunlugu);
  Serial.print("Char: ");
  Serial.println(Verilerim.metin);
  Serial.print("Int: ");
  Serial.println(Verilerim.tamSayi);
  Serial.print("Float: ");
  Serial.println(Verilerim.ondalikli);
  Serial.print("Bool: ");
  Serial.println(Verilerim.boolen);
  Serial.println();
}

  // Register the master as peer
  esp_now_peer_info_t peerInfo={};//={} yapılmazsa  ESPNOW: Peer interface is invalid Hatası veriyor
void setup() {

  pinMode(wificonnect_led,OUTPUT);
  // Init Serial Monitor
  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //ESP-NOW başlatıldı
  if (esp_now_init() != ESP_OK) 
  {
    Serial.println("ESP-NOW Baslatilamadi!!!");
    ESP.restart();
  }
  else
  {
    Serial.println("ESP-NOW Baslatildi");
    Serial.println("------Cihazin MAC Adresi---------");
    Serial.println(WiFi.macAddress());

    // Set the PMK key
    esp_now_set_pmk((uint8_t *)PMK_KEY_STR);
    
  
    memcpy(peerInfo.peer_addr, masterMacAddress, 6);
    peerInfo.channel = 0;
    // Setting the master device LMK key
    for (uint8_t i = 0; i < 16; i++) {
      peerInfo.lmk[i] = LMK_KEY_STR[i];
    }
    // Set encryption to true
    peerInfo.encrypt = true;
    
    // Add master as peer       
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
      Serial.println("Failed to add peer");
      return;
    }
    esp_now_register_recv_cb(VeriAlindi);
  }
  
}

void loop() 
{
  if(WiFi.isConnected() or esp_now_get_peer(masterMacAddress,&peerInfo) !=ESP_ERR_ESPNOW_NOT_FOUND )
  {
    
    digitalWrite(wificonnect_led,1);
    Serial.println("Cihaza Bagli");
    Serial.println(esp_now_is_peer_exist(masterMacAddress));
  
  }
  else
  {
    digitalWrite(wificonnect_led,0);
  }
}