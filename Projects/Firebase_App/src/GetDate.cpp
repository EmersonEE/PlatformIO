#include <GetDate.h>

GetDate::GetDate(/* args */)
{
}

void GetDate::init()
{
    setenv("TZ", "CST6", 1);
    tzset();
}

String GetDate::getDateLT(long long timestamp_ms)
{

    time_t timestamp_s = timestamp_ms / 1000;

    struct tm *tm_info = localtime(&timestamp_s);

    char buffer[30];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", tm_info);

    return String(buffer);
}
