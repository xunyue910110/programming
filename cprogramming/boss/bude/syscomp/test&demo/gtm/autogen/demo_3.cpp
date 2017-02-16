#ifndef DEMO_3_ORACLE
# include "demo_3.h"
#endif


/*****************************************************************/
//  generated method implementations for the FULL_NAME object type.
/*****************************************************************/

void *FullName::operator new(size_t size)
{
  return oracle::occi::PObject::operator new(size);
}

void *FullName::operator new(size_t size, const oracle::occi::Connection * sess,
  const OCCI_STD_NAMESPACE::string& table)
{
  return oracle::occi::PObject::operator new(size, sess, table, 
            (char *) "GUEST.FULL_NAME");
}

OCCI_STD_NAMESPACE::string FullName::getSQLTypeName() const
{
  return OCCI_STD_NAMESPACE::string("GUEST.FULL_NAME");
}

FullName::FullName()
{
}

void *FullName::readSQL(void *ctxOCCI_)
{
  FullName *objOCCI_ = new FullName(ctxOCCI_);
  oracle::occi::AnyData streamOCCI_(ctxOCCI_);

  try
  {
    if (streamOCCI_.isNull())
      objOCCI_->setNull();
    else
      objOCCI_->readSQL(streamOCCI_);
  }
  catch (oracle::occi::SQLException& excep)
  {
    delete objOCCI_;
    excep.setErrorCtx(ctxOCCI_);
    return (void *)NULL;
  }
  return (void *)objOCCI_;
}

void FullName::readSQL(oracle::occi::AnyData& streamOCCI_)
{
   FirstName = streamOCCI_.getString();
   LastName = streamOCCI_.getString();
}

void FullName::writeSQL(void *objectOCCI_, void *ctxOCCI_)
{
  FullName *objOCCI_ = (FullName *) objectOCCI_;
  oracle::occi::AnyData streamOCCI_(ctxOCCI_);

  try
  {
    if (objOCCI_->isNull())
      streamOCCI_.setNull();
    else
      objOCCI_->writeSQL(streamOCCI_);
  }
  catch (oracle::occi::SQLException& excep)
  {
    excep.setErrorCtx(ctxOCCI_);
  }
  return;
}

void FullName::writeSQL(oracle::occi::AnyData& streamOCCI_)
{
   streamOCCI_.setString(FirstName);
   streamOCCI_.setString(LastName);
}

/*****************************************************************/
//  generated method implementations for the ADDRESS object type.
/*****************************************************************/

void *ADDRESS::operator new(size_t size)
{
  return oracle::occi::PObject::operator new(size);
}

void *ADDRESS::operator new(size_t size, const oracle::occi::Connection * sess,
  const OCCI_STD_NAMESPACE::string& table)
{
  return oracle::occi::PObject::operator new(size, sess, table, 
            (char *) "GUEST.ADDRESS");
}

OCCI_STD_NAMESPACE::string ADDRESS::getSQLTypeName() const
{
  return OCCI_STD_NAMESPACE::string("GUEST.ADDRESS");
}

ADDRESS::ADDRESS()
{
}

void *ADDRESS::readSQL(void *ctxOCCI_)
{
  ADDRESS *objOCCI_ = new ADDRESS(ctxOCCI_);
  oracle::occi::AnyData streamOCCI_(ctxOCCI_);

  try
  {
    if (streamOCCI_.isNull())
      objOCCI_->setNull();
    else
      objOCCI_->readSQL(streamOCCI_);
  }
  catch (oracle::occi::SQLException& excep)
  {
    delete objOCCI_;
    excep.setErrorCtx(ctxOCCI_);
    return (void *)NULL;
  }
  return (void *)objOCCI_;
}

void ADDRESS::readSQL(oracle::occi::AnyData& streamOCCI_)
{
   STATE = streamOCCI_.getString();
   ZIP = streamOCCI_.getString();
}

void ADDRESS::writeSQL(void *objectOCCI_, void *ctxOCCI_)
{
  ADDRESS *objOCCI_ = (ADDRESS *) objectOCCI_;
  oracle::occi::AnyData streamOCCI_(ctxOCCI_);

  try
  {
    if (objOCCI_->isNull())
      streamOCCI_.setNull();
    else
      objOCCI_->writeSQL(streamOCCI_);
  }
  catch (oracle::occi::SQLException& excep)
  {
    excep.setErrorCtx(ctxOCCI_);
  }
  return;
}

void ADDRESS::writeSQL(oracle::occi::AnyData& streamOCCI_)
{
   streamOCCI_.setString(STATE);
   streamOCCI_.setString(ZIP);
}

