
/*
 * Copyright 2002,2005 Linkage All rights reserved.
 */
 
#include "Properties.h"
#include <strstream>

class PropertyParser
{
public:
	void parse(istream& in, Properties& properties)
	{
		ostrstream key, element;
		LexemType lexemType = BEGIN;
		char c;
		bool finished = false;

		if (!get(in, c))
		{
			return;
		}

		while (!finished)
		{
			switch(lexemType)
			{
			case BEGIN:
				switch(c)
				{
				case (' '):
				case ('\t'):
				case ('\n'):
				case ('\r'):
					if (!get(in, c))
						finished = true;
					break;

				case ('#'):
				case ('!'):
					lexemType = COMMENT;
					if (!get(in, c))
						finished = true;
					break;

				default:
					lexemType = KEY;
					break;
				}
				break;

			case KEY:
				switch(c)
				{
				case ('\\'):
					lexemType = KEY_ESCAPE;
					if (!get(in, c))
						finished = true;
					break;

				case ('\t'):
				case (' '):
				case (':'):
				case ('='):
					lexemType = DELIMITER;
					if (!get(in, c))
						finished = true;
					break;

				case ('\n'):
				case ('\r'):
					// key associated with an empty string element
					key << '\0'; element << '\0';
					properties.setProperty(key.str(), (""));
					key.rdbuf()->freeze(0);key.clear();key.seekp(0);
					element.rdbuf()->freeze(0);element.clear();element.seekp(0);
					lexemType = BEGIN;
					if (!get(in, c))
						finished = true;
					break;

				default:
					key << c;
					if (!get(in, c))
						finished = true;
					break;
				}
				break;

			case KEY_ESCAPE:
				switch(c)
				{
				case ('\t'):
				case (' '):
				case (':'):
				case ('='):
				case ('\\'):
					key << c;
					lexemType = KEY;
					if (!get(in, c))
						finished = true;
					break;

				case ('\n'):
					lexemType = KEY_CONTINUE;
					if (!get(in, c))
						finished = true;
					break;

				case ('\r'):
					lexemType = KEY_CONTINUE2;
					if (!get(in, c))
						finished = true;
					break;
				}
				break;

			case KEY_CONTINUE:
				switch(c)
				{
				case (' '):
				case ('\t'):
					if (!get(in, c))
						finished = true;
					break;

				default:
					lexemType = KEY;
					break;
				}
				break;

			case KEY_CONTINUE2:
				switch(c)
				{
				case ('\n'):
					if (!get(in, c))
						finished = true;
					lexemType = KEY_CONTINUE;
					break;

				default:
					lexemType = KEY_CONTINUE;
					break;
				}
				break;

			case DELIMITER:
				switch(c)
				{
				case ('\t'):
				case (' '):
				case (':'):
				case ('='):
					if (!get(in, c))
						finished = true;
					break;

				default:
					lexemType = ELEMENT;
					break;
				}
				break;

			case ELEMENT:
				switch(c)
				{
				case ('\\'):
					lexemType = ELEMENT_ESCAPE;
					if (!get(in, c))
						finished = true;
					break;

				case ('\n'):
				case ('\r'):
					// key associated with an empty string element
					key << '\0'; element << '\0';

					properties.setProperty(key.str(), element.str());
					
					key.rdbuf()->freeze(0);key.clear();key.seekp(0);
					element.rdbuf()->freeze(0);element.clear();element.seekp(0);
					//key.str((""));
					//element.str((""));
					lexemType = BEGIN;
					if (!get(in, c))
						finished = true;
					break;

				default:
					element << c;
					if (!get(in, c))
						finished = true;
					break;
				}
				break;

			case ELEMENT_ESCAPE:
				switch(c)
				{
				case ('t'):
				case (' '):
				case ('n'):
				case ('r'):
				case ('\''):
				case ('\\'):
				case ('\"'):
				case (':'):
				default:
					element << c;
					lexemType = ELEMENT;
					if (!get(in, c))
						finished = true;
					break;

				case ('\n'):
					lexemType = ELEMENT_CONTINUE;
					if (!get(in, c))
						finished = true;
					break;

				case ('\r'):
					lexemType = ELEMENT_CONTINUE2;
					if (!get(in, c))
						finished = true;
					break;
				}
				break;

			case ELEMENT_CONTINUE:
				switch(c)
				{
				case (' '):
				case ('\t'):
					if (!get(in, c))
						finished = true;
					break;

				default:
					lexemType = ELEMENT;
					break;
				}
				break;

			case ELEMENT_CONTINUE2:
				switch(c)
				{
				case ('\n'):
					if (!get(in, c))
						finished = true;
					lexemType = ELEMENT_CONTINUE;
					break;

				default:
					lexemType = ELEMENT_CONTINUE;
					break;
				}
				break;

			case COMMENT:
				if (c == ('\n') || c == ('\r'))
				{
					lexemType = BEGIN;
				}
				if (!get(in, c))
					finished = true;
				break;
			}
		}

		if (!(key.str()=="")) {
			key << '\0'; element << '\0';
			properties.setProperty(key.str(), element.str());
			key.rdbuf()->freeze(0);key.clear();key.seekp(0);
			element.rdbuf()->freeze(0);element.clear();element.seekp(0);
		}
	}

protected:
	bool get(istream& in, char& c)
	{
		in.get(c);

		if (in.eof())
		{
			return false;
		}

		if (in.bad())
		{
			return false;
		}

		return true;
	}

	typedef enum
	{
		BEGIN,
		KEY,
		KEY_ESCAPE,
		KEY_CONTINUE,
		KEY_CONTINUE2,
		DELIMITER,
		ELEMENT,
		ELEMENT_ESCAPE,
		ELEMENT_CONTINUE,
		ELEMENT_CONTINUE2,
		COMMENT
	}
	LexemType;
};

string Properties::setProperty(const string& key, const string& value)
{
	string oldValue = properties[key];
	properties[key] = value;
	//cout << ("setting property key=") << key << (", value=") << value << std::endl;
	return oldValue;
}

string Properties::getProperty(const string& key) const
{
	std::map<string, string>::const_iterator it = properties.find(key);
	return (it != properties.end()) ? it->second : string();
}

void Properties::load(istream& inStream)
{
	properties.clear();
	PropertyParser parser;
	parser.parse(inStream, *this);
}

std::vector<string> Properties::propertyNames() const
{
	std::vector<string> names;
	names.reserve(properties.size());

	std::map<string, string>::const_iterator it;
	for (it = properties.begin(); it != properties.end(); it++)
	{
		const string& key = it->first;
		names.push_back(key);
	}

	return names;
}





