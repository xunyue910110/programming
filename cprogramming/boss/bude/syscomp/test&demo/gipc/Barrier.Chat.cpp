#include "Test.h"

#define HELLO "Hello world"

/**
 * Test for queue with broadcasts
 * Usage: start N processes by "Chat N ID" command, where ID = 0,1,...,N-1
 * Output: none
 */
class Chat : public Test { 
  public:
    struct Message {
	unsigned ivalue;
	char     svalue[64];
    };

    void run(Session* session, int id) { 
	int nParticipants = nProcesses;
	Barrier* barrier = session->createBarrier("chat", nParticipants);
	Queue* queue = session->createQueue("chat");

	barrier->waitFor();
	int checksum = 0;
	Message msg;
	strcpy(msg.svalue, HELLO);
	for (int i = 0; i < nIterations; i++) { 	    
	    int msgid = id + i*nParticipants;
	    msg.ivalue = msgid;
	    queue->broadcast(&msg, sizeof(msg));
	    checksum += msgid;
	    for (int j = 1; j < nParticipants; j++) { 	    
		Message* msg = (Message*)queue->get();
		assert(msg->ivalue < (unsigned)nIterations*nParticipants);
		assert(strcmp(msg->svalue, HELLO) == 0);
		checksum += msg->ivalue;
		queue->deallocate(msg);
	    }
	}
	assert(checksum == nIterations*nParticipants*(nIterations*nParticipants-1)/2);
    }
};


int main(int argc, char* argv[]) { 
    Chat test;
    return test.main(argc ,argv);
}

