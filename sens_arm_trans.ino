
//----------bmp280-------------------
void getDataBMP(){
  float alt0=0,alt1=0,alt2=0,alt3=0,media_alt,percent=0.05;
  packet_1.data.pressao_bmp = bmp.readPressure();
  alt0=bmp.readAltitude(1013.25);
  alt1=bmp.readAltitude(1013.25);
  alt2=bmp.readAltitude(1013.25); 
  alt3=bmp.readAltitude(1013.25);
  media_alt = (alt1+alt2+alt3)/3;
  if(alt0>=(media_alt+(media_alt*percent)) || alt0<=(media_alt-(media_alt*percent))){ // verifica se o valor coletado está dentro do intervalo gerado pela média
    packet_1.data.alt_bmp = media_alt;
  }else{
    packet_1.data.alt_bmp =alt0;
  }
}
// ----checagem da conexão i2c------
bool check_I2C(uint8_t ADDRESS){
  Wire.beginTransmission(ADDRESS);
  byte erro = Wire.endTransmission();
  if(erro==0)return true;
  return false;
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
//----------EEPROM-------------------
void writeEEPROM(int deviceaddress, unsigned int eeaddress, byte data ) { // salva o dado (byte) na posição eeaddress
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.write(data);
  Wire.endTransmission();
  delay(4);
}
byte readEEPROM(int deviceaddress, unsigned int eeaddress ) {
  byte data = 0xFF;
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();
  Wire.requestFrom(deviceaddress,1);
  if (Wire.available()) data = Wire.read();
  return data;
}
void sendDataEEPROM(){ // consumo médio de tempo 160ms
 for(int i=1;i<=40;i++){ // no struct, os dados relevantes então no array de bytes [1-40]
  writeEEPROM(0x50,eeaddress,packet_1.data_byte[i]);
  eeaddress++;
 }
}
bool check_EEPROM(){ // Grava um byte na memoria e faz a leitura, se for igual passa.
  if(eeaddress<65536){ // impede de gravar dados em um endereço inexistente 
    writeEEPROM(0x50,eeaddress,150);
    if(readEEPROM(0x50,eeaddress)==150) return true;
  }
  return false;
}
//----------LORA-------------------
bool check_LoRa(){ //Essa função tabalha em conjunto com o callback, ela envia um byte de verificação
  LoRa.beginPacket();
  LoRa.write(255);
  LoRa.endPacket();
  delay(1000);
}
void onReceive(int packetSize) { // esse callback recebe a resposta e verificar se está ok
 if (packetSize){
  byte check = LoRa.read();
  if(check==255)status_LoRa=true;
 }
}
void SendStruct() { // será usado no pré e in voo 
  packet_1.data.checksum = get_checksum_struct();
  LoRa.beginPacket();
  LoRa.write((uint8_t *) &packet_1.data_byte, sizeof(packet_1.data));
  LoRa.endPacket();
  digitalWrite(led3, HIGH);
  delay(20);
  digitalWrite(led3, LOW);
}
void SendDataBatery(float batery) { // será chamado no pós voo
  LoRa.beginPacket();
  LoRa.write(batery);
  LoRa.endPacket();
  digitalWrite(led3, HIGH);
  delay(20);
  digitalWrite(led3, LOW);
}
byte get_checksum_struct(){
  byte checksum = 0;
  for(int i=0; i<sizeof(packet_1.data); i++){
   checksum ^= packet_1.data_byte[i];
  }
  return (255-checksum);
}
// ------------Bateria-------
float getDataBatery(){
  int val = analogRead(pin_batery);
  packet_1.data.batery = map(val, 0, 1023, 0, 100);// precisa ser adaptado
  return packet_1.data.batery;
}
