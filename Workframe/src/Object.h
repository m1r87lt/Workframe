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

namespace base {

bool is_running();
void ends_run();
std::string give_substring_after(std::string, std::string, bool, size_t = 1);
template<typename ... Arguments> auto make_scopes(std::string label,
		std::string ns, Arguments&& ... arguments) {
	std::string recursive = scopes(arguments ...);

	if (ns.empty())
		recursive = ns + "::" + recursive;

	return recursive;
}
template<typename ... Arguments> auto make_scopes(std::string label) {
	return label;
}

class Object {
	std::string label;
	std::string logger;
	static long long unsigned tracker;
public:
	virtual std::ostringstream prints() const = 0;
protected:
	Object(const Object*, std::string);
	virtual ~Object() = default;
	Object(const Object&);
	Object& operator =(const Object&);
	Object(Object&&);
	Object& operator =(Object&&) = delete;

	const std::string& has_label() const;
	const std::string& has_logger() const;
	static std::string makes_track(const Object*);
};

class Log: virtual public Object {
	long long unsigned track;
	bool open;

	template<typename ... Arguments> static auto log_arguments(Object& object,
			Arguments& ... arguments) {
		return object.prints() << ", " << log_arguments(arguments ...);
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
		log(logging.logger,
				logging.prints().str() + "<" + type.name() + ">("
						+ log_arguments(arguments ...).str() + ")",
				logging.open, returning);
	}
	static void log_return(Log&, Object&);
public:
	virtual ~Log();
	Log(const Log&) = delete;
	Log& operator =(const Log&) = delete;
	Log(Log&&);
	Log& operator =(Log&&);

	void notes(std::ostringstream) const;
	Object& returns(Object&) const;
	Object&& returns(Object&&) const;
	void logs_error(std::ostringstream) const;
	virtual std::ostringstream prints() const;
	static Log as_unary(std::string, const Object&, const Log* = nullptr, bool =
			true, std::type_index = typeid(void));
	static Log as_unary(const Object&, std::string, const Log* = nullptr, bool =
			true, std::type_index = typeid(void));
	static Log as_binary(const Object&, std::string, const Object&, const Log* =
			nullptr, bool = true, std::type_index = typeid(void));
	template<typename ... Arguments> static auto as_method(std::string label,
			bool open, const Log* caller = nullptr, std::type_index type =
					typeid(void), Arguments& ... arguments) {
		Log result(caller, label, open);

		log_method(result, type, nullptr, arguments ...);

		return result;
	}
protected:
	Log(const Log*, std::string, bool);

	template<bool Open = open> Log Log::as_unary(std::string operation,
			const Log* caller = nullptr,
			std::type_index type = typeid(void)) const {
		return as_unary(operation, *this, caller, Open, type);
	}
	template<bool Open = open> Log Log::as_unary(const Log* caller,
			std::type_index type, std::string operation) const {
		return as_unary(*this, operation, caller, Open, type);
	}
	template<bool Open = open> Log Log::as_binary(const Log* caller,
			std::type_index type, std::string operation,
			const Object& righthand) const {
		return as_binary(*this, operation, righthand, caller, Open, type);
	}
	template<bool Open = open, typename ... Arguments> Log as_method(
			std::string label, const Log* caller = nullptr,
			std::type_index type = typeid(void),
			Arguments& ... arguments) const {
		return as_method(has_label() + "." + label, Open, caller, type,
				arguments ...);
	}
};

struct Void final: Object {
	virtual std::ostringstream prints() const;

