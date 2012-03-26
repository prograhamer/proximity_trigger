#include <ir_remote.h>

// Sample frequency in ms
#define SAMPLE_FREQ 38
// Number of samples within range required to trigger output
#define N_SAMPLES 13

// Maximum shot frequency in ms
#define SHOT_FREQ 5000

// Distance sensor attached to analogue input A0
#define PIN_SENSOR    (A0)
// Use digital pin 13 as an indicator for whether the sensor is triggered
#define PIN_INDICATOR (13)

// Count of successive samples for which there has been a trigger signal
uint16_t sample_i = 0;

// Lower and upper bound to determine whether the input from the sensor should
// trigger the shutter or not
#define LOWER_BOUND (60)
#define UPPER_BOUND (300)

// Last trigger time, stored to enable imposing a limit on shutter activation
// frequency
unsigned long last_trigger_time;

// Infrared remote control interface object used to trigger the camera
IrRemote remote;

// Timings for the IR remote trigger command for Pentax SLRs
const int pentax_exp[][2] =
  { { 13000, 3000 },
    { 1000, 1000 },
    { 1000, 1000 },
    { 1000, 1000 },
    { 1000, 1000 },
    { 1000, 1000 },
    { 1000, 1000 },
    { 1000, 0 } };

void setup()
{
  delayMicroseconds( 43000 );

  pinMode( PIN_SENSOR, INPUT );
  pinMode( PIN_INDICATOR, OUTPUT );

  digitalWrite( PIN_INDICATOR, LOW );

  // Initialise IR remote control interface at 38KHz
  remote.initialise(38);
}

void loop()
{
  int sensor_value;
  
  sensor_value = analogRead( PIN_SENSOR );

  if( sensor_value >= LOWER_BOUND &&
      sensor_value <= UPPER_BOUND )
  {
    if( sample_i < N_SAMPLES )
      sample_i++;
    else
    {
      sample_i = 0;

      digitalWrite( PIN_INDICATOR, HIGH );

      if( millis() - last_trigger_time >= SHOT_FREQ )
      {
        last_trigger_time = millis();
        
        remote.sendCommand( pentax_exp );
      }
    }
  }
  else
  {
    sample_i = 0;

    digitalWrite( PIN_INDICATOR, LOW );
  }

  delay( SAMPLE_FREQ );
}
