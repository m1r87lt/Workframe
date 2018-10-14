/*
 * Element.h
 *
 *  Created on: 7 ott 2018
 *      Author: m1rma
 */

#ifndef ELEMENT_H_
#define ELEMENT_H_

#include "Object.h"
#include <chrono>
#include <set>
#include <map>

namespace base {

class Element: public Log {
	std::chrono::system_clock::time_point creation;
	std::chrono::system_clock::time_point modification;
	Element* position;
	std::map<std::string, std::string> attributes;
	std::map<std::string, std::string> last_attributes;
	static std::set<Element*> everything;
	friend class Ensemble;
public:
	using Modifications = std::map<std::string, std::pair<std::string, std::string>>;

	Class<const std::chrono::system_clock::time_point> exists_from(const Log* =
			nullptr) const;
	Class<const std::chrono::system_clock::time_point> since(
			const Log* = nullptr) const;
	void is_modified(const Log* = nullptr);
	Class<const std::map<std::string, std::string>> gives_attributes(const Log* =
			nullptr) const;
	void gets_attributes(Class<std::map<std::string, std::string>>, const Log* =
			nullptr);
	Class<Modifications> gives_modifications(const Log* = nullptr);

	virtual ~Element();
protected:
	Element(Class<std::string>, const Log* = nullptr,
			Class<std::map<std::string, std::string>> = Class<
					std::map<std::string, std::string>>(nullptr));
};

} /* namespace base */

#endif /* ELEMENT_H_ */
