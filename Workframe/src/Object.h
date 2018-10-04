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

std::string substring_after(std::string, std::string, size_t, bool = false);

class Object {
	std::string label;
	std::string logger;
	static long long unsigned tracker;
public:
	virtual std::ostringstream prints() const = 0;
protected:
	Object(const Object*);
	virtual ~Object() = default;
	Object(const Object&);
	Object& operator =(const Object&) = delete;
	Object(Object&&);
	Object& operator =(Object&&) = delete;

	const std::string& has_label() const;
	const std::string& has_logger() const;
	static std::string makes_track(const Object*, long long unsigned* = nullptr);
};

class Log: public Object {
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
				logging.prints() + "<" + type.name() + ">("
						+ log_arguments(arguments ...).str() + ")",
				logging.open, returning);
	}
	static void log_return(const Log&, Object&);

	Log(const Object*, std::string, bool);
public:
	static Log as_unary(const Object*, bool, std::type_index, std::string,
			const Object&);
	static Log as_unary(const Object*, bool, std::type_index, const Object&,
			std::string);
	static Log as_binary(const Object*, bool, std::type_index, const Object&,
			std::string, const Object&);
	template<typename ... Arguments> static Log as_method(const Object* caller,
			bool open, std::string label, std::type_index type,
			Arguments& ... arguments) {
		Log result(caller, label, open);

		log_method(result, type, nullptr, arguments ...);

		return result;
	}
	void notes(std::ostringstream) const;
	Object& returns(Object&) const;
	Object&& returns(Object&&) const;
	void logs_error(std::ostringstream) const;
	virtual std::ostringstream prints() const;

	virtual ~Log();
	Log(const Log&);
	Log& operator =(const Log&);
	Log(Log&&);
	Log& operator =(Log&&);
protected:
	Log as_unary(const Object*, std::string, std::type_index) const;
	Log as_unary(const Object*, std::type_index, std::string) const;
	Log as_binary(const Object*, std::type_index, std::string,
			const Object&) const;
	template<typename ... Arguments> Log as_method(const Object* caller,
			std::string label, std::type_index type,
			Arguments& ... arguments) const {
		return as_method(caller, open, label, type, arguments ...);
	}
};

} /* namespace base */

#endif /* OBJECT_H_ */

