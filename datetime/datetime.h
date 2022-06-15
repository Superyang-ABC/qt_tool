#ifndef DATATIME_H
#define DATATIME_H
#include <QDateTime>
struct DateTimeInfo
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int sec;
    int msec;
    int usec;
};

class DateTime:public QDateTime
{
private:
    DateTime();
public:
static DateTimeInfo timeinfo;
static qint64 getSecSSinceEpoch();
static qint64 getMsSinceEpoch();
//static qint64 getUsSinceEpoch();
static const DateTimeInfo& getCurrentTimeInfo();
};

#endif // DATATIME_H
