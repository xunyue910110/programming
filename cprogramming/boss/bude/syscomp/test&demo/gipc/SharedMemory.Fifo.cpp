#include <iostream>
#include "Test.h"


/**
 * Test for SharedMemory
 * Usage:
 * Start producer by "SharedMemoryFifo N 0" command, 
 * and start N-1 consumers by "SharedMemoryFifo N i" command, where 1 < i < N
 * Output: none
 */

class SharedMemoryFifo : public Test { 
  public:

    struct QueueElement {
      int    ivalue;
      char   svalue[64];
    };

    void run(Session* session, int id) { 
    	QueueElement element;
    	strcpy(element.svalue, "sharedMemory");
    	element.ivalue = 9999;
    	SharedMemory* queue  = session->createSharedMemory("queue",&element,sizeof(element));      
      SharedMemory* result = session->createSharedMemory("result",&element,sizeof(element));

      Semaphore* semaphorePut = session->createSemaphore("semaphorePut",0);
      
      int i;

      if (id == 0) {           
          int nConsumers = nProcesses - 1;
          
          for (i = 0; i < nConsumers; i++) { 
          	sprintf(element.svalue,"%s","Productor");
          	element.ivalue=i;
            queue->set(&element, sizeof(element));
            Object objDesc(&element, sizeof(element));
            semaphorePut->waitFor(5000); 
            result->get(objDesc);
            printf("get a response from %s : %d\n",element.svalue,element.ivalue);
          }            

      } else {
      	  Object objDesc(&element, sizeof(element));
      	  queue->get(objDesc);
      	  printf("Thread %d receive message from %s : %d\n", id, element.svalue , element.ivalue);
      	  sprintf(element.svalue,"%s","Consumer");
      	  element.ivalue=id;
      	  result->set(&element, sizeof(element));
      	  semaphorePut->signal(1);   
      }
    }
};

int main(int argc, char* argv[]) { 
    SharedMemoryFifo test;
    return test.main(argc ,argv);
}
