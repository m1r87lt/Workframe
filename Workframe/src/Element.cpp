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

template<typename First, typename Second> static std::ostringstream print_std__map(
		const std::map<First, Second>& container) {
	return Container_Printer(container, "\t", ": ", "")();
}
template<> std::function<
		std::ostringstream(const std::map<std::string, std::string>&)> Class<
		std::map<std::string, std::string>>::printer = print_std__map<
		std::string, std::string>;
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
	using Mods = Class<decltype(result)>;
	auto log = as_method(__func__, caller, typeid(Mods));
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

	return static_cast<Mods&&>(log.returns(Mods(&log, result)));
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

//
std::ostringstream Class<std::unique_ptr<Element>>::prints() const {
	std::ostringstream result;

	result << value.get();

	return result;
}
const Element& Class<std::unique_ptr<Element>>::operator *() const {
	return *value;
}
Element& Class<std::unique_ptr<Element>>::operator *() {
	return *value;
}
const Element* Class<std::unique_ptr<Element>>::operator ->() const {
	return value.operator ->();
}
Element* Class<std::unique_ptr<Element>>::operator ->() {
	return value.operator ->();
}
Class<std::unique_ptr<Element>>::operator const Element*() const {
	return value.get();
}

Class<std::unique_ptr<Element>>& Class<std::unique_ptr<Element>>::operator =(
		Class<std::unique_ptr<Element>> && assigning) {
	Object::operator =(assigning);
	value.reset(assigning.value.release());

	return *this;
}

//Ensemble
Element& Ensemble::operator [](Primitive<size_t> position) const {
	auto log = as_binary("[]", position, nullptr, typeid(Element&));

	return dynamic_cast<Element&>(log.returns(
			*localizes(position, &log).is()->second));
}

template<> std::function<std::ostringstream(const std::map<size_t, Ensemble*>&)> Class<
		std::map<size_t, Ensemble*>>::printer =
		print_std__map<size_t, Ensemble*>;
Class<std::map<size_t, Element*>> Ensemble::operator [](
		Class<std::string> name) const {
	std::map<size_t, Element*> result;
	using Result = Class<decltype(result)>;
	auto log = as_binary("[]", name, nullptr, typeid(Result));
	size_t position = 0;
	auto current = container.begin();
	auto size = container.size();

	while (position < size)
		if (names(name.is(), current->first))
			result[++position] = current->second.get();
		else
			++position;

	return static_cast<Result&&>(log.returns(Result(result, &log)));
}
Primitive<size_t> Ensemble::which_is(Class<std::string> name,
		const Log* caller) const {
	size_t result;
	auto log = as_method(__func__, caller, typeid(decltype(result)), name);

	for (auto searched : finds(name, &log).becomes()) {
		if (searched->first == name)
			return;
	}
}
/////////////////////////////////
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
/////////////////////////////////
Class<std::string> who_is(Primitive<size_t>, const Log* = nullptr) const;
Class<std::unique_ptr<Element>> gives(Primitive<size_t>, const Log* = nullptr);
Class<std::unique_ptr<Element>> gives(Class<std::string>, const Log* = nullptr);
void gets(Class<std::string>, Class<std::unique_ptr<Element>> &&,
		Primitive<size_t>, const Log* = nullptr);
void takes(Ensemble*, Primitive<size_t>, Primitive<size_t>,
		const Log* = nullptr);
void takes(Ensemble*, Class<std::string>, Primitive<size_t>, const Log* =
		nullptr);
Primitive<size_t> has_size(const Log* = nullptr) const;
void self_clears();
static Primitive<size_t> which_be(const Element&, const Log* = nullptr);
static Class<std::string> who_be(const Element&, const Log* = nullptr);
static Primitive<Ensemble*> where_be(const Element&, const Log* = nullptr);
static Class<std::unique_ptr<Element>> pop(const Element&,
		const Log* = nullptr);
static void take(Ensemble*, Primitive<size_t>, const Element&, const Log* =
		nullptr);
static Class<std::tuple<Ensemble*, Primitive<size_t>, std::string>> localize(
		const Element&, const Log* = nullptr);
static Class<std::vector<std::string>> have_path(const Element&);
private:
Container container;

std::string names(std::string);
bool names(std::string, std::string) const;
Class<Container::iterator> localizes(Primitive<size_t>,
		const Log* = nullptr) const;
Primitive<size_t> localizes(Class<Container::iterator>,
		const Log* = nullptr) const;
static Class<std::pair<Ensemble*, Container::iterator>> find(const Element*,
		const Log* = nullptr);
} /* namespace base */
