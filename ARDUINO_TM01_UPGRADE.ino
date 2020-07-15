/*____________________________________________________________________________________________
  /////////////////////////////////////////////////////DADOS INICIAIS/////////////////////////
  ____________________________________________________________________________________________*/

/////////////////////////////////////////////////////SELECAO DE DEBUG//////////////////////////
#define DEBUG                   //Habilita o monitor serial para debugar
#define DEBUG_MEDICAO           //Debuga se todos os sensores estao medindo no monitor serial
//#define DEBUG_OPERACAO          //Debuga se as condicionais de operacao estao funcionando
//#define DEBUG_PROBLEMA          //Debuga se entrou em alguma condicao de quebra do equipamento
//#define DEBUG_MANUTENCAO        //Debuga se o equipamento esta funcionando ou nao
//#define DEBUG_EEPROM            //Debuga se a EEPROM está gravando os dados
//#define DEBUG_BOTAO             //Debuga se o botao de interrupcao esta funcionando corretamente
//#define DEBUG_CORRENTE          //Debuga se há algum problema na medicao de correntes
//#define DEBUG_INFRA             //Debuga se há falha na medicao do infravermelho

/////////////////////////////////////////////////////BIBLIOTECAS//////////////////////////////
#include <SPI.h>                 //Habilita a biblioteca para comunicacao da Ethernet Shield com o Arduino com interface UART SPI
#include <Ethernet.h>            //Habilita a biblioteca da Ethernet Shield
#include <EEPROM.h>              //Habilita a biblioteca da EEPROM
#include <avr/pgmspace.h>        //Habilita a biblioteca para salvar dados estaticos que iriam para a RAM para a Flash
#include <Wire.h>                //Habilita a interface TWI com protocolo I2C para o display de LCD
#include <LiquidCrystal_I2C.h>   //Habilita a biblioteca do display LCD
#include <DHT.h>                 //Habilita a biblioteca do sensor de temperatura e humidade DHT22 ou DHT11 com interface one wire
#include <EmonLib.h>             //Habilita a biblioteca do sensor SCT-013-000 para medicao de corrente

/////////////////////////////////////////////////////INFORMACOES ETHERNET///////////////////
byte MAC[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
byte servidor[] = {192, 168, 0, 30};
//IPAddress IP(192, 168, 0, 173);
IPAddress IP(10, 50, 16, 10);
//IPAddress DNS(192, 168, 0, 1);
IPAddress DNS(10, 50, 16, 1);
//IPAddress GATEWAY(192, 168, 0, 1);
IPAddress GATEWAY(10, 50, 16, 1);
//IPAddress SUBNET(255, 255, 255, 0);
#define portaHTTP 80
EthernetClient clienteArduino;
#define TEMPO_PARA_ENVIAR_DADOS 30000


/////////////////////////////////////////////////////DISPLAY LCD/////////////////////////////
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);   //Configuracao do display com interface I2C
#define BOTAO_PIN 19                                             //Pino para acionar a interrupcao de troca de tela do LCD
int botaoCont;

////////////////////////////////////////////////////BUZZER//////////////////////////////////
#define BUZZER 49
//Introdução da melodia do Super Mario Bros
int melodia[] = {660, 660, 660, 510, 660, 770, 380};
//Duraçao de cada nota
int duracaodasnotas[] = {100, 100, 100, 100, 100, 100, 100, 100, 100,
                         100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100
                        };
//Notas do Alarme
int alarme[] = {50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 550,
                600, 650, 700, 750, 800, 850, 900, 950, 1000
               };

/////////////////////////////////////////////////////DEFINICOES DO DHT//////////////////////
#define DHTPIN A15          //Define o pino de comando do DHT
#define DHTTYPE DHT22       //Define sensor como DHT22
DHT dht(DHTPIN, DHTTYPE);   //configuracao do DHT

//////////////////////////////////////////////////////RESET AUTOMATICO///////////////////////
#define RESET 24

/////////////////////////////////////////////////////DEFINICOES DO INFRAVERMELHO/////////////
#define OBSTACULOS                4       //Quantidade de obstaculos em uma volta
#define INFRA_PIN                 18      //Pino que sera utilizado pelo sensor infravermelho
float revol;                              //Variavel que armazenara a quantidade de revolucoes
unsigned int contagem;                    //Variavel que armazenara quantos obstaculos foram contados
unsigned long t_sql_0, t_sql_1, t_sql_t;  //Temporizador de envio de dados para o mysql
unsigned long t_inicio, t_fim, t_total;   //Temporizador da contagem de rotações

/////////////////////////////////////////////////////DEFINICOES RELES//////////////////////
#define LIGA        0
#define DESLIGA     1

/////////////////////////////////////////////////////DEFINICOES DE MANUTENCAO//////////////
#define QUEBRADO    0
#define FUNCIONANDO 1

