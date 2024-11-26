#include <stdlib.h>

// PWM parameters
#define FREQUENCY_PWM   100
#define RESOLUTION_PWM  12

// Motor control
#define PIN_AIN1        25
#define PIN_AIN2        26
#define PIN_BIN1        27
#define PIN_BIN2        14

#define CH_PWM_AIN1     0
#define CH_PWM_AIN2     1

#define  CH_PWM_BIN1    2
#define  CH_PWM_BIN2    3

// Encoder sensing
#define CH_ENC_A1       33
#define CH_ENC_B1       32
#define CH_ENC_A2       34
#define CH_ENC_B2       35

// IMU
#define MPU9250_ADDR 0x68


// Cores configuration
#define CORE_CONTROL    1   // Core 0 is used for control tasks
#define CORE_COMM       0   // Core 1 is used for mqtt communication

// Protection pins for the driver
#define DRIVER_ON       21

// System definitions
#define DEFAULT_REFERENCE 0

/// Sampling time
#define SAMPLING_TIME     0.1
#define BUFFER_SIZE       25

// conversion from volts to 12bits-PWM
const float percent2pwm = (float) (4095.0/5.0);

// conversion from pulses to degrees from a 600 ppr encoder
const float pulses2degrees = (float) (360.0/2800.0);

/// Definitions for dead zone
# define DEAD_ZONE        0