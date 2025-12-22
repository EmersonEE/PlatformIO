#ifndef __GETDATE_H__
#define __GETDATE_H__

#include <Arduino.h>
#include <time.h>
#include <stdio.h>
#include <string> 

class GetDate
{
private:
    /* data */
public:
    GetDate(/* args */);
    String getDateLT(long long timestamp_ms);
    long long stoll (const String& str, size_t* idx = nullptr, int base = 10);
    
    void init();
};

#endif // __GETDATE_H__