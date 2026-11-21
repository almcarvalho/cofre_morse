#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>
#include <string.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const byte BTN_VERDE=7, BTN_VERMELHO=6
, BUZZER=10, RELE=2;
// Mude para LOW somente se seu modulo de rele for ativo em LOW.
const byte RELE_ATIVO=HIGH;
const byte RELE_DESLIGADO=(RELE_ATIVO==LOW)?HIGH:LOW;
const unsigned int PONTO_MS=150, TRACO_MS=450;
const unsigned long PAUSA_LETRA=1200;
const byte MAGIC=0xA7;
byte pontos=10, menu=0;

// Tabelas armazenadas na FLASH, nao na SRAM do UNO.
const char morseTabela[][6] PROGMEM={
 ".-","-...","-.-.","-..",".","..-.","--.","....","..",".---",
 "-.-",".-..","--","-.","---",".--.","--.-",".-.","...","-",
 "..-","...-",".--","-..-","-.--","--.."
};
const char palavras[][5] PROGMEM={
 "SOL","MAR","LUZ","PAZ","SOM","MEL","RIO","LUA","CEU","SAL",
 "CASA","GATO","PATO","BOLA","MALA","FOTO","VIDA","RATO","SAPO","BOLO"
};
const byte TOTAL_PALAVRAS=sizeof(palavras)/sizeof(palavras[0]);

