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
	time_t modification;
	Element* position;
	std::map<std::string, std::string> attributes;
	std::map<std::string, std::string> last_attributes;
	static std::set<Element*> everything;
	friend class Ensemble;
public:
	virtual ~Element();

	using Modifications = std::map<std::string, std::pair<std::string, std::string>>;

	time_t exists_from(const Log* = nullptr) const;
	void is_modified(const Log* = nullptr);
	std::map<std::string, std::string> gives_attributes(const Log* = nullptr) const;
	void gets_attributes(std::map<std::string, std::string>, const Log* = nullptr);
	Modifications gives_modifications(const Log* = nullptr);
protected:
	Element(const Log*, std::string, std::map<std::string, std::string>);
	Element(const Log*, std::string);
};

} /* namespace base */

#endif /* ELEMENT_H_ */
