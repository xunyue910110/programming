#ifndef _STACK_LIST_HEADER
#define _STACK_LIST_HEADER

// This header definition contains information of the construction,
// operation of a container class that holds data in linked list
// in stack form.

#include "anyobj.hpp"             // use Base class definition

namespace archy {
// Structure definition used to link the items in the stack
struct list_link
{
      list_link*   pLinkedItem;  // linker (pointer to next item in the list)
      any_object*  pItem;        // variable used to contain the data
};

class stack_list : public any_object
{
                                            // is still available
public:
	stack_list();

	// Initialise variables, and place item passed in a new list
	//     
	// Parameters:
	//     pItem    : Pointer to the object that is will to be stored.
	//                Item must be of descendant any_object.
	//
	stack_list(any_object* pItem);
          
	// Places a new item in the list (i.e on the stack). 
	//
	// Parameters:
	//     pItem    : Pointer to the object that is will to be stored.
	//                Item must be of descendant any_object.
	//
	// Return Values:
	//     int      : Returns a error code value to indicate whether operation
	//                was successful or not.  
	//                Value:
	//                0  =  No Worries, item stacked.
	//               -1  =  Item not stacked, memory allocation failure
	//
	int push(any_object* pItem);

	// Removes a item from the list and returns the value contained within it
	// back to the user. A NULL value is returns if there are no more items
	// within the list.
	//
	// Return Values:
	//     any_object* : Pointer to the object last object that was placed 
	//                  on the stack. Returns NULL pointer if operation
	//                  failed.
	//
	any_object* pop();

	// Peeks at items within the linked list without removing
	// them from the list.
	//
	// Parameters:
	//    int item :     item number in list.
	//
	// Return Values:
	//   any_object*  : Returns NULL if operation failed, else
	//                 pointer to the object contained at list
	//                 number selected!
	//
	any_object* peek (int item);

	// Returns the number of items current being stacked.
	//
	// Returns Values:
	//     int :    Num of items within queue.
	//
	int  size(){ return itemCount;    }

	// Method returns whether last operation failed due to memory allocation
	// failure.
	//
	// Return Values:
	//     int  : Returns 1 of two values ...
	//            Values:
	//              0  =  memory available
	//             -1  =  Last memory allocation failed.
	//       
	int space() { return spaceAvailable;   }

	// Method will remove all list items from memory if they still exist,
	// no garbage collection provided, or used.
	//
	~stack_list();

private:
	list_link*     pCurrentPos;        // pointer to the end of the list
	int           itemCount;          // number of items in list
	int           spaceAvailable;     // used to test if memory

};

}

#endif
