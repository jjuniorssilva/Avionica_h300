#include <SPI.h>
#include <Servo.h>
#include <Wire.h>
#include <EEPROM.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#define pinoBuzzer 6 // Ajustar 
#define pin_batery 3
#define pinServo 5
#define led1 1
#define led2 3
#define led3 4

Servo servoPara;
Adafruit_BMP280 bmp;

int rotina=0,ciclo=0,altura_min=50,altura_ant=0;
bool status_LoRa=false,status_alt_rotina=false, status_calib=true;


//Aqui as variaveis serão convertada em m array de bytes para a trsmissão;
union packet_type_1 {
  //o struct agrupa os dados e o union propociona a recuparação dos bytes desse grupo;
   struct data{
      long time_atual;
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
   }data;
   byte data_byte[sizeof(data)];
};
union address{
  long eeaddress;
  byte eeaddress_byte[sizeof(eeaddress)];
};
union alt_init{
  float alt_inicial;
  byte alt_inicial_byte[sizeof(alt_inicial)];
};

// "inicaliza" o union para buscas;
union packet_type_1 packet_1;
union address address_eeprom;
union alt_init alt_init_eeprom;

void setup() {
  Serial.begin(9600);
  SPI.begin ();
  pinMode(pinoBuzzer, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(pin_batery, INPUT);
  servoPara.attach(pinServo);
  if (!bmp.begin()) {  
    Serial.println("BMP Falhou!");
    while (1);
  }
  Serial.println("BMP Pronto!!");
  if(get_backup_eeprom(0)!=0){
    for(int i=0; i<4;i++){
      address_eeprom.eeaddress_byte[i]=get_backup_eeprom(i);
    }
  } 
}

void loop() {
  packet_1.data.time_atual=millis();
  update_rotina();
  rotinas(rotina);
}
