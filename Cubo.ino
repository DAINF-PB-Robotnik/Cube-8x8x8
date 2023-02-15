int SER_Pin = 11;   //pin 14 on the 75HC595 data
int RCLK_Pin = 10;  //pin 12 on the 75HC595 latch
int SRCLK_Pin = 13; //pin 11 on the 75HC595 clock 
int pushbutton = 0;

//How many of the shift registers - change this
#define number_of_74hc595s 8

//Referentes as entradas do arduino que controlam as layers
#define t1 2
#define t2 3
#define t3 4
#define t4 5
#define t5 6
#define t6 7
#define t7 8
#define t8 9

//do not touch
#define numOfRegisterPins number_of_74hc595s * 8

boolean registers[numOfRegisterPins];

int animCount = 0;

void setup(){
  pinMode(SER_Pin, OUTPUT);
  pinMode(RCLK_Pin, OUTPUT);
  pinMode(SRCLK_Pin, OUTPUT);
  pinMode(t1, OUTPUT);
  pinMode(t2, OUTPUT);
  pinMode(t3, OUTPUT);
  pinMode(t4, OUTPUT);
  pinMode(t5, OUTPUT);
  pinMode(t6, OUTPUT);
  pinMode(t7, OUTPUT);
  pinMode(t8, OUTPUT);
  Serial.begin(9600);  
  
  //reset all register and layer pins
  clearLayers();
  clearRegisters();
  writeRegisters();
  
  pinMode(pushbutton, INPUT_PULLUP);
}               

//set all register pins to LOW
void clearRegisters(){
  for(int i = numOfRegisterPins - 1; i >=  0; i--){
     registers[i] = LOW;
  }
} 

//set all register pins to HIGH
void lightAllRegisters(){
  for(int i = numOfRegisterPins - 1; i >=  0; i--){
     registers[i] = HIGH;
  }
}

//set all layer pins to LOW
void clearLayers(){
  for(int i = 2; i < 10; i++){
     digitalWrite(i, LOW);
  }
}

//set all layer pins to HIGH
void lightAllLayers(){
  for(int i = 2; i < 10; i++){
     digitalWrite(i, HIGH);
  }
}


//Set and display registers
//Only call AFTER all values are set how you would like (slow otherwise)
void writeRegisters(){

  digitalWrite(RCLK_Pin, LOW);

  for(int i = numOfRegisterPins - 1; i >=  0; i--){
    digitalWrite(SRCLK_Pin, LOW);

    int val = registers[i];

    digitalWrite(SER_Pin, val);
    digitalWrite(SRCLK_Pin, HIGH);

  }
  digitalWrite(RCLK_Pin, HIGH);

}

//set an individual pin HIGH or LOW
void setRegisterPin(int index, int value){
  registers[index] = value;
}

int nextAnimation(int count){
  if(count == 3)
    return 0;
  else{
    count++;
    return count;
  }
}

void loop(){
  if (digitalRead(pushbutton) == LOW)
  {
    animCount = nextAnimation(animCount);
    Serial.println(animCount);   
    while (digitalRead(pushbutton) == LOW);
    delay(100);
  }

  if(animCount == 0){
    turnOffAnimations();
  } else if(animCount == 1){
    RainAnimation();
  } else if(animCount == 2){
    ScanAnimation();  
  } else if(animCount = 3){
    ExpandAnimation(150);
  }
}

void turnOffAnimations(){
  clearRegisters();
  writeRegisters();
  clearLayers();  
}

//Reproduz a animação de scanner, direçoes aleatórias
void ScanAnimation(){
  int side = random(0,7);

  switch (side) {
  case 1:
      frontToBack(0);
    break;
  case 2:
      backToFront(56);
    break;
  case 3:
      topToBot();
    break;
  case 4:
      botToTop();
    break;
  case 5:
      leftToRight(0);
    break;
  case 6:
      rightToLeft(7);
    break;
  default:
    // statements
    break;
  }
  clearRegisters();
  writeRegisters();
  clearLayers();
}

//Reproduz a animação de chuva com 3 colunas de leds simultaneamente
void RainAnimation(){ 
  int columns[3];
  for(int i = 0; i < 3; i++){
    columns[i] = random(0,64);       
  }
  
  for(int i = 0; i < 3; i++){
    setRegisterPin(columns[i], HIGH);
  }
  writeRegisters();
  
  for(int i = t8; i >= t1 - 1; i--){
    digitalWrite(i, HIGH);
    digitalWrite(i + 1, LOW);
    digitalWrite(i - 1, LOW);        
    delay(50);
  }

  for(int i = 0; i < 3; i++){
    setRegisterPin(columns[i], LOW);
  }
  writeRegisters();
}

//Reproduz a animação de cubo expandindo e contraindo com um determinado intervalo
void ExpandAnimation(int delayTime){
    CubeExpanding(t5, t4, 36, 35, 28, 27, delayTime);
    delay(delayTime);
    CubeShrinking(t8, t1,63,56,7,0, delayTime);
  }

