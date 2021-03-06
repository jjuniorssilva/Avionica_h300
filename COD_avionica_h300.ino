#include <SPI.h>
#include <LoRa.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define pin_batery 3
#define ss_lora 10
#define rst_lora 9
#define dio0_lora 2
#define BAND 433E6
#define pinServo 5
#define led1 1
#define led2 3
#define led3 4

Servo servoPara;
Adafruit_BMP280 bmp;

int rotina=0,ciclo=0,altura_min=50,altura_ant=0;
bool status_LoRa=false,status_alt_rotina=false;
unsigned int eeaddress=0;

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
  LoRa.setPins (ss_lora, rst_lora,dio0_lora);
  LoRa.setTxPower(20);//potência TX em dB, o padrão é 17
  //LoRa.setFrequency(BAND); // seta a frequencia do lora
  LoRa.onReceive(onReceive);//callback de recebimeno de dados do lora
  if (!LoRa.begin(BAND)){
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
