/**
 * Autogenerated by Thrift Compiler (0.9.3)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#include "tutorial_types.h"

#include <algorithm>
#include <ostream>

#include <thrift/TToString.h>

namespace tutorial {

int _kOperationValues[] = {
  Operation::ADD,
  Operation::SUBSTRACT,
  Operation::MULTIPLY,
  Operation::DIVIDE
};
const char* _kOperationNames[] = {
  "ADD",
  "SUBSTRACT",
  "MULTIPLY",
  "DIVIDE"
};
const std::map<int, const char*> _Operation_VALUES_TO_NAMES(::apache::thrift::TEnumIterator(4, _kOperationValues, _kOperationNames), ::apache::thrift::TEnumIterator(-1, NULL, NULL));


Work::~Work() throw() {
}


void Work::__set_num1(const int32_t val) {
  this->num1 = val;
}

void Work::__set_num2(const int32_t val) {
  this->num2 = val;
}

void Work::__set_op(const Operation::type val) {
  this->op = val;
}

void Work::__set_comment(const std::string& val) {
  this->comment = val;
__isset.comment = true;
}

uint32_t Work::read(::apache::thrift::protocol::TProtocol* iprot) {

  apache::thrift::protocol::TInputRecursionTracker tracker(*iprot);
  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->num1);
          this->__isset.num1 = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->num2);
          this->__isset.num2 = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 3:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          int32_t ecast0;
          xfer += iprot->readI32(ecast0);
          this->op = (Operation::type)ecast0;
          this->__isset.op = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 4:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->comment);
          this->__isset.comment = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t Work::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  apache::thrift::protocol::TOutputRecursionTracker tracker(*oprot);
  xfer += oprot->writeStructBegin("Work");

  xfer += oprot->writeFieldBegin("num1", ::apache::thrift::protocol::T_I32, 1);
  xfer += oprot->writeI32(this->num1);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("num2", ::apache::thrift::protocol::T_I32, 2);
  xfer += oprot->writeI32(this->num2);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("op", ::apache::thrift::protocol::T_I32, 3);
  xfer += oprot->writeI32((int32_t)this->op);
  xfer += oprot->writeFieldEnd();

  if (this->__isset.comment) {
    xfer += oprot->writeFieldBegin("comment", ::apache::thrift::protocol::T_STRING, 4);
    xfer += oprot->writeString(this->comment);
    xfer += oprot->writeFieldEnd();
  }
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

void swap(Work &a, Work &b) {
  using ::std::swap;
  swap(a.num1, b.num1);
  swap(a.num2, b.num2);
  swap(a.op, b.op);
  swap(a.comment, b.comment);
  swap(a.__isset, b.__isset);
}

Work::Work(const Work& other1) {
  num1 = other1.num1;
  num2 = other1.num2;
  op = other1.op;
  comment = other1.comment;
  __isset = other1.__isset;
}
Work& Work::operator=(const Work& other2) {
  num1 = other2.num1;
  num2 = other2.num2;
  op = other2.op;
  comment = other2.comment;
  __isset = other2.__isset;
  return *this;
}
void Work::printTo(std::ostream& out) const {
  using ::apache::thrift::to_string;
  out << "Work(";
  out << "num1=" << to_string(num1);
  out << ", " << "num2=" << to_string(num2);
  out << ", " << "op=" << to_string(op);
  out << ", " << "comment="; (__isset.comment ? (out << to_string(comment)) : (out << "<null>"));
  out << ")";
}


InvalidOperation::~InvalidOperation() throw() {
}


void InvalidOperation::__set_whatOp(const int32_t val) {
  this->whatOp = val;
}

void InvalidOperation::__set_why(const std::string& val) {
  this->why = val;
}

uint32_t InvalidOperation::read(::apache::thrift::protocol::TProtocol* iprot) {

  apache::thrift::protocol::TInputRecursionTracker tracker(*iprot);
  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->whatOp);
          this->__isset.whatOp = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->why);
          this->__isset.why = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t InvalidOperation::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  apache::thrift::protocol::TOutputRecursionTracker tracker(*oprot);
  xfer += oprot->writeStructBegin("InvalidOperation");

  xfer += oprot->writeFieldBegin("whatOp", ::apache::thrift::protocol::T_I32, 1);
  xfer += oprot->writeI32(this->whatOp);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("why", ::apache::thrift::protocol::T_STRING, 2);
  xfer += oprot->writeString(this->why);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

void swap(InvalidOperation &a, InvalidOperation &b) {
  using ::std::swap;
  swap(a.whatOp, b.whatOp);
  swap(a.why, b.why);
  swap(a.__isset, b.__isset);
}

InvalidOperation::InvalidOperation(const InvalidOperation& other3) : TException() {
  whatOp = other3.whatOp;
  why = other3.why;
  __isset = other3.__isset;
}
InvalidOperation& InvalidOperation::operator=(const InvalidOperation& other4) {
  whatOp = other4.whatOp;
  why = other4.why;
  __isset = other4.__isset;
  return *this;
}
void InvalidOperation::printTo(std::ostream& out) const {
  using ::apache::thrift::to_string;
  out << "InvalidOperation(";
  out << "whatOp=" << to_string(whatOp);
  out << ", " << "why=" << to_string(why);
  out << ")";
}

const char* InvalidOperation::what() const throw() {
  try {
    std::stringstream ss;
    ss << "TException - service has thrown: " << *this;
    this->thriftTExceptionMessageHolder_ = ss.str();
    return this->thriftTExceptionMessageHolder_.c_str();
  } catch (const std::exception&) {
    return "TException - service has thrown: InvalidOperation";
  }
}

} // namespace
