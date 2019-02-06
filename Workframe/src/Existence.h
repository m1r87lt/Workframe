/*
 * Existence.h
 *
 *  Created on: 01 jan 2019
 *      Author: m1r
 */

#ifndef EXISTENCE_H_
#define EXISTENCE_H_

#include <string>
#include <typeindex>
#include <utility>
#include <chrono>
#include <map>
#include <sstream>
#include <functional>
#include <set>
#include <memory>
#include <list>
#include <vector>
#include <iostream>
#define NAME(Parameter) base::Name<Parameter>()(#Parameter)
#define TYPE(parameter) base::Name<decltype(parameter)>()(#parameter)
#define CLASS(type) base::Class<decltype(type)>(type, #type)
#define VARIABLE(type) ((base::Variable::Field) base::Class<decltype(type)>(type, #type))

namespace base {
template<typename Type> struct Name {
	std::string operator ()(const char* name) {
		return name;
	}
};

class Process {
	static bool run;
public:
	static void end();
	static bool running();
};

class Variable {
	std::string name;
	std::string type;
protected:
	virtual operator std::string() const = 0;
public:
	using Field = std::pair<const std::string, std::string>;

	operator Field() const;
	std::string operator ()() const;
	Variable(std::string, std::type_index);
	virtual ~Variable() = default;
	Variable(const Variable&) = delete;
	Variable& operator =(const Variable&) = delete;
	Variable(Variable&&) = default;
	Variable& operator =(Variable&&) = delete;
};
template<typename Type> class Class: public Variable {
	const Type* value;
	static std::function<std::string(const Type*)> printer;

	virtual operator std::string() const {
		return printer(value);
	}
public:
	Class(const Type& value, const char* name) :
			Variable(name, typeid(const Type)) {
		this->value = &value;
	}
};
template<typename First, typename Second> std::string print_std__map(
		const std::map<First, Second>* map) {
	std::string result = "{";

	for (auto field : *map)
		result += "\n\t" + VARIABLE(field.first).second + ": "
				+ VARIABLE(field.second).second;

	return result + (result == "{" ? " " : "\n") + "}";
}
template<typename Type> std::string print_std__set(const std::set<Type>* set) {
	std::string result = "{";

	for (auto content : *set)
		result += "\n\t( " + VARIABLE(content).second + " )";

	return result + (result == "{" ? " " : "\n") + "}";
}
template<typename Type> std::string print_std__vector(const std::vector<Type>* vector) {
	std::string result = "{";

	for (auto content : *vector)
		result += "\n\t" + VARIABLE(content).second + ",";

	if (result == "{")
		result += " ";
	else
		result.back() = '\n';

	return result + "}";
}
template<typename Type, size_t N> std::string print_std__array(const std::array<Type, N>* array) {
	std::string result = "{";

	for (auto content : *array)
		result += "\n\t" + VARIABLE(content).second + ",";

	if (result == "{")
		result += " ";
	else
		result.back() = '\n';

	return result + "}";
}
template<typename Type> std::string print_fundamental(const Type* type) {
	std::ostringstream result("{");

	result << *type << " }";

	return result.str();
}

struct Object {
	using Instant = std::chrono::steady_clock::time_point;
	using Fields = std::map<std::string, std::string>;
	using Modifications = std::map<std::string, Variable::Field>;

	Instant exists_from() const;
	Instant is_since() const;
	Fields gives_attributes() const;
	void gets_attributes(Fields);
	Modifications gives_modifications();
	bool operator ==(const Object&);
	bool operator !=(const Object&);
	virtual Fields shows() const;
	virtual std::string prints() const = 0;
private:
	Instant creation;
	Instant modification;
	Fields attributes;
	Fields last_attributes;
	static Instant start;
	friend Class<Instant> ;
	void initializes(Fields = Fields());
protected:
	void is_modified();
	Object();
	Object(Fields);
	virtual ~Object() = default;
	Object(const Object&);
	Object& operator =(const Object&);
};

struct Element: virtual public Object {
	virtual Fields shows() const;
	static std::set<Element*> give_everything();
	static Variable::Field shows_everything();
	virtual ~Element();
private:
	Element* position;
	static std::set<Element*> everything;

	void initializes();
protected:
	friend class Ensemble;
	Element();
	Element(Fields);
	Element(const Element&);
	Element& operator =(const Element&);
	Element(Element&&);
	Element& operator =(Element&&) = delete;
};
struct Ensemble: public Element {
	using Unique_ptr = std::unique_ptr<Element>;

	using Content = std::pair<std::string, Unique_ptr>;
	using Container = std::list<Content>;

	Element& operator [](size_t) const;
	std::map<size_t, Element*> operator [](std::string) const;
	template<typename Type> std::map<size_t, Element*> finds() const {
		std::map<size_t, Element*> result;
		auto current = container.begin();
		auto end = container.end();

		for (size_t position = 1; current != end; ++position) {
			if (std::is_same<Type, std::decay<decltype(*current->second)>>::value)
				result.emplace(position, *current);
			++current;
		}

		return result;
	}
	size_t which_is(std::string) const;
	std::string who_is(size_t = 1) const;
	Unique_ptr gives(size_t = 1);
	Unique_ptr gives(std::string);
	void gets(std::string, Unique_ptr&&, size_t = 1);
	void takes(size_t, Ensemble&, size_t = 1);
	void takes(std::string, Ensemble&, size_t = 1);
	size_t has_size() const;
	void self_clears();
	template<typename Type, typename ... Arguments> Type* generates(
			std::string name, size_t position = 1, Arguments&& ... arguments) {
		auto result = new Type(std::forward<Arguments&&>(arguments) ...);

		gets(name, Unique_ptr(result), position);

		return result;
	}
	virtual Fields shows() const;
	static std::tuple<Ensemble*, size_t, std::string> localize(const Element&);
	static Unique_ptr pop(Element&);
	static void take(Element&, Ensemble&, size_t);
	static std::vector<std::string> have_path(const Element&);
	static std::string log_root_element(const Element&);
	static std::string log_out_of_range_0(size_t, size_t);
	static std::out_of_range throw_out_of_range_0(size_t, size_t);
	static std::domain_error throw_root_element(const Element&);
	static std::runtime_error throw_wrong_position(const Element&,
			const Ensemble*);
private:
	Container container;

	std::string names(std::string) const;
	bool names(std::string, std::string) const;
	Container::iterator localizes(size_t) const;
	std::pair<size_t, Container::iterator> localizes(std::string) const;
	Unique_ptr gives(Container::iterator);
protected:
	Ensemble() = default;
	Ensemble(Fields);
	virtual ~Ensemble() = default;
	Ensemble(const Ensemble&);
	Ensemble& operator =(const Ensemble&) = delete;
	Ensemble(Ensemble&&) = delete;
};

struct Throw {
	static std::invalid_argument invalid_argument(const Object&);
	static std::invalid_argument null_argument(std::string, size_t);
	static std::logic_error not_allowed(std::string);
};

} /* namespace base */

#endif /* EXISTENCE_H_ */

