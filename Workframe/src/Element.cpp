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

time_t Element::exists_from(const Log* caller) const {
	auto log = as_method(caller, __func__, typeid(time_t));

	return log.returns(
			Primitive<time_t>(&log, creation));
}
void Element::is_modified(const Log* caller) {
	as_method(caller, __func__, typeid(void));
	modification = std::chrono::system_clock::now();
}
std::map<std::string, std::string> Element::gives_attributes(const Log* caller) const {
	return as_method<false>(caller, __func__, typeid(attributes)).returns(Class<decltype(attributes)>(this, ));
}
void Element::gets_attributes(std::map<std::string, std::string> attributes) {
	last_attributes = this->attributes;
	this->attributes = attributes;
	is_modified();
}
Element::Modifications Element::gives_modifications() {
	Modifications result;
	auto end = attributes.end();
	auto last_end = last_attributes.end();

	for (auto last_attribute : last_attributes) {
		auto last_first = last_attribute.first;
		auto last_second = last_attribute.second;

		if (attributes.find(last_first) == end)
			result.emplace(last_first, std::make_pair(last_second, ""));
		else {
			auto second = attributes.at(last_first);

			if (second != last_second)
				result.emplace(last_first, std::make_pair(last_second, second));
		}
	}
	for (auto attribute : attributes) {
		auto first = attribute.first;

		if (last_attributes.find(first) == last_end)
			result.emplace(attribute.first,
					std::make_pair("", attribute.second));
	}

	return result;
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
