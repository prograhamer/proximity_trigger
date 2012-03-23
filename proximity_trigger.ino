#include <ir_remote.h>

// Sample frequency in usec
#define SAMPLE_FREQ 1000
// Number of samples required to trigger output
#define N_SAMPLES 100

// Maximum shot frequency in ms
#define SHOT_FREQ 5000

// Distance sensor attached to analogue input A0
#define PIN_SENSOR    (A0)
// Use digital pin 13 as an indicator for whether the sensor is triggered
#define PIN_INDICATOR (13)

// Count of successive samples for which there has been a trigger signal
uint16_t sample_i;

// Lower and upper bound to determine whether the input from the sensor should
// trigger the shutter or not
#define LOWER_BOUND (80)
#define UPPER_BOUND (300)

// Last trigger time, stored to enable imposing a limit on shutter activation
// frequency
unsigned long last_trigger_time;

// State variable indicating whether the sensor is triggered
uint8_t state = 0;

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
      state = 1;
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

    if( state )
    {
      digitalWrite( PIN_INDICATOR, LOW );
      state = 0;
    }
  }

  delayMicroseconds( SAMPLE_FREQ );
}
