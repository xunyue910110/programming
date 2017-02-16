

#include "FSystemAgent.h"

#include "FTPImpl.h"
#include "DTAImpl.h"
#include "FTAMImpl.h"
#include "FILEImpl.h"


FSystemAgent* FSystemAgent::getInstance(const string & type) 
{
  FSystemAgent * transfer = NULL;

  if (type == "DTA") {
    transfer = new DTAImpl();
  } 
  else if (type == "FTAM") {
    transfer = new FTAMImpl();
  } 
    else if (type == "FILE") {
    transfer = new FILEImpl();
  } 
  else {
    transfer = new FTPImpl();
  }

  return transfer;
}

