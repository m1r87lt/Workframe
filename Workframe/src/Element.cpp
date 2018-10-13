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
	return method_primitive(creation, *this, __func__, caller);
}
void Element::is_modified(const Log* caller) {
	as_method<false>(__func__, caller);
	modification = std::chrono::system_clock::now();
}

std::ostringstream class_std__map_std__string_std__string__(
		Class<std::map<std::string, std::string>>& object) {
	std::ostringstream result("{");

	for (auto pair : object.is())
		result << "\n\t" << pair.first << "=" << pair.second;
	if (result.str() == "{")
		result << " }";
	else
		result << "\n}";

	return result;
}
std::function<std::ostringstream(std::map<std::string, std::string>&)> Class<
		std::map<std::string, std::string>>::printer =
		class_std__map_std__string_std__string__;
Class<std::map<std::string, std::string>> Element::gives_attributes(
		const Log* caller) const {
	return method_class(attributes, *this, __func__, caller);
}
void Element::gets_attributes(
		Class<std::map<std::string, std::string>> attributes,
		const Log* caller) {
	auto log = as_method(__func__, caller, typeid(void), attributes);

	last_attributes = this->attributes;
	this->attributes = attributes;
	is_modified(caller);
}

std::ostringstream class_Modifications_(Class<Element::Modifications>& object) {
	std::ostringstream result("{");

	for (auto pair : object.is())
		result << "\n\t" << pair.first << ": " << pair.second.first << " -> "
				<< pair.second.second;
	if (result.str() == "{")
		result << " }";
	else
		result << "\n}";

	return result;
}
std::function<std::ostringstream(Class<Element::Modifications>&)> Class<
		Element::Modifications>::printer = class_Modifications_;
Class<Element::Modifications> Element::gives_modifications(const Log* caller) {
	Modifications result;
	auto log = as_method(__func__, caller, typeid(decltype(result)));
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

	return log.returns(Class<Modifications>(result));
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
