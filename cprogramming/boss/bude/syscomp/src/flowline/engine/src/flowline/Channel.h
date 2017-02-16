#ifndef _CHANNEL_
#define _CHANNEL_
#include "Element.h"
#include "Definition.h"

#include <sstream>
USING_NAMESPACE(std);

namespace flowline {
class Channel : public Element {
public:    
    void setChannelNo(int channelNo) {
        m_channelNo = channelNo;
        ostringstream buffer;
        buffer << "ch" << m_channelNo;
        setName(buffer.str());
    }
    
    int getChannelNo() {
        return m_channelNo;
    }
    
    Definition& getDefinition() {
        return *m_definition;
    }
    
    void setDefinition(Definition* def) {
        m_definition = def;
    }
    
private:
    int m_channelNo;
    Definition* m_definition;
};

}
#endif /*_CHANNEL_*/
