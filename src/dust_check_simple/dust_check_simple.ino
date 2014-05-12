/*
# Smart Air Control #

Program by Iasc CHEN，April 2014

## Grove Dust Sensor ##

Use Shinyei Model PPD42NS Particle Sensor
http://www.sca-shinyei.com/pdf/PPD42NS.pdf

JST Pin 1 (Black Wire)  => Arduino GND
JST Pin 3 (Red wire)    => Arduino 5VDC
JST Pin 4 (Yellow wire) => Arduino Digital Pin 8
*/

#include <Bridge.h>

// dust check
int DUST_PIN = 8;

unsigned long DUST_SAMPLE_MS = 15000;//sampe 15s ;

unsigned long duration, starttime, now, lowpulseoccupancy = 0;
float ratio = 0, concentration = 0;

void setup() {
    Bridge.begin();
    Console.begin();
    // Serial.begin(9600);

    pinMode(DUST_PIN, INPUT);
    starttime = millis(); //get the current time;
    
    // while(!Console);
    Console.println(" Time , PM ");
}

void loop() {
    duration = pulseIn(DUST_PIN, LOW);
    lowpulseoccupancy = lowpulseoccupancy + duration;
    now = millis();

    if ((now - starttime) > DUST_SAMPLE_MS) {
        ratio = lowpulseoccupancy / (DUST_SAMPLE_MS * 10.0);  // Integer percentage 0=>100
        concentration = 1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62; // using spec sheet curve

        Console.print(now/1000);
        Console.print(",");
        Console.println(int(concentration/100));

        lowpulseoccupancy = 0;
        starttime = millis();
    }
}

