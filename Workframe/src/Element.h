/*
 * Element.h
 *
 *  Created on: 7 ott 2018
 *      Author: m1r
 */

#ifndef ELEMENT_H_
#define ELEMENT_H_

#include "Object.h"
#include <tuple>
#include <list>
#include <map>
#include <chrono>
#include <set>
#include <memory>
#include <vector>
#define NEW_LOG(caller, label, open) Object(caller, label), Log(caller, label, open)
#define ASSIGN_LOG(instance) Object(instance), Log(instance)
#define ELEMENT(open, label, caller, attributes) Object(caller, label), Log(caller, label, open), Element(label, caller, attributes)
#define ENSEMBLE(open, label, caller, attributes) Object(caller, label), Log(caller, label, open), Ensemble(label, caller, attributes)
#define TYPEID(type) #type

namespace base {
class Container_Printer {
	template<typename Content> struct size {
		static const size_t value = 1;
	};
	template<typename ... Types> struct size<std::tuple<Types ...>> {
		static const size_t value = sizeof...(Types);
	};

	std::string text;
	std::list<std::string> separator_list;

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
	template<typename ... Types> static std::list<std::string> print_tuple(
			const std::tuple<Types ...>& content) {
		return list_components<std::tuple<Types...>, Types ...>(content);
	}
	template<typename First, typename Second> static std::list<std::string> print_tuple(
			const std::pair<First, Second>& content) {
		std::tuple<First, Second> result = content;

		return list_components(result);
	}
	template<typename Container, typename ... Separators> void prints_tuple(
			const Container& container, const Separators& ... separators) {
		auto separator_size = (separator_list = prepare_separators(
				separators ...)).size();
		auto content_size =
				std::tuple_size<typename Container::value_type>::value;

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
		for (auto content : container) {
			auto contents = print_tuple(content);
			auto end = contents.end();
			auto separator = separator_list.begin();

			text += "\n";
			for (auto iterator = contents.begin(); iterator != end; ++iterator)
				text += *separator++ + *iterator;
			text += *separator;
		}
	}
public:
	std::ostringstream operator ()(bool close = true) const {
		return std::ostringstream(
				close ? (text.empty() ? "{ }" : "{" + text + "\n}") : text);
	}

	template<typename Container, typename ... Separators> Container_Printer(
			const Container& container, std::string before, std::string after) {
		std::ostringstream result;

		for (auto content : container)
			result << "\n" << before << content << after;
		text = result.str();
	}
	template<typename Container, typename ... Separators> Container_Printer(
			bool, const Container& container, Separators&& ... separators) {
		prints_tuple(container, separators ...);
	}
	template<typename Type, typename ... Separators> Container_Printer(
			std::string, const Type& object, Separators&& ... separators) {
		std::list<Type> container = { object };

		prints_tuple(container, separators ...);
	}
	Container_Printer(const Container_Printer&) = delete;
	Container_Printer& operator =(const Container_Printer&) = delete;
	Container_Printer(Container_Printer&&) = delete;
	Container_Printer& operator =(Container_Printer&&) = delete;
};

template<typename First, typename Second> std::ostringstream print_std__map(
		const std::map<First, Second>& container) {
	return Container_Printer(false, container, "\t", ": ", "")();
}
using Fields = Class<std::map<std::string, std::string>>;
template<typename Type> std::ostringstream print_std__set(
		const std::set<Type>& set) {
	return Container_Printer(set, "\t{", "}")();
}
struct Element: virtual public Log {
	using Instant = std::chrono::system_clock::time_point;
	using Modifications = std::map<std::string, std::pair<std::string, std::string>>;

	Class<Instant> exists_from(const Log* = nullptr) const;
	Class<Instant> since(const Log* = nullptr) const;
	void is_modified(const Log* = nullptr);
	Fields gives_attributes(const Log* = nullptr) const;
	void gets_attributes(Fields, const Log* = nullptr);
	Class<Modifications> gives_modifications(const Log* = nullptr);
	static Class<std::set<Element*>> give_everything(const Log* = nullptr);

	virtual ~Element();
private:
	Instant creation;
	Instant modification;
	Element* position;
	std::map<std::string, std::string> attributes;
	std::map<std::string, std::string> last_attributes;
	static std::set<Element*> everything;
	friend class Ensemble;
protected:
	Element(std::string, const Log* = nullptr, Fields = nullptr);
	Element(Element&&);
	Element& operator =(Element&&) = delete;
};

template<> class Class<std::unique_ptr<Element>> : public Object {
	std::unique_ptr<Element> value;
	friend class Ensemble;
	static std::unique_ptr<Element> is_from(Class<std::unique_ptr<Element>> &&);

	template<typename Type> Class(Type* object, const Log* caller = nullptr) :
			Object(caller, typeid(Type).name()), value(object) {
	}
public:
	virtual std::ostringstream prints() const;
	const Element& operator *() const;
	Element& operator *();
	const Element* operator ->() const;
	Element* operator ->();
	const Element* get() const;
	Element* get();
	template<typename Type, typename ... Arguments> static Class<
			std::unique_ptr<Element>> construct(const Log* caller = nullptr,
			Arguments&& ... arguments) {
		return Class<std::unique_ptr<Element>>(
				new Type(std::forward<Arguments&&>(arguments) ...), caller);
	}

	Class(std::unique_ptr<Element>&&, const Log* = nullptr);
	Class(const Class<std::unique_ptr<Element>>&) = delete;
	Class<std::unique_ptr<Element>>& operator =(
			const Class<std::unique_ptr<Element>>&) = delete;
	Class(Class<std::unique_ptr<Element>> &&);
	Class<std::unique_ptr<Element>>& operator =(
			Class<std::unique_ptr<Element>> &&);
};
using Unique_ptr = Class<std::unique_ptr<Element>>;

std::string log_out_of_range_0(Primitive<size_t>, Primitive<size_t>, const Log&,
		bool = true);
std::out_of_range throw_out_of_range_0(Primitive<size_t>, Primitive<size_t>,
		const Log&);
std::invalid_argument throw_null_invalid_argument(Object&, const Log&);
std::string log_not_root(const Element&, const Log&, bool = true);
std::domain_error throw_not_root(const Element&, const Log&);
template<typename Type> std::ostringstream print_std__vector(
		const std::vector<Type>& sequence) {
	return Container_Printer(sequence, "\t", "")();
}
template<typename Type> std::ostringstream unprint(const Type& unprintable) {
	std::ostringstream result("{ ");

	result << &unprintable << " }";

	return result;
}
template<typename First, typename Second> std::ostringstream unprint_std__pair_first(
		const std::pair<First, Second>& unprintable) {
	std::ostringstream result("{ ");

	result << unprintable.first << "; " << &unprintable.second << " }";

	return result;
}
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
			Class<std::string> name, Primitive<size_t> position = 1,
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
	Ensemble(std::string, const Log* = nullptr, Fields = nullptr);
	friend Unique_ptr;
};

} /* namespace base */

#endif /* ELEMENT_H_ */
