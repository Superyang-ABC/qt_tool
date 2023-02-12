/*
 * profile_timer.h
 *
 *  Created on: 2015骞?鏈?5鏃? *      Author: yanghui
 */

#ifndef PROFILE_TIMER_H_
#define PROFILE_TIMER_H_

#include <string>
#include <map>
#include "singleton.h"
#include <vector>

//#define PROFILE

using namespace std;

class ProfileTimer
{
public:
    void Start(const char *name);
    void Stop(const char *name);
    void Reset(const char *name);
    void Report();
    void Clear();
    void Record(long long times, int start_wheel, int end_wheel);

    typedef struct
    {
        long long time;
        int start_wheel;
        int end_wheel;
    } RecordInfo;

private:
    struct ProfileInfo
    {
        long long times;
        long long totalTime;
        long long minTime;
        long long maxTime;
        timespec startTime;
    };

    vector<RecordInfo> list;

    typedef std::map<std::string, ProfileInfo> Profiles;

    Profiles profiles_;

    ProfileTimer();
    void WriteToRecord(void);

public:
    static void GetCurrentTime(timespec *spec);
    static long long NanosecondsDiff(const timespec &end, const timespec &start);

    DECLARE_SINGLETON(ProfileTimer)
};

#ifdef PROFILE
#define PROFILE_START(name) ProfileTimer::GetInstance()->Start(name)
#define PROFILE_STOP(name) ProfileTimer::GetInstance()->Stop(name)
#define PROFILE_CLEAR ProfileTimer::GetInstance()->Clear()
#define PROFILE_REPORT ProfileTimer::GetInstance()->Report()

#else
#define PROFILE_START(name)
#define PROFILE_STOP(name)
#define PROFILE_CLEAR
#define PROFILE_REPORT
#endif

#endif /* PROFILE_TIMER_H_ */
