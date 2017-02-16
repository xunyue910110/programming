#include "FPush.h"

FPush::FPush(size_t maxLength)
{
    m_lineBuffer = new char[maxLength + 1];
    m_maxLength = maxLength;
}

FPush::~FPush()
{
    delete[] m_lineBuffer;
}

void FPush::open(const string& fileName)
{
    m_fileName = fileName;
    m_fileName += ".w";
}

void FPush::close()
{
}

void FPush::push(const char* data)
{
    m_os.clear();
    m_os.open(m_fileName.c_str(), ios::app | ios::out);
    if(m_os.fail())
    {
        throw FPushException("Open file error.");
    }

    escape(data, m_lineBuffer, m_maxLength);

    m_os << m_lineBuffer << endl << flush;

    m_os.close();
}

void FPush::escape(const char *msg, char *buffer, size_t size)
{
    char *q = buffer;
    for(const char *p = msg; *p != '\0' && q - buffer < size - 3; p++)
    {
        if(*p == '\n')
        {
            *q++ = '\\';
            *q++ = 'n';
        }
        else
        {
            *q++ = *p;
        }
    }
    *q = '\0';
}
