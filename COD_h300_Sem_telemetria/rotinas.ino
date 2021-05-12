void rotinas(int op){
  if(op==0){
    if(millis()-ciclo>=1000){
      if(check_I2C(0x77)==true && check_I2C(0x68)==true){ // realiza a checagem da conexão com os modulos
        digitalWrite(led2, LOW);
        if(status_calib==true){
          if(get_backup_eeprom(4)!=0){
            for(int i=4; i<7;i++){
              alt_init_eeprom.alt_inicial_byte[i]=get_backup_eeprom(i);
            }
          }else{
            alt_init_eeprom.alt_inicial = calibracao_alt();
            for(int i=4; i<7;i++){
              send_backup_eeprom(i,alt_init_eeprom.alt_inicial_byte[i]);
            }
          }
          status_calib=false;
        }
      }else{
        digitalWrite(led2, HIGH);
      }
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
            digitalWrite(led3, HIGH);
            send_backup_eeprom(12,254);
          }
        }
      }
      altura_ant= packet_1.data.alt_bmp; // guarda a altura coletada nesse clico para analisar no proximo
    }else{
      digitalWrite(led2, HIGH);
    }
    //--------armazenamento----------
    if(check_EEPROM()){ //verifica o armazenamento
      sendDataEEPROM();//Salvar os dados do struct
      digitalWrite(led2, LOW);
    }else{
      digitalWrite(led2, HIGH);
    }
    
  }else{
    for(int x=0;x<180;x++){
      //converte graus para radiando e depois obtém o valor do seno
      float seno=(sin(x*3.1416/180));
      //gera uma frequência a partir do valor do seno
      int frequencia = 2000+(int(seno*1000));
      tone(9,frequencia);
      delay(2);
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
