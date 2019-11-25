/*
  Peavy Midi Pedal
  Mode 1   Bank up          Bank Down     Boost           Tuner
  Mode 2   Preset 1         Preset 2      Preset 3        Preset 4
  Mode 3   Effect on/off    Delay on/off  Reverb on/off   Stomp on/Off
  Mode 4   Record/Play      Stop          Erase   
*/

// constants won't change. They're used here to set pin numbers:
#define button0_Pin PB12  // the number of the pushbutton pin
#define button1_Pin PB3  // the number of the pushbutton pin
#define button2_Pin PB4  // the number of the pushbutton pin
#define button3_Pin PB5  // the number of the pushbutton pin
#define button4_Pin PB6  // the number of the pushbutton pin
#define button5_Pin PB13 // the number of the pushbutton pin

#define led1_Pin PB11      // the number of the LED pin
#define led2_Pin PB10      // the number of the LED pin
#define led3_Pin PB1      // the number of the LED pin
#define led4_Pin PB0      // the number of the LED pin
#define led5_Pin PA7      // the number of the LED pin
#define led6_Pin PA6      // the number of the LED pin
#define led7_Pin PA5
#define led8_Pin PA4 
#define led9_Pin PA3
#define led10_Pin PA2

#define pedal_Pin PA0

#define BAUD 115200

//#define DEBUG

// variables will change:
int button0_State = HIGH;
int button1_State = HIGH;
int button2_State = HIGH;
int button3_State = HIGH;
int button4_State = HIGH;
int button5_State = HIGH;
int button0_Last = HIGH;
int button1_Last = HIGH;
int button2_Last = HIGH;
int button3_Last = HIGH;
int button4_Last = HIGH;
int button5_Last = HIGH;

int mode_State = 1;
int mode_Last = 1;

int pedal_State = 1;
int pedal_Last = 1;
int pedal_Value = 0;
int wah_State = 0;

int tuner_State = 0;
int tuner_Last = 0;

void setup() {
  // initialize the LED pins as an output:
  pinMode(led1_Pin, OUTPUT);
  pinMode(led2_Pin, OUTPUT);
  pinMode(led3_Pin, OUTPUT);
  pinMode(led4_Pin, OUTPUT);
  pinMode(led5_Pin, OUTPUT);
  pinMode(led6_Pin, OUTPUT);

  pinMode(led7_Pin, OUTPUT);
  pinMode(led8_Pin, OUTPUT);
  pinMode(led9_Pin, OUTPUT);
  pinMode(led10_Pin, OUTPUT);

  // initialize the pushbutton pins as an input:
  pinMode(button0_Pin, INPUT);
  pinMode(button1_Pin, INPUT);
  pinMode(button2_Pin, INPUT);
  pinMode(button3_Pin, INPUT);
  pinMode(button4_Pin, INPUT);
  pinMode(button5_Pin, INPUT);

  #ifdef DEBUG
    // open serial debug output
    Serial1.begin(BAUD);
  #else
    // open serial MIDI output
    Serial1.begin(31250);            //MIDI communicates at 31250 baud
    // Set MIDI program 00
    Serial1.write(0xC0); // Program change
    Serial1.write(0x00); //  00
  #endif

  // turnon led1 and  turn off others
  digitalWrite(led1_Pin, HIGH);
  digitalWrite(led2_Pin, LOW);
  digitalWrite(led3_Pin, LOW);
  digitalWrite(led4_Pin, LOW);
  digitalWrite(led5_Pin, LOW);
  digitalWrite(led6_Pin, LOW);

  digitalWrite(led7_Pin, LOW);
  digitalWrite(led8_Pin, LOW);
  digitalWrite(led9_Pin, LOW);
  digitalWrite(led10_Pin, LOW);

  pinMode(pedal_Pin, INPUT);
  analogReadResolution(7);
}