/////////////////////////////////////////////////////TEMPO PARA INICIO//////////////////////
#define TEMPO_PARTIDA_EQUIPAMENTO   1000    //Total de 2 minutos para evitar partidas indesejadas
//dos compressores no caso de variacao de energia

/////////////////////////////////////////////////////DEFINICOES DO SENSOR DE CORRENTE//////
#define sctPin7 A5 //Define o pino de entrada do sensor
#define sctPin8 A6
#define sctPin9 A7
#define sctPin1 A8
#define sctPin2 A9
#define sctPin3 A10
#define sctPin4 A11
#define sctPin5 A12
#define sctPin6 A13

EnergyMonitor emon1, emon2, emon3, emon4, emon5, emon6, emon7, emon8, emon9;

float Temperatura;
float Umidade;
int RPM;
double Irms1;//variavel que traz a corrente de cada sensor
double Irms2;
double Irms3;
double Irms4;
double Irms5;
double Irms6;
double Irms7;
double Irms8;
double Irms9;
uint8_t operacao;
uint8_t manutencao;

byte TempAlta = 22.5;
byte TempBaixa = 21.5;
byte UmidAlta = 50;
byte UmidBaixa = 45;
int RotAlarme = 600;
int RotMin = 450;
byte CorrenteAlta = 19;
byte CorrenteRisco = 16;
byte CorrenteMin = 1;


byte VentiladorRele = 47;
byte ResistenciaRele1 = 45;
byte ResistenciaRele2 = 43;
byte CompressorRele1 = 41;
byte CompressorRele2 = 39;
byte UmidificadorRele = 37;
//byte DesumidificadorRele = 35;
//byte GeralRele = 33;

/*//////////////////////////////////////////////////////EEPROM/////////////////////////////
  #define espaco_da_EEPROM 4000                   //4kb é o espaço que o Atmega2560 possui
  void EEPROM_Write_Int(int endereco, int valor); //Os parâmetros classificados como int consomem
                                                  //2 Bytes (16 bits) e usam 2 espacos na EEPROM
  int  EEPROM_Read_Int(int endereco);*/

/*_______________________________________________________________________________________
  /////////////////////////////////////////////////////SETUP ARDUINO///////////////////////
  _________________________________________________________________________________________*/

