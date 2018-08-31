// include the library code:
#include <LiquidCrystal.h>
#include <RCSwitch.h> //отправка
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
RCSwitch mySwitch = RCSwitch(); //отправка

bool alg(int n, char *s2);//перебирает все варианты кода при n макимально возможном количестве запаяных ножек и коде сигнала s2
bool rec(char *s1, int n1, int nj, char *s2);/*перебирает варианты для ровно nj запаянных ног, 
                                              n1 - позиция с которой начинает перебирать, s1 - строка для 1ой части сигнала, s2 - строка для кода кнопки*/
bool check(char *s1, char *s2); //отправляет сигнал, смотрит, не нажата ли кнопка паузы (left)
void clear();//заполняет экран пробелами
int readWithWait(int pin);// ждет нажатия кнопки, возвращает сопротивление нажатой кнопки
int enterLegNumber(int maxn);//ввод максимально возможного количество запаянных ног, maxn - ограничение сверху
int enterButtonCodeLenght();//ввод длины кода кнопки
void enterButtonCode(char *s2);//содержимое s2 меняется на введенное с помощью кнопок
int enterNumber(int initn, int minn, int maxn); //считывает число, вводимое с помощью кнопок
bool reverse;//флаг направления (forward == false/backward == true)
int st;//номер просмотренного варианта(~)
bool rec_infull(char *s, int n1, int nj);//перебирает варианты сигнала кода
bool full(int l);//перебирает вообще все варианты (~9 часов)
int iternum;//столько раз повторяется отправление сигнала, увеличивается при повороте (backward/forward)
char *s3;//код отправляемый на момент поворота вперед (forward)
char *s4;//код отправляемый на момент поворота назад (backward)
int delayBetweenSends = 50; //задержка между отправкой сообщений (мс)
int delayBetweenButtons = 30;  //задержка между проверяемыми кнопками (мин)


void setup() {
  Serial.begin(9600); //отправка на монитор порта
  mySwitch.enableTransmit(4); // Transmitter is connected to Arduino Pin #10
  mySwitch.setPulseLength(260); //время отправки сигнала
  lcd.begin(16, 2);// set up the LCD's number of columns and rows
  lcd.setCursor(0, 0);
  lcd.print("right - custom");
  lcd.setCursor(0, 1);
  lcd.print("left - full");
}

void loop() {
  int x;
  x = analogRead(0);
  lcd.setCursor(0, 0);
  if (x < 60) {//right
    delay(500);
    int l = enterButtonCodeLenght();
    char *s2 = (char*)malloc((l + 1) * sizeof(char));
    for (int i = 0; i < l + 1; ++i)
      s2[i] = '0';
    s2[l] = '\0';
    enterButtonCode(s2);
    int n = enterLegNumber(12 - l);
    clear();
    if (alg(n, s2))
    {
      lcd.setCursor(0, 0);
      lcd.print("select - custom");
      lcd.setCursor(0, 1);
      lcd.print("left - full");
    }
  } 
  else if (x < 200) {} //up
  else if (x < 400) {} //down
  else if (x < 600) { //left
    int l = enterButtonCodeLenght();
    clear();
    full(l);
    lcd.setCursor(0, 0);
      lcd.print("select - custom");
      lcd.setCursor(0, 1);
      lcd.print("left - full");
  }
  else if (x < 800) {} //select 
}

//n - максимальное кол-во возможных зап.ног
//s2 - код кнопки
bool alg(int n, char *s2)
{
  st = 0;
  reverse = 0;
  iternum = 0;
  int k = 13 - strlen(s2);
  char *s1 = (char*)malloc(k * sizeof(char));
  for (int i = 0; i < k; ++i)
    s1[i] = 'F';
  s1[k - 1] = '\0';
  s3 = (char*)malloc(k * sizeof(char));
  for (int i = 0; i < k; ++i)
    s3[i] = 'F';
  s3[k - 1] = '\0';
  s3[0] = '0';
  s4 = (char*)malloc(k * sizeof(char));
  for (int i = 0; i < k; ++i)
    s4[i] = 'F';
  s4[k - 1] = '\0';
  for (int i = k - n; i < k; ++i)
    s4[i] = '1';
  bool bi = true;
  int i = 1;
  while (bi)
  {
    if (rec(s1, 0, i, s2)) return true;
    i = !reverse ? i + 1 : i - 1;
    if (!reverse && i > n)  bi = false;
    if (reverse && i < 1) bi = false;
  }
  free(s1);
  free(s2);
  free(s3);
  free(s4);
  return false;
}

//BOOLEAN rec(char *s1, int n1, int nj) - нужно точно знать кол-во зап. ножек
//s1 - первый блок кода
// n1 - позиция с какой начинаем подставлять 1/0
// nj - запаянные ножки
//возвращает 1 если прервано, 0 если просмотрел все варианты
bool rec(char *s1, int n1, int nj, char *s2)
{
  bool bi = true;
  int i = 0;
  i = !reverse ? n1 : strlen(s1) - 1;
  while (bi)
  {
    bool bp = true;
    int p = !reverse ? 0 : 1;
    while (bp)
    {
      s1[i] = p + '0';
      if (nj == 1)
      {
        bool prevrev = reverse;
        if (check(s1, s2))
          return true;
        if (prevre != reverse)
        {
          p=!prevrev?p+1:p-1;
          st=!prevrev?st+1:st-1;
        }
      }
      else if (i != strlen(s1) - 1)
        if (rec(s1, i + 1, nj - 1, s2))
          return 1;
      s1[i] = 'F';
      p = !reverse ? p + 1 : p - 1;
      if (!reverse && p > 1) bp = false;
      if (reverse && p < 0) bp = false;
    }
    i = !reverse ? i + 1 : i - 1;
    if (!reverse && i >= strlen(s1))  bi = false;
    if (reverse && i < n1) bi = false;
  }
  return false;
}