/*****************************************************************/
//  generated method implementations for the PERSON object type.
/*****************************************************************/

void *PERSON::operator new(size_t size)
{
  return oracle::occi::PObject::operator new(size);
}

void *PERSON::operator new(size_t size, const oracle::occi::Connection * sess,
  const OCCI_STD_NAMESPACE::string& table)
{
  return oracle::occi::PObject::operator new(size, sess, table, 
            (char *) "GUEST.PERSON");
}

OCCI_STD_NAMESPACE::string PERSON::getSQLTypeName() const
{
  return OCCI_STD_NAMESPACE::string("GUEST.PERSON");
}

PERSON::PERSON()
{
   NAME = (FullName *) 0;
}

void *PERSON::readSQL(void *ctxOCCI_)
{
  PERSON *objOCCI_ = new PERSON(ctxOCCI_);
  oracle::occi::AnyData streamOCCI_(ctxOCCI_);

  try
  {
    if (streamOCCI_.isNull())
      objOCCI_->setNull();
    else
      objOCCI_->readSQL(streamOCCI_);
  }
  catch (oracle::occi::SQLException& excep)
  {
    delete objOCCI_;
    excep.setErrorCtx(ctxOCCI_);
    return (void *)NULL;
  }
  return (void *)objOCCI_;
}

void PERSON::readSQL(oracle::occi::AnyData& streamOCCI_)
{
   ID = streamOCCI_.getNumber();
   NAME = (FullName *) streamOCCI_.getObject();
   CURR_ADDR = streamOCCI_.getRef();
   getVector(streamOCCI_, PREV_ADDR_L);
}

void PERSON::writeSQL(void *objectOCCI_, void *ctxOCCI_)
{
  PERSON *objOCCI_ = (PERSON *) objectOCCI_;
  oracle::occi::AnyData streamOCCI_(ctxOCCI_);

  try
  {
    if (objOCCI_->isNull())
      streamOCCI_.setNull();
    else
      objOCCI_->writeSQL(streamOCCI_);
  }
  catch (oracle::occi::SQLException& excep)
  {
    excep.setErrorCtx(ctxOCCI_);
  }
  return;
}

void PERSON::writeSQL(oracle::occi::AnyData& streamOCCI_)
{
   streamOCCI_.setNumber(ID);
   streamOCCI_.setObject(NAME);
   streamOCCI_.setRef(CURR_ADDR);
   setVector(streamOCCI_, PREV_ADDR_L);
}

/*****************************************************************/
//  generated method implementations for the STUDENT object type.
/*****************************************************************/

void *STUDENT::operator new(size_t size)
{
  return oracle::occi::PObject::operator new(size);
}

void *STUDENT::operator new(size_t size, const oracle::occi::Connection * sess,
  const OCCI_STD_NAMESPACE::string& table)
{
  return oracle::occi::PObject::operator new(size, sess, table, 
            (char *) "GUEST.STUDENT");
}

OCCI_STD_NAMESPACE::string STUDENT::getSQLTypeName() const
{
  return OCCI_STD_NAMESPACE::string("GUEST.STUDENT");
}

STUDENT::STUDENT()
{
}

void *STUDENT::readSQL(void *ctxOCCI_)
{
  STUDENT *objOCCI_ = new STUDENT(ctxOCCI_);
  oracle::occi::AnyData streamOCCI_(ctxOCCI_);

  try
  {
    if (streamOCCI_.isNull())
      objOCCI_->setNull();
    else
      objOCCI_->readSQL(streamOCCI_);
  }
  catch (oracle::occi::SQLException& excep)
  {
    delete objOCCI_;
    excep.setErrorCtx(ctxOCCI_);
    return (void *)NULL;
  }
  return (void *)objOCCI_;
}

void STUDENT::readSQL(oracle::occi::AnyData& streamOCCI_)
{
   PERSON::readSQL(streamOCCI_);
   SCHOOL_NAME = streamOCCI_.getString();
}

void STUDENT::writeSQL(void *objectOCCI_, void *ctxOCCI_)
{
  STUDENT *objOCCI_ = (STUDENT *) objectOCCI_;
  oracle::occi::AnyData streamOCCI_(ctxOCCI_);

  try
  {
    if (objOCCI_->isNull())
      streamOCCI_.setNull();
    else
      objOCCI_->writeSQL(streamOCCI_);
  }
  catch (oracle::occi::SQLException& excep)
  {
    excep.setErrorCtx(ctxOCCI_);
  }
  return;
}

void STUDENT::writeSQL(oracle::occi::AnyData& streamOCCI_)
{
   PERSON::writeSQL(streamOCCI_);
   streamOCCI_.setString(SCHOOL_NAME);
}
