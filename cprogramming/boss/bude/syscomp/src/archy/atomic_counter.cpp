 #include "atomic_counter.hpp"

BEGIN_NAMESPACE_ARCHY

atomic_counter::atomic_counter()
:m_counter(0)
{
}
atomic_counter::atomic_counter(int value)
:m_counter(value)
{
}
atomic_counter::~atomic_counter()
{
}

atomic_counter::atomic_counter(atomic_counter& r)
{
	scoped_lock lock(r.m_mutex);
	m_counter = r.m_counter;
}
atomic_counter& atomic_counter::operator = (atomic_counter& r)
{
	if( this != &r ) {
		scoped_lock lock(m_mutex);
		m_counter = (int)r;
	}
	return *this;
}

int atomic_counter::operator++()
{
	scoped_lock lock(m_mutex);
	return ++m_counter;
}
int atomic_counter::operator--()
{
	scoped_lock lock(m_mutex);
	return --m_counter;
}
int atomic_counter::operator++(int)
{
	scoped_lock lock(m_mutex);
	return m_counter++;
}
int atomic_counter::operator--(int)
{
	scoped_lock lock(m_mutex);
	return m_counter--;
}
int atomic_counter::operator+=(int change)
{
	scoped_lock lock(m_mutex);
	m_counter += change;
	return m_counter;
}
int atomic_counter::operator-=(int change)
{
	scoped_lock lock(m_mutex);
	m_counter -= change;
	return m_counter;
}
int atomic_counter::operator+(int change)
{
	scoped_lock lock(m_mutex);
	return m_counter + change;
}
int atomic_counter::operator-(int change)
{
	scoped_lock lock(m_mutex);
	return m_counter - change;
}
int atomic_counter::operator=(int value)
{
	int ret;
	scoped_lock lock(m_mutex);
	ret = m_counter;
	m_counter = value;
	return ret;
}
bool atomic_counter::operator!(void)
{
	scoped_lock lock(m_mutex);
	return m_counter == 0 ;
}
atomic_counter::operator int()
{
	scoped_lock lock(m_mutex);
	return m_counter;
}

END_NAMESPACE_ARCHY
