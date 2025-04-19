#ifndef FILTERS_H
#define FILTERS_H

#include "Arduino.h"

class Filters {
  private:
    float EMA_ALPHA;
    int EMA_LP;
    int EMA_HP;
    
  public:
    Filters(float alpha = 0.6);
    int EMALowPassFilter(int value);
    int EMAHighPassFilter(int value);
};

#endif
