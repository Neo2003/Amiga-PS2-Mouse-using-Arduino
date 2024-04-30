/*
 * *****************************************
 * Amiga mouse-port version 2024 by Rodrik *
 * *****************************************
 * 
 * Mouse port of the Amiga:
 * 9 PIN D-SUB MALE
 * 
 *   _________________________
 *   \                       /
 *    \  1   2   3   4   5  /
 *     \                   /
 *      \  6   7   8   9  /
 *       \_______________/
 * 
 * 1  V-pulse               
 * 2  H-pulse
 * 3  VQ-pulse
 * 4  HQ-pulse
 * 5  BUTTON 3 (Middle)
 * 6  BUTTON 1 (Lefl)
 * 7  +5V
 * 8  GND
 * 9  BUTTON 2 (Right)
 * 
 */

#include "PS2Mouse.h"
#include "MiniTimer.h"

#define CLOCK_PIN 11    // D11 from PS2
#define DATA_PIN  12    // D12 from PS2

#define V_PULSE    2    // D2 to Amiga
#define H_PULSE    3    // D3 to Amiga
#define VQ_PLSE    4    // D4 to Amiga
#define HQ_PLSE    5    // D5 to Amiga

#define LMB        6    // D6 (Left Mouse Button) to Amiga
#define RMB        7    // D7 (Right Mouse Button) to Amuga
#define MMB        8    // D8 (Middle Mouse Button) to Amiga

#define SAMPLING   480  // Size of sampling to complete it in 25ms (PS/2 sampling rate at 40/sec)

uint8_t H[4]  = { LOW, LOW, HIGH, HIGH };
uint8_t HQ[4] = { LOW, HIGH, HIGH, LOW };

uint8_t QX = 3;
uint8_t QY = 3;
bool LeftButtonActive = false;
bool RightButtonActive = false;
bool MiddleButtonActive = false;
bool HasWord = true;
uint8_t XSTEPS;
uint8_t YSTEPS;
uint8_t XSIGN;
uint8_t YSIGN;

enum _state_t {STATE_HALT, STATE_RUN};
MiniTimer timer;
_state_t state;

PS2Mouse mouse(CLOCK_PIN, DATA_PIN);

void LeftButtonUp() {
    digitalWrite(LMB, HIGH);
}
void LeftButtonDown() {
    digitalWrite(LMB, LOW);
}
void RightButtonUp() {
    digitalWrite(RMB, HIGH);
}
void RightButtonDown() {
    digitalWrite(RMB, LOW);
}
void MiddleButtonUp() {
    digitalWrite(MMB, HIGH);
}
void MiddleButtonDown() {
    digitalWrite(MMB, LOW);
}

// X/Y moves - Set pins according to current position in quadrature sequence
void AMIGAHorizontalMove() {
    digitalWrite(H_PULSE, H[QX]);
    digitalWrite(HQ_PLSE, HQ[QX]);
}
void AMIGAVerticalMove() {
    digitalWrite(V_PULSE, H[QY]);
    digitalWrite(VQ_PLSE, HQ[QY]);
}

// Call the move and update the position in quadrature sequence in proper direction
void AMIGA_Left() {
    AMIGAHorizontalMove();
    QX = (QX >= 3) ? 0 : ++QX;
}
void AMIGA_Right() {
    AMIGAHorizontalMove();
    QX = (QX <= 0) ? 3 : --QX;
}
void AMIGA_Down() {
    AMIGAVerticalMove();
    QY = QY <= 0 ? 3 : --QY;
}
void AMIGA_Up() {
    AMIGAVerticalMove();
    QY = QY >= 3 ? 0 : ++QY;
}

void activate_pins() {
    // Set button and quadrature output pins to output
    pinMode(V_PULSE, OUTPUT);  // V-Pulse
    pinMode(H_PULSE, OUTPUT);  // H-Pulse
    pinMode(VQ_PLSE, OUTPUT);  // VQ-pulse
    pinMode(HQ_PLSE, OUTPUT);  // HQ-pulse
    pinMode(LMB, OUTPUT);  // LMB
    pinMode(RMB, OUTPUT);  // RMB
    pinMode(MMB, OUTPUT);  // MMB

    // Set quadrature output pins to low
    digitalWrite(V_PULSE, LOW);
    digitalWrite(H_PULSE, LOW);
    digitalWrite(VQ_PLSE, LOW);
    digitalWrite(HQ_PLSE, LOW); 

    // Set mouse button output pins to high, they are inverted
    digitalWrite(LMB, HIGH);
    digitalWrite(RMB, HIGH);
    digitalWrite(MMB, HIGH);
    state = STATE_RUN;

    delay(50);
}