void setup() {
  //Auto reset
  pinMode(RESET, OUTPUT);
  digitalWrite(RESET, HIGH);

  //Monitoramento serial
#ifdef DEBUG
  Serial.begin(115200); //Inicia o monitor serial
#endif

  //Inicialização da Ethernet Shield
  //Ethernet.begin(MAC, IP, DNS, GATEWAY);
  Ethernet.begin(MAC);

  if (Ethernet.begin(MAC) == 0) {
    Serial.println("A conexão falhou");
    //Ethernet.begin(MAC, IP, DNS, GATEWAY);
    Ethernet.begin(MAC);
  }
  Serial.println("Conectado a rede");
  Serial.println(Ethernet.localIP());
  

// Verifica se ha hardware conectado
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Nao foi encontrado Ethernet shield. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("O cabo Ethernet nao esta conectado.");


  //A limpeza da EEPROM executada uma vez, necessitando que o sketch seja executado de novo com este código comentado
  /*for (int nL = 0; nL < espaco_da_EEPROM; nL++) {
    EEPROM.write(nL, 0);
    }*/

  //Escrita dos parametros da configuracao
  EEPROM.write(0, TempAlta);
  EEPROM.write(1, TempBaixa);
  EEPROM.write(2, UmidAlta);
  EEPROM.write(3, UmidBaixa);
  EEPROM_Write_Int(4, RotAlarme);
  EEPROM_Write_Int(6, RotMin);
  EEPROM.write(8, CorrenteAlta);
  EEPROM.write(9, CorrenteRisco);
  EEPROM.write(10, CorrenteMin);

  //Leitura dos parametros da configuracao
  TempAlta = EEPROM.read(0);
  TempBaixa = EEPROM.read(1);
  UmidAlta = EEPROM.read(2);
  UmidBaixa = EEPROM.read(3);
  RotAlarme = EEPROM_Read_Int(4); //Int consome 2 bytes na memoria EEPROM, por isso o proximo endereco e 6
  RotMin = EEPROM_Read_Int(6);
  CorrenteAlta = EEPROM.read(8);
  CorrenteRisco = EEPROM.read(9);
  CorrenteMin = EEPROM.read(10);

#ifdef DEBUG_EEPROM
  Serial.println(TempAlta);
  Serial.println(TempBaixa);
  Serial.println(UmidAlta);
  Serial.println(UmidBaixa);
  Serial.println(RotAlarme);
  Serial.println(RotMin);
  Serial.println(CorrenteRisco);
  Serial.println(CorrenteAlta);
  Serial.println(CorrenteMin);
#endif

  //Setup DHT
  dht.begin();         //Inicia o sensor DHT

  /////////////////////////////////////////////////////SETUP DO DISPLAY LCD////////////

  //Setup do LCD 16x4
  lcd.begin(16, 4);               //Inicia o LCD
  lcd.setBacklight(HIGH);         //Liga a iluminacao de fundo
  lcd.setCursor(0, 0);            //Indica para escrever o proximo texto na linha 0 e coluna 0
  lcd.print("HC 4.0");            //Texto de abertura da tela
  lcd.setCursor(0, 1);            //Indica para escrever o proximo texto na linha 1 e coluna 0
  lcd.print("Eng Mec Danilo");    //Texto de abertura da tela
  lcd.setCursor(0, 2);            //Indica para escrever o proximo texto na linha 2 e coluna 0
  lcd.print("Ligando o ");        //Texto de abertura da tela
  lcd.setCursor(0, 3);            //Indica para escrever o proximo texto na linha 3 e coluna 0
  lcd.print("Ventilador");        //Texto de abertura da tela

  //Botao de acionamento do LCD
  pinMode(BOTAO_PIN, INPUT);//Botao de acionamento sem necessidade de resistência no circuito

  //Infravermelho
  pinMode(INFRA_PIN, INPUT);

  /////////////////////////////////////////////////////SETUP DOS RELES////////////////////

  //Definicao dos pinos dos reles como saidas
  pinMode(VentiladorRele, OUTPUT);
  pinMode(ResistenciaRele1, OUTPUT);
  pinMode(ResistenciaRele2, OUTPUT);
  pinMode(CompressorRele1, OUTPUT);
  pinMode(CompressorRele2, OUTPUT);
  pinMode(UmidificadorRele, OUTPUT);
  //pinMode(DesumidificadorRele, OUTPUT);
  //pinMode(GeralRele, OUTPUT);

  //Informacões dos reles
  digitalWrite(VentiladorRele, LIGA);
  digitalWrite(ResistenciaRele1, DESLIGA);
  digitalWrite(ResistenciaRele2, DESLIGA);
  digitalWrite(CompressorRele1, DESLIGA);
  digitalWrite(CompressorRele2, DESLIGA);
  digitalWrite(UmidificadorRele, DESLIGA);
  //digitalWrite(DesumidificadorRele, DESLIGA);
  //digitalWrite(GeralRele, DESLIGA);
  operacao = 0;
  manutencao = FUNCIONANDO;

#ifdef DEBUG_OPERACAO
  Serial.println(F("Operacao 0: Ventilacao"));
#endif

#ifdef DEBUG_MANUTENCAO
  Serial.println(F("Manutencao: Funcionando"));
#endif



  /*Calibracao do sensor SCT-013-000
    O resistor foi definido na funcao comp1() (R = 150 Ohm);
    Fator de calibracao da corrente = 2000/33 = 60.606 (N1/N2 = i_2/i_1)
    O valor de calibracao altera conforme altera o resistor em funcao da faixa de medicao
    A corrente maxima considerada foi de 22A.
    O calculo teoria do calculo pode ser encontrada em
    https://portal.vidadesilicio.com.br/sct-013-sensor-de-corrente-alternada/
    em
    http://tyler.anairo.com/projects/open-energy-monitor-calculator
    em
    https://portal.vidadesilicio.com.br/sct-013-sensor-de-corrente-alternada/
  */
  emon1.current(sctPin1, 60.606);
  emon2.current(sctPin2, 60.606);
  emon3.current(sctPin3, 60.606);
  emon4.current(sctPin4, 60.606);
  emon5.current(sctPin5, 60.606);
  emon6.current(sctPin6, 60.606);
  emon7.current(sctPin7, 60.606);
  emon8.current(sctPin8, 60.606);
  emon9.current(sctPin9, 60.606);

  //Tempo de partida do equipamento
  delay(TEMPO_PARTIDA_EQUIPAMENTO);

  //Indica início do sistema funcionando
  for (int nota = 0; nota < 7; nota++) {

    int duracaodanota = duracaodasnotas[nota];
    tone(BUZZER, melodia[nota], duracaodanota);
    //pausa depois das notas
    int pausadepoisdasnotas[] = {150, 300, 300, 100, 300, 550, 575};
    delay(pausadepoisdasnotas[nota]);
  }
}

/*___________________________________________________________________________________
  /////////////////////////////////////////////////////LOOP ARDUINO//////////////////
  ___________________________________________________________________________________*/

