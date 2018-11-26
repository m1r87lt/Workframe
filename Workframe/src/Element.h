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
template<typename First, typename Second> std::ostringstream print_std__map(
		const std::map<First, Second>& container) {
	return Container_Printer(container, "\t", ": ", "")();
}
using Fields = Class<std::map<std::string, std::string>>;
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
	void gets_attributes(Fields, const Log* = nullptr);
	Class<Modifications> gives_modifications(const Log* = nullptr);
	static Class<std::set<Element*>> give_everything(const Log* = nullptr);

	virtual ~Element();
protected:
	Element(Class<std::string>, const Log* = nullptr, Fields = nullptr);
	Element(Element&&);
	Element& operator =(Element&&) = delete;
};
using Unique_ptr = Class<std::unique_ptr<Element>>;
template<> class Class<std::unique_ptr<Element>> final: public Object {
	std::unique_ptr<Element> value;

	friend class Ensemble;
	static std::unique_ptr<Element> is_from(Unique_ptr&&);

	template<typename Type> Class(Type* object, const Log* caller = nullptr) :
			Object(caller, typeid(Type).name()), value(object) {
	}
public:
	virtual std::ostringstream prints() const;
	const Element& operator *() const;
	Element& operator *();
	const Element* operator ->() const;
	Element* operator ->();
	operator const Element*() const;
	operator Element*();

	Class(const Unique_ptr&) = delete;
	Unique_ptr& operator =(const Unique_ptr&) = delete;
	Class(Unique_ptr&&);
	Unique_ptr& operator =(Unique_ptr&&);
	Class(std::unique_ptr<Element>&&, const Log* = nullptr);
	template<typename Type, typename ... Arguments> static Unique_ptr construct(
			const Log* caller = nullptr, Arguments&& ... arguments) {
		return Unique_ptr(new Type(std::forward<Arguments&&>(arguments) ...),
				caller);
	}
};
std::domain_error throw_root_element(const Element&, const Log&);
struct Ensemble: public Element {
	using Content = std::pair<std::string, std::unique_ptr<Element>>;
	using Container = std::list<Content>;

	Element& operator [](Primitive<size_t>) const;
	Class<std::map<size_t, Element*>> operator [](Class<std::string>) const;
	Primitive<size_t> which_is(Class<std::string>, const Log* = nullptr) const;
	Class<std::string> who_is(Primitive<size_t>, const Log* = nullptr) const;
	Unique_ptr gives(Primitive<size_t>, const Log* = nullptr);
	Unique_ptr gives(Class<std::string>, const Log* = nullptr);
	void gets(Class<std::string>, Unique_ptr&&, Primitive<size_t>, const Log* =
			nullptr);
	void takes(Primitive<Ensemble*>, Primitive<size_t>, Primitive<size_t>,
			const Log* = nullptr);
	void takes(Primitive<Ensemble*>, Class<std::string>, Primitive<size_t>,
			const Log* = nullptr);
	Primitive<size_t> has_size(const Log* = nullptr) const;
	void self_clears(const Log* = nullptr);
	template<typename Type, typename ... Arguments> void generates(
			Class<std::string> name, Primitive<size_t> position = 0,
			const Log* caller = nullptr, Arguments&& ... arguments) {
		auto log = as_method(__func__, caller, typeid(void), name, position,
				arguments ...);

		gets(name,
				Unique_ptr::construct<Type>(caller,
						std::forward<Arguments&&>(arguments) ...), position,
				caller);
	}
	static Unique_ptr pop(Element&, const Log* = nullptr);
	static void take(Primitive<Ensemble*>, Primitive<size_t>, Element&,
			const Log* = nullptr);
	static Class<std::tuple<Ensemble*, size_t, std::string>> localize(
			const Element&, const Log* = nullptr);
	static Class<std::vector<std::string>> have_path(const Element&,
			const Log* = nullptr);

	virtual ~Ensemble();
	Ensemble(Ensemble&&) = delete;
private:
	Container container;

	std::string names(std::string, const Log* = nullptr) const;
	bool names(std::string, std::string, const Log* = nullptr) const;
	Container::iterator localizes(size_t, const Log* = nullptr) const;
	std::pair<size_t, Container::iterator> localizes(std::string, const Log* =
			nullptr) const;
	std::unique_ptr<Element> gives(Container::iterator, const Log* = nullptr);
	static std::pair<Ensemble*, Container::iterator> find(Element&, const Log* =
			nullptr);
protected:
	Ensemble(Class<std::string>, const Log* = nullptr, Fields = nullptr);
	friend Unique_ptr;
};

} /* namespace base */

#endif /* ELEMENT_H_ */
