#include <iostream>
#include "TpDate.h"
#include "TpTime.h"
#include "TpDateTime.h"

int main(int argc, char *argv[])
{
	//-----------------------TpDate---------------------

	TpDate currentDate = TpDate::currentDate();

	std::cout << "CurrentDate Year Month Day: " << currentDate.year() << " " << currentDate.month() << " " << currentDate.day() << std::endl;

	std::cout << "CurrentDate: " << TpDate::currentDate().toString("yyyy-MM-dd") << std::endl;

	/// @return 当周的第几天，1为星期一，7为星期日
	std::cout << "CurrentDate DayOfWeek: " << TpDate::currentDate().dayOfWeek() << std::endl;

	/// @brief 获取当前日期在该年中的天数
	std::cout << "CurrentDate dayOfYear: " << TpDate::currentDate().dayOfYear() << std::endl;

	/// @brief 获取当前日期所在月份的天数
	std::cout << "CurrentDate daysInMonth: " << TpDate::currentDate().daysInMonth() << std::endl;

	/// @brief 返回当前日期所在年份共计多少天
	std::cout << "CurrentDate daysInYear: " << TpDate::currentDate().daysInYear() << std::endl;

	TpDate nextDay = currentDate.addDays(1);
	std::cout << "NextData Is: " << nextDay.day() << std::endl;

	TpDate nextMonth = currentDate.addMonths(1);
	std::cout << "NextMonth Is: " << nextMonth.month() << std::endl;

	TpDate nextYear = currentDate.addYears(1);
	std::cout << "NextYear Is: " << nextYear.year() << std::endl;

	std::cout << std::endl
			  << std::endl
			  << "------------------------------" << std::endl
			  << std::endl;

	//-----------------------TpTime---------------------
	TpTime testTime = TpTime::currentTime();

	std::cout << "CurrentTime hour minute second msec: " << testTime.hour() << " " << testTime.minute() << " " << testTime.second() << " " << testTime.msec() << std::endl;

	TpTime addTimeS = testTime.addSecs(20);
	std::cout << "next 20s LaterTime hour minute second msec: " << addTimeS.hour() << " " << addTimeS.minute() << " " << addTimeS.second() << " " << addTimeS.msec() << std::endl;
	std::cout << "secsTo: " << testTime.secsTo(addTimeS) << std::endl;
	std::cout << "secsTo22: " << addTimeS.secsTo(testTime) << std::endl;

	TpTime addTimeMS = testTime.addMSecs(20);
	std::cout << "next 20ms LaterTime hour minute second msec: " << addTimeMS.hour() << " " << addTimeMS.minute() << " " << addTimeMS.second() << " " << addTimeMS.msec() << std::endl;
	std::cout << "msecsTo: " << testTime.msecsTo(addTimeMS) << std::endl;

	std::cout << std::endl
			  << std::endl
			  << "------------------------------" << std::endl
			  << std::endl;

	//-----------------------TpDateTime---------------------

	TpDateTime testDateTime = TpDateTime::currentDateTime();

	std::cout << "toMSecsSinceEpoch: " << testDateTime.toMSecsSinceEpoch() << std::endl;
	std::cout << "toSecsSinceEpoch: " << testDateTime.toSecsSinceEpoch() << std::endl;

	std::cout << "format dateTime: " << testDateTime.toString("yyyy-MM-dd HH:mm:ss") << std::endl;

	TpDateTime nextDayDateTime = testDateTime.addDays(1);
	std::cout << "current DateTime to nextDay DateTime: " << testDateTime.daysTo(nextDayDateTime) << std::endl;

	return 0;
}
