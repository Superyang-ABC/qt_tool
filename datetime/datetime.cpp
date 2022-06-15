#include "datetime.h"

DateTimeInfo DateTime::timeinfo;
DateTime::DateTime()
{

}

qint64 DateTime::getSecSSinceEpoch()
{
    return currentMSecsSinceEpoch() / 1000;
}
qint64 DateTime::getMsSinceEpoch()
{
    return currentMSecsSinceEpoch();
}
//qint64 DateTime::getUsSinceEpoch()
//{


//}
const DateTimeInfo& DateTime::getCurrentTimeInfo()
{
    QDateTime nowTime = QDateTime::currentDateTime();
    QDate date = nowTime.date();
    QTime time = nowTime.time();
    timeinfo.year = date.year();
    timeinfo.month = date.month();
    timeinfo.day = date.day();
    timeinfo.hour = time.hour();
    timeinfo.minute = time.minute();
    timeinfo.sec = time.second();
    timeinfo.msec = time.msec();
    timeinfo.usec = 0;
    return timeinfo;
}