void loop() {
  t_sql_0 = millis();
    do {
  /////////////////////////////////////////////////////SENSOREAMENTO/////////////////////////////////////////////

  //MEDICAO DE TEMPERATURA E UMIDADE ________________________________________________________

  Temperatura = dht.readTemperature();                  //Faz a leitura da temperatura
  Umidade = dht.readHumidity();                         //Faz a leitura da umidade

#ifdef DEBUG_MEDICAO
  Serial.print(F("Temperatura = "));
  Serial.print(Temperatura);
  Serial.println(F("°C"));
  Serial.print("Umidade = ");
  Serial.print(Umidade);
  Serial.println(F("%"));
#endif

  //MEDICAO DA ROTACAO DO VENTILADOR ________________________________________________________
  t_inicio = millis();
  contagem = 0;
  do {
    attachInterrupt(digitalPinToInterrupt(INFRA_PIN), InfraVermelho, RISING);
    //Realiza a interrupcao para ir para a função InfraVermelho quando sai do sinal baixo para o alto,
    //reduz as falhas do fator de forma.

    t_fim = millis();
    t_total = t_fim - t_inicio;
  } while (t_total <= 1000);

  detachInterrupt(digitalPinToInterrupt(INFRA_PIN));
  //correção do tempo
  t_fim = millis();
  t_total = t_fim - t_inicio;

#ifdef DEBUG_INFRA
  Serial.print("Tempo = ");
  Serial.println(t_total);
#endif

  //Realiza cálculo de rotacao
  revol = ((contagem - 1) / OBSTACULOS);
  //Total de obstaculos contados sobre o total de obstaculos na polia
  RPM = revol * (60 / (t_total * 0.001));


#ifdef DEBUG_INFRA
  Serial.print(F("Rotacao = "));
  Serial.println(RPM);
  Serial.print(F(" RPM"));
#endif

  //MEDICAO DAS CORRENTES DO VENTILADOR ________________________________________________________
  Irms7 = emon7.calcIrms(1996);
  Irms8 = emon8.calcIrms(1996);
  Irms9 = emon9.calcIrms(1996);

#ifdef DEBUG_CORRENTE
  Serial.print("Irms7 = ");
  Serial.println(Irms7);
  Serial.print("Irms8 = ");
  Serial.println(Irms8);
  Serial.print("Irms9 = ");
  Serial.println(Irms9);
#endif

  //MEDICAO DAS CORRENTES DO COMPRESSOR 1 ________________________________________________________
  /*A corrente maxima definida foi 100A
    i(pico) = i(sensor).raiz(2)
    i(pico) = 141,42A
    i(sensor) = i(pico)/enrolamento secundario
    i(sensor) = 0,070711A
    tensao(max) = 5(Arduino)/2 = 2,5
    Resistor de carga = tensao(max)/i(sensor)
    Resistor de carga = 35,35 Ohm
    Resistor de carga real = 33 Ohm

    o valor para calcIrms para 60Hz foi extraído de
    https://www.filipeflop.com/blog/medidor-de-corrente-ac-acs712-emonlib/
  */
  Irms1 = emon1.calcIrms(1996);//Para 50Hz 1480 e para 60Hz 1996
  Irms2 = emon2.calcIrms(1996);
  Irms3 = emon3.calcIrms(1996);

#ifdef DEBUG_CORRENTE
  Serial.print("Irms1 = ");
  Serial.println(Irms1);
  Serial.print("Irms2 = ");
  Serial.println(Irms2);
  Serial.print("Irms3 = ");
  Serial.println(Irms3);
#endif

  //MEDICAO DAS CORRENTES DO COMPRESSOR 2 ________________________________________________________
  Irms4 = emon4.calcIrms(1996);
  Irms5 = emon5.calcIrms(1996);
  Irms6 = emon6.calcIrms(1996);

#ifdef DEBUG_CORRENTE
  Serial.print("Irms4 = ");
  Serial.println(Irms4);
  Serial.print("Irms5 = ");
  Serial.println(Irms5);
  Serial.print("Irms6 = ");
  Serial.println(Irms6);
#endif

  //ACOES DOS RELES ________________________________________________________
  ComandoRele();

  //TELA DE LCD ________________________________________________________
  //Interrupcao para mudanca de tela no Display de LCD
  attachInterrupt(digitalPinToInterrupt(BOTAO_PIN), botaoInterrupcao, HIGH);
  delayMicroseconds(10000);
  detachInterrupt(digitalPinToInterrupt(BOTAO_PIN));
  botaoCont;
  switch (botaoCont) {                           //LCD da temperatura e umidade
    case 0:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Ambiente");
      lcd.setCursor(0, 1);
      lcd.print("T = ");
      lcd.print(Temperatura);
      lcd.print("C ");
      lcd.setCursor(0, 2);
      lcd.print("U = ");
      lcd.print(Umidade);
      lcd.print(" % ");
      break;
    case 1:                                          //LCD da rotação
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("VentRot = ");
      lcd.print(RPM);
      lcd.print("RPM");
      lcd.setCursor(0, 1);
      lcd.print("i7 = ");
      lcd.print(Irms7);
      lcd.print("A ");
      lcd.setCursor(0, 2);
      lcd.print("i8 = ");
      lcd.print(Irms8);
      lcd.print("A ");
      lcd.setCursor(0, 3);
      lcd.print("i9 = ");
      lcd.print(Irms9);
      lcd.print("A ");
      break;

    case 2:
      //LCD compressor 1
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Comp 1 (A)");
      lcd.setCursor(0, 1);
      lcd.print("i1 = ");
      lcd.print(Irms1);
      lcd.print("A ");
      lcd.setCursor(0, 2);
      lcd.print("i2 = ");
      lcd.print(Irms2);
      lcd.print("A ");
      lcd.setCursor(0, 3);
      lcd.print("i3 = ");
      lcd.print(Irms3);
      lcd.print("A ");
      break;

    case 3:                         //LCD compressor 2
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Comp 2 (A)");
      lcd.setCursor(0, 1);
      lcd.print("i4 = ");
      lcd.print(Irms4);
      lcd.print("A ");
      lcd.setCursor(0, 2);
      lcd.print("i5 = ");
      lcd.print(Irms5);
      lcd.print("A ");
      lcd.setCursor(0, 3);
      lcd.print("i6 = ");
      lcd.print(Irms6);
      lcd.print("A ");
      break;
  }

  //DEBUG DE TODAS AS MEDICOES ________________________________________________________
#ifdef DEBUG_MEDICAO
  //Variável temperatura do ambiente
  Serial.print(Temperatura);
  Serial.print(F("°C "));
  Serial.print(F("UR = "));
  Serial.print(Umidade);
  Serial.println(F(" % "));
  Serial.print(F("Vent: "));          //Variáveis do ventilador
  Serial.print(RPM);
  Serial.println(F(" RPM "));
  Serial.print(F("Comp_01: "));       //Variáveis do compressor 1
  Serial.print(Irms1);
  Serial.print(F("A "));
  Serial.print(Irms2);
  Serial.print(F("A "));
  Serial.print(Irms3);
  Serial.println(F("A "));
  Serial.print(F("Comp_02: "));       //Variáveis do compressor 2
  Serial.print(Irms4);
  Serial.print(F("A "));
  Serial.print(Irms5);
  Serial.print(F("A "));
  Serial.print(Irms6);
  Serial.println(F("A "));
  Serial.print(F("vent: "));       //Variáveis do compressor 3
  Serial.print(Irms7);
  Serial.print(F("A "));
  Serial.print(Irms8);
  Serial.print(F("A "));
  Serial.print(Irms9);
  Serial.println(F("A "));
  Serial.print(F("Operacao: "));
  Serial.println(operacao);
  Serial.print(F("Manutencao: "));
  Serial.println(manutencao);
#endif

  t_sql_1 = millis();
    t_sql_t = t_sql_1 - t_sql_0;

    } while (t_sql_t <= TEMPO_PARA_ENVIAR_DADOS); //Finaliza loop de dados e ações

  //Enviando dados para o MySQL
  if (clienteArduino.connect(servidor, portaHTTP)) {

    clienteArduino.print("GET /projeto/salvarhc.php");
    clienteArduino.print("?s1=");
    clienteArduino.print(Temperatura);
    clienteArduino.print("&s2=");
    clienteArduino.print(Umidade);
    clienteArduino.print("&s3=");
    clienteArduino.print(RPM);
    clienteArduino.print("&s4=");
    clienteArduino.print(Irms1);
    clienteArduino.print("&s5=");
    clienteArduino.print(Irms2);
    clienteArduino.print("&s6=");
    clienteArduino.print(Irms3);
    clienteArduino.print("&s7=");
    clienteArduino.print(Irms4);
    clienteArduino.print("&s8=");
    clienteArduino.print(Irms5);
    clienteArduino.print("&s9=");
    clienteArduino.print(Irms6);
    clienteArduino.print("&s10=");
    clienteArduino.print(Irms7);
    clienteArduino.print("&s11=");
    clienteArduino.print(Irms8);
    clienteArduino.print("&s12=");
    clienteArduino.print(Irms9);
    clienteArduino.print("&s13=");
    clienteArduino.print(operacao);
    clienteArduino.print("&s14=");
    clienteArduino.print(manutencao);
    clienteArduino.println(" HTTP/1.0");
    clienteArduino.println("Host: 192.168.0.30");
    clienteArduino.println("Connection: close");
    clienteArduino.println();

    clienteArduino.stop();
    Serial.println("conectado com o banco de dados");
  }
  else {
    Serial.println("Falha ao conectar com o servidor");

    clienteArduino.stop();
  }
  delay(5000);
}
/*___________________________________________________________________________________________
  /////////////////////////////////////////////////////FUNCOES EXTERNAS DO PROGRAMA//////////
  ___________________________________________________________________________________________*/

