#ifndef _CYCLEINSTANCE_H_
#define _CYCLEINSTANCE_H_
#include "config-all.h"
#include <string>
#include <ctime>
USING_NAMESPACE(std);

namespace flowline
{


class Cycle {
	
public:
	class Instance {
	public:		
		Instance();
		string toString() const;
		time_t getBegin() const;
		time_t getEnd() const;
		const string& getId() const;
		void setBegin(time_t b);
		void setEnd(time_t e);
		void setId(const string& i);
	private:	
		friend class Cycle;
		time_t begin;
		time_t end;
		string id;		
	};
    
    enum Unit{
        U_SECOND = 's',
        U_MINUTE = 'm',
        U_HOUR = 'h',
        U_DAY = 'D',
        U_WEEK = 'W',
        U_MONTH = 'M',
        U_QUARTER = 'Q',
        U_YEAR = 'Y'
    };
    
    Cycle( time_t base = 0, int cycle=1, Unit unit =U_SECOND);
    
    void setBaseTime(time_t base);
    time_t getBaseTime() const;
    
    void setCycle(int cycle);
    int getCycle() const;
    
    Cycle::Unit getUnit() const;
    void setUnit(Unit unit);
    
	void compute(time_t atime, Instance& instance) const;
private:	
	void compute_sec( time_t atime , Instance& instance) const;
	void compute_min( time_t atime , Instance& instance) const;
	void compute_hour( time_t atime , Instance& instance) const;
	void compute_day( time_t atime , Instance& instance) const;
	void compute_week( time_t atime , Instance& instance) const;	
	void compute_month( time_t atime , Instance& instance) const;
	void compute_year( time_t atime , Instance& instance) const ;
	void compute_quarter( time_t atime , Instance& instance) const;
	
	int getMaxDayOfMonth(const tm& atm) const;	
	//0..
	int getFirstSundayOfYear(const tm& atm) const;
	
    //基准时间
    time_t m_baseTime;
    //交付周期
    int m_cycle;
    //交付周期的时间单位
    Unit m_cycleUnit;
};

};

#endif //_CYCLEINSTANCE_H_
