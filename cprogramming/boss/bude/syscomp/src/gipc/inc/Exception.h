#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

class Exception {};

class DeadlockException : public Exception {};

class InternalException : public Exception {};

class InvalidParameterException : public Exception {};

class NotFoundException : public Exception {};

class NotOwnerException : public Exception {};

class InterruptedException : public Exception {};

#endif
