/*
  Peavy Vypyr Midi Pedal - 11/24/2019 nixfu
  Mode 1   Bank up          Bank Down     Boost           Tuner
  Mode 2   Preset 1         Preset 2      Preset 3        Preset 4
  Mode 3   Effect on/off    Delay on/off  Reverb on/off   Stomp on/Off
  Mode 4   Record/Play      Stop          Erase   
*/

//#define DEBUG

// pin assignments for STM32 Bluepill
// digital pins
#define buttonModePin   PB12
#define button1_Pin     PB3
#define button2_Pin     PB4
#define button3_Pin     PB5
#define button4_Pin     PB6
#define buttonPedal_Pin PB13
#define ledMode1_Pin    PB11
#define ledMode2_Pin    PB10    
#define ledMode3_Pin    PB1 
#define ledMode4_Pin    PB0
#define ledPedal1_Pin   PA7
#define ledPedal2_Pin   PA6
#define ledButton1_Pin  PA5
#define ledButton2_Pin  PA4
#define ledButton3_Pin  PA3
#define ledButton4_Pin  PA2
// adc / analog pins
#define pedal_Pin       PA0

// variables
int buttonModeState = HIGH;
int button1_State = HIGH;
int button2_State = HIGH;
int button3_State = HIGH;
int button4_State = HIGH;
int buttonPedal_State = HIGH;
int buttonModeLast = HIGH;
int button1_Last = HIGH;
int button2_Last = HIGH;
int button3_Last = HIGH;
int button4_Last = HIGH;
int buttonPedal_Last = HIGH;
int mode_State = 1;
int mode_Last = 1;
int pedal_State = 1;
int pedal_Last = 1;
int pedal_Value = 0;
int tuner_State = 0;
int tuner_Last = 0;


void setup() {
  // initialize the LED pins as an output:
  pinMode(ledMode1_Pin, OUTPUT);
  pinMode(ledMode2_Pin, OUTPUT);
  pinMode(ledMode3_Pin, OUTPUT);
  pinMode(ledMode4_Pin, OUTPUT);
  pinMode(ledPedal1_Pin, OUTPUT);
  pinMode(ledPedal2_Pin, OUTPUT);
  pinMode(ledButton1_Pin, OUTPUT);
  pinMode(ledButton2_Pin, OUTPUT);
  pinMode(ledButton3_Pin, OUTPUT);
  pinMode(ledButton4_Pin, OUTPUT);

  // initialize the pushbutton pins as an input:
  pinMode(buttonModePin, INPUT);
  pinMode(button1_Pin, INPUT);
  pinMode(button2_Pin, INPUT);
  pinMode(button3_Pin, INPUT);
  pinMode(button4_Pin, INPUT);
  pinMode(buttonPedal_Pin, INPUT);

  #ifdef DEBUG
    // open serial debug output
    Serial1.begin(115200);
  #else
    // open serial MIDI output
    Serial1.begin(31250);   //MIDI communicates at 31250 baud
    // Set MIDI program 00
    Serial1.write(0xC0); // Program change
    Serial1.write(0x00); //  00
  #endif

  // turnon ledMode1 and  turn off others
  digitalWrite(ledMode1_Pin, HIGH);
  digitalWrite(ledMode2_Pin, LOW);
  digitalWrite(ledMode3_Pin, LOW);
  digitalWrite(ledMode4_Pin, LOW);
  digitalWrite(ledPedal1_Pin, LOW);
  digitalWrite(ledPedal2_Pin, LOW);

  digitalWrite(ledButton1_Pin, LOW);
  digitalWrite(ledButton2_Pin, LOW);
  digitalWrite(ledButton3_Pin, LOW);
  digitalWrite(ledButton4_Pin, LOW);

  pinMode(pedal_Pin, INPUT); // adc pedal pin
  analogReadResolution(7);   // midi can only use 7bits/0-127
}

