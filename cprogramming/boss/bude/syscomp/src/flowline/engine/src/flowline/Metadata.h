#ifndef _METADATA_H_
#define _METADATA_H_
#include "flowline/flbase.h"
#include "flowline/Element.h"
#include "flowline/Field.h"
#include <vector>

USING_NAMESPACE(std);

namespace flowline
{
class Metadata : public flowline::Element
{
public:
	void addField(const Field& field) {
		m_fields.push_back(field);
	}
	const vector<Field>& fields() const {
		return m_fields;
	}
	
	PDataSet createDataSet() const {
		PDataSet ds(new dataset::DataSet);
		for(int i=0; i<m_fields.size(); i++) {
			dataset::Field fld;
			ds->addField(m_fields[i].toDataSetField(fld));
		}
		return ds;
	}
private:	
	vector<Field> m_fields;
};

}

#endif //_METADATA_H_