void loop() {

  // read the state of the pushbutton value:
  button0_State = digitalRead(button0_Pin);
  button1_State = digitalRead(button1_Pin);
  button2_State = digitalRead(button2_Pin);
  button3_State = digitalRead(button3_Pin);
  button4_State = digitalRead(button4_Pin);
  button5_State = digitalRead(button5_Pin);

  // handle wah button presses
  if (button5_State && !button5_Last ) {
    #ifdef DEBUG
      Serial1.println("Button-5-PRESS");
    #endif
    pedal_State ++;
    if (pedal_State > 3) {
      pedal_State = 1;
    }
    
    if (pedal_State != pedal_Last ) {
      #ifdef DEBUG
        Serial1.println("pedal_State:");
        Serial1.println(pedal_State);
      #endif
      if (pedal_State == 1) {
          digitalWrite(led5_Pin, LOW);
          digitalWrite(led6_Pin, LOW);
          send_disable_wah();
          // turn off wah if needed
          //Serial1.write(0xB0);
          //Serial1.write(0x05);
          //Serial1.write(0x00);
          //wah_State = 0;        
      } else if (pedal_State == 2) {
          digitalWrite(led5_Pin, HIGH);
          digitalWrite(led6_Pin, LOW);
          send_disable_wah();
          // disable wah
          //Serial1.write(0xB0);
          //Serial1.write(0x05);
          //Serial1.write(0x00);
          //wah_State = 0;        
      } else if (pedal_State == 3) {
          digitalWrite(led5_Pin, LOW);
          digitalWrite(led6_Pin, HIGH);
          // enabe wah
          //Serial1.write(0x90);
          //Serial1.write(0x11);
          //Serial1.write(0x7F);
          //Serial1.write(0xB0);
          //Serial1.write(0x05);
          //Serial1.write(0x7F);
          send_enable_wah();
      }
      #ifdef DEBUG
        Serial1.println("wah_State:");
        Serial1.println(wah_State);
      #endif
    }
  }
  button5_Last = button5_State;
  pedal_Last = pedal_State;
  //end wah/button5

  // update pedal levels
  if (pedal_State > 1 ) {
    pedal_Value = analogRead(pedal_Pin);
    if (pedal_State == 3) {
      #ifdef DEBUG
        Serial1.println("Wah Value:");
        Serial1.println(pedal_Value);
        Serial1.println(pedal_Value, HEX);
      #endif
      send_wah_value(pedal_Value);
        //Serial1.write(0xB0);
        //Serial1.write(0x22);
        //Serial1.write(pedal_Value);
        //Serial1.write(0xB0);
        //Serial1.write(0x06);
        //Serial1.write(pedal_Value);
    } else if (pedal_State == 2) {
      #ifdef DEBUG
       Serial1.println("Volume Value:");
       Serial1.println(pedal_Value);
       Serial1.println(pedal_Value, HEX);
      #endif
      send_volume_value(pedal_Value);
      //Serial1.write(0xB0);
      //Serial1.write(0x24);
      //Serial1.write(pedal_Value);  // value in hex
    }
  }

  // handle mode change button presses
  if (button0_State && !button0_Last ) {
    #ifdef DEBUG
      Serial1.println("Button-0-PRESS");
    #endif
    delay(10);
    mode_State ++;
    if (mode_State > 4) {
      mode_State = 1;
    }
    #ifdef DEBUG
      Serial1.println("mode_State:");
      Serial1.println(mode_State);
    #endif

    if (mode_State != mode_Last ) {
      digitalWrite(led1_Pin, LOW);
      digitalWrite(led2_Pin, LOW);
      digitalWrite(led3_Pin, LOW);
      digitalWrite(led4_Pin, LOW);
      switch (mode_State) {
        case 1:
          //Serial1.println("LED-1");
          digitalWrite(led1_Pin, HIGH);
          break;
        case 2:
          //Serial1.println("LED-2");
          digitalWrite(led2_Pin, HIGH);
          break;
        case 3:
          //Serial1.println("LED-3");
          digitalWrite(led3_Pin, HIGH);
          break;
        case 4:
          //Serial1.println("LED-4");
          digitalWrite(led4_Pin, HIGH);
          break;
        default:
          //Serial1.println("LED-1");
          digitalWrite(led1_Pin, HIGH);
          mode_State = 1;
          break;
      }
    }
  }
  button0_Last = button0_State;
  mode_Last = mode_State;
  //end mode/button0

  //BUTTON 1
  if (button1_State && !button1_Last ) {
    digitalWrite(led7_Pin, HIGH); 
    #ifdef DEBUG
      Serial1.println("Button-1-PRESS");
    #endif
    switch (mode_State) {
      case 1:
        send_bank_up();
        break;
      case 2:
        send_preset_1();
        break;
      case 3:
        send_stomp_onoff();
        break;
      case 4:
        send_loop_playrec();
        break;
    }
    delay(20);
    digitalWrite(led7_Pin, LOW); 
  }
  button1_Last = button1_State;
  
  // BUTTON 2
  if (button2_State && !button2_Last ) {
    #ifdef DEBUG
      Serial1.println("Button-2-PRESS");
    #endif
    switch (mode_State) {
      case 1:
        send_bank_down();
        break;
      case 2:
        send_preset_2();
        break;
      case 3:
        send_effect_onoff();
        break;
      case 4:
        send_loop_stop();
        break;
    }
    delay(10);
    // case do different actions based on current mode_State
  }
  button2_Last = button2_State;
  
  // BUTTON 3
  if (button3_State && !button3_Last ) {
    #ifdef DEBUG
      Serial1.println("Button-3-PRESS");
    #endif
    switch (mode_State) {
      case 1:
        send_boost();
        break;
      case 2:
        send_preset_3();
        break;
      case 3:
        send_delay_onoff();
        break;
      case 4:
        send_loop_erase();
        break;       
    }
    delay(10);
    // case do different actions based on current mode_State
  }
  button3_Last = button3_State;
  
  // BUTTON 4
  if (button4_State && !button4_Last ) {
    #ifdef DEBUG
      Serial1.println("Button-4-PRESS");
    #endif
    switch (mode_State) {
      case 1:
        send_tuner();
        break;
      case 2:
        send_preset_4();
        break;
      case 3:
        send_reverb_onoff();
        break;
      case 4:
        send_taptempo();
        break;        
    }
    delay(10);
    // case do different actions based on current mode_State
  }
  button4_Last = button4_State;
  //
  // end process 4 action buttons


  // adjust delay based on reading wah or not
  if (pedal_State > 1) {
    delay(10);
  } else {
    delay(100);
  }
  // end loop
}

