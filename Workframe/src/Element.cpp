/*
 * Element.cpp
 *
 *  Created on: 7 ott 2018
 *      Author: m1rma
 */

#include "Element.h"

namespace base {

//Element
std::set<Element*> Element::everything;

std::ostringstream print_std__chrono__system_clock__time_point(
		const std::chrono::system_clock::time_point& object) {
	std::ostringstream result;
	auto tp = std::chrono::system_clock::to_time_t(object);

	result << ctime(&tp);

	return result;
}
template<> std::function<
		std::ostringstream(const std::chrono::system_clock::time_point&)> Class<
		const std::chrono::system_clock::time_point>::printer =
		print_std__chrono__system_clock__time_point;
Class<const std::chrono::system_clock::time_point> Element::exists_from(
		const Log* caller) const {
	return method_class(
			std::forward<const std::chrono::system_clock::time_point>(creation),
			*this, __func__, caller);
}
void Element::is_modified(const Log* caller) {
	as_method<false>(__func__, caller);
	modification = std::chrono::system_clock::now();
}
/*
template<> std::function<
		std::ostringstream(const std::map<std::string, std::string>&)> Class<
		std::map<std::string, std::string>>::printer = Container_Printer<
		std::map<std::string, std::string>>::map_print;
Class<const std::map<std::string, std::string>> Element::gives_attributes(
		const Log* caller) const {
	return method_class(
			std::forward<const std::map<std::string, std::string>>(attributes),
			*this, __func__, caller);
}
void Element::gets_attributes(
		Class<std::map<std::string, std::string>> attributes,
		const Log* caller) {
	auto log = as_method(__func__, caller, typeid(void), attributes);

	last_attributes = this->attributes;
	this->attributes = attributes.is();
	is_modified(caller);
}

std::ostringstream print_modifications(const Element::Modifications& object) {
	std::ostringstream result("{");

	for (auto pair : object)
		result << "\n\t" << pair.first << ": " << pair.second.first << " -> "
				<< pair.second.second;
	if (result.str() == "{")
		result << " }";
	else
		result << "\n}";

	return result;
}
template<> std::function<std::ostringstream(const Element::Modifications&)> Class<
		Element::Modifications>::printer = print_modifications;
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

	return (Class<Modifications> &&) log.returns(
			Class<Modifications>(&log, result));
}

Element::Element(Class<std::string> label, const Log* caller,
		Class<std::map<std::string, std::string>> attributes) :
		Object(caller, label.is()), Log(caller, label.is(), false) {
	as_constructor<false>("base", __func__, caller, attributes);
	modification = creation = std::chrono::system_clock::now();
	position = nullptr;
	this->attributes = attributes.is();
	everything.emplace(this);
}

Element::~Element() {
	as_destructor("base", __func__);
	if (is_running())
		everything.erase(this);
}
/*
 //Ensemble
 Element& Ensemble::operator [](Primitive<size_t> position) const {
 auto log = as_binary("[]", position, nullptr, typeid(Element&));

 return log.returns(*finds(position).is()->second);
 }

 std::ostringstream class_std__map_size_t__element___(
 const std::map<size_t, Ensemble*>& results) {
 std::ostringstream result("{");

 for (auto position : results)
 result << "\n\t" << position.first << ": " << position.second;
 if (result.str() == "{")
 result << " ";

 return result;
 }
 template<> std::function<
 std::ostringstream(const std::tuple<Ensemble*, size_t, std::string>&)> Class<
 std::tuple<Ensemble*, size_t, std::string>>::printer =
 class_std__tuple_Ensemble__size_t__std__string__;
 Class<std::map<size_t, Element*>> operator [](Class<std::string>) const;

 std::ostringstream class_std__tuple_Ensemble__size_t__std__string__(
 const std::tuple<Ensemble*, size_t, std::string>& position) {
 std::ostringstream result("{ ");

 result << std::get<0>(position) << "; " << std::get<1>(position) << "; \""
 << std::get<2>(position) << "\" }";

 return result;
 }
 template<> std::function<
 std::ostringstream(const std::tuple<Ensemble*, size_t, std::string>&)> Class<
 std::tuple<Ensemble*, size_t, std::string>>::printer =
 class_std__tuple_Ensemble__size_t__std__string__;
 */
} /* namespace base */
