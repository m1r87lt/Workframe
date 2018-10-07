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
std::string substring_after(std::string, std::string, bool, size_t = 1);
template<typename ... Arguments> std::string scopes(std::string label,
		std::string ns, Arguments&& ... arguments) {
	std::string recursive = scopes(arguments ...);

	if (ns.empty())
		recursive = ns + "::" + recursive;

	return recursive;
}
template<typename ... Arguments> std::string scopes(std::string label) {
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

	template<typename ... Arguments> static std::ostringstream log_arguments(
			Object& object, Arguments& ... arguments) {
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
	static Log as_unary(const Log*, bool, std::type_index, std::string,
			const Object&);
	static Log as_unary(const Log*, bool, std::type_index, const Object&,
			std::string);
	static Log as_binary(const Log*, bool, std::type_index, const Object&,
			std::string, const Object&);
	template<typename ... Arguments> static Log as_method(const Log* caller,
			bool open, std::string label, std::type_index type,
			Arguments& ... arguments) {
		Log result(caller, label, open);

		log_method(result, type, nullptr, arguments ...);

		return result;
	}
protected:
	Log(const Log*, std::string, bool);

	Log as_unary(const Log*, std::string, std::type_index) const;
	Log as_unary(const Log*, std::type_index, std::string) const;
	Log as_binary(const Log*, std::type_index, std::string,
			const Object&) const;
	template<typename ... Arguments> Log as_method(const Log* caller,
			std::string label, std::type_index type,
			Arguments& ... arguments) const {
		return as_method(caller, open, this->label + "::" + label, type,
				arguments ...);
	}
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
	operator Type&() const {
		return value;
	}
	virtual std::ostringstream prints() const {
		std::ostringstream result;

		result << this;

		return result;
	}

	template<typename ... Arguments> Class(const Log* caller, Type value,
			std::string name, Arguments&& ... arguments) :
			Object(caller, name) {
		value(arguments ...);
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
	operator const std::string&() const {
		return value;
	}
	operator std::string&() {
		return value;
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

} /* namespace base */

#endif /* OBJECT_H_ */

