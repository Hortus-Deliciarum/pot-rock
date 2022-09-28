// include the ResponsiveAnalogRead library
#include <ArduinoOSCWiFi.h>
#include <Button2.h>
//#include <HardwareSerial.h>
#include <ResponsiveAnalogRead.h>

#define DEBUG 0
#define POTROCK 4
#define N_POTS POTROCK
#define N_BUTTONS POTROCK
#define RX 16
#define TX 17
#define POT0 33
#define POT1 32
#define POT2 35
#define POT3 34
#define BUTTON0 12
#define BUTTON1 14
#define BUTTON2 27
#define BUTTON3 26

const char* ssid = "HORTUS";
const char* pwd = "francesco";
const IPAddress ip(192, 168, 1, 86);
const IPAddress gateway(192, 168, 0, 1);
const IPAddress subnet(255, 255, 255, 0);

const char* host = "192.168.1.85";
const int recv_port = 54321;
const int bind_port = 54345;
const int send_port = 55555;
const int publish_port = 54445;

#if POTROCK == 4
const int POT_PIN[] = { POT0, POT1, POT2, POT3 };
const int BUTTON_PIN[] = { BUTTON0, BUTTON1, BUTTON2, BUTTON3 };
#elif POTROCK == 3
// const int POT_PIN[] = { POT0, POT1, POT2  };
// const int BUTTON_PIN[] = { BUTTON2, BUTTON1, BUTTON0 };
const int POT_PIN[] = { POT2, POT1, POT0 };
const int BUTTON_PIN[] = { BUTTON0, BUTTON1, BUTTON2 };
#endif

// ResponsiveAnalogRead analog(ANALOG_PIN, true);
ResponsiveAnalogRead* analog[N_POTS];
Button2* button[N_BUTTONS];
// HardwareSerial SerialPort(2);

// the next optional argument is snapMultiplier, which is set to 0.01 by default
// you can pass it a value from 0 to 1 that controls the amount of easing
// increase this to lessen the amount of easing (such as 0.1) and make the responsive values more responsive
// but doing so may cause more noise to seep through if sleep is not enabled

void setup()
{
#if DEBUG
    Serial.begin(115200);
#endif
#ifdef ESP_PLATFORM
    WiFi.disconnect(true, true); // disable wifi, erase ap info
    delay(1000);
    WiFi.mode(WIFI_STA);
#endif
    WiFi.begin(ssid, pwd);
    WiFi.config(ip, gateway, subnet);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.print("WiFi connected, IP = ");
    Serial.println(WiFi.localIP());

    // SerialPort.begin(115200, SERIAL_8N1, RX, TX);

    for (int i = 0; i < N_POTS; i++) {
        analog[i] = new ResponsiveAnalogRead(POT_PIN[i], true);
        analog[i]->setAnalogResolution(4096);
        analog[i]->setActivityThreshold(32.0);
    }

    for (int j = 0; j < N_BUTTONS; j++) {
        button[j] = new Button2();
        button[j]->begin(BUTTON_PIN[j]);
        button[j]->setPressedHandler(pressed);
        button[j]->setReleasedHandler(released);
    }
}

void loop()
{
    for (int j = 0; j < N_BUTTONS; j++) {
        button[j]->loop();
    }

    for (int i = 0; i < N_POTS; i++) {
        analog[i]->update();

        if (analog[i]->hasChanged()) {

            float value = (float)(analog[i]->getValue() >> 2);

#if DEBUG
            Serial.print("/pot_");
            Serial.print(i);
            Serial.print(" ");
            Serial.println(value / 1024);
#endif
            /*
            SerialPort.print("/pot_");
            SerialPort.print(i);
            SerialPort.print(" ");
            SerialPort.println(value / 1024);
            */
            float cur = value / 1024;
            String prefix = "/pot_";
            String postfix = prefix + i;
            OscWiFi.send(host, send_port, postfix, cur);
        }
    }

    // Serial.println("");
    delay(50);
}

void pressed(Button2& btn)
{
    int but = 10;

    for (int i = 0; i < N_BUTTONS; i++) {
        if (btn == *button[i]) {
            but = i;
        }
    }

    send_msg(but, 1);
}

void released(Button2& btn)
{
    // Serial.print("released: ");
    // Serial.println(btn.wasPressedFor());
    int but = 10;

    for (int i = 0; i < N_BUTTONS; i++) {
        if (btn == *button[i]) {
            but = i;
        }
    }

    send_msg(but, 0);
}

void send_msg(int n_button, int value)
{
#if DEBUG
    Serial.print("/button_");
    Serial.print(n_button);
    Serial.print(" ");
    Serial.println(value);
#endif
    /*
    SerialPort.print("/button_");
    SerialPort.print(n_button);
    SerialPort.print(" ");
    SerialPort.println(value);
    */
    String prefix = "/button_";
    String postfix = prefix + n_button;
    OscWiFi.send(host, send_port, postfix, value);
}
