
//----------bmp280-------------------
void getDataBMP(){
  float alt0=0,alt1=0,alt2=0,alt3=0,media_alt,percent=0.05;
  packet_1.data.pressao_bmp = bmp.readPressure();
  alt0=bmp.readAltitude(1013.25);
  alt1=bmp.readAltitude(1013.25);// 
  alt2=bmp.readAltitude(1013.25);// 
  alt3=bmp.readAltitude(1013.25);// 
  media_alt = (alt1+alt2+alt3)/3;
  if(alt0>=(media_alt+(media_alt*percent)) || alt0<=(media_alt-(media_alt*percent))){
    packet_1.data.alt_bmp = media_alt;
  }else{
    packet_1.data.alt_bmp =alt0;
  }
}
bool check_BMP(){

}
//----------MPU6050-------------------
void getDataMPU(){
  Wire.beginTransmission(0x68);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  //Solicita os dados do sensor
  Wire.requestFrom(0x68,14,true);  
  //Armazena o valor dos sensores nas variaveis correspondentes
  packet_1.data.AcX_mpu=Wire.read()<<8|Wire.read();  //0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  packet_1.data.AcY_mpu=Wire.read()<<8|Wire.read();  //0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  packet_1.data.AcZ_mpu=Wire.read()<<8|Wire.read();  //0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  packet_1.data.Tmp_mpu=Wire.read()<<8|Wire.read();  //0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  packet_1.data.GyX_mpu=Wire.read()<<8|Wire.read();  //0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  packet_1.data.GyY_mpu=Wire.read()<<8|Wire.read();  //0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  packet_1.data.GyZ_mpu=Wire.read()<<8|Wire.read();  //0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
}
bool check_MPU(){

}
//----------EEPROM-------------------
void writeEEPROM(int deviceaddress, unsigned int eeaddress, byte data ) { // salva o dado (byte) na posição eeaddress
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.write(data);
  Wire.endTransmission();
  delay(4);
}
void sendDataEEPROM(){ // consumo médio de tempo 160ms
 for(int i=1;i<=40;i++){ // no struct o dados relevantes então no array de bytes [1-40]
  writeEEPROM(0x50,eeaddress,packet_1.data_byte[i]);
  eeaddress++;
 }
}
bool check_EEPROM(){

}
//----------LORA-------------------
bool check_LoRa(){
  
}
void SendStruct(const void *TheStructure, uint16_t size_) {
  LoRa.beginPacket();
  LoRa.write((uint8_t *) TheStructure, size_);
  LoRa.endPacket();
  digitalWrite(led3, HIGH);
  delay(50);
  digitalWrite(led3, LOW);
}
void SendDataBatery(float batery) {
  LoRa.beginPacket();
  LoRa.write(batery);
  LoRa.endPacket();
  digitalWrite(led3, HIGH);
  delay(50);
  digitalWrite(led3, LOW);
}
void onReceive(int packetSize) {
 if (packetSize){
  LoRa.readBytes((uint8_t *)&packet_1.data_byte, sizeof(packet_1.data));
 }
}
// ------------Bateria-------
void getDataBatery(){
  int val = analogRead(0);
  packet_1.data.batery = map(val, 0, 1023, 0, 100);// precisa ser adaptado
}
float getdataBatery_end(){
  int val = analogRead(0);
  return map(val, 0, 1023, 0, 100);// precisa ser adaptado
}
