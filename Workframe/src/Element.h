/*
 * Element.h
 *
 *  Created on: 7 ott 2018
 *      Author: m1rma
 */

#ifndef ELEMENT_H_
#define ELEMENT_H_

#include "Object.h"
#include <array>
#include <tuple>
#include <chrono>
#include <set>
#include <map>
#include <memory>
#include <list>
#include <vector>

namespace base {
class Container_Printer {
	template<typename Content> struct size {
		static const size_t value = 1;
	};
	template<typename ... Types> struct size<std::tuple<Types ...>> {
		static const size_t value = sizeof...(Types);
	};

	std::string text;

	std::list<std::string> static prepare_separators() {
		return std::list<std::string>();
	}
	template<typename ... Separators> static std::list<std::string> prepare_separators(
			std::string separator, Separators&& ... separators) {
		auto result = prepare_separators(separators ...);

		result.emplace_front(separator);

		return result;
	}
	template<typename Content> static std::list<std::string> list_components(
			const Content& content) {
		std::ostringstream print;

		print << std::get<0>(content);

		return std::list<std::string> { print.str() };
	}
	template<typename Content, typename Parameter, typename ... Parameters> static std::list<
			std::string> list_components(const Content& content) {
		auto result = list_components<Content, Parameters ...>(content);
		std::ostringstream print;

		print << std::get<sizeof...(Parameters)>(content);
		result.emplace_back(print.str());

		return result;
	}
	template<typename ... Types> static std::list<std::string> print_content(
			const std::tuple<Types ...>& content) {
		return list_components<std::tuple<Types...>, Types ...>(content);
	}
	template<typename First, typename Second> static std::list<std::string> print_content(
			const std::pair<First, Second>& content) {
		std::tuple<First, Second> result = content;

		return list_components(result);
	}
	template<typename Type> static std::list<std::string> print_content(
			const Type& content) {
		std::tuple<Type> result = std::make_tuple(content);

		return list_components(result);
	}
public:
	std::ostringstream operator ()() const {
		return std::ostringstream(text);
	}

	template<typename Container, typename ... Separators> Container_Printer(
			const Container& container, Separators&& ... separators) {
		auto separator_list = prepare_separators(separators ...);
		auto separator_size = separator_list.size();
		auto content_size = size<typename Container::value_type>::value;

		if (separator_size == 0)
			separator_list.emplace_front("{ ");
		if (separator_size < content_size + 1) {
			separator_list.resize(content_size, "; ");
			separator_list.back() = separator_list.front() == "{ " ? " }" : "";
		} else if (separator_size > content_size + 1) {
			auto back = separator_list.back();

			separator_list.resize(content_size);
			separator_list.back() = back;
		}
		text = "{";
		for (auto content : container) {
			auto content_list = print_content(content);
			auto content_end = content_list.end();
			auto separator = separator_list.begin();

			text += "\n";
			for (auto iterator = content_list.begin(); iterator != content_end;
					++iterator)
				text += *separator++ + *iterator;
			text += *separator;
		}
		text += text == "{" ? " }" : "\n}";
	}
	Container_Printer(const Container_Printer&) = delete;
	Container_Printer& operator =(const Container_Printer&) = delete;
	Container_Printer(Container_Printer&&) = delete;
	Container_Printer& operator =(Container_Printer&&) = delete;
};

class Element: virtual public Log {
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
	Class<const std::chrono::system_clock::time_point> since(const Log* =
			nullptr) const;
	void is_modified(const Log* = nullptr);
	Class<const std::map<std::string, std::string>> gives_attributes(
			const Log* = nullptr) const;
	void gets_attributes(Class<std::map<std::string, std::string>>, const Log* =
			nullptr);
	Class<Modifications> gives_modifications(const Log* = nullptr);

	virtual ~Element();
protected:
	Element(Class<std::string>, const Log* = nullptr,
			Class<std::map<std::string, std::string>> = Class<
					std::map<std::string, std::string>>(nullptr));
};

template<> class Class<std::unique_ptr<Element>> final: Object {
	std::unique_ptr<Element> value;

	template<typename Type> Class<std::unique_ptr<Element>>(Type* object,
			const Log* caller = nullptr) :
			Object(caller, typeid(Type).name()), value(object) {
	}
public:
	virtual std::ostringstream prints() const;
	const Element& operator *() const;
	Element& operator *();
	const Element* operator ->() const;
	Element* operator ->();
	operator const Element*() const;

	Class(const Class<std::unique_ptr<Element>>&) = delete;
	Class& operator =(const Class<std::unique_ptr<Element>>&) = delete;
	Class<std::unique_ptr<Element>>& operator =(
			Class<std::unique_ptr<Element>> &&);
	template<typename Type, typename ... Arguments> Class<
			std::unique_ptr<Element>> construct(const Log* caller = nullptr,
			Arguments&& ... arguments) {
		return Class<std::unique_ptr<Element>>(new Type(arguments ...), caller);
	}
};
struct Ensemble: public Element {
	using Content = std::pair<std::string, std::unique_ptr<Element>>;
	using Container = std::list<Content>;

	Element& operator [](Primitive<size_t>) const;
	Class<std::map<size_t, Element*>> operator [](Class<std::string>) const;
	Primitive<size_t> which_is(Class<std::string>, const Log* = nullptr) const;
	Class<std::string> who_is(Primitive<size_t>, const Log* = nullptr) const;
	Class<std::unique_ptr<Element>> gives(Primitive<size_t>, const Log* =
			nullptr);
	Class<std::unique_ptr<Element>> gives(Class<std::string>, const Log* =
			nullptr);
	void gets(Class<std::string>, Class<std::unique_ptr<Element>> &&,
			Primitive<size_t>, const Log* = nullptr);
	void takes(Ensemble*, Primitive<size_t>, Primitive<size_t>, const Log* =
			nullptr);
	void takes(Ensemble*, Class<std::string>, Primitive<size_t>, const Log* =
			nullptr);
	Primitive<size_t> has_size(const Log* = nullptr) const;
	void self_clears(const Log* = nullptr);
	template<typename Type, typename ... Arguments> void generates(
			Class<std::string> name, Primitive<size_t> position = 0,
			const Log* caller = nullptr, Arguments&& ... arguments) {
		auto log = as_method(__func__, caller, typeid(void), name, position,
				arguments ...);

		gets(name,
				Class<std::unique_ptr<Element>>::construct(caller,
						arguments ...), position, caller);
	}
	static Primitive<size_t> which_be(const Element&, const Log* = nullptr);
	static Class<std::string> who_be(const Element&, const Log* = nullptr);
	static Primitive<Ensemble*> where_be(const Element&, const Log* = nullptr);
	static Class<std::unique_ptr<Element>> pop(const Element&, const Log* =
			nullptr);
	static void take(Ensemble*, Primitive<size_t>, const Element&, const Log* =
			nullptr);
	static Class<std::tuple<Ensemble*, Primitive<size_t>, std::string>> localize(
			const Element&, const Log* = nullptr);
	static Class<std::vector<std::string>> have_path(const Element&,
			const Log* = nullptr);
private:
	Container container;

	Class<Container::iterator> localizes(Primitive<size_t>,
			const Log* = nullptr) const;
	Primitive<size_t> localizes(Class<Container::iterator>,
			const Log* = nullptr) const;
	Class<std::set<Container::iterator>> finds(Class<std::string>, const Log* =
			nullptr) const;
	static Class<std::pair<Ensemble*, Container::iterator>> find(const Element*,
			const Log* = nullptr);
};

} /* namespace base */

#endif /* ELEMENT_H_ */
