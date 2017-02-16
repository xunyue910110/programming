
#ifndef REGISTERMAPPINGS_3_ORACLE
# include "RegisterMappings_3.h"
#endif

void RegisterMappings_3(oracle::occi::Environment* envOCCI_)
{
  oracle::occi::Map *mapOCCI_ = envOCCI_->getMap();
  mapOCCI_->put("GUEST.FULL_NAME", FullName::readSQL, FullName::writeSQL);
  mapOCCI_->put("GUEST.ADDRESS", ADDRESS::readSQL, ADDRESS::writeSQL);
  mapOCCI_->put("GUEST.PERSON", PERSON::readSQL, PERSON::writeSQL);
  mapOCCI_->put("GUEST.STUDENT", STUDENT::readSQL, STUDENT::writeSQL);
}
