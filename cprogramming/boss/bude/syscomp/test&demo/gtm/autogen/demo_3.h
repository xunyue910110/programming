#ifndef DEMO_3_ORACLE
# define DEMO_3_ORACLE

#ifndef OCCI_ORACLE
# include <occi.h>
#endif


/************************************************************/
//  generated declarations for the FULL_NAME object type.
/************************************************************/

class FullName : public oracle::occi::PObject {

protected:

   OCCI_STD_NAMESPACE::string FirstName;
   OCCI_STD_NAMESPACE::string LastName;

public:

   void *operator new(size_t size);

   void *operator new(size_t size, const oracle::occi::Connection * sess,
      const OCCI_STD_NAMESPACE::string& table);

   OCCI_STD_NAMESPACE::string getSQLTypeName() const;

   FullName();

   FullName(void *ctxOCCI_) : oracle::occi::PObject (ctxOCCI_) { };

   static void *readSQL(void *ctxOCCI_);

   virtual void readSQL(oracle::occi::AnyData& streamOCCI_);

   static void writeSQL(void *objOCCI_, void *ctxOCCI_);

   virtual void writeSQL(oracle::occi::AnyData& streamOCCI_);

};

/************************************************************/
//  generated declarations for the ADDRESS object type.
/************************************************************/

class ADDRESS : public oracle::occi::PObject {

protected:

   OCCI_STD_NAMESPACE::string STATE;
   OCCI_STD_NAMESPACE::string ZIP;

public:

   void *operator new(size_t size);

   void *operator new(size_t size, const oracle::occi::Connection * sess,
      const OCCI_STD_NAMESPACE::string& table);

   OCCI_STD_NAMESPACE::string getSQLTypeName() const;

   ADDRESS();

   ADDRESS(void *ctxOCCI_) : oracle::occi::PObject (ctxOCCI_) { };

   static void *readSQL(void *ctxOCCI_);

   virtual void readSQL(oracle::occi::AnyData& streamOCCI_);

   static void writeSQL(void *objOCCI_, void *ctxOCCI_);

   virtual void writeSQL(oracle::occi::AnyData& streamOCCI_);

};

/************************************************************/
//  generated declarations for the PERSON object type.
/************************************************************/

class PERSON : public oracle::occi::PObject {

protected:

   oracle::occi::Number ID;
   FullName * NAME;
   oracle::occi::Ref< ADDRESS > CURR_ADDR;
   OCCI_STD_NAMESPACE::vector< oracle::occi::Ref< ADDRESS > > PREV_ADDR_L;

public:

   void *operator new(size_t size);

   void *operator new(size_t size, const oracle::occi::Connection * sess,
      const OCCI_STD_NAMESPACE::string& table);

   OCCI_STD_NAMESPACE::string getSQLTypeName() const;

   PERSON();

   PERSON(void *ctxOCCI_) : oracle::occi::PObject (ctxOCCI_) { };

   static void *readSQL(void *ctxOCCI_);

   virtual void readSQL(oracle::occi::AnyData& streamOCCI_);

   static void writeSQL(void *objOCCI_, void *ctxOCCI_);

   virtual void writeSQL(oracle::occi::AnyData& streamOCCI_);

};

/************************************************************/
//  generated declarations for the STUDENT object type.
/************************************************************/

class STUDENT : public PERSON {

protected:

   OCCI_STD_NAMESPACE::string SCHOOL_NAME;

public:

   void *operator new(size_t size);

   void *operator new(size_t size, const oracle::occi::Connection * sess,
      const OCCI_STD_NAMESPACE::string& table);

   OCCI_STD_NAMESPACE::string getSQLTypeName() const;

   STUDENT();

   STUDENT(void *ctxOCCI_) : PERSON (ctxOCCI_) { };

   static void *readSQL(void *ctxOCCI_);

   virtual void readSQL(oracle::occi::AnyData& streamOCCI_);

   static void writeSQL(void *objOCCI_, void *ctxOCCI_);

   virtual void writeSQL(oracle::occi::AnyData& streamOCCI_);

};

#endif