/* 
 *  ####################################################################
 *  End Mail Loop
 *  Begin Function Defintions
 *  ####################################################################
 */
 
void send_enable_wah() {
  Serial1.write(0xB0);
  Serial1.write(0x05);
  Serial1.write(0x7F);
}

void send_disable_wah() {
  Serial1.write(0xB0);
  Serial1.write(0x05);
  Serial1.write(0x00);
}

void send_wah_value(int value) {
  Serial1.write(0xB0);
  Serial1.write(0x22);
  Serial1.write(value);
}
void send_volume_value(int value) {
  Serial1.write(0xB0);
  Serial1.write(0x24);
  Serial1.write(value);
}


void send_bank_down() {

  //Bank Down
  Serial1.write(0x90);
  Serial1.write(0x10);
  Serial1.write(0x7F);
  Serial1.write(0x90);
  Serial1.write(0x10);
  Serial1.write(0x00);

}

void send_bank_up() {

  //Bank Up
  Serial1.write(0x90);
  Serial1.write(0x0E);
  Serial1.write(0x7F);
  Serial1.write(0x90);
  Serial1.write(0x0E);
  Serial1.write(0x00);
  
}

void send_preset_1() {
  //Preset 1
  Serial1.write(0x90);
  Serial1.write(0x05);
  Serial1.write(0x7F);
  Serial1.write(0x90);
  Serial1.write(0x05);
  Serial1.write(0x00);  
}

void send_preset_2() {
  //Preset 2
  Serial1.write(0x90);
  Serial1.write(0x04);
  Serial1.write(0x7F);
  Serial1.write(0x90);
  Serial1.write(0x04);
  Serial1.write(0x00);  
}

void send_preset_3() {
  //Preset 3
  Serial1.write(0x90);
  Serial1.write(0x09);
  Serial1.write(0x7F);
  Serial1.write(0x90);
  Serial1.write(0x09);
  Serial1.write(0x00); 
}

void send_preset_4() {
  //Preset 3
  Serial1.write(0x90);
  Serial1.write(0x07);
  Serial1.write(0x7F);
  Serial1.write(0x90);
  Serial1.write(0x07);
  Serial1.write(0x00); 
}