//Funções auxiliares da animação de expansão
void CubeExpanding(int topLayer, int botLayer,int supEsq, int supDir, int infEsq, int infDir, int delayTime){
    
  if(supEsq < 64){
    digitalWrite(topLayer,HIGH); 
    digitalWrite(botLayer,HIGH);
    setRegisterPin(supDir,HIGH);
    setRegisterPin(supEsq,HIGH);
    setRegisterPin(infDir,HIGH);
    setRegisterPin(infEsq,HIGH);   
    writeRegisters();
    
    for(int i = infDir; i < infEsq; i++){
      setRegisterPin(i,HIGH);  
    }

    for(int i = infEsq; i < supEsq;){
      setRegisterPin(i,HIGH);
      i = i + 8;       
    }

    for(int i = supDir; i < supEsq; i++){
      setRegisterPin(i,HIGH);  
    }

    for(int i = infDir; i < supDir;){
      setRegisterPin(i,HIGH);
      i = i + 8;       
    }

    writeRegisters();         
    delay(delayTime);
    CubeExpanding(topLayer + 1, botLayer - 1, supEsq + 9, supDir + 7, infEsq - 7, infDir - 9, delayTime);     
  }
}

void CubeShrinking(int topLayer, int botLayer,int supEsq, int supDir, int infEsq, int infDir, int delayTime){
    
  if(supEsq > 36){
    setRegisterPin(supDir,LOW);
    setRegisterPin(supEsq,LOW);
    setRegisterPin(infDir,LOW);
    setRegisterPin(infEsq,LOW);   
    writeRegisters();
    
    for(int i = infDir; i < infEsq; i++){
      setRegisterPin(i,LOW);  
    }

    for(int i = infEsq; i < supEsq;){
      setRegisterPin(i,LOW);
      i = i + 8;       
    }

    for(int i = supDir; i < supEsq; i++){
      setRegisterPin(i,LOW);  
    }

    for(int i = infDir; i < supDir;){
      setRegisterPin(i,LOW);
      i = i + 8;       
    }
      
    writeRegisters();   
    digitalWrite(botLayer, LOW);
    digitalWrite(topLayer, LOW);     
    delay(delayTime);
    CubeShrinking(topLayer - 1, botLayer + 1, supEsq - 9, supDir - 7, infEsq + 7, infDir + 9, delayTime);
  }
}

//Funções auxiliares da animação de scanner
void backToFront(int startingPoint){
  clearRegisters();
  writeRegisters();
  lightAllLayers();
  if(startingPoint >= 0){  
    for(int i = startingPoint; i < startingPoint + 8; i++){
      setRegisterPin(i, HIGH);    
    }
    writeRegisters();
    delay(100);
    backToFront(startingPoint - 8);
  }   
}

void frontToBack(int startingPoint){
  clearRegisters();
  writeRegisters();
  lightAllLayers();
  if(startingPoint < 64){  
    for(int i = startingPoint; i < startingPoint + 8; i++){
      setRegisterPin(i, HIGH);    
    }
    writeRegisters();
    delay(100);
    frontToBack(startingPoint + 8);
  }   
}

void rightToLeft(int startingPoint){
  clearRegisters();
  writeRegisters();
  lightAllLayers();
  if(startingPoint >= 0){  
    for(int i = startingPoint; i < 64;){
      setRegisterPin(i, HIGH);
      i = i+8;    
    }
    writeRegisters();
    delay(100);
    rightToLeft(startingPoint - 1);
  }
}

void leftToRight(int startingPoint){
  clearRegisters();
  writeRegisters();
  lightAllLayers();
  if(startingPoint < 8){  
    for(int i = startingPoint; i < 64;){
      setRegisterPin(i, HIGH);
      i = i+8;    
    }
    writeRegisters();
    delay(100);
    leftToRight(startingPoint + 1);
  }   
}

void topToBot(){
  lightAllRegisters();
  writeRegisters();
  
  for(int i = t8; i >= t1 - 1; i--){
    digitalWrite(i, HIGH);
    digitalWrite(i + 1, LOW);
    digitalWrite(i - 1, LOW);        
    delay(100);
  }
  clearRegisters();
  writeRegisters();
  clearLayers();
}

void botToTop(){
  lightAllRegisters();
  writeRegisters();
  
  for(int i = t1; i <= t8 + 1; i++){
    digitalWrite(i, HIGH);
    digitalWrite(i + 1, LOW);
    digitalWrite(i - 1, LOW);        
    delay(100);
  }
  clearRegisters();
  writeRegisters();
  clearLayers();
}

//Inútil?
void Around(){
  digitalWrite(t1,HIGH);
   
  for(int i = 7; i < 64;){
    setRegisterPin(i,HIGH);   
    writeRegisters();
    setRegisterPin(i - 8,LOW);   
    writeRegisters();    
      
    i = i + 8;
  }
  
  for(int i = 2; i < 10; i++){
    digitalWrite(i,HIGH);
    delay(1);
    digitalWrite(i,LOW);
  }
  
  digitalWrite(t8,HIGH);
  for(int i = 63; i > 0;){
    setRegisterPin(i,HIGH);   
    writeRegisters();
    setRegisterPin(i,LOW);   
    writeRegisters();
    
    i = i - 8;
  }
  
  setRegisterPin(7,HIGH);   
  writeRegisters();

  for(int i = 9; i >= 2; i--){
    digitalWrite(i,HIGH);
    delay(1);
    digitalWrite(i,LOW);
  }
}