void codigoMorse(byte indice,char *destino){
 strcpy_P(destino,(PGM_P)morseTabela[indice]);
}
void palavraAleatoria(char *destino){
 byte i=random(TOTAL_PALAVRAS);
 strcpy_P(destino,(PGM_P)palavras[i]);
}
void tela(const __FlashStringHelper *titulo,const char *centro,const __FlashStringHelper *rodape=nullptr){
 display.clearDisplay();
 display.setTextColor(SSD1306_WHITE);
 display.setTextSize(1);
 display.setCursor(0,0);display.print(titulo);
 display.drawLine(0,13,127,13,SSD1306_WHITE);
 display.setTextSize(2);
 display.setCursor(0,24);display.print(centro);
 display.setTextSize(1);
 display.setCursor(0,55);if(rodape)display.print(rodape);
 display.display();
}
void telaPontos(const __FlashStringHelper *titulo,const char *centro){
 display.clearDisplay();display.setTextColor(SSD1306_WHITE);
 display.setTextSize(1);display.setCursor(0,0);display.print(titulo);
 display.setCursor(85,0);display.print(pontos);display.print(F("/10"));
 display.drawLine(0,13,127,13,SSD1306_WHITE);
 display.setTextSize(2);display.setCursor(0,25);display.print(centro);
 display.display();
}
void beep(char c){
 tone(BUZZER,850);delay(c=='.'?PONTO_MS:TRACO_MS);noTone(BUZZER);delay(70);
}
void aviso(bool ok){
 tone(BUZZER,ok?1400:250,ok?130:300);delay(ok?180:350);noTone(BUZZER);
}
void transmitir(const char *palavra){
 for(byte i=0;palavra[i];i++){
   char codigo[6];codigoMorse(palavra[i]-'A',codigo);
   for(byte j=0;codigo[j];j++){
     beep(codigo[j]);
     if(codigo[j+1])delay(100);
   }
   delay(650);
 }
}
// Cada clique gera um unico sinal, ao soltar o botao.
char lerBotao(){
 if(digitalRead(BTN_VERMELHO)==LOW){
   delay(30);
   if(digitalRead(BTN_VERMELHO)==LOW){
     while(digitalRead(BTN_VERMELHO)==LOW)delay(5);
     delay(30);return '.';
   }
 }
 if(digitalRead(BTN_VERDE)==LOW){
   delay(30);
   if(digitalRead(BTN_VERDE)==LOW){
     while(digitalRead(BTN_VERDE)==LOW)delay(5);
     delay(30);return '-';
   }
 }
 return 0;
}
void esperarSoltar(){
 while(digitalRead(BTN_VERMELHO)==LOW||digitalRead(BTN_VERDE)==LOW)delay(5);
 delay(70);
}
char decodificar(const char *sinais){
 char codigo[6];
 for(byte i=0;i<26;i++){
   codigoMorse(i,codigo);
   if(strcmp(sinais,codigo)==0)return 'A'+i;
 }
 return '?';
}
// Espera a quantidade exata de letras: nao termina palavra por silencio.
void receberPalavra(char *resposta,byte quantidade,bool ocultar){
 resposta[0]=0;esperarSoltar();
 for(byte pos=0;pos<quantidade;pos++){
   char sinais[9]="";byte n=0;unsigned long ultimo=0;
   char exibicao[8];
   while(true){
     char s=lerBotao();
     if(s){
       if(n<8){sinais[n++]=s;sinais[n]=0;beep(s);}
       else aviso(false);
       ultimo=millis();
     }
     for(byte k=0;k<pos;k++)exibicao[k]=ocultar?'*':resposta[k];
     exibicao[pos]='_';exibicao[pos+1]=0;
     display.clearDisplay();display.setTextColor(SSD1306_WHITE);
     display.setTextSize(1);display.setCursor(0,0);
     display.print(F("DIGITE "));display.print(pos+1);display.print('/');display.print(quantidade);
     display.drawLine(0,13,127,13,SSD1306_WHITE);
     display.setTextSize(2);display.setCursor(0,23);display.print(exibicao);
     display.setTextSize(1);display.setCursor(0,55);display.print(sinais);
     display.display();
     if(n>0 && millis()-ultimo>=PAUSA_LETRA){
       resposta[pos]=decodificar(sinais);resposta[pos+1]=0;break;
     }
     delay(20);
   }
 }
 esperarSoltar();
}
void carregarPontos(){
 byte p=EEPROM.read(1);
 if(EEPROM.read(0)!=MAGIC||p>10){
   EEPROM.update(0,MAGIC);EEPROM.update(1,10);p=10;
 }
 pontos=p;
}
void salvarPontos(byte p){pontos=p;EEPROM.update(1,p);}
void abrirCofre(){
 tela(F("ACESSO LIBERADO"),"MUSICA",F("4 segundos"));
 const unsigned int notas[]={523,659,784,1047,784,880,1047,1319};
 for(byte i=0;i<8;i++){tone(BUZZER,notas[i],380);delay(500);}
 noTone(BUZZER);
 tela(F("ACESSO LIBERADO"),"ABRINDO",F("Rele: 4 segundos"));
 digitalWrite(RELE,RELE_ATIVO);delay(4000);
 digitalWrite(RELE,RELE_DESLIGADO);
 tela(F("COFRE MORSE"),"FECHADO",F("Voltando ao menu"));delay(1200);
}
void modoUnlock(){
 char segredo[6]="";esperarSoltar();
 for(byte i=0;i<5;i++){
   char s=0;
   while(!s){tela(F("UNLOCK - 5 SINAIS"),segredo,F("Verm . / Verde -"));s=lerBotao();delay(12);}
   segredo[i]=s;segredo[i+1]=0;beep(s);
 }
 if(strcmp(segredo,"----.")==0)abrirCofre();
 else{tela(F("SEGREDO INCORRETO"),"ERRO",F("Voltando ao menu"));aviso(false);delay(1000);}
}
void modoAlfabeto(){
 char resposta[8],codigo[6],dica[9];
 for(byte i=0;i<26;i++){
   codigoMorse(i,codigo);
   dica[0]='A'+i;dica[1]=' ';strcpy(dica+2,codigo);
   while(true){
     tela(F("ALFABETO"),dica,F("Digite o codigo"));delay(1400);
     receberPalavra(resposta,1,false);
     if(resposta[0]=='A'+i){tela(F("ALFABETO"),"CERTO!",F("Proxima letra"));aviso(true);delay(600);break;}
     tela(F("ALFABETO"),"ERROU",F("Tente novamente"));aviso(false);delay(850);
   }
 }
 tela(F("ALFABETO"),"PARABENS!",F("A ate Z concluido"));delay(1800);
}
void modoTreino(){
 char palavra[5],resposta[8];palavraAleatoria(palavra);
 while(true){
   tela(F("TREINO"),palavra,F("Memorize e digite"));delay(2000);
   receberPalavra(resposta,strlen(palavra),false);
   if(strcmp(resposta,palavra)==0){tela(F("TREINO"),"CERTO!",F("Proxima vez: outra"));aviso(true);delay(1100);return;}
   tela(F("TREINO"),"ERROU",F("Tente novamente"));aviso(false);delay(1000);
 }
}
void modoValendo(){
 if(pontos==0){abrirCofre();salvarPontos(10);return;}
 while(pontos>0){
   char palavra[5],resposta[8];palavraAleatoria(palavra);
   bool acertou=false;
   while(!acertou){
     telaPontos(F("VALENDO"),"OUCA");delay(750);
     transmitir(palavra);
     receberPalavra(resposta,strlen(palavra),true);
     acertou=strcmp(resposta,palavra)==0;
     if(!acertou){telaPontos(F("VALENDO"),"ERROU");aviso(false);delay(900);}
   }
   salvarPontos(pontos-1);
   telaPontos(F("ACERTOU!"),"CERTO!");aviso(true);delay(1100);
 }
 abrirCofre();salvarPontos(10);
}
void mostrarMenu(){
 const __FlashStringHelper *nome;
 switch(menu){
   case 0:nome=F("TREINO");break;
   case 1:nome=F("ALFABETO");break;
   case 2:nome=F("VALENDO");break;
   default:nome=F("UNLOCK");break;
 }
 display.clearDisplay();display.setTextColor(SSD1306_WHITE);
 display.setTextSize(1);display.setCursor(0,0);display.print(F("COFRE MORSE "));
 display.print(pontos);display.print(F("/10"));
 display.drawLine(0,13,127,13,SSD1306_WHITE);
 display.setTextSize(2);display.setCursor(0,25);display.print(nome);
 display.setTextSize(1);display.setCursor(0,55);display.print(F("Verde: prox Verm: OK"));
 display.display();
}
void setup(){
 // Desliga rele ANTES de ativar o pino como saida.
 digitalWrite(RELE,RELE_DESLIGADO);pinMode(RELE,OUTPUT);
 pinMode(BUZZER,OUTPUT);noTone(BUZZER);digitalWrite(BUZZER,LOW);
 pinMode(BTN_VERDE,INPUT_PULLUP);pinMode(BTN_VERMELHO,INPUT_PULLUP);
 Serial.begin(9600);
 // Mesmo procedimento OLED do Genius funcional.
 if(!display.begin(SSD1306_SWITCHCAPVCC,0x3C)){
   Serial.println(F("Erro ao iniciar o display OLED"));
   while(true){digitalWrite(RELE,RELE_DESLIGADO);delay(100);}
 }
 display.clearDisplay();display.setTextSize(1);display.setTextColor(SSD1306_WHITE);
 display.setCursor(0,0);display.println(F("COFRE MORSE"));
 display.setCursor(0,10);display.println(F("By @BR.LCSISTEMAS"));display.display();
 carregarPontos();randomSeed(analogRead(A0));delay(2000);mostrarMenu();
}
void loop(){
 char tecla=lerBotao();
 if(tecla=='-'){menu=(menu+1)%4;mostrarMenu();}
 else if(tecla=='.'){
   switch(menu){
     case 0:modoTreino();break;
     case 1:modoAlfabeto();break;
     case 2:modoValendo();break;
     case 3:modoUnlock();break;
   }
   mostrarMenu();
 }
 delay(15);
}
