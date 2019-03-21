#pragma once
#ifndef WILDCAT_CALENDAR_HPP
#define WILDCAT_CALENDAR_HPP

/* Wildcat: Calendar
  #include "Time/Calendar.hpp"

    I'm going with the following simple approach:

    60 seconds per minute.
    60 minutes per hour.
    24 hours per day.
    30 days per month.
    12 months per year.
    
    Some scaling can be implemented by lowering seconds per minute.
    
    You can pass the calendar which will basically represent a date.
*/

class Calendar
{
	public:
	
	
	int year, month, day, hour, minute, second;
	Vector <std::string> vMonthName;
  
  int secondsPerMinute;
	
	Calendar()
	{
		year=0;
		month=0;
		day=0;
		hour=CALENDAR_INITIAL_HOUR;
		minute=CALENDAR_INITIAL_MINUTE;
		second=0;
    
    secondsPerMinute=CALENDAR_SECONDS_PER_MINUTE;
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
		if(second>=secondsPerMinute)
		{
			int nMinutes = second/secondsPerMinute;
			minute+=nMinutes;
			second-=(nMinutes*secondsPerMinute);
		}
		/* Turn excess minutes into hours. */
		if(minute>=60)
		{
			int nHours = minute/60;
			hour+=nHours;
			minute-=(nHours*60);
		}
		/* Turn excess hours into days. */
		if(hour>=24)
		{
			int nDays = hour/24;
			day+=nDays;
			hour-=(nDays*24);
		}
		/* Turn excess days into months. */
		if(day>=30)
		{
			int nMonths = day/30;
			month+=nMonths;
			day-=(nMonths*30);
		}
		/* Turn excess months into years. */
		if(month>=12)
		{
			int nYears = month/12;
			year+=nYears;
			month-=(nYears*12);
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
  
};



#endif
