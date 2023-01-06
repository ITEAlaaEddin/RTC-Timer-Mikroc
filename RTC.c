unsigned int timeValue[]={31,12,22,23,59,0,1,55};
unsigned int colOfTimeValue[]={0,3,8,11,14,6,9};
unsigned int rowOfTimeValue[]={0,0,0,0,0,1,1};
unsigned char monthArray[]={0,31,29,31,30,31,30,31,31,30,31,30,31};


void init (void);
void interrupt_initial();
void timer2_init (void);


void getNumberDigits(int number);
void button_to_shift_right();
void increment_by_button();
void goToPosition(int row, int col);
void LCDInit();
void twiInit();
void twiStart();
void twiAddr(unsigned char addr);
void twiData(unsigned char dataa);
void twiStop();
void LCDCommand(int p);
void LCDInit();
void LCDPrint(unsigned char t, int isNumeric);
void goToPosition(int row , int col);
void LCDScheme();
void LCDRefresh();
void LCDUpdateAlarm();


int highnumber=0;
int lownumber=0;
unsigned char column = 0;
int row = 0;
unsigned char alarmStatus = 0;
unsigned char reachToAlarmsHour = 0;

unsigned char pressChangeAlarmStatus=0;
unsigned char pressButtonToShiftRight =0 ;
unsigned char pressIncrementByButton =0;
unsigned char pressDecreaseByButton =0;

void changeAlarmStatus() org 0x0004
{   if(pressChangeAlarmStatus==0)
    {
       pressChangeAlarmStatus=1;
       if(alarmStatus==0)
       {
         alarmStatus = 1;
         goToPosition(1,12);
         LCDPrint('O',0);
         LCDPrint('N',0);
         LCDPrint(' ',0);
         goToPosition(row,colOfTimeValue[column]);
         if(timeValue[3] == timeValue[5])
            reachToAlarmsHour = 1;
       }
       else
       {
         alarmStatus = 0;
         reachToAlarmsHour = 0;
         PORTB.B5 = 0;
         goToPosition(1,12);
         LCDPrint('O',0);
         LCDPrint('F',0);
         LCDPrint('F',0);
         goToPosition(row,colOfTimeValue[column]);

       }
    }

}

void buttonToShiftRight() org 0x0006
 {
   if(pressButtonToShiftRight ==0)
   {
     pressButtonToShiftRight =1;
     if(PINB.B2 == 1)
      {

        column = (column+1)%7;
        if(column > 4)
           row = 1;
        else
           row = 0;
        goToPosition(row,colOfTimeValue[column]);
      }
    }
 }


void incrementByButton() org 0x00008
{
  if(pressIncrementByButton == 0)
  {
    pressIncrementByButton =1 ;
    if( PINC.B3 == 1)
    {
      int value = timeValue[column];
      value++;
            switch(column)
            {
             case 0:
                  if(value > monthArray[timeValue[1]])
                           value = 1;
                  timeValue[7]=0;
                  break;
             case 1:
                  if(value > 12)
                           value = 1;
                  timeValue[7]=0;
                  break;
             case 3:
                  if(value == 24)
                           value = 0;
                  timeValue[7]=0;
                  break;
             case 4:
                  if(value == 60)
                           value = 0;
                  timeValue[7]=0;
                  break;
             case 5:
                  if(value == 24)
                           value = 0;
                  break;
             case 6:
                  if(value == 60)
                           value = 0;
                  break;
            }
            timeValue[column] = value;
            LCDUpdateAlarm();
            LCDRefresh();
    }
  }
}



