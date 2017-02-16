#include "Cycle.h"
#include "base/exceptions.h"

namespace flowline {

Cycle::Instance::Instance() : begin(0), end(0) {}

string Cycle::Instance::toString() const {
	tm btm, etm;
	char buffer[128];
    localtime_r(&begin, &btm);
    localtime_r(&end, &etm);    
    sprintf(buffer, "cycleid=%s, begin=%04d-%02d-%02d %02d:%02d:%02d, end=%04d-%02d-%02d %02d:%02d:%02d", 
    	id.c_str(), 
    	btm.tm_year + 1900, btm.tm_mon + 1, btm.tm_mday, btm.tm_hour, btm.tm_min, btm.tm_sec,
    	etm.tm_year + 1900, etm.tm_mon + 1, etm.tm_mday, etm.tm_hour, etm.tm_min, etm.tm_sec);
    return buffer;
}
void Cycle::Instance::setBegin(time_t b) {
	begin = b;
}
void Cycle::Instance::setEnd(time_t e) {
	end = e;
}
void Cycle::Instance::setId(const string& i) {
	id = i;
}

time_t Cycle::Instance::getBegin() const {
	return begin;
}
time_t Cycle::Instance::getEnd() const {
	return end;
}
const string& Cycle::Instance::getId() const{
	return id;
}

Cycle::Cycle( time_t base, int cycle, Unit unit ) : 
	m_baseTime( base ), m_cycle( cycle ), m_cycleUnit( unit ) {}

void Cycle::setBaseTime(time_t base) {
	m_baseTime = base;
}
time_t Cycle::getBaseTime() const {
	return m_baseTime;
}

void Cycle::setCycle(int cycle) {
	m_cycle = cycle;
}
int Cycle::getCycle() const {
	return m_cycle;
}

Cycle::Unit Cycle::getUnit() const {
	return m_cycleUnit;
}

void Cycle::setUnit(Unit unit) {
	m_cycleUnit = unit;
}

void Cycle::compute( time_t atime, Instance& instance ) const {

	/*cout<<"before:time_t:"<<atime<<endl;
	cout<<"before:before:instance.begin"<<instance.begin<<endl;
	cout<<"before:berore:instance.end"<<instance.end<<endl;
    */
    if ( m_cycleUnit == Cycle::U_SECOND ) {
        compute_sec( atime, instance );
    } else if ( m_cycleUnit == Cycle::U_MINUTE ) {
        compute_min( atime, instance );
    } else if ( m_cycleUnit == Cycle::U_HOUR ) {
        compute_hour( atime, instance );
    } else if ( m_cycleUnit == Cycle::U_DAY ) {
        compute_day( atime, instance );
    } else if ( m_cycleUnit == Cycle::U_WEEK ) {
        compute_week( atime, instance );
    } else if ( m_cycleUnit == Cycle::U_MONTH ) {
        compute_month( atime, instance );
    } else if ( m_cycleUnit == Cycle::U_QUARTER ) {
        compute_quarter( atime, instance );
    } else if ( m_cycleUnit == Cycle::U_YEAR ) {
        compute_year( atime, instance );
    }
   /* tm atm;
    printf("周期%d, 单位%c\n", m_cycle, m_cycleUnit);
    localtime_r(&atime, &atm);
    printf("计算:%04d-%02d-%02d %02d:%02d:%02d\n", atm.tm_year + 1900, atm.tm_mon + 1, atm.tm_mday, atm.tm_hour, atm.tm_min, atm.tm_sec);
    localtime_r(&m_baseTime, &atm);
    printf("基准:%04d-%02d-%02d %02d:%02d:%02d\n", atm.tm_year + 1900, atm.tm_mon + 1, atm.tm_mday, atm.tm_hour, atm.tm_min, atm.tm_sec);
    localtime_r(&instance.begin, &atm);	
	printf("起始:%04d-%02d-%02d %02d:%02d:%02d\n", atm.tm_year + 1900, atm.tm_mon + 1, atm.tm_mday, atm.tm_hour, atm.tm_min, atm.tm_sec);	
	localtime_r(&instance.end, &atm);	
	printf("截至:%04d-%02d-%02d %02d:%02d:%02d\n", atm.tm_year + 1900, atm.tm_mon + 1, atm.tm_mday, atm.tm_hour, atm.tm_min, atm.tm_sec);	
    cout<<"after:time_t:"<<atime<<endl;
	cout<<"after:before:instance.begin"<<instance.begin<<endl;
	cout<<"after:berore:instance.end"<<instance.end<<endl;
    */
    
    //if(m_cycleUnit != Cycle::U_MONTH )
    ASSERT(atime >= instance.begin&& atime < instance.end );
    
}
void Cycle::compute_sec( time_t atime , Instance& instance ) const {
    int w = m_cycle;
    int f = m_baseTime > atime;
    time_t offset = ( ( atime - m_baseTime ) / w - ( ( ( atime - m_baseTime ) % w ) != 0 ) * f );
    //
    instance.begin = m_baseTime + offset * w;
    instance.end = m_baseTime + ( offset + 1 ) * w;
    tm atm;
    localtime_r( &instance.begin, &atm );
    char buffer[ 32 ];
    sprintf( buffer, "%04d%02d%02d%02d%02d%02d", atm.tm_year + 1900, atm.tm_mon + 1, atm.tm_mday, atm.tm_hour, atm.tm_min, atm.tm_sec );
    instance.id = buffer;
}
void Cycle::compute_min( time_t atime , Instance& instance ) const {
    int w = m_cycle * 60;
    int f = m_baseTime > atime;
    time_t offset = ( ( atime - m_baseTime ) / w - ( ( ( atime - m_baseTime ) % w ) != 0 ) * f );
    //
    instance.begin = m_baseTime + offset * w;
    instance.end = m_baseTime + ( offset + 1 ) * w;
    tm atm;
    localtime_r( &instance.begin, &atm );
    char buffer[ 32 ];
    sprintf( buffer, "%04d%02d%02d%02d%02d", atm.tm_year + 1900, atm.tm_mon + 1, atm.tm_mday, atm.tm_hour, atm.tm_min );
    instance.id = buffer;
}
void Cycle::compute_hour( time_t atime , Instance& instance ) const {
    int w = m_cycle * 60 * 60;
    int f = m_baseTime > atime;
    time_t offset = ( ( atime - m_baseTime ) / w - ( ( ( atime - m_baseTime ) % w ) != 0 ) * f );
    //
    instance.begin = m_baseTime + offset * w;
    instance.end = m_baseTime + ( offset + 1 ) * w;
    tm atm;
    localtime_r( &instance.begin, &atm );
    char buffer[ 32 ];
    sprintf( buffer, "%04d%02d%02d%02d", atm.tm_year + 1900, atm.tm_mon + 1, atm.tm_mday, atm.tm_hour );
    instance.id = buffer;
}
void Cycle::compute_day( time_t atime , Instance& instance )  const{
    int w = m_cycle * 60 * 60 * 24;
    int f = m_baseTime > atime;
    time_t offset = ( ( atime - m_baseTime ) / w - ( ( ( atime - m_baseTime ) % w ) != 0 ) * f );
    //
    instance.begin = m_baseTime + offset * w;
    instance.end = m_baseTime + ( offset + 1 ) * w;
    tm atm;
    localtime_r( &instance.begin, &atm );
    char buffer[ 32 ];
    sprintf( buffer, "%04d%02d%02d", atm.tm_year + 1900, atm.tm_mon + 1, atm.tm_mday );
    instance.id = buffer;
}

int Cycle::getMaxDayOfMonth( const tm& atm )  const{
    tm btm = atm;
    btm.tm_mon += 1;
    btm.tm_mday = 1;
    time_t t = mktime( &btm );
    t -= 3600 * 24;
    localtime_r( &t, &btm );
    return btm.tm_mday;
}

//0..
int Cycle::getFirstSundayOfYear( const tm& atm ) const {
    tm btm = atm;
    btm.tm_mon = 0;
    btm.tm_mday = 0;
    time_t t = mktime( &btm );
    while ( true ) {
        localtime_r( &t, &btm );
        if ( btm.tm_wday == 0 ) {
            break;
        }
        t += 24 * 3600;
    }
    return btm.tm_yday;
}

void Cycle::compute_week( time_t atime , Instance& instance ) const {
    int w = m_cycle * 60 * 60 * 24 * 7;
    int f = m_baseTime > atime;
    time_t offset = ( ( atime - m_baseTime ) / w - ( ( ( atime - m_baseTime ) % w ) != 0 ) * f );
    //
    cout<<"offset week:"<<offset<<endl;
    instance.begin = m_baseTime + offset * w;
    instance.end = m_baseTime + ( offset + 1 ) * w;
    tm atm;
    localtime_r( &instance.begin, &atm );
    char buffer[ 32 ];
    int b = getFirstSundayOfYear( atm );
    sprintf( buffer, "%04d%02d", atm.tm_year + 1900 - ( b != 0 ), ( atm.tm_yday - b ) / 7 + 1 );
    instance.id = buffer;
}

void Cycle::compute_month( time_t atime , Instance& instance )  const{
    tm btm, atm ;
  
    localtime_r( &atime, &atm );
    localtime_r( &m_baseTime, &btm );
    int bid = btm.tm_year * 12 + btm.tm_mon;
    int aid = atm.tm_year * 12 + atm.tm_mon;
   
   
    int f = m_baseTime > atime;
    int w = m_cycle;
    int offset = ( ( aid - bid ) / w - ( ( ( aid - bid ) % w ) != 0 ) * f );
    int idx = bid	 + ( offset + 1 ) * w ;
    btm.tm_year = idx / 12;
    btm.tm_mon = idx % 12;
    
   // if(atm.tm_mday==1 &&btm.tm_mon>atm.tm_mon)
  //		btm.tm_mon = atm.tm_mon;
    int maxd = getMaxDayOfMonth( btm );
    if ( btm.tm_mday > maxd )
        btm.tm_mday = maxd;                                               
    instance.end = mktime( &btm );
    
    idx = bid + offset * w;
    btm.tm_year = idx / 12;
    btm.tm_mon = idx % 12 ;
    btm.tm_hour = 0;// 
    btm.tm_min = 0;//
    btm.tm_sec = 0;//
    maxd = getMaxDayOfMonth( btm );
  
    if ( btm.tm_mday > maxd )
        btm.tm_mday = maxd;
      
    instance.begin = mktime( &btm );
    char buffer[ 32 ];
    sprintf( buffer, "%04d%02d", btm.tm_year + 1900, btm.tm_mon + 1 );
    instance.id = buffer;
}
void Cycle::compute_year( time_t atime , Instance& instance )  const{
    tm btm, atm;
    localtime_r( &atime, &atm );
    localtime_r( &m_baseTime, &btm );
    int f = m_baseTime > atime;
    int w = m_cycle;
    int offset = ( ( atm.tm_year - btm.tm_year ) / w - ( ( ( atm.tm_year - btm.tm_year ) % w ) != 0 ) * f );
    //end
    int idx = btm.tm_year + ( offset + 1 ) * w;
    btm.tm_year = idx;
    int maxd = getMaxDayOfMonth( btm );
    if ( btm.tm_mday > maxd )
        btm.tm_mday = maxd;
    instance.end = mktime( &btm );
    //begin
    idx -= w;
    btm.tm_year = idx;
    maxd = getMaxDayOfMonth( btm );
    if ( btm.tm_mday > maxd )
        btm.tm_mday = maxd;
    instance.begin = mktime( &btm );
    //sid
    char buffer[ 32 ];
    sprintf( buffer, "%04d", btm.tm_year + 1900 );
    instance.id = buffer;
}
void Cycle::compute_quarter( time_t atime , Instance& instance ) const {
    tm btm, atm;
    localtime_r( &atime, &atm );
    localtime_r( &m_baseTime, &btm );
    int bid = btm.tm_year * 12 + btm.tm_mon;
    cout << bid - 104*12 << endl;
    int aid = atm.tm_year * 12 + atm.tm_mon;
    cout << aid - 104*12 << endl;
    int f = m_baseTime > atime;
    int w = 3 * m_cycle;
    int offset = ( ( aid - bid ) / w - ( ( ( aid - bid ) % w ) != 0 ) * f );
    //end
    int idx = bid + ( offset + 1 ) * w;
    btm.tm_year = idx / 12;
    btm.tm_mon = idx % 12;
    int maxd = getMaxDayOfMonth( btm );
    if ( btm.tm_mday > maxd )
        btm.tm_mday = maxd;
           
    instance.end = mktime( &btm );
    //begin
    idx = bid + offset * w;
    btm.tm_year = idx / 12;
    btm.tm_mon = idx % 12;
    maxd = getMaxDayOfMonth( btm );
    if ( btm.tm_mday > maxd )
        btm.tm_mday = maxd;
    instance.begin = mktime( &btm );
    //sid
    char buffer[ 32 ];
    sprintf( buffer, "%04d%02d", btm.tm_year + 1900, idx % 4 );
    instance.id = buffer;
}
};
