#pragma once
#ifndef WILDCAT_TIME_CALENDAR_HPP
#define WILDCAT_TIME_CALENDAR_HPP

#include <Data/DataTools.hpp>

/* #include <Time/Calendar.hpp> Created: 022-171. Updated: 022-171. Component of GaroSoft WildCat game engine.
	Class to create a simple calendar for games. No leap years or months of different length.
*/

class Calendar
{
	private:
	int year, month, day, unit;
	int daysPerMonth, monthsPerYear, unitsPerDay;
	int dayNumberZeroes, monthNumberZeroes, unitNumberZeroes;
	public:

	Calendar()
	{
		year=0;
		month=0;
		day=0;
		unit=0;
		daysPerMonth=0;
		monthsPerYear=0;
		unitsPerDay=0;
	}
	
	
	void setDaysPerMonth ( const int _daysPerMonth )
	{
		if(_daysPerMonth>0)
		{
			daysPerMonth = _daysPerMonth;
			dayNumberZeroes = DataTools::toString(daysPerMonth-1).length();
		}
		else
		{ std::cout<<"Calendar.hpp. Error: Days per month must be greater than zero.\n"; }
	}
	
	void setMonthsPerYear ( const int _monthsPerYear )
	{
		if(_monthsPerYear>0)
		{
			monthsPerYear = _monthsPerYear;
			monthNumberZeroes = DataTools::toString(monthsPerYear-1).length();
		}
		else
		{ std::cout<<"Calendar.hpp. Error: Months per year must be greater than zero.\n"; }
	}
	
	void setUnitsPerDay (const int _unitsPerDay )
	{
		if(_unitsPerDay>0)
		{
			unitsPerDay=_unitsPerDay;
			unitNumberZeroes = DataTools::toString(unitsPerDay-1).length();
		}
		else
		{ std::cout<<"Calendar.hpp. Error: Units per day must be greater than zero.\n"; }
	}

	void advanceUnits(const int _units)
	{
		unit+=_units;
		
		if(unit>=unitsPerDay)
		{
			//std::cout<<"New day.\n";
			
			int nDays = unit/unitsPerDay;
			//std::cout<<"Ndays: "<<nDays<<".\n";
			//sleep(10);
			
			day+=nDays;
			unit-=(nDays*unitsPerDay);
			
			if(day>=daysPerMonth)
			{
			//	std::cout<<"New month.\n";
				int nMonths = day/daysPerMonth;
				
				month+=nMonths;
				day-=(nMonths*daysPerMonth);
			
				if(month>=monthsPerYear)
				{
					int nYears = month/monthsPerYear;
					year+=nYears;
					month-=(nYears*monthsPerYear);
				}
			}
			
		}
		
	}
	
	std::string toString ( const std::string delimiter )
	{
		std::string strUnit = DataTools::toString(unit,unitNumberZeroes);
		std::string strDay = DataTools::toString(day,dayNumberZeroes);
		std::string strMonth = DataTools::toString(month,monthNumberZeroes);
		std::string strYear = DataTools::toString(year);
	
	
	
		std::string strDate = strYear+delimiter+strMonth+delimiter+strDay+delimiter+strUnit;
		return strDate;
	}
	
};


/* Date: Stores a particular date of the calendar. */

class Date
{
	public:
};

#endif
