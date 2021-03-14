void rotinas(int op){
  if(op==0){
    if(millis()-ciclo>=1000){
      ciclo=millis();
      rotinas(1);
    }
  }else if(op==1){
    //--------coleta----------
    if(check_I2C(0x77)==true && check_I2C(0x68)==true){ // realiza a checagem da conexão com os modulos
      digitalWrite(led2, LOW);
      getDataBMP(); // coleta dados do bmp
      getDataMPU(); // coleta dados do mpu
      getDataBatery(); // coleta dados da carga da bateria
      if(check_direction){
        if(check_height){
          if(check_angle){
            servo_control(179); // aciona o servo para liberar o paraquedas parametro:angulo
          }
        }
      }
      altura_ant= packet_1.data.alt_bmp; // guarda a altura coletada nesse clico para analisar no proximo
    }else{
      packet_1.data.error=1;
      digitalWrite(led2, HIGH);
    }
    //--------armazenamento----------
    if(check_EEPROM()){ //verifica o armazenamento
      sendDataEEPROM();//Salvar os dados do struct
    }else{
      if(packet_1.data.error=1)packet_1.data.error=3;
      if(packet_1.data.error=0)packet_1.data.error=2;
    }
    //----------trasmissão---------
    SendStruct();
  }else{
      if(millis()-ciclo>=1000){
      ciclo=millis();
      SendDataBatery(getDataBatery()); //enviar o status da bateria
    }
  }
}
void update_rotina(){ // a ideia aqui é que >=1m de altura o modo de voo mude e ao retornar a <=1m de altura mude de novo;
  if(packet_1.data.alt_bmp>=1 && status_alt_rotina==false){
    status_alt_rotina=true;
    rotina=1;
  }else if(packet_1.data.alt_bmp<=1 && status_alt_rotina==true){
    rotina=2;
  }
}