void send_loop_playrec() {
  Serial1.write(0x90);
  Serial1.write(0x00);
  Serial1.write(0x7F);
  Serial1.write(0x90);
  Serial1.write(0x00);
  Serial1.write(0x00);
  
}

void send_loop_stop() {
  Serial1.write(0x90);
  Serial1.write(0x02);
  Serial1.write(0x7F);
  Serial1.write(0x90);
  Serial1.write(0x02);
  Serial1.write(0x00);
  
}

void send_loop_erase() {
  Serial1.write(0x90);
  Serial1.write(0x02);
  Serial1.write(0x7F);
  delay(5000);
  Serial1.write(0x90);
  Serial1.write(0x02);
  Serial1.write(0x00);
  
}

void send_taptempo() {
  
}

void send_boost() {
  
}

void send_tuner() {
    Serial1.write(0x90);
    Serial1.write(0x15);
    Serial1.write(0x7F);
}

void send_effect_onoff() {
  //Effect on/off
  Serial1.write(0x90); //Note
  Serial1.write(0x0C); //Manual
  Serial1.write(0x7F); //127 (on)
  Serial1.write(0x90); //Note
  Serial1.write(0x0C); //Manual
  Serial1.write(0x00);

  Serial1.write(0x90); //Note
  Serial1.write(0x04); //Effect
  Serial1.write(0x7F); //127 (on)
  Serial1.write(0x90); //Note
  Serial1.write(0x04); //Effect
  Serial1.write(0x00);

  Serial1.write(0x90); //Note
  Serial1.write(0x0C); //Manual
  Serial1.write(0x7F); //127 (on)
  Serial1.write(0x90); //Note
  Serial1.write(0x0C); //Manual
  Serial1.write(0x00);
}

void send_stomp_onoff() {

  //stomp on/off
  Serial1.write(0x90); //Note
  Serial1.write(0x0C); //Manual
  Serial1.write(0x7F); //127 (on)
  Serial1.write(0x90); //Note
  Serial1.write(0x0C); //Manual
  Serial1.write(0x00);

  Serial1.write(0x90); //Note
  Serial1.write(0x05); //Stomp
  Serial1.write(0x7F); //127 (on)
  Serial1.write(0x90); //Note
  Serial1.write(0x05); //Stomp
  Serial1.write(0x00);

  Serial1.write(0x90); //Note
  Serial1.write(0x0C); //Manual
  Serial1.write(0x7F); //127 (on)
  Serial1.write(0x90); //Note
  Serial1.write(0x0C); //Manual
  Serial1.write(0x00);

}


void send_delay_onoff() {
  //Delay on off
  Serial1.write(0x90); //Note
  Serial1.write(0x0C); //Manual
  Serial1.write(0x7F); //127 (on)
  Serial1.write(0x90); //Note
  Serial1.write(0x0C); //Manual
  Serial1.write(0x00);

  Serial1.write(0x90); //Note
  Serial1.write(0x09); //Delay
  Serial1.write(0x7F); //127 (on)
  Serial1.write(0x90); //Note
  Serial1.write(0x09); //Delay
  Serial1.write(0x00);

  Serial1.write(0x90); //Note
  Serial1.write(0x0C); //Manual
  Serial1.write(0x7F); //127 (on)
  Serial1.write(0x90); //Note
  Serial1.write(0x0C); //Manual
  Serial1.write(0x00);
}

void send_reverb_onoff() {
  //reverb on off
  Serial1.write(0x90); //Note
  Serial1.write(0x0C); //Manual
  Serial1.write(0x7F); //127 (on)
  Serial1.write(0x90); //Note
  Serial1.write(0x0C); //Manual
  Serial1.write(0x00);

  Serial1.write(0x90); //Note
  Serial1.write(0x07); //Reverb
  Serial1.write(0x7F); //127 (on)
  Serial1.write(0x90); //Note
  Serial1.write(0x07); //Reverb
  Serial1.write(0x00);

  Serial1.write(0x90); //Note
  Serial1.write(0x0C); //Manual
  Serial1.write(0x7F); //127 (on)
  Serial1.write(0x90); //Note
  Serial1.write(0x0C); //Manual
  Serial1.write(0x00);
}
