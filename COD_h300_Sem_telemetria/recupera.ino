bool check_direction(){
  if(altura_ant>packet_1.data.alt_bmp)return true; // se a altura anterior for maior que a atual o foguete está descendo
  return false;
}
bool check_height(){
 if(packet_1.data.alt_bmp>=altura_min)return true; // se a altura atual é maior ou igual a minima(altura minima de abertura do paraquedas)
 return false;
}
bool check_angle(){
  if(packet_1.data.AcZ_mpu >= 91)return true; // verifica a inclinação do foguete está dentro do esperado para a abertura d paraquedas
  return false;
}

void servo_control(int angle){
  servoPara.write(angle);
}
