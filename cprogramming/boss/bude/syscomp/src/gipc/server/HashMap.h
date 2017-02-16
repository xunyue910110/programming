#ifndef __HASH_MAP_H__
#define __HASH_MAP_H__

#include "PrimitiveImpl.h"

class HashMap { 
  public:
    class Entry { 
      public:
	PrimitiveImpl* prim;
	Entry*             next;
	unsigned           hashCode;
    };

    static inline unsigned hashCode(char const* s) 
    { 
	unsigned h = 0;
	while (*s != '\0') { 
	    h = h*31 + (*s++ & 0xFF);
	}
	return h;
    }

    void put(PrimitiveImpl* prim) 
    { 
	if (++size > threshold) {
	    // Rehash the table if the threshold is exceeded
	    rehash();
	}
	unsigned h = hashCode(prim->name);
	hashTable[h % hashTableSize] = allocateEntry(prim, h, hashTable[h % hashTableSize]);
    }

    PrimitiveImpl* get(char const* name) 
    { 
	unsigned h = hashCode(name);
	for (Entry* entry = hashTable[h % hashTableSize]; 
	     entry != NULL;
	     entry = entry->next)
	{
	    if (entry->hashCode == h && strcmp(entry->prim->name, name) == 0) { 
		return entry->prim;
	    }
	}
	return NULL;
    }

    void rehash() 
    { 
	size_t newHashTableSize = hashTableSize*2 + 1; 
	Entry** newHashTable = new Entry*[newHashTableSize];
	memset(newHashTable, 0, newHashTableSize*sizeof(Entry*));
	threshold = (int)(newHashTableSize * loadFactor);
	for (int i = hashTableSize; --i >= 0;) { 
	    Entry *entry, *next;
	    for (entry = hashTable[i]; entry != NULL; entry = next) {
		next = entry->next;
		entry->next = newHashTable[entry->hashCode % newHashTableSize];
		newHashTable[entry->hashCode % newHashTableSize] = entry;
	    }
	}
	delete[] hashTable;
	hashTable = newHashTable;
	hashTableSize = newHashTableSize;
    }

    void remove(PrimitiveImpl* prim) 
    { 
	unsigned h = hashCode(prim->name);
	Entry *entry, **epp;
	for (epp = &hashTable[h % hashTableSize]; (entry = *epp)->prim != prim; epp = &entry->next);
	*epp = entry->next;
	deallocateEntry(entry);
    }

    class Iterator { 
	int i;
	Entry  *entry;
	Entry **table;	

      public:
	Iterator(HashMap const& map){
	    i = map.hashTableSize;
	    table = map.hashTable;
	    entry = NULL;
	}

	PrimitiveImpl* next() { 
	    while (entry == NULL) { 
		if (--i < 0) { 
		    return NULL;
		}
		entry = table[i]; 
	    }
	    PrimitiveImpl* prim = entry->prim;
	    entry = entry->next;
	    return prim;
	}
    };

    HashMap(size_t initHashTableSize = 117, double loadFactor = 1.1) 
    { 
	size = 0;
	hashTableSize = initHashTableSize;
	hashTable = new Entry*[hashTableSize];
	memset(hashTable, 0, hashTableSize*sizeof(PrimitiveImpl*));
	this->loadFactor = loadFactor;
	threshold = (int)(hashTableSize * loadFactor);
	freeEntries = NULL;
    }

    ~HashMap() { 
	for (int i = hashTableSize; --i >= 0;) { 
	    Entry *entry, *next;
	    for (entry = hashTable[i]; entry != NULL; entry = next) {
		next = entry->next;
		delete entry;
	    }
	}
	delete[] hashTable;
	Entry *e, *next;
	for (e = freeEntries; e != NULL; e = next) { 
	    next = e->next;
	    delete e;
	}
    }

    Entry* allocateEntry(PrimitiveImpl* prim, unsigned hashCode, Entry* chain) { 
	Entry* e = freeEntries;
	if (e != NULL) { 
	    freeEntries = e->next;
	} else { 
	    e = new Entry();
	}
	e->prim = prim;
	e->hashCode = hashCode;
	e->next = chain;
	return e;
    }

    void deallocateEntry(Entry* e) { 
	e->next = freeEntries;
	freeEntries =  e;
    }


    size_t  hashTableSize;
    size_t  threshold;
    size_t  size;
    double  loadFactor;
    Entry*  freeEntries;
    Entry** hashTable;
};

#endif



