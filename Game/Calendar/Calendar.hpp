#pragma once
#ifndef WILDCAT_CALENDAR_HPP
#define WILDCAT_CALENDAR_HPP

/* Wildcat: Calendar
  #include "Time/Calendar.hpp"

	Tracks Calendar for games. Useful for knowing what day/time it is, and also event chronology.
*/



class Calendar
{
	public:
	
	int MONTHS_PER_YEAR;
	int DAYS_PER_MONTH;
	int HOURS_PER_DAY;
	int MINUTES_PER_HOUR;
	int SECONDS_PER_MINUTE;
	
	int year, month, day, hour, minute, second;
	Vector <std::string> vMonthName;

	
	Calendar(int _year=0, int _month=0, int _day=0, int _hour=0, int _minute=0, int _second=0)
	{
		year=_year;
		month=_month;
		day=_day;
		hour=_hour;
		minute=_minute;
		second=_second;
	 
		MONTHS_PER_YEAR = 4;
		DAYS_PER_MONTH = 28;
		HOURS_PER_DAY = 24;
		MINUTES_PER_HOUR = 60;
		SECONDS_PER_MINUTE = 1;
	}
	
	void configure
	( int _SECONDS_PER_MINUTE, int _MINUTES_PER_HOUR, int _HOURS_PER_DAY, int _DAYS_PER_MONTH, int _MONTHS_PER_YEAR )
	{
		SECONDS_PER_MINUTE = _SECONDS_PER_MINUTE;
		MINUTES_PER_HOUR = _MINUTES_PER_HOUR;
		HOURS_PER_DAY = _HOURS_PER_DAY;
		DAYS_PER_MONTH = _DAYS_PER_MONTH;
		MONTHS_PER_YEAR = _MONTHS_PER_YEAR;
	}
  
	bool operator < (Calendar * date)
	{
		//std::cout<<"<";
		if(year<date->year)
		{ return true; }
		else if(year>date->year)
		{ return false; }
		else
		{
			// if (month<date->month)
			// { return true; }
			// else if(month>date->month)
			// { return false; }
			// else
			// {
				// if(day<date->day)
				// { return true; }
				// else if (day>date->day)
				// { return false; }
				// else
				// {
					// /* Increase resolution to hours and minutes and seconds later. */
					// return false;
				// }
			// }
		}
		return false;
	}

	bool operator == (Calendar& date)
	{
    return (year==date.year && month==date.month && day==date.day
      && hour==date.hour && second==date.second);
	}
	bool operator != (Calendar& date)
	{
    return (second!=date.second || year!=date.year || month!=date.month || day!=date.day
      || hour!=date.hour);
	}
	
	void set(const int _year, const int _month, const int _day, const int _hour, const int _minute, const int _second)
	{
		year=_year;
		month=_month;
		day=_day;
		hour=_hour;
		minute=_minute;
		second=_second;
	}
  void set  ( Calendar* _calendar )
  {
    if ( !_calendar ) { return; }
		year=_calendar->year;
		month=_calendar->month;
		day=_calendar->day;
		hour=_calendar->hour;
		minute=_calendar->minute;
		second=_calendar->second;
  }
	
	//void setMonthNames (const std::string _m1, const std::string
	void addMonthName(const std::string _monthName)
	{
		vMonthName.push(_monthName);
	}
	
	void advanceSeconds(const int _seconds)
	{
		second+=_seconds;
		normalise();
	}
	void advanceSecond(const int _seconds) { advanceSeconds(_seconds); }
	void advanceMinutes(const int _minutes)
	{
		minute+=_minutes;
		normalise();
	}
	void advanceMinute(const int _minutes) { advanceMinutes(_minutes); }
	void advanceHours(const int _hours)
	{
		hour+=_hours;
		normalise();
	}
	void advanceHour(const int _hours) { advanceHours(_hours); }
	void advanceDays(const int _days)
	{
		day+=_days;
		normalise();
	}
	void advanceDay(const int _days) { advanceDays(_days); }
	
	void normalise()
	{
		/* Turn excess seconds into minutes. */
		if(second>=SECONDS_PER_MINUTE)
		{
			int nMinutes = second/SECONDS_PER_MINUTE;
			minute+=nMinutes;
			second-=(nMinutes*SECONDS_PER_MINUTE);
		}
		/* Turn excess minutes into hours. */
		if(minute>=MINUTES_PER_HOUR)
		{
			int nHours = minute/MINUTES_PER_HOUR;
			hour+=nHours;
			minute-=(nHours*MINUTES_PER_HOUR);
		}
		/* Turn excess hours into days. */
		if(hour>=HOURS_PER_DAY)
		{
			int nDays = hour/HOURS_PER_DAY;
			day+=nDays;
			hour-=(nDays*HOURS_PER_DAY);
		}
		/* Turn excess days into months. */
		if(day>=DAYS_PER_MONTH)
		{
			int nMonths = day/DAYS_PER_MONTH;
			month+=nMonths;
			day-=(nMonths*DAYS_PER_MONTH);
		}
		/* Turn excess months into years. */
		if(month>=MONTHS_PER_YEAR)
		{
			int nYears = month/MONTHS_PER_YEAR;
			year+=nYears;
			month-=(nYears*MONTHS_PER_YEAR);
		}
	}
	
	std::string toString()
	{
		normalise();
		std::string output = "";
		output+=DataTools::toString(day,2);
		output+="/";
		output+=DataTools::toString(month,2);
		output+="/";
		output+=DataTools::toString(year,4);
		output+=" ";
		output+=DataTools::toString(hour,2);
		output+=":";
		output+=DataTools::toString(minute,2);
		output+=":";
		output+=DataTools::toString(second,2);
		return output;
	}
  
	int getDistanceYears(Calendar *c)
	{
		int nYears = year - c->year;
		if ( nYears<0 ) { nYears*=-1; }
		return nYears;
	}

	
	int getDistanceDays(Calendar* c)
	{
		int nDays=0;
		std::cout<<"Get distance days.\n";
		
		
		Calendar counter;
		counter.set(this);
		
	
		//int currentYear = year;
		while(counter < c)
		{
			++nDays;
			counter.advanceDay(1);
		}
		
		std::cout<<"\nReturning: "<<nDays<<".\n";
		return nDays;
	}
	int distanceDays ( Calendar* c)
	{ return getDistanceDays(c); }
  
  //note that this will return maximum of 31,536,000 seconds (1 year)
	long long unsigned int distanceSeconds(Calendar* c)
	{
		int nSeconds=0;
		std::cout<<"Get distance seconds.\n";
		
		
		Calendar counter;
		counter.set(this);
    

		while(counter < c)
		{
			++nSeconds;
			counter.advanceSeconds(1);
		}
		
		std::cout<<"\nReturning: "<<nSeconds<<".\n";
		return nSeconds;
	}

};



#endif
