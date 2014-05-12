/*
# Smart Air Control #
  
Program by Iasc CHEN，April 2014

## Grove Dust Sensor ##
  
Use Shinyei Model PPD42NS Particle Sensor
http://www.sca-shinyei.com/pdf/PPD42NS.pdf
  
JST Pin 1 (Black Wire)  => Arduino GND
JST Pin 3 (Red wire)    => Arduino 5VDC
JST Pin 4 (Yellow wire) => Arduino Digital Pin 8

## Relay ##
  
Relay Pin 1 (Black Wire)=> Arduino GND
Relay Pin 2 (Red wire)  => Arduino 5VDC
Relay Pin 3 (Green wire)=> Arduino Digital Pin 4 
*/

#include <Bridge.h>
#include <FileIO.h>

float RELAY_ON_VALUE  = 7000;
float RELAY_OFF_VALUE = 2000;

// dust check
int DUST_PIN = 8;

unsigned long DUST_SAMPLE_MS = 15000;//sampe 15s ;

unsigned long duration, starttime, lowpulseoccupancy = 0;
float ratio = 0, concentration = 0;

// relay control
int RELAY_PIN = 4;
unsigned long MAX_RELAY_RUNNING_MS = 1200000;// If after 1200s the air filter are still running, send a warning;

bool relay_on = false;
unsigned long now, relay_start_time, relay_running_time = 0;

void uploadScript() {
    File script = FileSystem.open("/mnt/sda1/workspaces/dust_check/dust_log.sh", FILE_WRITE);
    script.print("#!/bin/sh\n");
    script.print("echo $(date +'%Y-%m-%d %H:%M:%S') , $1 >> /mnt/sda1/workspaces/dust_check/logs/dust.log");
    script.close();
  
    Process chmod;
    chmod.begin("chmod");      // chmod: change mode
    chmod.addParameter("+x");  // x stays for executable
    chmod.addParameter("/mnt/sda1/workspaces/dust_check/dust_log.sh");
    chmod.run();
}

void runLogScript(String msg){
    Process logscript;
    logscript.begin("/mnt/sda1/workspaces/dust_check/dust_log.sh");
    logscript.addParameter(msg);
    logscript.run();
}

void setup() {
    Bridge.begin();
    //Serial.begin(9600);

    pinMode(DUST_PIN,INPUT);
    starttime = millis(); //get the current time;
  
    pinMode(RELAY_PIN, OUTPUT);
    relay_start_time = millis();
  
    FileSystem.begin();
    uploadScript() ;
    runLogScript("Time , PM , Status , Running");
    
    Console.begin();
}

void loop() {
    duration = pulseIn(DUST_PIN, LOW);
    lowpulseoccupancy = lowpulseoccupancy + duration;
    now = millis();

    if ((now - starttime) > DUST_SAMPLE_MS){    //if the sampel time == 30s

        ratio = lowpulseoccupancy / (DUST_SAMPLE_MS * 10.0);  // Integer percentage 0=>100
        concentration = 1.1 * pow(ratio,3) - 3.8 * pow(ratio,2) + 520 * ratio + 0.62; // using spec sheet curve
    
        if (concentration < RELAY_OFF_VALUE){
            if(relay_on){
                digitalWrite(RELAY_PIN, LOW);
                relay_on = false;
            }
        } else if (concentration > RELAY_ON_VALUE){
            if(! relay_on){
                digitalWrite(RELAY_PIN, HIGH);
                relay_on = true;

                relay_start_time = millis();
            }
        }
    
        if(relay_on){
            relay_running_time = now - relay_start_time ;
        }
        else{
            relay_running_time = 0 ;
        }
        
        String output = String(int(concentration/100), DEC) 
            + " , " + String(relay_on)
            + " , " + String(relay_running_time / 1000, DEC); 
        Console.println(output);
        runLogScript(output);
        
        lowpulseoccupancy = 0;
        starttime = millis();
    }
}
