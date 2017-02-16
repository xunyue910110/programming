#ifndef _STACK_LIST_CODE
#define _STACK_LIST_CODE

// This file contains the methods that were defined in stacklist.h
// header file (i.e container class that stores items in a linked list,
// in stack form)

#include "stacklist.hpp"

namespace archy {

// Initialise internal variables.
//
stack_list::stack_list ()
{
    pCurrentPos    = 0;
    itemCount      = 0;
    spaceAvailable = 0; // space is available
}

// Initalise variables, and place item passed in a new list
//     
// Parameters:
//     pItem    : Pointer to the object that is will to be stored.
//                Item must be of descendant any_object.
//
stack_list::stack_list (any_object* pItem)
{
    pCurrentPos    = 0;
    itemCount      = 0;
    spaceAvailable = 0; // space is available
    push (pItem);
}


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
int stack_list::push (any_object* pItem)
{
    list_link*    newItem = new list_link;

    if (newItem)           // not NULL
    {
         // update contents of structure
         newItem->pItem       = pItem;
         newItem->pLinkedItem = 0;

         if (pCurrentPos)  // not NULL
             newItem->pLinkedItem = pCurrentPos;

         // update start of stack pointer
         pCurrentPos = newItem;
         itemCount++;
         return spaceAvailable;
    }
    else
    {
        spaceAvailable = -1;
        return  spaceAvailable;
    }
}

// Removes a item from the list and returns the value contained within it
// back to the user. A NULL value is returns if there are no more items
// within the list.
//
// Return Values:
//     any_object* : Pointer to the object last object that was placed 
//                  on the stack. Returns NULL pointer if operation
//                  failed.
//
any_object* stack_list::pop ()
{
    list_link*    pBackUp = pCurrentPos;

    if (pCurrentPos) // not NULL
    {
         any_object* pTemp = pCurrentPos->pItem;
         pCurrentPos      = pCurrentPos->pLinkedItem;
         delete pBackUp;
         itemCount--;
         return pTemp;
    }
    else
        return 0;    
}

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
any_object* stack_list::peek (int item)
{
    list_link*   pPeekPos = pCurrentPos;

    // invalid range !
    if ( ((item < 1) || (item > itemCount)) || (pPeekPos == 0) )
              return 0;

    while (item > 1)
    {
        pPeekPos = pPeekPos -> pLinkedItem;
        item--;
    }

    return pPeekPos -> pItem;
}


// Method will remove all list items from memory if they still exist,
// no garbage collection provided, or used.
//
stack_list::~stack_list  ()
{
    any_object* pTest = pop();
    while (pTest != 0)
    {
        delete pTest;
        pTest = pop();
    }

}

}

#endif
