#include <Cube.h>
#include <LiquidCrystal.h>

//Defines para as portas das camadas do cubo
#define t1 0
#define t2 0
#define t3 0
#define t4 0
#define t5 0
#define t6 0
#define t7 0
#define t8 0

//Defines dos pinos de controle
#define LATCH_Pin 0 //RCLK_Pin
#define DATA_Pin  0 //SER_Pin
#define CLK_Pin   0 //SRCLK_Pin

//Define para as portas do display
#define RS  11
#define E   12
#define d4  4
#define d5  5
#define d6  6
#define d7  7

//Defines para as portas do encoder 
//SW e DT devem ficar nas portas 2 e 3 se estiver utilizando um UNO
#define ROT_SW_Pin  3 
#define ROT_DT_Pin  2  
#define ROT_CLK_Pin 13

//Defines auxiliares
#define totalAnimations 3

//Inicialização dos pinos do display
LiquidCrystal lcd(RS,E,d4,d5,d6,d7);
Cube Cube(10,11,12,t1,t2,t3,t4,t5,t6,t7,t8);

//Controla qual animação deve ser exibida
int currentAnimation = 0;

//Pinos do Encoder e aux para controle de qual opçâo do menu está selecionada
int EncoderAtual;
int EncoderUltimo;
bool aux = true; 

void setup(){
  //Setup dos pinos do cubo
  Cube.begin();

  //Setup dos pinos do encoder
  pinMode(ROT_CLK_Pin, INPUT);
  pinMode(ROT_DT_Pin, INPUT);
  pinMode(ROT_SW_Pin, INPUT_PULLUP);  
  EncoderUltimo = digitalRead(ROT_CLK_Pin);

  //Setup dos interrupts
  attachInterrupt(digitalPinToInterrupt(ROT_DT_Pin), controlMenu, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROT_SW_Pin), changeAnimation, FALLING);

  //Setup do display
  lcd.begin(16, 2); 
  lcd.setCursor(0,1);
  lcd.print("Proximo  "); 
  lcd.setCursor(0,0);
  lcd.print("->Anterior");
  lcd.setCursor(11,0);  
  lcd.print("Exib:");
  lcd.setCursor(11,1);  
  lcd.print("NULL");

  //Setup do monitor serial
  Serial.begin(9600);
}

void loop(){
  animationControl(currentAnimation);
}

//Controla a posição do menu exibido no display
int controlMenu(){
  EncoderAtual = digitalRead(ROT_CLK_Pin);
	// Se a leitura atual do clock for diferente da anterior então o encoder rodou para algum lado
	// EncoderAtual == 1 age como filtro das mudanças de forma que ele só reage a uma mudança por vez
	if(EncoderAtual != EncoderUltimo && EncoderAtual == 1){

		if(digitalRead(ROT_DT_Pin) == EncoderAtual){
      // Se a leitura de DT for igual a leitura do clock então o encoder girou para a direita (CW)
      lcd.setCursor(0,0);
      lcd.print("Anterior  "); 
      lcd.setCursor(0,1);
      lcd.print("->Proximo");
      aux = false;       			
		} else {
      // Se a leitura de DT for diferente da leitura do clock então o encoder girou para a esquerda (CCW)
      lcd.setCursor(0,1);
      lcd.print("Proximo  "); 
      lcd.setCursor(0,0);
      lcd.print("->Anterior");
      aux = true;
		}
  } 
  EncoderUltimo = EncoderAtual;     
}

//Seleciona a opção escolhida
int changeAnimation(){ //<---------- Adicionar um debounce pro interrupt
  if(digitalRead(ROT_SW_Pin) == LOW && aux){
    //Se pressionar o botão do encoder e aux for True então a opção selecionada é a 'Anterior'
    currentAnimation = prevAnimation(currentAnimation);   
    while (digitalRead(ROT_SW_Pin) == LOW);
      delay(100);
  } else if(digitalRead(ROT_SW_Pin) == LOW && !aux){
    //Se pressionar o botão do encoder e aux for True então a opção selecionada é a 'Anterior'
    currentAnimation = nextAnimation(currentAnimation); 
    while (digitalRead(ROT_SW_Pin) == LOW);
      delay(100);
  }    
}


void animationControl(int currentAnimation){
  switch(currentAnimation){
    case 0:
      lcd.setCursor(11,0);  
      lcd.print("Exib:");
      lcd.setCursor(11,1);  
      lcd.print("NULL");
      Cube.turnOff();
      break;    
    case 1:
      lcd.setCursor(11,0);  
      lcd.print("Exib:");
      lcd.setCursor(11,1);
      lcd.print("Rain");
      Cube.rainAnimation();
      break;
    case 2:
      lcd.setCursor(11,0);  
      lcd.print("Exib:");
      lcd.setCursor(11,1);
      lcd.print("Scan");
      Cube.scanAnimation();
      break;
    case 3:
      lcd.setCursor(11,0);  
      lcd.print("Exib:");
      lcd.setCursor(11,1);
      lcd.print("Expd");
      Cube.expandAnimation(150);
      break;   
  }   
}

int nextAnimation(int count){
  if(count == totalAnimations)
    return 0;
  else{
    count++;
    return count;
  }
}

int prevAnimation(int count){
  if(count == 0)
    return totalAnimations;
  else{
    count--;
    return count;
  }
}