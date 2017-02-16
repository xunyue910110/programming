#ifndef __TEST_H__
#define __TEST_H__

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "Thread.h"
#include "Factory.h"
#include "Session.h"
#include "ServerFactory.h"
#include "ClientFactory.h"


class Test { 
  public:

    //多线程管理类
    class TestThread : public Thread {  
      public:
        void start() { 
            Thread::start(&runTest, this);
        }
        static void thread_proc runTest(void* arg) { 
            
            ((TestThread*)arg)->run();
        }
        void run() { 
            test->startSession(id);
        }
        TestThread(Test* test, int id) { 
            this->test = test;
            this->id = id;
        }
        int id;
        Test* test;
    };


    virtual void run(Session* session, int id) = 0;

    void startSession(int id) { 
        Session* session = getSession();
        run(session, id);
        delete session;
    }

    Session* getSession() { 
        return factory->create(hostname, port);
    }

    int main(int argc, char* argv[]) 
    { 
        int i;
        int nParams = 0;
        int param;
        int id = 0;
       
        fprintf(stderr,"OK startup!\n");
        
        for (i = 1; i < argc; i++) { 
            if (*argv[i] == '-') { 
                if (strcmp(argv[i], "-port") == 0) { 
                    if (i+1 == argc || sscanf(argv[i+1], "%d", &port) != 1) { 
                        fprintf(stderr, "Invalid port number\n");
                        return 1;
                    }
                    i += 1;
                } else if (strcmp(argv[i], "-iterations") == 0) { 
                    if (i+1 == argc || sscanf(argv[i+1], "%d", &nIterations) != 1) { 
                        fprintf(stderr, "Invalid number of iterations\n");
                        return 1;
                    }
                    i += 1;
                } else if (strcmp(argv[i], "-host") == 0) { 
                    if (i+1 == argc) { 
                        fprintf(stderr, "Hostname is missing\n");
                        return 1;
                    }
                    hostname = argv[++i];
                } else if (strcmp(argv[i], "-nodeadlock") == 0) { 
                    options |= NO_DEADLOCK;
                } else if (strcmp(argv[i], "-timed") == 0) { 
                    options |= TIMED_WAIT;
                } else if (strcmp(argv[i], "-timeout") == 0) { 
                    if (i+1 == argc || sscanf(argv[i+1], "%d", &timeout) != 1) { 
                        fprintf(stderr, "Invalid value of timeout\n");
                        return 1;
                    }
                    i += 1;
                } else { 
                    fprintf(stderr, "Unknown option %s\n", argv[i]);
                    return 1;
                }
            } else { 
                if (sscanf(argv[i], "%d", &param) != 1) { 
                    fprintf(stderr, "Invalid parameter (integer number excepted)\n");
                    return 1;
                }
                if (nParams == 0) { 
                    nProcesses = param;
                } else if (nParams == 1) { 
                    id = param;
                } else { 
                    fprintf(stderr, "Test should be called with one or two parameters: N_PROCESSES [ID]\n");
                    return 1;
                }
                nParams += 1;
            }                   
        }
        if (port == 0) { 
            factory = ServerFactory::getInstance();
        } else { 
            factory = ClientFactory::getInstance();
        }

        time_t started = time(NULL);
        
        printf("ID:%d\n",id);
        printf("nProcesses:%d\n",nProcesses);

        if (nParams == 2) {
            startSession(id);
        } else { 
            TestThread** tests = new TestThread*[nProcesses];
            
            for (i = 0; i < nProcesses; i++) { 
                tests[i] = new TestThread(this, i);
                tests[i]->start();
            }

            //等待各个线程结束
            for (i = 0; i < nProcesses; i++) { 	
                tests[i]->join();
            }
            printf("Elapsed time %u\n", (unsigned)(time(NULL) - started));
        }
        return 0;
    }
    
    Test() 
    {
        port = 6000;
        hostname = "localhost";
        nProcesses = 5;
        options = 0;
        nIterations = 1000;
        timeout = 100;
    }

    enum Options { 
        NO_DEADLOCK = 1,
        TIMED_WAIT  = 2
    };

    int          port;
    int          nProcesses;
    char const*  hostname;
    int          options;
    int          nIterations;
    unsigned     timeout;
    Factory*     factory;
};

#endif

 

