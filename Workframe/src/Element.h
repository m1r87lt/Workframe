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
template<typename Container, typename ... Separators,
		typename Content = typename Container::value_type, size_t T =
				std::tuple_size<Content>::value> class Container_Printer {
	std::string text;

	template<typename ... Rest> std::list<std::string> prepares_separators(
			std::string separator, Rest&& ... rest) const {
		auto result = prepares_separators(rest ...);

		result.emplace_front(separator);

		return result;
	}
	std::list<std::string> prepares_separators() const {
		return std::list<std::string>();
	}
	template<typename Type, typename ... Types, size_t N = sizeof...(Types)> std::list<
			std::string> prints(const Content& content) const {
		auto result = prints<Types ...>(content);
		std::ostringstream print;

		print << std::get<N>(content);
		result.emplace_front(print.str());

		return result;
	}
public:
	std::ostringstream operator ()() const {
		return std::ostringstream(text);
	}
	static std::ostringstream simple_content(const Container& container,
			std::string former_separator, std::string last_separator) {
		std::ostringstream result("{");

		for (auto content : container)
			result << "\n" << former_separator << content << last_separator;
		result << (result.str() == "{" ? " " : "\n") << "}";

		return result;
	}
	static std::ostringstream simple_print(const Container& container) {
		return simple_content(container, "\t", "");
	}
	static std::ostringstream map_print(const Container& container) {
		return Container_Printer<Container, std::string, std::string>(container,
				"\t", ": ")();
	}

	Container_Printer(const Container& container, Separators&& ... separators) {
		auto separator_list = prepares_separators(separators ...);
		auto separator_size = separator_list.size();
		auto content_size = T + 1;

		if (separator_size < content_size) {
			separator_list.resize(content_size, "\t");
			separator_list.back() = separator_list.front() == "{" ? "}" : "";
		} else if (separator_size > content_size) {
			auto back = separator_list.back();

			separator_list.resize(content_size);
			separator_list.back() = back;
		}
		text = "{";
		for (auto content : container) {
			auto content_list = prints<>(content);
			auto end = content_list.end();
			auto separator = separator_list.begin();

			text += "\n";
			for (auto iterator = content_list.begin(); iterator != end;
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
	virtual std::ostringstream prints() const {
		std::ostringstream result;

		result << value.get();

		return result;
	}
	const Element& operator *() const {
		return *value;
	}
	Element& operator *() {
		return *value;
	}
	const Element* operator ->() const {
		return value.operator ->();
	}
	Element* operator ->() {
		return value.operator ->();
	}
	operator const Element*() const {
		return value.get();
	}

	Class(const Class<std::unique_ptr<Element>>&) = delete;
	Class& operator =(const Class<std::unique_ptr<Element>>&) = delete;
	Class<std::unique_ptr<Element>>& operator =(
			Class<std::unique_ptr<Element>> && assigning) {
		Object::operator =(assigning);
		value.reset(assigning.value.release());

		return *this;
	}
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
	Primitive<size_t> size(const Log* = nullptr) const;
	void clear();
	template<typename Type, typename ... Arguments> void generates(
			Class<std::string> name, Primitive<size_t> position = 0,
			const Log* caller = nullptr, Arguments&& ... arguments) {
		auto log = as_method(__func__, caller, typeid(void), name, position,
				arguments ...);

		gets(name,
				Class<std::unique_ptr<Element>>::construct(caller,
						arguments ...), position, caller);
	}
	static Primitive<size_t> which_is(const Element&, const Log* = nullptr);
	static Class<std::string> who_is(const Element&, const Log* = nullptr);
	static Primitive<Ensemble*> where_is(const Element&, const Log* = nullptr);
	static Class<std::unique_ptr<Element>> pop(const Element&, const Log* =
			nullptr);
	static void take(Ensemble*, Primitive<size_t>, const Element&, const Log* =
			nullptr);
	static Class<std::tuple<Ensemble*, Primitive<size_t>, std::string>> localize(
			const Element&, const Log* = nullptr);
	static Class<std::vector<std::string>> has_path(const Element&);
private:
	Container container;

	Class<Container::iterator> finds(Primitive<size_t>) const;
	Class<Container::iterator> finds(Class<std::string>) const;
	static Class<std::pair<Ensemble*, Container::iterator>> find(
			const Element*);
};

} /* namespace base */

#endif /* ELEMENT_H_ */