/////////////////////////////////////////////////////FUNCOES DA EEPROM///////////////////////
void EEPROM_Write_Int(int endereco, int valor) {
  byte Byte_alto = highByte(valor);
  byte Byte_baixo = lowByte(valor);

  EEPROM.write(endereco, Byte_alto);
  EEPROM.write(endereco + 1, Byte_baixo);
}

int EEPROM_Read_Int(int endereco) {
  byte Byte_alto = EEPROM.read(endereco);
  byte Byte_baixo = EEPROM.read(endereco + 1);

  return word(Byte_alto, Byte_baixo);
}

/////////////////////////////////////////////////////FUNCAO MEDICAO DA ROTACAO///////////////

void InfraVermelho() {
  //Esta funcao e acionada pela interrupcao do infravermelho
  contagem = contagem + 1;

#ifdef DEBUG_INFRA
  Serial.print(F("contagem do infravermelho = "));
  Serial.print(contagem);
  Serial.println(F(" obstaculos"));
#endif
}

/////////////////////////////////////////////////////FUNCAO COMANDO DE RELES///////////////////

void ComandoRele() {


  if (isnan(Temperatura) || isnan(Umidade)) { //Se houver erro na leitura de temperatura ou na leitura de umidade

#ifdef DEBUG_PROBLEMA
    Serial.println(F("Existe um problema no sensor DHT"));
    Serial.println(F("Se o equipamento estiver com jumper de reset, será reiniciado agora..."));
    Serial.println(F("Caso contrário, irá desligar e manter somente o ventilador"));
#endif
    digitalWrite(VentiladorRele, DESLIGA);
    digitalWrite(ResistenciaRele1, DESLIGA);
    digitalWrite(ResistenciaRele2, DESLIGA);
    digitalWrite(CompressorRele1, DESLIGA);
    digitalWrite(CompressorRele2, DESLIGA);
    digitalWrite(UmidificadorRele, DESLIGA);

    digitalWrite(RESET, LOW); //Reinicia o sistema automaticamente caso o jump tenha sido feito fisicamente no microcontrolador
  }
  else if (RPM <= RotMin) {
    digitalWrite(VentiladorRele, DESLIGA);
    digitalWrite(ResistenciaRele1, DESLIGA);
    digitalWrite(ResistenciaRele2, DESLIGA);
    digitalWrite(CompressorRele1, DESLIGA);
    digitalWrite(CompressorRele2, DESLIGA);
    digitalWrite(UmidificadorRele, DESLIGA);

    operacao = 8;
    manutencao = QUEBRADO;

#ifdef DEBUG_PROBLEMA
    Serial.println(F("Manutencao 0 : Equipamento desligado por baixa rotacao"));
#endif

#ifdef DEBUG_OPERACAO
    Serial.print(F("Operacao 8 : Desligado por problema na ventilacao"));
#endif

    //Alarme
    tone(BUZZER, 700, 2000);
  }
  //A partir daqui, após análise da rotação, verifica-se as condições do ambiente

  else if (Temperatura >= TempAlta && Umidade >= UmidAlta) {
    //Temperatura alta e umidade alta
    digitalWrite(VentiladorRele, LIGA);
    digitalWrite(ResistenciaRele1, LIGA);
    digitalWrite(ResistenciaRele2, DESLIGA);
    digitalWrite(CompressorRele1, LIGA);
    digitalWrite(CompressorRele2, LIGA);
    digitalWrite(UmidificadorRele, DESLIGA);
    //digitalWrite(DesumidificadorRele, LIGA);

    operacao = 3;
    manutencao = FUNCIONANDO;



#ifdef DEBUG_PROBLEMA
    Serial.println(F("Manutencao 1 : Funcionando"));
#endif

#ifdef DEBUG_OPERACAO
    Serial.print(F("Operacao 3 : Resfriamento com desumificacao de uma resistencia"));
#endif
  }

  else if (Temperatura >= TempAlta
           && Umidade > UmidBaixa && Umidade < UmidAlta) {
    //Temperatura alta e umidade no padrão
    digitalWrite(VentiladorRele, LIGA);
    digitalWrite(CompressorRele1, LIGA);
    digitalWrite(CompressorRele2, LIGA);
    //digitalWrite(DesumidificadorRele, DESLIGA);

    operacao = 1;
    manutencao = FUNCIONANDO;


#ifdef DEBUG_PROBLEMA
    Serial.println(F("Manutencao 1 : Funcionando"));
#endif

#ifdef DEBUG_OPERACAO
    Serial.print(F("Operacao 1 : Resfriamento"));
#endif
  }

  else if (Temperatura >= TempAlta
           && Umidade <= UmidBaixa) {
    //Temperatura alta e umidade baixa
    digitalWrite(VentiladorRele, LIGA);
    digitalWrite(ResistenciaRele1, DESLIGA);
    digitalWrite(ResistenciaRele2, DESLIGA);
    digitalWrite(CompressorRele1, LIGA);
    digitalWrite(CompressorRele2, LIGA);
    //digitalWrite(DesumidificadorRele, DESLIGA);
    if (Umidade <= 0.95 * UmidBaixa) {
      digitalWrite(UmidificadorRele, LIGA);
#ifdef DEBUG_OPERACAO
      Serial.print(F("Operacao 2.1 : Resfriamento umidificado com umidificador"));
#endif
    }
    else {
#ifdef DEBUG_OPERACAO
      Serial.print(F("Operacao 2.2 : Resfriamento umidificado sem umidificador"));
#endif
    }

    operacao = 2;
    manutencao = FUNCIONANDO;


#ifdef DEBUG_PROBLEMA
    Serial.println(F("Manutencao 1 : Funcionando"));
#endif

#ifdef DEBUG_OPERACAO
    Serial.print(F("Operacao 2 : Resfriamento com umidificacao"));
#endif
  }

  else if (Temperatura > TempBaixa
           && Temperatura < TempAlta && Umidade >= UmidAlta) {
    //temperatura no padrão e umidade alta
    digitalWrite(VentiladorRele, LIGA);
    digitalWrite(ResistenciaRele1, LIGA);
    digitalWrite(ResistenciaRele2, DESLIGA);
    digitalWrite(UmidificadorRele, DESLIGA);
    //digitalWrite(DesumidificadorRele, LIGA);

    operacao = 4;
    manutencao = FUNCIONANDO;


#ifdef DEBUG_PROBLEMA
    Serial.println(F("Manutencao 1 : Funcionando"));
#endif

#ifdef DEBUG_OPERACAO
    Serial.print(F("Operacao 4 : Desumificacao com banco de resistencias 1"));
#endif
  }

  else if (Temperatura > TempBaixa
           && Temperatura < TempAlta && Umidade < UmidAlta
           && Umidade > UmidBaixa) {
    //temperatura no padrão e umidade no padrão
    digitalWrite(VentiladorRele, LIGA);

    operacao = 9;
    manutencao = FUNCIONANDO;


#ifdef DEBUG_PROBLEMA
    Serial.println(F("Manutencao 1 : Funcionando"));
#endif

#ifdef DEBUG_OPERACAO
    Serial.print(F("Operacao 9 : Sem ação"));
#endif
  }

  else if (Temperatura > TempBaixa &&
           Temperatura < TempAlta && Umidade >= UmidBaixa) {
    //temperatura no padrão e umidade baixa
    digitalWrite(VentiladorRele, LIGA);
    digitalWrite(ResistenciaRele1, DESLIGA);
    digitalWrite(ResistenciaRele2, DESLIGA);
    //digitalWrite(DesumidificadorRele, LIGA);
    if (Umidade <= 0.95 * UmidBaixa) {
      digitalWrite(UmidificadorRele, LIGA);
#ifdef DEBUG_OPERACAO
      Serial.print(F("Operacao 10.1 : Umidificação com umidificador ligado"));
#endif
    }
    else {
#ifdef DEBUG_OPERACAO
      Serial.print(F("Operacao 10.2 : Umidificação com umidificador desligado"));
#endif
    }

    operacao = 4;
    manutencao = FUNCIONANDO;


#ifdef DEBUG_PROBLEMA
    Serial.println(F("Manutencao 1 : Funcionando"));
#endif
  }

  else if (Temperatura <= TempBaixa
           && Umidade >= UmidAlta) {
    //Temperatura baixa e umidade alta
    digitalWrite(VentiladorRele, LIGA);
    digitalWrite(ResistenciaRele1, LIGA);
    digitalWrite(ResistenciaRele2, DESLIGA);
    digitalWrite(CompressorRele1, DESLIGA);
    digitalWrite(CompressorRele2, DESLIGA); //Neste caso um compressor desliga para a temperatura subir aos poucos
    digitalWrite(UmidificadorRele, DESLIGA);
    // digitalWrite(DesumidificadorRele, LIGA);

    operacao = 5;
    manutencao = FUNCIONANDO;


#ifdef DEBUG_PROBLEMA
    Serial.println(F("Manutencao 1 : Funcionando"));
#endif

#ifdef DEBUG_OPERACAO
    Serial.print(F("Operacao 5 : Aquecimento rapido"));
#endif
  }

  else if (Temperatura <= 0.85 * TempBaixa
           && Umidade >= UmidAlta) {
    //Temperatura MUITO baixa e umidade alta
    digitalWrite(VentiladorRele, LIGA);
    digitalWrite(ResistenciaRele1, LIGA);
    digitalWrite(ResistenciaRele2, LIGA);
    digitalWrite(CompressorRele1, DESLIGA);
    digitalWrite(CompressorRele2, DESLIGA); //Neste caso um compressor desliga para a temperatura subir aos poucos
    digitalWrite(UmidificadorRele, DESLIGA);
    // digitalWrite(DesumidificadorRele, LIGA);

    operacao = 5;
    manutencao = FUNCIONANDO;


#ifdef DEBUG_PROBLEMA
    Serial.println(F("Manutencao 1 : Funcionando"));
#endif

#ifdef DEBUG_OPERACAO
    Serial.print(F("Operacao 5 : Aquecimento rapido"));
#endif
  }

  else if (Temperatura <= TempBaixa
           && Umidade > UmidBaixa && Umidade < UmidAlta) {
    //Temperatura Baixa e umidade no padrão
    digitalWrite(VentiladorRele, LIGA);
    digitalWrite(ResistenciaRele1, DESLIGA); //Aquecimento lento por troca natural
    digitalWrite(ResistenciaRele2, DESLIGA);
    digitalWrite(CompressorRele1, DESLIGA);
    digitalWrite(CompressorRele2, DESLIGA);
    // digitalWrite(DesumidificadorRele, DESLIGA);

    operacao = 7;
    manutencao = FUNCIONANDO;


#ifdef DEBUG_PROBLEMA
    Serial.println(F("Manutencao 1 : Funcionando"));
#endif

#ifdef DEBUG_OPERACAO
    Serial.print(F("Operacao 7 : Aquecimento lento"));
#endif
  }

  else if (Temperatura <= 0.90 * TempBaixa
           && Umidade > UmidBaixa && Umidade < UmidAlta) {
    //Temperatura MUITO Baixa e umidade no padrão
    digitalWrite(VentiladorRele, LIGA);
    digitalWrite(ResistenciaRele1, DESLIGA); //Aquecimento lento
    digitalWrite(ResistenciaRele2, LIGA);
    digitalWrite(CompressorRele1, DESLIGA);
    digitalWrite(CompressorRele2, DESLIGA);
    // digitalWrite(DesumidificadorRele, DESLIGA);

    operacao = 7;
    manutencao = FUNCIONANDO;


#ifdef DEBUG_PROBLEMA
    Serial.println(F("Manutencao 1 : Funcionando"));
#endif

#ifdef DEBUG_OPERACAO
    Serial.print(F("Operacao 7 : Aquecimento lento"));
#endif
  }

  else if (Temperatura <= TempBaixa
           && Umidade <= UmidBaixa) {
    //Temperatura baixa e umidade baixa
    digitalWrite(VentiladorRele, LIGA);
    digitalWrite(ResistenciaRele1, DESLIGA); //Apenas um grupo de resistência liga para aumentar a temperatura devagar
    digitalWrite(ResistenciaRele2, DESLIGA);
    digitalWrite(CompressorRele1, DESLIGA);
    digitalWrite(CompressorRele2, DESLIGA);
    // digitalWrite(DesumidificadorRele, DESLIGA);
    digitalWrite(UmidificadorRele, LIGA);

    operacao = 6;
    manutencao = FUNCIONANDO;


#ifdef DEBUG_PROBLEMA
    Serial.println(F("Manutencao 1 : Funcionando"));
#endif

#ifdef DEBUG_OPERACAO
    Serial.print(F("Operacao 6 : Aquecimento com umidificacao"));
#endif
  }

  else if (Temperatura <= 0.90 * TempBaixa
           && Umidade <= UmidBaixa) {
    //Temperatura MUITO baixa e umidade baixa
    digitalWrite(VentiladorRele, LIGA);
    digitalWrite(ResistenciaRele1, LIGA); //Apenas um grupo de resistência liga para aumentar a temperatura devagar
    digitalWrite(ResistenciaRele2, DESLIGA);
    digitalWrite(CompressorRele1, DESLIGA);
    digitalWrite(CompressorRele2, DESLIGA);
    // digitalWrite(DesumidificadorRele, DESLIGA);
    digitalWrite(UmidificadorRele, LIGA);

    operacao = 6;
    manutencao = FUNCIONANDO;


#ifdef DEBUG_PROBLEMA
    Serial.println(F("Manutencao 1 : Funcionando"));
#endif

#ifdef DEBUG_OPERACAO
    Serial.print(F("Operacao 6 : Aquecimento com umidificacao"));
#endif
  }
}
/////////////////////////////////////////////////////FUNCOES DO DISPLAY LCD////////////////
void botaoInterrupcao() {
  noInterrupts();

#ifdef DEBUG_BOTAO
  Serial.println(F("A funcao botaoInterrupcao() foi acionada"));
#endif

  botaoCont = botaoCont + 1;

#ifdef DEBUG_BOTAO
  Serial.print("botaoCont antes do IF = ");
  Serial.println(botaoCont);
#endif


  if (botaoCont < 1 || botaoCont > 3) {
    botaoCont = 0;
  }
  else {
    botaoCont;
  }
#ifdef DEBUG_BOTAO
  Serial.print("botaoCont depois do IF = ");
  Serial.println(botaoCont);
#endif
  interrupts();
}
