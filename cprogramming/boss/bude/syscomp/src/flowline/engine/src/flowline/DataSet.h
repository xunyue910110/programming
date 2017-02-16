#ifndef _DATASET_H_
#define _DATASET_H_
#include "flowline/flbase.h"
#include <iostream>
#include "dataset/DataSet.h"
#include "base/SmartPtr.h"


USING_NAMESPACE(std);
namespace dataset {
	inline ostream& operator << (ostream& out, DataSet& dataset) {
		int fcount = dataset.fieldCount();
		for(int i=0; i<fcount; i++) {
			if(i !=0)
				out << "\t";
			out << dataset.getField(i).getFieldName();
		}
		out << endl;
		DataSet::iterator it = dataset.begin();
		for(; it != dataset.end(); it++) {
			for(int i=0; i<fcount; i++) {
				if(i !=0)
					out << "\t";
				out << (*it)[i];
			}	
			out << endl;
		}
		return out;
	}	
}

namespace flowline {
	
	using dataset::DataSet;	
	using dataset::DataType;
	using dataset::FTINT;
	using dataset::FTSTRING;
	using dataset::FTDOUBLE;
	using dataset::FTINT64;
	
	struct TAG {
		enum {NONE, DATATIME};
	};
	
	typedef counted_ptr<DataSet> PDataSet ;	
}
#endif //_DATASET_H_
