#if !defined(AFX_ATOMIC_COUNTER_HPP__F39C2D27_6711_4072_BBD2_853168BC81FF__INCLUDED_)
#define AFX_ATOMIC_COUNTER_HPP__F39C2D27_6711_4072_BBD2_853168BC81FF__INCLUDED_
#include "archy_config.hpp"
#include "mutex.hpp"
BEGIN_NAMESPACE_ARCHY

class atomic_counter  
{
public:
	atomic_counter();
	atomic_counter(int value);
	atomic_counter(atomic_counter& r);
	atomic_counter& operator = (atomic_counter& r);
	~atomic_counter();

	int operator++();
	int operator--();
	int operator++(int);
	int operator--(int);
	int operator+=(int change);
	int operator-=(int change);
	int operator+(int change);
	int operator-(int change);
	int operator=(int value);
	bool operator!(void);
	operator int();

private:
	int m_counter;
	mutex m_mutex;
};

END_NAMESPACE_ARCHY

#endif // !defined(AFX_ATOMIC_COUNTER_HPP__F39C2D27_6711_4072_BBD2_853168BC81FF__INCLUDED_)
