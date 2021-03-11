uint8_t handshake1 = 0;
uint8_t handshake2 = 0;
uint8_t handshake3 = 0;
uint8_t done = 0;
int break_loop = 0;
int break_loop2 = 0;
int count = 0;

void setup() {
  noInterrupts();
  CLKPR = 0x80;                                                                                     /*Enabling the clock prescaler function*/
  CLKPR = 0x00;                                                                                     /*Setting the prescaler to div by 1*/
  interrupts();
  
  pinMode(16, OUTPUT);
  
  pinMode(35, OUTPUT);                                                                              /*PIN for Error Image*/
  pinMode(36, OUTPUT);                                                                              /*PIN for Non-Earth Image*/
  pinMode(37, OUTPUT);                                                                              /*PIN for Earth Image*/

  Serial.begin(9600);                                                                               /*UART with PC Terminal*/
  Serial1.begin(9600);                                                                              /*UART with PIC MCU*/

  Serial.println("UART Communication test\r\n\n");
  Serial1.write(32);
  Serial.println("Sending 1st Handshake to STM32 MCU\n");
 
  while(!break_loop){
    if (Serial1.available()){
    handshake1 = Serial1.read();
    Serial.println (handshake1, HEX);
    delay(500);
      
      if (handshake1 == 24){
        Serial.println("1st Acknowledgement Received from STM32 MCU\r\n\n");
        delay(500);

        Serial1.write(64);
        Serial.println("Sending 2nd Handshake to STM32 MCU\n");

        while(1) {
          if (Serial1.available()){
            handshake2 = Serial1.read();
            Serial.println (handshake2, HEX);
            delay(500);
            
            if (handshake2 == 48) {
              Serial.println("2nd Acknowledgement Received from STM32 MCU\r\n\n");
              delay(500);
              break_loop = 1;
              break;
            }
            else {
              continue;
            }
          }
        }
      }
      else {
        continue;
      }
    }
  }
  
}

void loop() {
  count++;

  if (count == 3) {
    Serial1.write(0x29);
    Serial.println("Sending 3rd Handshake to STM32 MCU\n");
  }

  if (Serial1.available() > 0){
    handshake3 = Serial1.read();
    Serial.println (handshake3, HEX);

    if (handshake3 == 0x04) {
      Serial.println("3rd Acknowledgement Received from STM32 MCU\r\n\n");
      
      while (!break_loop2) {
        if (Serial1.available() > 0){
          done = Serial1.read();
          Serial.println (done, HEX);
  
          if (done == 0x02){
            Serial.println ("ICU predicted Earth Image");
            break_loop2 = 1;
          }
          else if (done == 0x03){
            Serial.println ("ICU predicted Non-Earth Image");
            break_loop2 = 1;
          }
          else if (done == 0x05){
            Serial.println ("Error Image");
            break_loop2 = 1;
          }
        }
      }
    }
    
  }

  if( done == 0x02 || done == 0x03 || done == 0x05 ) {
    if (done == 0x02) {
      digitalWrite(37, HIGH);
    }
    else if (done == 0x03) {
      digitalWrite(36, HIGH);
    }
    else if (done == 0x05) {
      digitalWrite(35, HIGH);
    }
  }
  
  digitalWrite(16, HIGH); 
  delay(500);                      
  digitalWrite(16, LOW);   
  delay(500); 

  if (count == 1000) {
    count = 0;             
  }
  
}
