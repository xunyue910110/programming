#include "runtimeclass.hpp"
#include <string.h>


	
static char szCObject[] = "root_object";
struct runtime_class root_object::classroot_object =
        { szCObject, NULL, NULL };
static AFX_CLASSINIT _init_CObject(&root_object::classroot_object);


runtime_class* runtime_class::pFirstClass = NULL;

AFX_CLASSINIT::AFX_CLASSINIT(runtime_class* pNewClass)
{
        pNewClass->m_pNextClass = runtime_class::pFirstClass;
        runtime_class::pFirstClass = pNewClass;
}

root_object* runtime_class::create_object()
{
        return (*m_pfnCreateObject)();
}

runtime_class* runtime_class::load(LPCSTR lpszClassName)
{
        runtime_class* pClass;

		
        for (pClass = pFirstClass; pClass != NULL; pClass = pClass->m_pNextClass)
        {
                if ( strcmp(lpszClassName, pClass->m_lpszClassName) == 0 )
                        return pClass;
        }
        return NULL; // not found
}

root_object::root_object()
{
}

root_object::~root_object()
{
}


runtime_class* root_object::get_runtime_class() const
{
        return &root_object::classroot_object;
}

bool root_object::is_kindof(const runtime_class* pClass) const
{
        runtime_class* pClassThis = get_runtime_class();
        while (pClassThis != NULL)
        {
                if (pClassThis == pClass)
                        return true;
                pClassThis = pClassThis->m_pBaseClass;
        }
        return false;       // walked to the top, no match
}



