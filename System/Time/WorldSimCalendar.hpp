//#pragma once
#ifndef WILDCAT_WORLDSIM_CALENDAR_HPP
#define WILDCAT_WORLDSIM_CALENDAR_HPP

/* Wildcat: Calendar
	#include "Time/WorldSimCalendar.hpp"

	Calendar system revised for WorldSim conventions. Scales down to about 8% of realtime in total, with individual days
	scaled down to about 25% of realtime, and individual years scaled down to about 30% of realtime.

	Summary of units:

	60 ticks per minute.
	60 minutes per hour.
	7 hours per day ( dawn, morning, day, afternoon, dusk, night, late night )
	7 days per week
	4 weeks per season (making all seasons 28 days)
	4 seasons per year (summer, autumn, winter, spring) or (dry, wet) depending on location.
	Unlimited years. Possibly later divided into eras or whatever.
	
	The class also functions as a timestamp system, and you can perform normal calculations using these objects.
	
	I think this scale is good and has a good balance between sense of scale but also cuts back on long boring periods.
	
	It is hardcoded, no need to overengineer this.
*/

#include <string>

#include <Data/DataTools.hpp>

class WorldSimCalendar
{
	private:
	unsigned long int tick; // 0-59
	unsigned long int minute; // 0-59
	unsigned long int hour; // 0-5
	unsigned long int day; // 0-6
	unsigned long int week; //0-3
	unsigned long int season; //0-3
	unsigned long int year; //0+
	
	unsigned long int totalTick; // worth tracking as it makes some calcs easier.
	
	public:
	
	void init()
	{
		tick=0;
		minute=0;
		hour=0;
		day=0;
		week=0;
		season=0;
		year=0;
	}
	
	WorldSimCalendar()
	{
		init();
	}
	// Copy constructors must take reference of themself to prevent infinite recursion.
	WorldSimCalendar(WorldSimCalendar& c)
	{
		tick=c.tick;
		minute=c.minute;
		hour=c.hour;
		day=c.day;
		week=c.week;
		season=c.season;
		year=c.year;
	}
	WorldSimCalendar(WorldSimCalendar *c)
	{
		if ( c == 0 )
		{
			init();
		}
		tick=c->tick;
		minute=c->minute;
		hour=c->hour;
		day=c->day;
		week=c->week;
		season=c->season;
		year=c->year;
	}
	
	void increment(unsigned long int nTick)
	{
		// todo: optimise
		tick += nTick;
		while (tick > 59)
		{
			tick-=60;
			++minute;
		}
		while (minute > 59)
		{
			minute-=60;
			++hour;
		}
		while (hour > 6)
		{
			hour-=7;
			++day;
		}
		while (day > 6)
		{
			day -= 7;
			++week;
		}
		while (week > 3)
		{
			week -= 4;
			++season;
		}
		while (season > 3)
		{
			season -= 4;
			++year;
		}
	}
	
	// find distance between 2 calendars in ticks.
	long int distanceTo(WorldSimCalendar c)
	{
		return 0;
	}
	
	std::string toString( /* format */ )
	{
		return DataTools::toString(tick);
	}
};

#endif
