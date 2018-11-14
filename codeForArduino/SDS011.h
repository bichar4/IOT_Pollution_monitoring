#pragma once
#include <Arduino.h>
#include <SoftwareSerial.h>




class SDS011 {
  public:
    SDS011(void);
    void begin(uint8_t pin_rx, uint8_t pin_tx);
    void begin(HardwareSerial* serial);
    void begin(SoftwareSerial* serial);
    int read(float *p25, float *p10);
    void sleep();
    void wakeup();
  private:
    uint8_t _pin_rx, _pin_tx;
    Stream *sds_data;
};
