const char WIFI_SSID[] = "lab_control";
const char WIFI_PASSWORD[] = "lab_control";

#define CH_ENC_A  18//  WEMOS 26  ESP32S3 18     Channel A of the motor encoder
#define CH_ENC_B  17 // WEMOS 25  ESP32S3 17     Channel B of the motor encoder

const float pulses2degrees = (float) (360.0/2800.0);