void decreaseByButton() org 0x000A
{
  int value = timeValue[column];
  if(pressDecreaseByButton ==0)
  {   pressDecreaseByButton = 1;
      if( PIND.B1 == 1)
      {
            value--;
                switch(column)
                {
                 case 0:
                      if(value == 0)
                               value = monthArray[timeValue[1]];
                               timeValue[7]=0;
                      break;
                 case 1:
                      if(value == 0)
                               value = 12;
                      timeValue[7]=0;
                      break;
                 case 3:
                      if(value < 0)
                               value = 23;
                      timeValue[7]=0;
                      break;
                 case 4:
                      if(value < 0)
                               value = 59;
                      timeValue[7]=0;
                      break;
                 case 5:
                      if(value < 0)
                               value = 23;
                      break;
                 case 6:
                      if(value < 0)
                               value = 59;
                      break;
                }
                timeValue[column] = value;
                LCDUpdateAlarm();
                LCDRefresh();


        }
    }
}


 void time_increment(void) org 0x12
{
     //index:   0          1          2          3           4             7
    // name : days      months      years       hours      minutes       seconds
  timeValue[7]++;
  if(timeValue[7]==60)
  {

     timeValue[7]=0;
     timeValue[4]++;

     if(timeValue[4]==60)
     {
        timeValue[4]=0;
        timeValue[3]++;
        if(timeValue[3]==24)
        {
           timeValue[3]=0;
           timeValue[0]++;
           if(timeValue[0] > monthArray[timeValue[1]])
           {
              timeValue[0]=1;
              timeValue[1]++;
              if(timeValue[1] > 12)
              {
                 timeValue[1]=1;
                 timeValue[2]++;
              }
           }
        }
        if(timeValue[3] == timeValue[5])  // reach to alarms hour
            reachToAlarmsHour = 1;
     }
     if((alarmStatus==1) && (reachToAlarmsHour == 1) && (timeValue[4] == timeValue[6]))    // reach to alarms value
          PORTB.B5 = 1;
     LCDRefresh();
  }

}


void resetPress () org 0x001A
{
   pressChangeAlarmStatus=0;
   pressButtonToShiftRight =0 ;
   pressIncrementByButton =0;
   pressDecreaseByButton =0;

}

void main() {



  init();
  interrupt_initial();
  timer2_init();

  twiInit();
  twiStart();
  twiAddr(0x27);
  LCDInit();
  LCDScheme();

  DDRB.B5=1; //for alarm output


}


void twiInit(){
    TWBR=0x00;TWBR=72;  //freq 100KHZ
}

void twiStart(){

   TWCR =(1<<TWINT)|(1<<TWEN)|(1<<TWSTA); //send start
   while(TWCR.B7==0) ;
   while((TWSR&(0xF8))!=0x08)  ;
}

void twiAddr(unsigned char addr){   //send Address
    TWDR=addr<<1;
    TWCR=(1<<TWINT)|(1<<TWEN);
    while((TWCR &(1<<TWINT))== 0) ;
    while((TWSR &(0xF8))!= 0x18)  ;
}

void twiData(unsigned char dataa){  //send data
    TWDR=dataa;
    TWCR=(1<<TWINT)|(1<<TWEN);
    while((TWCR &(1<<TWINT))== 0) ;
    while((TWSR &(0xF8))!= 0x28)  ;
}

void twiStop(){
   TWCR =(1<<TWINT)|(1<<TWEN)|(1<<TWSTO); //send stop

}

void LCDCommand(int p){
     twidata((p&0b11110000)|0b00001100);
     twidata((p&0b11110000)|0b00001000);
     twidata(((p&0b00001111)<<4)|0b00001100);
     twidata(((p&0b00001111)<<4)|0b00001000);
}


void LCDInit(){

//LCDCommand(0x3c) ;
LCDCommand(0x32) ;
LCDCommand(0x01) ;      //clear Display
LCDCommand(0x02) ;      //Return Home
LCDCommand(0x0e) ;      //Display On ,No Courser



}




void LCDPrint(unsigned char t, int isNumeric){
   unsigned char p ;
   if(isNumeric)
      p=t+48;
   else
      p=t;

       twidata((p&0b11110000)|0b00001101);
       twidata((p&0b11110000)|0b00001001);
       twidata(((p&0b00001111)<<4)|0b00001101);
       twidata(((p&0b00001111)<<4)|0b00001001);
}

void goToPosition(int row , int col){
  if(row==0)
     LCDCommand(128+col);
  else
     LCDCommand(192+col);


}

