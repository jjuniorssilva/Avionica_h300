#include <SPI.h>
#include <LoRa.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define SS 18
#define RST 14
#define DI0 26
#define BAND 433E6
#define pinServo 5
#define led1 5
#define led2 5
#define led3 5

Servo servoPara;
Adafruit_BMP280 bmp;

int rotina=0,ciclo=0,altura_max=50,altura_ant=0;
bool status_LoRa=false;

//Aqui as variaveis serão convertada em m array de bytes para a trsmissão;
union packet_type_1 {
  //o struct agrupa os dados e o union propociona a recuparação dos bytes desse grupo;
   struct data{
      float pressao_bmp; 
      float alt_bmp;
      float batery;
      float AcX_mpu;
      float AcY_mpu;
      float AcZ_mpu;
      float Tmp_mpu;
      float GyX_mpu;
      float GyY_mpu;
      float GyZ_mpu;
      byte error;
      byte checksum;
   }data;
   byte data_byte[sizeof(data)];
};
// "inicaliza" o union para buscas;
union packet_type_1 packet_1;

void setup() {
  Serial.begin(9600);
  SPI.begin ();
  servoPara.attach(pinServo);
  LoRa.setPins (SS, RST);
  LoRa.setTxPower(20);//potência TX em dB, o padrão é 17
  //LoRa.setFrequency(433E6); // seta a frequencia do lora
  LoRa.onReceive(onReceive);//callback de recebimeno de dados do lora
  if (!LoRa.begin(433E6)){
    Serial.println(" LoRa Falhou!");
    while (1);
  }
  Serial.println("LoRa Pronto!!");
  if (!bmp.begin()) {  
    Serial.println("BMP Falhou!");
    while (1);
  }
  Serial.println("BMP Pronto!!");
}

void loop() {
  if(!status_LoRa){
    check_LoRa();// funcão verifica o lora e atualiza a variavel status_LoRa;
  }else{
     update_rotina();
     rotinas(rotina);
  }
}
