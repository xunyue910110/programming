#ifndef ARCHY_CRC32_CHECKSUM_HEADER
#define ARCHY_CRC32_CHECKSUM_HEADER
#include "archy_config.hpp"
#include "archy_type.hpp"
BEGIN_NAMESPACE_ARCHY

class checksum
{
public:
	checksum();
	
	ARCHY_Uint4 get_checksum()const { return m_Checksum; }
	
	void clear() { m_Checksum = 0; }
	
    void add_chars(const char* str, size_t length);

private:
	ARCHY_Uint4 m_Checksum;
};

struct checksum_helper
{
//  This code assumes that an unsigned is at least 32 bits wide and
//  that the predefined type char occupies one 8-bit byte of storage.

//  The polynomial used is
//  x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x^1+x^0
 	enum { CRC32_POLYNOM = 0x04c11db7 };
	checksum_helper();
    static ARCHY_Uint4 sm_CRC32Table[256];
    static void InitTables();
    static ARCHY_Uint4 UpdateCRC32(ARCHY_Uint4 checksum, const char* str, size_t count);
};

END_NAMESPACE_ARCHY

#endif

