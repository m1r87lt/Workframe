/*
 * Object.h
 *
 *  Created on: 30 set 2018
 *      Author: m1r
 */

#ifndef OBJECT_H_
#define OBJECT_H_
#include <string>
#include <sstream>
#include <typeindex>
#include <iostream>
#include <functional>
#include <utility>

namespace base {

bool is_running();
void ends_run();
std::string give_substring_after(std::string, std::string, bool, size_t = 1);
template<typename ... Arguments> std::string make_scopes(std::string label) {
	return label;
}
template<typename ... Arguments> std::string make_scopes(std::string label,
		std::string ns, Arguments&& ... arguments) {
	auto recursive = make_scopes(arguments ...);

	if (ns.empty())
		recursive = ns + "::" + recursive;

	return recursive;
}

class Object {
	std::string label;
	std::string logger;
	static long long unsigned tracker;
public:
	virtual std::ostringstream prints() const = 0;
protected:
	const std::string& has_label() const;
	const std::string& has_logger() const;
	static std::string make_track(const Object*);

	Object(const Object*, std::string);
	virtual ~Object() = default;
	Object(const Object&);
	Object& operator =(const Object&);
	Object(Object&&);
	Object& operator =(Object&&) = delete;
};

class Log: virtual public Object {
	long long unsigned track;
	bool open;

	template<typename ... Arguments> static std::ostringstream log_arguments(
			Object& object, Arguments& ... arguments) {
		std::ostringstream result;

		result << object.prints().str() << ", "
				<< log_arguments(arguments ...).str();

		return result;
	}
	static std::ostringstream log_arguments();
	static void log(std::string, std::string, bool, const Object*);
	static void log_unary(const Log&, std::type_index, const Object&,
			const Object*);
	static void log_unary(const Object&, const Log&, std::type_index,
			const Object*);
	static void log_binary(const Log&, std::type_index, const Object&,
			const Object&, const Object*);
	template<typename ... Arguments> static void log_method(const Log& logging,
			std::type_index type, const Object* returning,
			Arguments&& ... arguments) {
		log(logging.has_logger(),
				logging.prints().str() + "<" + type.name() + ">("
						+ log_arguments(arguments ...).str() + ")",
				logging.open, returning);
	}
	static void log_return(Log&, Object&);
public:
	void notes(std::ostringstream) const;
	template<typename Type> Type& returns(Type& returning) const {
		log_return(const_cast<Log&>(*this), returning);

		return returning;
	}
	template<typename Type> Type&& returns(Type&& returning) const {
		log_return(const_cast<Log&>(*this), returning);

		return std::move(returning);
	}
	void logs_error(std::ostringstream) const;
	virtual std::ostringstream prints() const;
	static Log as_unary(std::string, const Object&, const Log* = nullptr, bool =
			true, std::type_index = typeid(void));
	static Log as_unary(const Object&, std::string, const Log* = nullptr, bool =
			true, std::type_index = typeid(void));
	static Log as_binary(const Object&, std::string, const Object&, const Log* =
			nullptr, bool = true, std::type_index = typeid(void));
	template<typename ... Arguments> static Log as_method(std::string label,
			bool open, const Log* caller = nullptr, std::type_index type =
					typeid(void), Arguments& ... arguments) {
		Log result(caller, label, open);

		log_method(result, type, nullptr, arguments ...);

		return result;
	}

	virtual ~Log();
	Log(const Log&) = delete;
	Log& operator =(const Log&) = delete;
	Log(Log&&);
	Log& operator =(Log&&);
protected:
	template<bool Open = true> Log as_unary(std::string operation,
			const Log* caller = nullptr,
			std::type_index type = typeid(void)) const {
		return as_unary(operation, *this, caller, Open, type);
	}
	template<bool Open = true> Log as_unary(const Log* caller,
			std::string operation, std::type_index type = typeid(void)) const {
		return as_unary(*this, operation, caller, Open, type);
	}
	template<bool Open = true> Log as_binary(std::string operation,
			const Object& righthand, const Log* caller = nullptr,
			std::type_index type = typeid(void)) const {
		return as_binary(*this, operation, righthand, caller, Open, type);
	}
	template<bool Open = true, typename ... Arguments> Log as_method(
			std::string label, const Log* caller = nullptr,
			std::type_index type = typeid(void),
			Arguments& ... arguments) const {
		return as_method(has_label() + "." + label, Open, caller, type,
				arguments ...);
	}
	template<bool Open = true, typename ... Arguments> Log as_constructor(
			std::string ns, std::string label, const Log* caller = nullptr,
			Arguments& ... arguments) {
		Log result(caller, label, Open);

		log(has_logger(),
				make_scopes(label, ns, label) + "("
						+ log_arguments(arguments ...).str() + ")", Open,
				nullptr);

		return result;
	}
	template<bool Open = false, typename ... Arguments> Log as_destructor(
			std::string ns, std::string label) {
		Log result(nullptr, label, Open);

		log(has_logger(),
				make_scopes(label, ns,
						give_substring_after(label, "~", false, 1)) + "()",
				Open, nullptr);

		return result;
	}

	Log(const Log*, std::string, bool);
};

struct Void final: Object {
	virtual std::ostringstream prints() const;

