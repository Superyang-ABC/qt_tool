/*
 * profile_timer.cpp
 *
 *  Created on: 2015年7月15日
 *      Author: yanghui
 */

#include "linux_comm.h"
#include "profile_timer.h"
#include "logger.h"
#include "file.h"
IMPLEMENT_SINGLETON(ProfileTimer)

ProfileTimer::ProfileTimer()
{
	list.reserve(1000);
}

void ProfileTimer::Start(const char *name)
{
	Profiles::iterator i = profiles_.find(name);
	if (i == profiles_.end())
	{
		ProfileInfo profile;
		profile.times = 0;
		profile.totalTime = 0;
		profile.minTime = LLONG_MAX;
		profile.maxTime = LLONG_MIN;
		GetCurrentTime(&profile.startTime);
		profiles_.insert(Profiles::value_type(name, profile));
	}
	else
	{
		ProfileInfo &profile = (*i).second;
		GetCurrentTime(&profile.startTime);
	}
}

void ProfileTimer::Stop(const char *name)
{
	Profiles::iterator i = profiles_.find(name);
	if (i != profiles_.end())
	{
		ProfileInfo &profile = (*i).second;
		timespec currentTime;
		GetCurrentTime(&currentTime);
		long long duration = NanosecondsDiff(currentTime, profile.startTime);
		profile.times++;
		profile.totalTime += duration;
		if (duration < profile.minTime)
		{
			profile.minTime = duration;
		}
		if (duration > profile.maxTime)
		{
			profile.maxTime = duration;
		}
	}
}

void ProfileTimer::Reset(const char *name)
{
	profiles_.erase(name);
}

void ProfileTimer::Clear()
{
	profiles_.clear();
}

void ProfileTimer::Report()
{
	char fileName[MAX_PATH_LENGTH];
	time_t now = time(NULL);
	tm *t = localtime(&now);
	sprintf(fileName, "/mnt/sdmmc/profile_%04d%02d%02d%02d%02d%02d.txt", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	LOG_INFO("ProfileTimer", "Save profile to '%s'", fileName);
	ccm::File file;
	if (file.Open(fileName, ccm::File::CREATE | ccm::File::WRITE))
	{
		char buffer[2048];
		for (Profiles::const_iterator i = profiles_.begin(); i != profiles_.end(); ++i)
		{
			const ProfileInfo &profile = (*i).second;

			if (profile.times > 2)
			{
				sprintf(buffer, "%s - Times:%lld; Average:%lld(us); Min:%lld(us); Max:%lld(us) \n",
						(*i).first.c_str(), profile.times, ((profile.totalTime - profile.minTime - profile.maxTime) / (profile.times - 2)) / 1000,
						profile.minTime / 1000, profile.maxTime / 1000);
			}
			else if (profile.times == 1)
			{
				sprintf(buffer, "%s - Times:%lld; Average:%lld(us);\n", (*i).first.c_str(), profile.times, profile.totalTime / 1000);
			}
			else
			{
				sprintf(buffer, "%s - Times:%lld;\n", (*i).first.c_str(), profile.times);
			}
			file.Write(buffer, strlen(buffer));
			printf("%s \n", buffer);
		}
		file.Close();
	}
	else
	{
		LOG_ERROR("ProfileTimer", "profile save failed");
	}

	WriteToRecord();
}

void ProfileTimer::GetCurrentTime(timespec *spec)
{
	clock_gettime(CLOCK_REALTIME, spec);
}

long long ProfileTimer::NanosecondsDiff(const timespec &end, const timespec &start)
{
	return (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
}

void ProfileTimer::Record(long long times, int start_wheel, int end_wheel)
{
#ifdef PROFILE

	RecordInfo info = {times, start_wheel, end_wheel};
	//    LOG_WARN("ProfileTimer", "record %d %d %d", times , start_wheel, end_wheel);
	list.push_back(info);
#endif
}

void ProfileTimer::WriteToRecord(void)
{
#ifdef PROFILE

	char fileName[MAX_PATH_LENGTH];
	time_t now = time(NULL);
	tm *t = localtime(&now);

	int time_ms, wheel_between, wheel_mean;
	ccm::File time_file;

	if (list.empty())
		return;

	vector<RecordInfo>::iterator i = list.begin();
	RecordInfo last = list[0];
	i++;
	sprintf(fileName, "/mnt/udisk/record/record_%04d%02d%02d%02d%02d%02d.csv", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

	if (time_file.Open(fileName, ccm::File::CREATE | ccm::File::WRITE))
	{
		char buffer[2048];
		time_file.Write("time(ms) , between codes , self codes\n", strlen("time     , between codes , self codes\n"));

		for (; i != list.end(); ++i)
		{
			time_ms = int((i->time - last.time) / 1000000);
			wheel_between = i->start_wheel - last.end_wheel;
			wheel_mean = i->end_wheel - i->start_wheel;
			last = *i;

			sprintf(buffer, "%d , %d , %d \n", time_ms, wheel_between, wheel_mean);
			time_file.Write(buffer, strlen(buffer));
		}
		time_file.Close();
	}
	else
	{

		LOG_ERROR("ProfileTimer", "record save failed");
	}

	list.clear();
#endif
}