void LCDScheme()
{
 goToPosition(0,2);
 LCDPrint('/',0);

 goToPosition(0,5);
 LCDPrint('/',0);

 goToPosition(0,6);
 LCDPrint(2,1);
 LCDPrint(0,1);

 goToPosition(0,13);
 LCDPrint(':',0);

 goToPosition(1,0);
 LCDPrint('A',0);
 LCDPrint('l',0);
 LCDPrint('a',0);
 LCDPrint('r',0);
 LCDPrint('m',0);

 goToPosition(1,8);
 LCDPrint(':',0);

 goToPosition(1,12);
 LCDPrint('O',0);
 LCDPrint('F',0);
 LCDPrint('F',0);

 LCDRefresh();     // print default time  value
 LCDUpdateAlarm(); // print default alarm value
}


void LCDRefresh()
{
   int c;
   LCDCommand(0x0c);
   for(c = 0 ; c < 5 ; c++)
   {
     getNumberDigits(timeValue[c]);
     goToPosition(0,colOfTimeValue[c]);
     LCDPrint(highnumber,1);
     LCDPrint(lownumber,1);

   }
   goToPosition(row,colOfTimeValue[column]);
   LCDCommand(0x0e);
}

void LCDUpdateAlarm()
{
int c;
   LCDCommand(0x0c);
   for(c = 5 ; c < 7 ; c++)
   {
     getNumberDigits(timeValue[c]);
     goToPosition(1,colOfTimeValue[c]);
     LCDPrint(highnumber,1);
     LCDPrint(lownumber,1);

   }
   goToPosition(row,colOfTimeValue[column]);
   LCDCommand(0x0e);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////






void interrupt_initial()
{

  // int 1 set timer
  EICRA.B2 = 1;
  EICRA.B3 = 1;
  EIMSK.B1 = 1;
//  SREG.B7 = 1;

  // Pin Change Interrupt Request PCINT17
  PCICR.B2=1;
  PCMSK2.B1=1;

  // Pin Change Interrupt Request PCINT0
  PCICR.B0=1;
  PCMSK0.B2=1;

  // Pin Change Interrupt Request PCINT0
  PCICR.B1=1;
  PCMSK1.B3=1;
}



void getNumberDigits(int number)
{

   lownumber = number % 10;
   number = number / 10;
   highnumber = number % 10;

}

void timer2_init (void) {

  // set counter/timer 2 (RTC)
  TCCR2A = 0;
  TCCR2B = 0;  //stop Timer 2
  TIMSK2 = 0; // disable Timer 2 interrupts
  ASSR = (1 << AS2); // select asynchronous operation of Timer2
  TCNT2 = 0; // clear Timer 2 counter
  TCCR2B = (1 << CS22) | (1 << CS20); // select prescaler 128 => 1 sec between each overflow

  while (ASSR & ((1 << TCN2UB) | (1 << TCR2BUB))); // wait for TCN2UB and TCR2BUB to be cleared

  TIFR2 = (1 << TOV2); // clear interrupt-flag
  TIMSK2 = (1 << TOIE2); // enable Timer2 overflow interrupt

  // set timer
  TCCR0A = 0;
  TCCR0B = 0b00000001; // set mode and prescale
  //TIMSK0 = 0b00000001;  // enable interrupt overflow


  TCCR1A=0;              // initial timer/counter1
  TCCR1B=0b00000011;
  TIMSK1=0b00000001;
}

void init (void) {
 //PRR Power Reduction Register (set PRADC after ADCSRA=0)
  //Bit 7 - PRTWI: Power Reduction TWI
  //Bit 6 - PRTIM2: Power Reduction Timer/Counter2
  //Bit 5 - PRTIM0: Power Reduction Timer/Counter0
  //Bit 3 - PRTIM1: Power Reduction Timer/Counter1
  //Bit 2 - PRSPI: Power Reduction Serial Peripheral Interface
  //Bit 1 - PRUSART0: Power Reduction USART0
  //Bit 0 - PRADC: Power Reduction ADC

   ADCSRA = 0; //disable ADC
  // turn off  unneeded modules.


  PRR |=  (1 << PRSPI) | (1 << PRADC); // | (1 << PRUSART0) ;

  // reprogram timer 2 for this application
   SREG.B7=0b1; // global interrupt enable
}