	Void(const Log*);
};

template<typename Type> class Primitive final: public Object {
	Type value;
public:
	operator Type() const {
		return value;
	}
	virtual std::ostringstream prints() const {
		std::ostringstream result;

		result << value;

		return result;
	}

	Primitive(Type value, const Log* caller = nullptr) :
			Object(caller, typeid(Type).name()) {
		this->value = value;
	}
	Primitive<Type>& operator =(Type value) {
		this->value = value;

		return *this;
	}
};
template<> class Primitive<const char*> final: public Object {
	const char* value;
public:
	operator const char*() const;
	virtual std::ostringstream prints() const;

	Primitive(const char*, const Log* = nullptr);
	Primitive<const char*>& operator =(const char*);
};

template<typename Type> class Class final: public Object {
	Type value;
public:
	static std::function<std::ostringstream(const Type&)> printer;

	const Type& is() const {
		return value;
	}
	Type& is() {
		return value;
	}
	Type&& becomes() {
		return std::move(value);
	}
	virtual std::ostringstream prints() const {
		return printer(value);
	}

	template<typename ... Arguments> Class(const Log* caller,
			Arguments&& ... arguments) :
			Object(caller, typeid(Type).name()), value(arguments ...) {
	}
	Class(Type& copy, const Log* caller = nullptr) :
			Object(caller, typeid(Type).name()), value(copy) {
	}
	Class<Type>& operator =(Type& copy) {
		value = copy;

		return *this;
	}
	Class(Type&& copy, const Log* caller = nullptr) :
			Object(caller, typeid(Type).name()), value(std::move(copy)) {
	}
	Class<Type>& operator =(Type&& copy) {
		value = std::move(copy);

		return *this;
	}
};
template<> class Class<std::string> final: public Object {
	std::string value;
public:
	const std::string& is() const;
	std::string& is();
	std::string&& becomes();
	virtual std::ostringstream prints() const;

	Class(std::string, const Log* = nullptr);
	Class<std::string>& operator =(std::string);
};

template<typename Type> Primitive<Type> unary_primitive(std::string operation,
		const Object& object, Type returning, const Log* caller = nullptr) {
	auto log = Log::as_unary(operation, object, caller, false,
			typeid(returning));

	return log.returns(Class<Type>(&log, returning));
}
template<typename Type> Class<Type> unary_class(std::string operation,
		const Object& object, Type&& returning, const Log* caller = nullptr) {
	auto log = Log::as_unary(operation, object, caller, false,
			typeid(returning));

	return log.returns(Class<Type>(&log, returning));
}
template<typename Type> Primitive<Type> unary_primitive(const Object& object,
		std::string operation, Type returning, const Log* caller = nullptr) {
	auto log = Log::as_unary(object, operation, caller, false,
			typeid(returning));

	return log.returns(Primitive<Type>(&log, returning));
}
template<typename Type> Class<Type> unary_class(const Object& object,
		std::string operation, Type&& returning, const Log* caller = nullptr) {
	auto log = Log::as_unary(object, operation, caller, false,
			typeid(returning));

	return log.returns(Class<Type>(&log, returning));
}
template<typename Type> Primitive<Type> binary_primitive(const Object& lefthand,
		std::string operation, const Object& righthand, Type returning,
		const Log* caller = nullptr) {
	auto log = Log::as_binary(lefthand, operation, righthand, caller, false,
			typeid(returning));

	return log.returns(Primitive<Type>(&log, returning));
}
template<typename Type> Class<Type> binary_class(const Object& lefthand,
		std::string operation, const Object& righthand, Type&& returning,
		const Log* caller = nullptr) {
	auto log = Log::as_binary(lefthand, operation, righthand, caller, false,
			typeid(returning));

	return log.returns(Class<Type>(&log, returning));
}
template<typename Type, typename ... Arguments> Primitive<Type> method_primitive(
		Type returning, std::string label, const Log* caller = nullptr,
		Arguments& ... arguments) {
	auto log = as_method(label, false, caller, typeid(returning),
			arguments ...);

	return log.returns(Primitive<Type>(&log, returning));
}
template<typename Type, typename ... Arguments> Primitive<Type> method_primitive(
		Type returning, const Object& object, std::string label,
		const Log* caller = nullptr, Arguments& ... arguments) {
	auto log = as_method(object.has_label() + "." + label, false, caller,
			typeid(returning), arguments ...);

	return log.returns(Primitive<Type>(&log, returning));
}
template<typename Type, typename ... Arguments> Class<Type> method_class(
		Type&& returning, std::string label, const Log* caller = nullptr,
		Arguments& ... arguments) {
	auto log = as_method(label, false, caller, typeid(returning),
			arguments ...);

	return log.returns(Class<Type>(&log, returning));
}
template<typename Type, typename ... Arguments> Class<Type> method_class(
		Type&& returning, const Object& object, std::string label,
		const Log* caller = nullptr, Arguments& ... arguments) {
	auto log = Log::as_method(object.prints().str() + "." + label, false,
			caller, typeid(returning), arguments ...);

	return (Class<Type> &&) log.returns(Class<Type>(&log, returning));
}

} /* namespace base */

#endif /* OBJECT_H_ */

