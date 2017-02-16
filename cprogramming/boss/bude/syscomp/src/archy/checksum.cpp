#include "checksum.hpp"

BEGIN_NAMESPACE_ARCHY


checksum::checksum()
:m_Checksum(0)
{

}
void checksum::add_chars(const char* str, size_t count)
{
    m_Checksum = checksum_helper::UpdateCRC32(m_Checksum, str, count);
}


checksum_helper g_checksum_helper;

checksum_helper::checksum_helper()
{
	InitTables();
}

ARCHY_Uint4 checksum_helper::sm_CRC32Table[256];
void checksum_helper::InitTables()
{
    if ( sm_CRC32Table[0] == 0 ) {
        for ( size_t i = 0;  i < 256;  ++i ) {
            ARCHY_Uint4 byteCRC = ARCHY_Uint4(i) << 24;
            for ( int j = 0;  j < 8;  ++j ) {
                if ( byteCRC & 0x80000000L )
                    byteCRC = (byteCRC << 1) ^ CRC32_POLYNOM;
                else
                    byteCRC = (byteCRC << 1);
            }
            sm_CRC32Table[i] = byteCRC;
        }
    }
}

ARCHY_Uint4 checksum_helper::UpdateCRC32(ARCHY_Uint4 checksum, const char* str, size_t count)
{
    for ( size_t j = 0;  j < count;  ++j ) {
        size_t tableIndex = (int(checksum >> 24) ^ *str++) & 0xff;
        checksum = ((checksum << 8) ^ sm_CRC32Table[tableIndex]) & 0xffffffff;
    }
    return checksum;

}




END_NAMESPACE_ARCHY
