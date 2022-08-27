// include the ResponsiveAnalogRead library
#include <ResponsiveAnalogRead.h>
#include <Button2.h>
#include <HardwareSerial.h>

#define N_POTS    3
#define N_BUTTONS 3
#define RX        16
#define TX        17

// define the pin you want to use
const int POT_PIN[] = { 35, 32, 33 };
const int BUTTON_PIN[] = { 12, 14, 27 };


//ResponsiveAnalogRead analog(ANALOG_PIN, true);
ResponsiveAnalogRead *analog[N_POTS];
Button2 *button[N_BUTTONS];
HardwareSerial SerialPort(2);


// the next optional argument is snapMultiplier, which is set to 0.01 by default
// you can pass it a value from 0 to 1 that controls the amount of easing
// increase this to lessen the amount of easing (such as 0.1) and make the responsive values more responsive
// but doing so may cause more noise to seep through if sleep is not enabled

void setup() {
  //Serial.begin(115200);
  SerialPort.begin(115200, SERIAL_8N1, RX, TX);

  for (int i=0; i < N_POTS; i++) {
    analog[i] = new ResponsiveAnalogRead(POT_PIN[i], true);
    analog[i]->setAnalogResolution(4096);
    analog[i]->setActivityThreshold(32.0);   
  }

  for (int j=0; j < N_BUTTONS; j++) {
    button[j] = new Button2();
    button[j]->begin(BUTTON_PIN[j]);
    button[j]->setPressedHandler(pressed);
    button[j]->setReleasedHandler(released);
  }
}

void loop() {
  for (int j=0; j < N_BUTTONS; j++) {
    button[j]->loop();
  }
  
  for (int i=0; i < N_POTS; i++) {
    analog[i]->update(); 
    
    if(analog[i]->hasChanged()) {
    
      float value = (float)(analog[i]->getValue() >> 2);
      SerialPort.print("/pot_");
      SerialPort.print(i);
      SerialPort.print(" ");
      SerialPort.println(value / 1024);
    } 
  }
  
  
  //Serial.println("");
  delay(50);
}

void pressed(Button2& btn) {
    int but = 10;
    
    for (int i=0; i < N_BUTTONS; i++) {
      if (btn == *button[i]) {
        but = i;
      }
    }

    send_msg(but, 1);
}

void released(Button2& btn) {
    //Serial.print("released: ");
    //Serial.println(btn.wasPressedFor());
    int but = 10;
    
    for (int i=0; i < N_BUTTONS; i++) {
      if (btn == *button[i]) {
        but = i;
      }
    }
    
    send_msg(but, 0);
}

void send_msg(int n_button, int value) {
    SerialPort.print("/button_");
    SerialPort.print(n_button);
    SerialPort.print(" ");
    SerialPort.println(value);  
}
