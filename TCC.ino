#define portaswitch (1 << PB4)  //pino 12
#define btnIniciar (1 << PD7)   //pino7
#define btnParar (1 << PD6)     //pino6
#define btnAumentar (1 << PD5)  //pino5
#define btnDiminuir (1 << PD4)  //pino4


#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

Servo ESC;
LiquidCrystal_I2C lcd(0x27, 16, 2);

bool lcdclear1 = false, lcdclear2 = false;
static bool iniciar = false, parar = false, aumentar = false, diminuir = false, rodando = false, mostrar_msg = false, selecionar_vel = false, tempoExiste=false;
short int var1 = 0, var2 = 0, varM = 0, btns = 1, bnt_lcd = 9;
short int velocidade = 0;
short int exibirTempoLCD=0, tempoLimpaLCD=0;
unsigned long int tempoEscolhido, tempoAtual;

void setup() {
  DDRB &= ~portaswitch;  //configura entrada para o botão
  PORTB |= portaswitch;  //habilita o pull-up interno

  DDRD &= ~btnIniciar;  //configura entrada para o botão
  PORTD |= btnIniciar;  //habilita o pull-up interno

  DDRD &= ~btnParar;  //configura entrada para o botão
  PORTD |= btnParar;  //habilita o pull-up interno

  DDRD &= ~btnAumentar;  //configura entrada para o botão
  PORTD |= btnAumentar;  //habilita o pull-up interno

  DDRD &= ~btnDiminuir;  //configura entrada para o botão
  PORTD |= btnDiminuir;  //habilita o pull-up interno

  Serial.begin(9600);
  ESC.attach(10, 1000, 2000);
  lcd.init();
}

void lerBotoes() {
  if (!(PIND & btnAumentar)) aumentar = true;
  if (!(PIND & btnDiminuir)) diminuir = true;
  if (!(PIND & btnParar)) parar = true;
  if (!(PIND & btnIniciar)) iniciar = true;

  if ((PIND & btnAumentar) && aumentar && !rodando) {
    btns++;
    aumentar = false;
    if (btns > 6) btns = 1;
  }

  if ((PIND & btnDiminuir) && diminuir && !rodando) {
    btns--;
    diminuir = false;
    if (btns < 1) btns = 6;
  }

  if ((PIND & btnIniciar) && iniciar && !rodando) {
    velocidade = btns;
    rodando = true;
    iniciar = false;
    controla_lcd(3);
  }

  if ((PIND & btnParar) && parar && rodando) {
    parar = false;
    rodando = false;
    velocidade=0;
    controla_lcd(4);
    while ((PINB & portaswitch) == 0) {
    motor();
    }
  }
}

void motor() {
  if (velocidade == 0){
    var1=0;
    ESC.write(0);
  }
  if (velocidade == 1) {
    while (var1 < 30) {
      while (var2 < 30) {
        ESC.write(var1);
        var2++;
      }
      var1++;
      var2 = 0;
      Serial.println(var1);
    }
  } else if (velocidade == 2) {
    while (var1 < 60) {
      while (var2 < 60) {
        ESC.write(var1);
        var2++;
      }
      var1++;
      var2 = 0;
    }
  } else if (velocidade == 3) {
    while (var1 < 90) {
      while (var2 < 90) {
        ESC.write(var1);
        var2++;
      }
      var1++;
      var2 = 0;
    }
  } else if (velocidade == 4) {
    while (var1 < 120) {
      while (var2 < 120) {
        ESC.write(var1);
        var2++;
      }
      var1++;
      var2 = 0;
    }
  } else if (velocidade == 5) {
    while (var1 < 150) {
      while (var2 < 150) {
        ESC.write(var1);
        var2++;
      }
      var1++;
      var2 = 0;
    }
  } else if (velocidade == 6) {
    while (var1 < 180) {
      while (var2 < 180) {
        ESC.write(var1);
        var2++;
      }
      var1++;
      var2 = 0;
    }
  }
  ESC.write(var1);
}

