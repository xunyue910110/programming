// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "Calculator.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/TToString.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::tutorial;
using namespace ::shared;
using namespace std;

class CalculatorHandler : virtual public CalculatorIf {
 public:
  CalculatorHandler() {
    // Your initialization goes here
  }

  void ping() {
    // Your implementation goes here
    printf("ping\n");
  }

  int32_t add(const int32_t num1, const int32_t num2) {
    // Your implementation goes here
    cout << "add(" << num1 << ", " << num2 << ")" << endl;
    return num1 + num2;
  }

  int32_t calculate(const int32_t logid, const Work& w) {
    // Your implementation goes here
    cout << "calculate(" << logid << ", " << w << ")" << endl;
    int32_t val;

    switch (w.op){
      case Operation::ADD:
        val = w.num1 + w.num2;
        break;
      case Operation::SUBSTRACT:
        val = w.num1 - w.num2;
        break;
      case Operation::MULTIPLY:
        val = w.num1 * w.num2;
        break;
      case Operation::DIVIDE:
        if ( w.num2 == 0){
          InvalidOperation io;
          io.whatOp = w.op;
          io.why = "Cannot divide by 0";
          throw io;
        }
        val = w.num1 / w.num2;
        break;
      default:
        InvalidOperation io;
        io.whatOp = w.op;
        io.why = "Invalid Operation";
        throw io;
      }

    SharedStruct ss;
    ss.key = logid;
    ss.value = to_string(val);

    log[logid] == ss;
    return val;
  }

  void getStruct(SharedStruct& ret, const int32_t logid){
    cout << "getStruct(" << logid << ")" << endl;
    ret = log[logid];
  }

  void zip() {
    // Your implementation goes here
    printf("zip\n");
  }
  
 protected:
  map<int32_t, SharedStruct> log;
};

int main(int argc, char **argv) {
  int port = 9090;
  shared_ptr<CalculatorHandler> handler(new CalculatorHandler());
  shared_ptr<TProcessor> processor(new CalculatorProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
  return 0;
}

