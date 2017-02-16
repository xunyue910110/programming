/***************************************************************** lm-cpr beg
 *
 * AI Loom - A cross platform artificial intelligence framework
 * AI Loom is (c) Copyright 2003 Josh Brown
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * -----------------------------------------------------------------
 * File:          $RCSfile: UUID.cpp,v $
 * Date modified: $Date: 2011/04/15 07:35:53 $
 * Version:       $Revision: 1.1 $
 * -----------------------------------------------------------------
 * -----------------------------------------------------------------
 * Mark:          $编译选项中  加上"-luuid"  $
 * Last modified: $juyf@lianchuang.com       $	
 * Date modified: $Date: 2011/04/15 07:35:53 $
 * Version:       $Revision: 1.1 $
 * -----------------------------------------------------------------
 * 
 ************************************************************** lm-cpr-end */

#include "UUID.h"
#include <string>
#include <ctype.h>

#include <iostream>
#include <sstream>

#include <sys/time.h>
#include <unistd.h>
#include <netdb.h>
#include <math.h>


   using namespace std;

   const char* UUID::ch = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-_"; 	
   const UUID UUID::NullUUID("00000000-0000-0000-0000-000000000000");
  
     
   UUID::UUID()
      : mUuid(NullUUID.mUuid)
   {
   }

   UUID::UUID(const std::string& uuid)
   {
      makeFromString(uuid);      
   }

   UUID::UUID(const UUID& uuid)
   {
      memcpy(&mUuid, &uuid.mUuid, sizeof(germ_uuid_t));     
   }

   UUID&
   UUID::operator=(const UUID& uuid)
   {
      // Make sure this isn't an assignment against ourselves
      if (&uuid != this)
      {
         memcpy(&mUuid, &uuid.mUuid, sizeof(germ_uuid_t));
      }

      return *this;
   }

   UUID 
   UUID::generate()
   {
      // Generate the UUID
      UUID result;
      //uuid_t uuid;
      //uuid_generate(uuid);
      //memcpy(&result.mUuid.standard, uuid, sizeof(germ_uuid_t));    
            
      result.mUuid.packed.p0=  result.getUIP();        
      result.mUuid.packed.p1=  result.getPID(); 
      result.mUuid.packed.p2=  result.getCurrentTime();           
      result.mUuid.packed.p3 = result.getCount();     
     
      return result;
   	}
   
    
    
    static time_t g_uuid_starttime = time(0);
    static long g_uuid_count = 0;
    
    unsigned long UUID::getUIP(){    	
	    //取本机名
	    char hostname[ 32 ];
	    if ( gethostname( hostname, 32 ) ) {
	        cout << "[uuid]获取本机名失败" <<endl;
	    }
	    //in_addr_t ip2;	
	    hostent* host = gethostbyname( hostname );
	    if ( host != NULL ) {
	         return ( ( in_addr * ) * host->h_addr_list )->s_addr ;
		}   	
		return 8;
    }
	long UUID::getPID(){
		 return getpid();		
	}
	
	long UUID::getCurrentTime(){		     	       
	     return time(0);
	}
	
	long UUID::getCount(){
		 g_uuid_count ++ ;
		 return g_uuid_count;
    }
     
   
   bool
   UUID::isNull() const
   {
      return operator==(NullUUID);
   }

   std::string
   UUID::str() const
   {
      char guid_str[37];

      /* Create the formatted string
      snprintf(guid_str, 37,
              "%08lx-%04hx-%04hx-%02x%02x-%02x%02x%02x%02x%02x%02x",
              mUuid.standard.m0,
              mUuid.standard.m1,
              mUuid.standard.m2,
              (u16)mUuid.standard.m3,
              (u16)mUuid.standard.m4,
              (u16)mUuid.standard.m5[0],
              (u16)mUuid.standard.m5[1],
              (u16)mUuid.standard.m5[2],
              (u16)mUuid.standard.m5[3],
              (u16)mUuid.standard.m5[4],
              (u16)mUuid.standard.m5[5]                
      );
      */
    snprintf(guid_str, 37,
              "%08lx-%08lx-%08lx-%08lx",
               mUuid.packed.p0,
               mUuid.packed.p1,
               mUuid.packed.p2,
               mUuid.packed.p3);
      return std::string(guid_str);
   }

	std::string UUID::ascii() const{
	
	  ostringstream oss;
	  u64 m = mUuid.ascii.a0;
	  int u ;
	  int i;
	  for(i=0;i < 11;i++){	  	
	  	u = m & 0x3F;
	  	oss << ch[u];
	    m =	m >> 6;
      }
       m = mUuid.ascii.a1;
       for(i=0;i < 11;i++){	  	
	  	u = m & 0x3F ;
	  	oss << ch[u];
	  	m =	m >> 6;
      }      
      return oss.str();	
	}
    
	void UUID::testascii(){
		 cout << "-------------------"<<endl;
      
		 memset(&mUuid,0xFF,sizeof(mUuid));	
	     cout << ascii() << endl;
	}
 
 

   bool
   UUID::operator==(const UUID& uuid) const
   {
      return ((mUuid.packed.p0 == uuid.mUuid.packed.p0) &&
              (mUuid.packed.p1 == uuid.mUuid.packed.p1) &&
              (mUuid.packed.p2 == uuid.mUuid.packed.p2) &&
              (mUuid.packed.p3 == uuid.mUuid.packed.p3));
   }

   bool
   UUID::operator!=(const UUID& uuid) const
   {
      return !operator==(uuid);
   }

   bool
   UUID::operator<(const UUID& uuid) const
   {
      bool is_less(false);

      // Check first 4 bytes
      if (mUuid.packed.p0 < uuid.mUuid.packed.p0)
      {
         is_less = true;
      }
      else if (mUuid.packed.p0 == uuid.mUuid.packed.p0)
      {
         // Check second 4 bytes
         if (mUuid.packed.p1 < uuid.mUuid.packed.p1)
         {
            is_less = true;
         }
         else if (mUuid.packed.p1 == uuid.mUuid.packed.p1)
         {
            // Check third 4 bytes
            if (mUuid.packed.p2 < uuid.mUuid.packed.p2)
            {
               is_less = true;
            }
            else if (mUuid.packed.p2 == uuid.mUuid.packed.p2)
            {
               // Check last 4 bytes
               if (mUuid.packed.p3 < uuid.mUuid.packed.p3)
               {
                  is_less = true;
               }
            }
         }
      }

      return is_less;
   }

   void
   UUID::makeFromString(const std::string& uuid)
   {
      //ASSERT(uuid.size() == 36);
      //ASSERT(isalpha(uuid[0]) || isdigit(uuid[0]));

      // Use a u16 buffer for the 8 bit values to avoid alignment problems
      u16 buf[8];
      sscanf(uuid.c_str(),
             "%08lx-%04hx-%04hx-%02hx%02hx-%02hx%02hx%02hx%02hx%02hx%02hx",
             &mUuid.standard.m0,
             &mUuid.standard.m1,
             &mUuid.standard.m2,
             &buf[0],
             &buf[1],
             &buf[2],
             &buf[3],
             &buf[4],
             &buf[5],
             &buf[6],
             &buf[7]
      );

      // Fill in the remainder of mUuid
      mUuid.standard.m3 = (u8)buf[0];
      mUuid.standard.m4 = (u8)buf[1];
      for (int i=2; i<8; ++i)
      {
         mUuid.standard.m5[i-2] = (u8)buf[i];
      }
   }

   std::ostream&
   operator<<(std::ostream& out, const UUID& uuid)
   {
      return out << uuid.str();
   }


/*
int main(){
   for (int i = 0;i<10 ; i++){
   UUID uid = UUID::generate();   
   cout << uid << endl;
   string s = uid.ascii();
   cout << s <<" size :" << s.size() << endl;  
   }

  UUID uid=  UUID::generate();   
  uid.testascii();
  

  return 1;
} 
*/