void loop() {

  // read the state of the pushbutton value:
  buttonModeState = digitalRead(buttonModePin);
  button1_State = digitalRead(button1_Pin);
  button2_State = digitalRead(button2_Pin);
  button3_State = digitalRead(button3_Pin);
  button4_State = digitalRead(button4_Pin);
  buttonPedal_State = digitalRead(buttonPedal_Pin);

  // handle wah button presses
  if (buttonPedal_State && !buttonPedal_Last ) {
    pedal_State ++;
    if (pedal_State > 3) {
      pedal_State = 1;
    }
    if (pedal_State != pedal_Last ) {
      switch (pedal_State) {
        case 1:
          digitalWrite(ledPedal1_Pin, LOW);
          digitalWrite(ledPedal2_Pin, LOW);
          send_disable_wah();
          break;
        case 2:
          digitalWrite(ledPedal1_Pin, HIGH);
          digitalWrite(ledPedal2_Pin, LOW);
          send_disable_wah();
          break;
        case 3:
          digitalWrite(ledPedal1_Pin, LOW);
          digitalWrite(ledPedal2_Pin, HIGH);
          send_enable_wah();
          break;
      }
    }
  }
  buttonPedal_Last = buttonPedal_State;
  pedal_Last = pedal_State;
  //end wah/vol button

  // update pedal levels if enabled
  if (pedal_State > 1 ) {
    pedal_Value = analogRead(pedal_Pin);
    if (pedal_State == 3) {
      send_wah_value(pedal_Value);
    } else if (pedal_State == 2) {
      send_volume_value(pedal_Value);
    }
  }

  // handle mode change button presses
  if (buttonModeState && !buttonModeLast ) {
    mode_State ++;
    if (mode_State > 4) {
      mode_State = 1;
    }
    if (mode_State != mode_Last ) {
      digitalWrite(ledMode1_Pin, LOW);
      digitalWrite(ledMode2_Pin, LOW);
      digitalWrite(ledMode3_Pin, LOW);
      digitalWrite(ledMode4_Pin, LOW);
      switch (mode_State) {
        case 1:
          digitalWrite(ledMode1_Pin, HIGH);
          break;
        case 2:
          digitalWrite(ledMode2_Pin, HIGH);
          break;
        case 3:
          digitalWrite(ledMode3_Pin, HIGH);
          break;
        case 4:
          digitalWrite(ledMode4_Pin, HIGH);
          break;
        default:
          digitalWrite(ledMode1_Pin, HIGH);
          mode_State = 1;
          break;
      }
    }
  }
  buttonModeLast = buttonModeState;
  mode_Last = mode_State;
  //end mode/button0

  //BUTTON 1
  if (button1_State && !button1_Last ) {
    digitalWrite(ledButton1_Pin, HIGH); 
    switch (mode_State) {
      case 1:
        send_bank_up();
        break;
      case 2:
        send_preset_1();
        break;
      case 3:
        send_effect_onoff();
        break;
      case 4:
        send_loop_playrec();
        break;
    }
    delay(20);
    digitalWrite(ledButton1_Pin, LOW); 
  }
  button1_Last = button1_State;
  
  // BUTTON 2
  if (button2_State && !button2_Last ) {
    digitalWrite(ledButton2_Pin, HIGH); 
    switch (mode_State) {
      case 1:
        send_bank_down();
        break;
      case 2:
        send_preset_2();
        break;
      case 3:
        send_delay_onoff();        
        break;
      case 4:
        send_loop_stop();
        break;
    }
    delay(20);
    digitalWrite(ledButton2_Pin, LOW); 
  }
  button2_Last = button2_State;
  
  // BUTTON 3
  if (button3_State && !button3_Last ) {
    digitalWrite(ledButton3_Pin, HIGH);         
    switch (mode_State) {
      case 1:
        break;
      case 2:
        send_preset_3();
        break;
      case 3:
        send_reverb_onoff();
        break;
      case 4:
        send_loop_erase();
        break;       
    }
    delay(20);
    digitalWrite(ledButton3_Pin, LOW); 
    // case do different actions based on current mode_State
  }
  button3_Last = button3_State;
  
  // BUTTON 4
  if (button4_State && !button4_Last ) {
    digitalWrite(ledButton4_Pin, HIGH);     
    switch (mode_State) {
      case 1:
        send_tuner();
        break;
      case 2:
        send_preset_4();
        break;
      case 3:      
        send_stomp_onoff();
        break;
      case 4:
        break;        
    }
    delay(20);
    digitalWrite(ledButton4_Pin, LOW); 
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