bool check(char *s1, char *s2)
{
  st = !reverse ? st+1 : st-1;
  char s[13] = "";
  for (int i = 0; i < strlen(s1); ++i)
    s[i] = s1[i];
  for (int i = strlen(s1); i < 13; ++i)
    s[i] = s2[i - strlen(s1)];
  Serial.println(s);
  for (int j = 0; j <= iternum; ++j)
  {
    mySwitch.sendTriState(s);
    delay(delayBetweenSends);
  }
  lcd.setCursor(0, 0);
  lcd.print (s);
  lcd.setCursor(0, 1);
  lcd.print ("     ");
  lcd.setCursor(0, 1);
  lcd.print (st);
  int x;
  x = analogRead (0);
  if (((x > 400) && (x < 600)) || (reverse && strcmp(s3, s1) == 0) || (!reverse && strcmp(s4, s1) == 0)) 
  {
    clear();
    lcd.setCursor(0, 0);
    if (!reverse)   lcd.print ("backward");
    else   lcd.print ("forward");  
    lcd.setCursor(0, 1);
    lcd.print ("cancel");
    delay(500);
    lcd.blink();
    lcd.setCursor(0, 0);
    int c = 0;
    while (x > 60)
    {

      x = readWithWait(0);
      if (x > 60 && x < 200)
      {
        lcd.setCursor(0, 0);
        c = 0;
      }
      else if ( x > 200 && x < 400) {
        lcd.setCursor(0, 1);
        c = 1;
      } 
      else if (x > 400 && x<600)    
      {
        lcd.noBlink();
        delay(500);
        return true;
      }
    }
    if(c==0)
    {
      if (!reverse)
          s4 = strcpy(s4, s1);
      else
          s3 = strcpy(s3, s1);
      delay(400);
      enterButtonCode(s2);
    }
    Serial.println("_____________");
    Serial.println(s3);
    Serial.println(s4);
    Serial.println("_____________");
    if (!reverse) reverse = c == 0 ? true : false;
    else reverse = c == 0 ? false : true;
    if (c != 1)++iternum;
    lcd.noBlink();
    clear();
  }
  return false;
}

void clear()
{
  lcd.setCursor(0, 0);
  lcd.print ("                ");
  lcd.setCursor(0, 1);
  lcd.print ("                ");
}

int readWithWait(int pin)
{
  int n = 0;
  do {
    ++n;
    --n;
  } while (analogRead(pin) > 800);
  return analogRead(pin);
}

int enterLegNumber(int maxn)
{
  clear();
  lcd.setCursor(0, 0);
  lcd.print ("enter leg number");
  lcd.setCursor(0, 1);
  lcd.print ("use up/down");
  int n = enterNumber(3, 1, maxn);
  return n;
}

int enterButtonCodeLenght()
{
  clear();
  lcd.setCursor(0, 0);
  lcd.print ("enter the button");
  lcd.setCursor(0, 1);
  lcd.print ("code length:");
  //int x=readWithWait(0);
  int l = enterNumber(4, 1, 6);
  return l;
}

void enterButtonCode(char *s2)
{
  int l = strlen(s2);
  clear();
  lcd.setCursor(0, 0);
  lcd.print ("enter the button");
  lcd.setCursor(0, 1);
  lcd.print ("code:");
  lcd.setCursor(6, 1);
  lcd.print (s2);
  int c = 6;
  lcd.setCursor(c, 1);
  lcd.blink();
  int x = 0;
  do
  {
    x = readWithWait(0);
    if (x < 200 && x > 60) {
      if (s2[c - 6] == '0') s2[c - 6] = '1';
      else s2[c - 6] = '0';
    }
    else if (x < 400 && x > 200 && (c - 6) < l - 1) ++c; else if (c - 6 == l - 1) c = 6;
    lcd.setCursor(6, 1);
    lcd.print (s2);
    lcd.setCursor(c, 1);
    delay(700);
  } while (x > 60);
  lcd.noBlink();
}

int enterNumber(int initn, int minn, int maxn)
{
  int n = initn;
  lcd.setCursor(13, 1);
  lcd.print (n);
  int x = 900;
  while (x > 60)
  {
    lcd.setCursor(13, 1);
    lcd.print ("   ");
    if (x < 200 ) {
      if (n == maxn) n = minn;
      else ++n;
    }
    else if (x < 400 && x > 200 ) {
      if (n == minn) n = maxn;
      else --n;
    }
    lcd.setCursor(13, 1);
    lcd.print (n);
    x = readWithWait(0);
    delay(700);
  }
  return n;
}

bool full(int l)
{
  char * sb = (char *)malloc((l + 1) * sizeof(char));
  for (int i = 0; i < l; ++i)
    sb[i] = '0';
  sb[l] = '\0';
  for (int i = 0; i < l; ++i)
  {
    if (rec_infull(sb, 0, i)) return 1;
  }
  return 0;
}

bool rec_infull(char *s2, int n1, int nj)
{
  for (int i = n1; i < strlen(s2); ++i)
  {
    s2[i] = '1';
    if (nj == 1)
    {
      if (alg(12 - strlen(s2), s2))
        return 1;
      delay(delayBetweenButtons * 60 * 1000);
      s2[i] = '0';
    }
    else if (i != strlen(s2) - 1)
      if (rec_infull(s2, i + 1, nj - 1))
        return 1;
    s2[i] = '0';
  }
  return 0;
}

