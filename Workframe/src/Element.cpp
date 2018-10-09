/*
 * Element.cpp
 *
 *  Created on: 7 ott 2018
 *      Author: m1rma
 */

#include "Element.h"
#include <chrono>
#include <utility>

namespace base {

std::set<Element*> Element::everything;

time_t Element::exists_from() const {
	return creation;
}
void Element::is_modified(time_t instant) {
	modification = instant;
}
std::map<std::string, std::string> Element::gives_attributes() const {
	return attributes;
}
void gets_attributes(std::map<std::string, std::string> attributes) {
	for ()
}
Modifications gives_modifications();

Element::Modifications Element::modifications() {
	Modifications result;
	auto end = attributes.end();
	auto last_end = last_attributes.end();

	for (auto last_attribute : last_attributes) {
		auto iterator = result.emplace(last_attribute.first,
				std::make_pair(last_attribute.second, ""));

		if (attributes.find(last_attribute.first) != end)
			iterator.first->second.second = attributes[last_attribute.first];
	}
	for (auto attribute : attributes)
		if (last_attributes.find(attribute.first) != last_end)
			result.emplace(attribute.first,
					std::make_pair("", attribute.second));
}

Element::~Element() {
	// TODO Auto-generated destructor stub
	if (is_running())
		everything.erase(this);
}

Element::Element(const Log* caller, std::string label) :
		Log(caller, label, 4) {
	// TODO Auto-generated constructor stub
	creation = std::chrono::system_clock::now();
}

} /* namespace base */
