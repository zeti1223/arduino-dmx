#include <DmxSimple.h>
#include <LiquidCrystal_I2C.h> 

#define CLK 2
#define DT 3
#define SW 4
int counter = 0;
int mode2 = 0;
int currentStateCLK;
int lastStateCLK;
int ch = 1;
int val = 0;
unsigned long lastButtonPress = 0;
bool mode = true;
bool extendedMode = false;
int previousVal = val;
int egyseg = 1;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
    pinMode(CLK, INPUT);
    pinMode(DT, INPUT);
    pinMode(SW, INPUT_PULLUP);
    DmxSimple.usePin(5);
    DmxSimple.maxChannel(512);
    Serial.begin(9600);
    lastStateCLK = digitalRead(CLK);
    lcd.init(); 
    lcd.backlight(); 
    lcd.setCursor(0, 0); 
    lcd.print("Starting"); 
    delay(1000); 
    lcd.clear(); 
    lcd.setCursor(0, 0); 
}

void loop() {
    encoder();
    screen();
    dmx();
    delay(1);
}

void encoder() {
    int btnState = digitalRead(SW);
    static unsigned long buttonPressStartTime = 0;

    if (btnState == LOW) {
        if (buttonPressStartTime == 0) {
            buttonPressStartTime = millis();
        }

        if (millis() - buttonPressStartTime > 1000) { 
            extendedMode = !extendedMode;
            Serial.println(extendedMode ? "Extended Mode activated" : "Normal Mode activated");
            buttonPressStartTime = 0; 
        } else if (millis() - lastButtonPress > 50 && mode2 != 2) {
            mode = !mode;
            if (mode) {
                mode2 = extendedMode ? 2 : 1;
            } else {
                mode2 = 0;
            }
        }
    } else {
        buttonPressStartTime = 0;
    }

    lastButtonPress = millis();

    if (mode2 == 0) {
        currentStateCLK = digitalRead(CLK);
        if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {
            val = 0;
            if (digitalRead(DT) != currentStateCLK) {
                ch--;
            } else {
                ch++;
            }
            if (ch < 1) {
                ch = 1;
            } else if (ch > 512) {
                ch = 512;
            }
        }
        lastStateCLK = currentStateCLK;
    } else if (mode2 == 1) {
        currentStateCLK = digitalRead(CLK);
        if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {
            if (digitalRead(DT) != currentStateCLK) {
                val -= egyseg;
            } else {
                val += egyseg;
            }
            if (val < 0) {
                val = 0;
            } else if (val > 255) {
                val = 255;
            }
        }
        lastStateCLK = currentStateCLK;
    } else if (mode2 == 2) {
        currentStateCLK = digitalRead(CLK);
        if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {
            if (digitalRead(DT) != currentStateCLK) {
                egyseg--;
            } else {
                egyseg++;
            }
            if (egyseg < 1) {
                egyseg = 1;
            }
        }
        lastStateCLK = currentStateCLK;
    }
}

void dmx() {
    if (val != previousVal) {
        Serial.print("Channel: ");
        Serial.print(ch);
        Serial.print(" Value: ");
        Serial.println(val);
        DmxSimple.write(ch, val);
        previousVal = val;
    }
}

void screen() {
    if (mode2 < 2)
    {
        lcd.setCursor(0, 0);
        lcd.print("Channel: ");
        lcd.print(ch);
        lcd.setCursor(0, 1);
        lcd.print("Value: ");
        lcd.print(val); 
    }
    else
    {
    lcd.setCursor(0, 0);
    lcd.print("Egyseg: ");
    lcd.print(egyseg);
    }
}