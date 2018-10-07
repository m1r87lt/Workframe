/*
 * Element.h
 *
 *  Created on: 7 ott 2018
 *      Author: m1rma
 */

#ifndef ELEMENT_H_
#define ELEMENT_H_

#include "Object.h"
#include <set>
#include <map>

namespace base {

class Element: public Log {
	time_t creation;
	Element* position;
	static std::set<Element*> everything;
public:
	virtual ~Element();

	std::map<std::string, std::string> attributes;

	time_t exists_from() const;
protected:
	Element(const Log*, std::string, Element*,
			std::map<std::string, std::string>);

	time_t modification;
};

} /* namespace base */

#endif /* ELEMENT_H_ */