	Void(const Log*);
	~Void();
};

template<typename Type> class Primitive final: Object {
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

	Primitive(const Log* caller, Type value) :
			Object(caller, typeid(Type).name()) {
		this->value = value;
		std::clog << has_logger() << ": " << __func__ << "<" << has_label()
				<< ">" << value << std::endl;
	}
	~Primitive() {
		std::clog << has_logger() << "  " << __func__ << "<" << has_label()
				<< ">" << value << std::endl;
	}
};
template<> class Primitive<const char*> final: Object {
	const char* value;
public:
	operator const char*() const {
		return value;
	}
	virtual std::ostringstream prints() const {
		std::ostringstream result;

		result << "\"" << value << "\"";

		return result;
	}

	Primitive(const Log* caller, const char* value) :
			Object(caller, value) {
		this->value = value;
		std::clog << has_logger() << ": " << __func__ << "<" << has_label()
				<< ">" << value << std::endl;
	}
	~Primitive() {
		std::clog << has_logger() << "  " << __func__ << "<" << has_label()
				<< ">" << value << std::endl;
	}
};

template<typename Type> class Class final: Object {
	Type value;
public:
	Type& is() const {
		return value;
	}
	Type&& becomes() const {
		return std::move(value);
	}
	virtual std::ostringstream prints() const {
		std::ostringstream result;

		result << this;

		return result;
	}

	template<typename ... Arguments> Class(const Log* caller, Type value,
			std::string name, Arguments&& ... arguments) :
			Object(caller, name), value(arguments ...) {
		std::clog << has_logger() << ": " << __func__ << "<" << has_label()
				<< ">" << value << std::endl;
	}
	~Class() {
		std::clog << has_logger() << "  " << __func__ << "<" << has_label()
				<< ">" << value << std::endl;
	}
};
template<> class Class<std::string> final: Object {
	std::string value;
public:
	std::string& is() const {
		return value;
	}
	std::string&& becomes() const {
		return std::move(value);
	}
	virtual std::ostringstream prints() const {
		std::ostringstream result;

		result << "\"" << value << "\"";

		return result;
	}

	Class(const Log* caller, std::string value) :
			Object(caller, value) {
		this->value = value;
		std::clog << has_logger() << ": " << __func__ << "<" << has_label()
				<< ">" << value << std::endl;
	}
	~Class() {
		std::clog << has_logger() << "  " << __func__ << "<" << has_label()
				<< ">" << value << std::endl;
	}
};

template<typename Type> auto unary_primitive(std::string operation,
		const Object& object, Type returning, const Log* caller = nullptr) {
	auto log = Log::as_unary(operation, object, caller, false,
			typeid(returning));

	return log.returns(Primitive<Type>(&log, returning));
}
template<typename Type> auto unary_class(std::string operation,
		const Object& object, Type&& returning, const Log* caller = nullptr) {
	auto log = Log::as_unary(operation, object, caller, false,
			typeid(returning));

	return log.returns(Class<Type>(&log, returning));
}
template<typename Type> auto unary_primitive(const Object& object,
		std::string operation, Type returning, const Log* caller = nullptr) {
	auto log = Log::as_unary(object, operation, caller, false,
			typeid(returning));

	return log.returns(Primitive<Type>(&log, returning));
}
template<typename Type> auto unary_class(const Object& object,
		std::string operation, Type&& returning, const Log* caller = nullptr) {
	auto log = Log::as_unary(object, operation, caller, false,
			typeid(returning));

	return log.returns(Class<Type>(&log, returning));
}
template<typename Type> auto binary_primitive(const Object& lefthand,
		std::string operation, const Object& righthand, Type returning,
		const Log* caller = nullptr) {
	auto log = Log::as_binary(lefthand, operation, righthand, caller, false,
			typeid(returning));

	return log.returns(Primitive<Type>(&log, returning));
}
template<typename Type> auto binary_class(const Object& lefthand,
		std::string operation, const Object& righthand, Type&& returning,
		const Log* caller = nullptr) {
	auto log = Log::as_binary(lefthand, operation, righthand, caller, false,
			typeid(returning));

	return log.returns(Class<Type>(&log, returning));
}
template<typename Type, typename ... Arguments> auto method_primitive(
		Type returning, std::string label, const Log* caller = nullptr,
		Arguments& ... arguments) {
	auto log = as_method(label, false, caller, typeid(returning),
			arguments ...);

	return log.returns(Primitive<Type>(&log, returning));
}
template<typename Type, typename ... Arguments> auto method_primitive(
		Type returning, const Object& object, std::string label,
		const Log* caller = nullptr, Arguments& ... arguments) {
	auto log = as_method(object.has_label() + "." + label, false, caller,
			typeid(returning), arguments ...);

	return log.returns(Primitive<Type>(&log, returning));
}
template<typename Type, typename ... Arguments> auto method_class(
		Type&& returning, std::string label, const Log* caller = nullptr,
		Arguments& ... arguments) {
	auto log = as_method(label, false, caller, typeid(returning),
			arguments ...);

	return log.returns(Class<Type>(&log, returning));
}
template<typename Type, typename ... Arguments> auto method_class(
		const Log* caller, Type&& returning, const Object& object,
		std::string label, Arguments& ... arguments) {
	auto log = as_method(object.has_label() + "." + label, false, caller,
			typeid(returning), arguments ...);

	return log.returns(Class<Type>(&log, returning));
}

} /* namespace base */

#endif /* OBJECT_H_ */