void controla_lcd(int i) {
  if ((mostrar_msg) && i == 1) {
    if (lcdclear1 != lcdclear2) lcd.clear();
    lcdclear1 = lcdclear2;
    lcd.setBacklight(HIGH);
    lcd.setCursor(0, 0);
    lcd.print("TAMPA");
    lcd.setCursor(0, 1);
    lcd.print("FECHADA");
    delay(500);
    mostrar_msg = false;
  }
  if (i == 0) {
    if (lcdclear1 == lcdclear2) lcd.clear();
    var1 = 0;
    velocidade = '0';
    lcdclear1 = !lcdclear2;
    lcd.setBacklight(HIGH);
    ESC.write(0);
    lcd.setCursor(0, 0);
    lcd.print("FECHAR A");
    lcd.setCursor(0, 1);
    lcd.print("PORTA");
  }
  if((i==3) && (lcdclear1==lcdclear2)){
    lcdclear1 = !lcdclear2;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Rodando");
    lcd.setCursor(0, 1);
    int percent=0;
    if(tempoEscolhido>=millis()){
        percent = millis();
    }
    int i = map(percent, 0, tempoEscolhido, 0, 100);
    String tempoComPer = i+"%";
    lcd.print(tempoComPer);
  }
  if((i==4) && (lcdclear1!=lcdclear2)){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Abra a porta");
    lcdclear1=lcdclear2;
  }
}


void loop() {
  if ((PINB & portaswitch) == 0) {
    mostrar_msg = true;
    controla_lcd(1);
    while ((selecionar_vel == false) && ((PINB & portaswitch) == 0)) {
      if (bnt_lcd != btns) lcd.clear();
      lerBotoes();
      bnt_lcd = btns;
      lcd.setCursor(0, 0);
      lcd.print("Velocidade:");
      lcd.setCursor(0, 12);
      lcd.print(btns);
      lerBotoes();
      motor();
      if (rodando) selecionar_vel = true;
    }
    while(!tempoExiste){
      if (!(PIND & btnAumentar)) aumentar = true;
      if (!(PIND & btnDiminuir)) diminuir = true;
      if (!(PIND & btnIniciar)) iniciar = true;

      if ((PIND & btnAumentar) && aumentar) {
        exibirTempoLCD++;
        aumentar = false;
        if (exibirTempoLCD > 15) exibirTempoLCD = 1;
      }
      if ((PIND & btnDiminuir) && diminuir) {
        exibirTempoLCD++;
        diminuir = false;
        if (exibirTempoLCD <= 0) exibirTempoLCD = 15;
      }
      if ((PIND & btnIniciar) && iniciar) {
        tempoExiste = true;
        iniciar = false;
        tempoAtual=millis();
        tempoEscolhido = tempoAtual + ((exibirTempoLCD*60)*1000);
      }
      if(exibirTempoLCD != tempoLimpaLCD) lcd.clear();
      tempoLimpaLCD = exibirTempoLCD;
      lcd.setCursor(0, 0);
      lcd.print("TEMPO:");
      lcd.setCursor(7, 0);
      String tempoComMin = exibirTempoLCD + " Min";
      lcd.print(tempoComMin);
      motor();
    }
    while ((rodando) && ((PINB & portaswitch) == 0) && (tempoExiste)) {
      lerBotoes();
      controla_lcd(3);
      motor();
      if(millis()-tempoAtual>=3000){
          lcdclear1 = lcdclear1;
          tempoAtual=millis();
      }
      if(tempoEscolhido-millis()<=0){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Mistura separada");
        lcd.setCursor(0, 1);
        lcd.print("Abrir porta");
        while ((PINB & portaswitch) == 0) {
          velocidade = 0;
          motor();
        }
      }
    }
  } else { //Reseta todas as variaveis necessarias
    controla_lcd(0);
    mostrar_msg = false;
    iniciar = false;
    parar = false;
    aumentar = false;
    diminuir = false;
    rodando = false;
    mostrar_msg = false;
    selecionar_vel = false;
    var1 = 0;
    var2 = 0;
    varM = 0;
    btns = 1;
    bnt_lcd = 9;
    velocidade = 0;
    motor();
    tempoEscolhido=0;
    tempoAtual=0;
    exibirTempoLCD=0;
    tempoLimpaLCD=0;
    tempoExiste=false;
  }
}