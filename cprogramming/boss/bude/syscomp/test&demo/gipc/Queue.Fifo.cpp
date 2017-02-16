#include "Test.h"

#define HELLO "Hello world"

/**
 * Test for queue
 * Usage:
 * Start producer by "Fifo N 0" command, 
 * and start N-1 consumers by "Fifo N i" command, where 1 < i < N
 * Output: none
 */

class Fifo : public Test { 
  public:

    struct QueueElement {
	int    ivalue;
	char   svalue[64];
    };

    void run(Session* session, int id) { 
	Queue* queue  = session->createQueue("fifo");
	Queue* result = session->createQueue("result");
	QueueElement element;
	int i;

	if (id == 0) { 
	    strcpy(element.svalue, HELLO);
	    int nConsumers = nProcesses - 1;
	    for (i = 1; i <= nIterations; i++) { 
		element.ivalue = i;
		queue->put(&element, sizeof element);
		printf("put %d\n",i);
	    }
	    // Notify consumers about termination
	    element.ivalue = 0;
	    for (i = 0; i < nConsumers; i++) { 
		queue->put(&element, sizeof element);
	    }		
	    int checksum = 0;
	    *element.svalue = '\0';
	    for (i = 0; i < nConsumers; i++) { 
		Object objDesc(&element, sizeof(element));
		result->get(objDesc);
		printf("get a response from Consumer:%d,%s\n",i,element.svalue);
		//assert(strcmp(element.svalue, HELLO) == 0);
		checksum += element.ivalue;
	    }
	    assert(checksum == nIterations*(nIterations+1)/2);

	} else { 
	    int checksum = 0;
	    while (true) { 
		Object objDesc(&element, sizeof(element));
		queue->get(objDesc);
		printf("Thread %d receive message %d\n", id, element.ivalue);
		if (element.ivalue == 0) { 
		    break;
		}
		checksum += element.ivalue;
		
	    }
	    element.ivalue = checksum;
	    strcpy(element.svalue,"consumer");
	    result->put(&element, sizeof(element));
	}
	queue->close();
	result->close();
	session->close();
    }
};



int main(int argc, char* argv[]) { 
    Fifo test;
    return test.main(argc ,argv);
}