void free_pins() {
    pinMode(V_PULSE, INPUT);  // V-Pulse
    pinMode(H_PULSE, INPUT);  // H-Pulse
    pinMode(VQ_PLSE, INPUT);  // VQ-pulse
    pinMode(HQ_PLSE, INPUT);  // HQ-pulse
    pinMode(LMB, INPUT);  // LMB
    pinMode(RMB, INPUT);  // RMB
    pinMode(MMB, INPUT);  // MMB
    digitalWrite(V_PULSE, LOW);
    digitalWrite(H_PULSE, LOW);
    digitalWrite(VQ_PLSE, LOW);
    digitalWrite(HQ_PLSE, LOW);
    // Nothing moved so button are all high already, will be INPUT_PULLUP
    state = STATE_HALT;
    delay(50);
}

void setup() {
    Serial.begin(57600); // Just setup the communication if you ever need it
    mouse.initialize();
    Serial.println("Hello from PS2 to Amiga mouse adapter by Rodrik");
    activate_pins();
    timer.start(); 
}

void loop() {
    MouseData data = mouse.readData();  // Get the data from PS2 1/40 sec = 25 ms
    uint32_t temps = micros();
    bool activity = data.status & 1 || data.status & 2 || data.status & 4 || data.position.x != 0 || data.position.y != 0;
    if (state == STATE_HALT && activity) {
        activate_pins();
        timer.start(); 					// this resets the timer
    } else {
        //Serial.println(timer.read());
        if (state == STATE_RUN && timer.read() > 5000) {
            timer.stop();
            free_pins();	
        }
    }
    if (activity)
        timer.start();
    if (state == STATE_HALT) {
        //Serial.println("Halted");
        delay(24);
        return;
    }
    if (data.status & 1) {              // Let's remember to not toggle a pin already properly set
        if (!LeftButtonActive) {
            LeftButtonDown();
            LeftButtonActive = true;
            //Serial.print(data.status, BIN);
            //Serial.println("\tLeft Button down");
        }
    } else if (LeftButtonActive) {
        LeftButtonUp();
        LeftButtonActive = false;
        //Serial.print(data.status, BIN);
        //Serial.println("\tLeft Button up");
    }
    if (data.status & 2) {
        if (!RightButtonActive) {
            RightButtonDown();
            RightButtonActive = true;
        }
    } else if (RightButtonActive) {
        RightButtonUp();
        RightButtonActive = false;
    }
    if (data.status & 4) {
        if (!MiddleButtonActive) {
            MiddleButtonDown();
            MiddleButtonActive = true;
        }
    } else if (MiddleButtonActive){
        MiddleButtonUp();
        MiddleButtonActive = false;
    }
    if (data.position.x != 0 || data.position.y != 0) {
        XSTEPS = abs(data.position.x);
        YSTEPS = abs(data.position.y);
        XSIGN = (data.position.x > 0 ? 1 : 0) ;
        YSIGN = (data.position.y > 0 ? 1 : 0) ;

        float dx = (XSTEPS > 0 ? SAMPLING / XSTEPS : 0); // Divide the array by number of pulses
        float dy = (YSTEPS > 0 ? SAMPLING / YSTEPS : 0);

        float valx = dx;
        float valy = dy;

        for (int i=1;i<=SAMPLING;++i) { // Loop into the array
            if (int(valx)==i) {         // and progress the quadrature if we are on a divider
                // play quadrature
                if (XSIGN)
                    AMIGA_Right();
                else
                    AMIGA_Left();
                valx+=dx;               // Next pulse
            } else {
                delayMicroseconds(20);  // No pulse, then we need to compensate the not spent 20 micro seconds 
            }
    
            if (int(valy)==i) {         // Same for Y
                // play quadrature
                if (YSIGN)
                    AMIGA_Up();
                else
                    AMIGA_Down();
                valy+=dy;
            } else {
                delayMicroseconds(20);
            }
        }
    }
    else
        delay(24); // No movement, then compensate the 24 ms not spent
    Serial.print("Time taken : ");
    Serial.println(micros()-temps);
}
