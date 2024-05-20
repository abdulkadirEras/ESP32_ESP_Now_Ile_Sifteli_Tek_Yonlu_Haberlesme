#include <Arduino.h>

#include <esp_now.h>
#include <WiFi.h>

#define wificonnect_led  2 //yesil

// Tüm Adreslere yayın yapıyor
uint8_t yayinAdresi[] = {0x30, 0xC6, 0xF7, 0x20, 0x32, 0x3C};//Kullanacağınız ALıcı ESP32 nin MAc adresini yazın. BURADA YAZILI OLAN ÖRNEKTİR DENEMEYİN!

// mesaj yapısı oluşturuluyor. aynı mesaj yapısı alıcı da oluşturulmalıdır
typedef struct mesajYapisi {
  char metin[32];
  int tamSayi;
  float ondalikli;
  bool boolen;
} mesajYapisi;

//mesajYapisi ndan Verilerim adında değişken oluşturuluyor
mesajYapisi Verilerim;

esp_now_peer_info_t peerInfo;

// PMK(Primary Master Key) and LMK(Local Master Key) keys. 16 byte olmalı
static const char* PMK_KEY_STR = "pmkSifre16Bytolm";//ÖRNEK BİR ŞİFRE
static const char* LMK_KEY_STR = "lmkSifre16Bytolm";//ÖRNEK BİR ŞİFRE


// Veri hangi ESP den geldiyse onun MAC adresini yazdırır
void MACEkranaYaz(const uint8_t * mac_addr){
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
}

// veri gönderildiğinde durumu haber verecek callback fonksiyonu
void GondermeIslemiSonucu(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nonceki veri paketin durumu:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "teslimat basarili" : "teslimat Basarisiz!!!");
}
 


void setup() {

  pinMode(wificonnect_led,OUTPUT);
  // Init Serial Monitor
  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //ESP-NOW başlatıldı
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Baslatilamadi!!!");
    ESP.restart();
    
  }
  else
  {
    Serial.println("ESP-NOW Baslatildi");
    Serial.println("------Cihazin MAC Adresi---------");
    Serial.println(WiFi.macAddress());
    // Set PMK key
    esp_now_set_pmk((uint8_t *)PMK_KEY_STR);
    // Once ESPNow is successfully Init, we will register for Send CB to
    // get the status of Trasnmitted packet
    esp_now_register_send_cb(GondermeIslemiSonucu);

    // Register peer
    memcpy(peerInfo.peer_addr, yayinAdresi, 6);
    //hangi kanalınk kullanılacağı ayarlandı
    peerInfo.channel = 0;  

     //Set the receiver device LMK key
    for (uint8_t i = 0; i < 16; i++) {
      peerInfo.lmk[i] = LMK_KEY_STR[i];
    }

    //Verinin şifrelenmeyeceği ayarlandı
    peerInfo.encrypt = true;

    // Gönderilecek cihazın mac adresi ekleniyor      
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
      Serial.println("Cihaz Eklenemedi!!!");

    }
    else
    {
      Serial.println("Cihaz eklendi");
    }
  }

  
}
 esp_err_t gondermeSonucu;
void loop() {
  // gönderilecek veriler ayarlandı
  strcpy(Verilerim.metin, "Vericinin Str Verisi");
  Verilerim.tamSayi = random(1,20);
  Verilerim.ondalikli = 1.2;
  Verilerim.boolen = false;
  
  // ESP-now ile veriler gönderildi
  gondermeSonucu = esp_now_send(yayinAdresi ,(uint8_t *) &Verilerim, sizeof(Verilerim));
   
  if ( gondermeSonucu == ESP_OK) {
    Serial.println("veri gonderme basarili");
    digitalWrite(wificonnect_led,0);
  }
  else if(gondermeSonucu==ESP_ERR_ESPNOW_NOT_FOUND)//esp bir eşe bağlı değilse
  {
    digitalWrite(wificonnect_led,1);
  }
  else {
    Serial.println("veri gonderilemedi");
  }
  delay(1);
}