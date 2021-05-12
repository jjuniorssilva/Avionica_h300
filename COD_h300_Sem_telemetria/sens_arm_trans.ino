 //----------bmp280-------------------
float calibracao_alt(){
 float alt0=0,alt1=0,alt2=0,alt3=0,media_alt,percent=0.05;
 alt0=bmp.readAltitude();
 alt1=bmp.readAltitude();
 alt2=bmp.readAltitude(); 
 alt3=bmp.readAltitude();
 media_alt = (alt1+alt2+alt3)/3;
 if(alt0>=(media_alt+(media_alt*percent)) || alt0<=(media_alt-(media_alt*percent))){ // verifica se o valor coletado está dentro do intervalo gerado pela média
    return media_alt;
 }else{
    return alt0;
 }
}
void getDataBMP(){
  float alt0=0,alt1=0,alt2=0,alt3=0,media_alt,percent=0.05;
  packet_1.data.pressao_bmp = bmp.readPressure();
  packet_1.data.temp_bmp = bmp.readTemperature();
  alt0=bmp.readAltitude();
  alt1=bmp.readAltitude();
  alt2=bmp.readAltitude(); 
  alt3=bmp.readAltitude();
  media_alt = (alt1+alt2+alt3)/3;
  if(alt0>=(media_alt+(media_alt*percent)) || alt0<=(media_alt-(media_alt*percent))){ // verifica se o valor coletado está dentro do intervalo gerado pela média
    packet_1.data.alt_bmp = media_alt - alt_init_eeprom.alt_inicial;
  }else{
    packet_1.data.alt_bmp =alt0 - alt_init_eeprom.alt_inicial;
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
  Wire.read()<<8|Wire.read();  //0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
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
 for(int i=0;i<44;i++){ // no struct, os dados relevantes então no array de bytes [1-44]
  if(address_eeprom.eeaddress<65536){ // impede de gravar dados em um endereço inexistente 
    writeEEPROM(0x50,address_eeprom.eeaddress,packet_1.data_byte[i]);
    address_eeprom.eeaddress++;
    for(int i=0; i<4;i++){
      send_backup_eeprom(i,address_eeprom.eeaddress_byte[i]);
    }
  }else{
    Serial.println("Memoria eeprom cheia! -> ultimo endereço: "+String(address_eeprom.eeaddress));
  }
 }
}
bool check_EEPROM(){ // Grava um byte na memoria e faz a leitura, se for igual passa.
  if(address_eeprom.eeaddress<65536){ // impede de gravar dados em um endereço inexistente 
    writeEEPROM(address_eeprom.eeaddress,address_eeprom.eeaddress,0x50);
    if(readEEPROM(address_eeprom.eeaddress,address_eeprom.eeaddress)==0x50) return true;
  }
  return false;
}
void send_backup_eeprom(long address, byte data){
    EEPROM.write(address, data);
}
byte get_backup_eeprom(long address){
   return EEPROM.read(address);
}
// ------------Bateria-------
float getDataBatery(){
  int val = analogRead(pin_batery);
  packet_1.data.batery = map(val, 0, 1023, 0, 100);// precisa ser adaptado
  return packet_1.data.batery;
}
