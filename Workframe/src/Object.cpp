/*
 * Object.cpp
 *
 *  Created on: 30 set 2018
 *      Author: m1r
 */

#include "Object.h"
#include <iostream>
#include <cstring>

namespace base {

bool run = true;
bool is_running() {
	return run;
}
void ends_run() {
	run = false;
}
std::string give_substring_after(std::string string, const std::string match,
		bool from_end = false, int number) {
	auto position = from_end ? string.rfind(match) : string.find(match);
	auto after = position + match.length();
	auto rest = string.substr(after);

	string.substr(position);
	if (number == 1)
		return rest;
	else if (number == -1)
		return string.substr(0, position);
	else if (number > 1)
		return give_substring_after(rest, match, from_end, --number);
	else if (number < -1)
		return string.substr(0, after)
				+ give_substring_after(rest, match, from_end, ++number);
	else
		return string;
}
std::string make_scopes(std::string scope0, std::string scope1) {
	return scope0 + "::" + scope1;
}

//Object
long long unsigned Object::tracker = 0;

void Object::makes_track(const Object* caller) {
	if (caller && !caller->logger.empty())
		logger = caller->logger + ".";
}
void Object::makes_track(std::string logger) {
	if (!logger.empty())
		try {
			this->logger = give_substring_after(logger, ".", true, -1) + ".";
		} catch (...) {
			this->logger = logger + ".";
		}
}
void Object::constructs(std::string label) {
	this->label = label;
	logger += std::to_string(track = ++tracker);
}
void Object::becomes(Object&& moving) {
	track = moving.track;
	label = moving.label;
	logger = moving.logger;
}

const std::string& Object::has_label() const {
	return label;
}
const std::string& Object::has_logger() const {
	return logger;
}

Object::Object(const Object* caller, std::string label) {
	makes_track(caller);
	constructs(label);
}
Object::Object(const Object& copy) {
	makes_track(copy.logger);
	constructs(copy.label);
}
Object::Object(Object&& moving) {
	becomes(std::forward<Object&&>(moving));
}
Object& Object::operator =(Object&& assigning) {
	becomes(std::forward<Object&&>(assigning));

	return *this;
}

//Log
std::ostringstream Log::log_arguments() {
	return std::ostringstream();
}
void Log::log(std::string logger, std::string function, bool open,
		const Object* returning) {
	std::clog
			<< logger + ": "
					+ (returning ?
							(open ? "  }=" : function + "=")
									+ returning->prints().str() :
							function + (open ? " {" : "")) << std::endl;
}
void Log::log_unary(const Log& logging, std::type_index type,
		const Object& operand, const Object* returning) {
	log(logging.has_logger(),
			logging.prints().str() + "<" + type.name() + ">"
					+ operand.prints().str(), logging.open, returning);
}
void Log::log_unary(const Object& operand, const Log& logging,
		std::type_index type, const Object* returning) {
	log(logging.has_logger(),
			operand.prints().str() + "<" + type.name() + ">"
					+ logging.prints().str(), logging.open, returning);
}
void Log::log_binary(const Log& logging, std::type_index type,
		const Object& lefthand, const Object& righthand,
		const Object* returning) {
	log(logging.has_logger(),
			std::string(type.name()) + " " + lefthand.prints().str()
					+ (logging.has_label() == "[]" ?
							"[" + righthand.prints().str() + "]" :
							" " + logging.prints().str() + " "
									+ righthand.prints().str()), logging.open,
			returning);
}
void Log::log_return(Log& logging, const Object& returning) {
	log(logging.has_logger(), returning.prints().str(), logging.open,
			&returning);
	logging.open = false;
}

void Log::notes(std::ostringstream message) const {
	if (open)
		std::clog << has_logger() << "  " << message.str() << std::endl;
}
std::string Log::logs_error(std::ostringstream message) const {
	std::string result = has_logger() + ": " + has_label() + " "
			+ message.str();

	std::cerr << result << std::endl;

	return result;
}

std::ostringstream Log::prints() const {
	std::ostringstream result;

	result << has_label() << "{" << track << "}";

	return result;
}

Log Log::as_unary(std::string operation, const Object& object, bool open,
		std::type_index type) {
	Log result(nullptr, operation, open);

	log_unary(result, type, object, nullptr);

	return result;
}
Log Log::as_unary(const Object& object, std::string operation, bool open,
		std::type_index type) {
	Log result(nullptr, operation, open);

	log_unary(object, result, type, nullptr);

	return result;
}
Log Log::as_binary(const Object& lefthand, std::string operation,
		const Object& righthand, bool open, std::type_index type) {
	Log result(nullptr, operation, open);

	log_binary(result, type, lefthand, righthand, nullptr);

	return result;
}

Log::Log(const Log* caller, std::string label, bool open) :
		Object(caller, label) {
	this->open = open;
}

Log::~Log() {
	if (open)
		std::clog << has_logger() << "  }" << std::endl;
}
Log::Log(Log&& moving) :
		Object(std::forward<Log&&>(moving)) {
	open = moving.open;
	moving.open = false;
}
Log& Log::operator =(Log&& moving) {
	Object::operator =(std::forward<Log&&>(moving));
	open = moving.open;
	moving.open = false;

	return *this;
}

//Primitive<const char*>
Primitive<const char*>::operator const char*() const {
	return value;
}
std::ostringstream Primitive<const char*>::prints() const {
	std::ostringstream result;

	result << "\"" << value << "\"";

	return result;
}

Primitive<const char*>::Primitive(const char* value, const Log* caller) :
		Object(caller, value) {
	this->value = value;
}
Primitive<const char*>::Primitive(const Primitive<const char*>& copy) :
		Object(copy) {
	delete value;
	const_cast<char*&>(value) = new char[strlen(copy.value)];
	strcpy(const_cast<char*&>(value), copy.value);
}
Primitive<const char*>& Primitive<const char*>::operator =(
		const Primitive<const char*> & copy) {
	delete value;
	const_cast<char*&>(value) = new char[strlen(copy.value)];
	strcpy(const_cast<char*&>(value), copy.value);

	return *this;
}

//Class<std::string>
const std::string& Class<std::string>::is() const {
	return value;
}
std::string& Class<std::string>::is() {
	return value;
}
std::string&& Class<std::string>::becomes() {
	return std::move(value);
}
Class<std::string>& Class<std::string>::operator =(std::string copy) {
	value = copy;

	return *this;
}
std::ostringstream Class<std::string>::prints() const {
	std::ostringstream result;

	result << "\"" << value << "\"";

	return result;
}

Class<std::string>::Class(std::string value, const Log* caller) :
		Object(caller, value) {
	this->value = value;
}
Class<std::string>& Class<std::string>::operator =(
		const Class<std::string>& copy) {
	value = copy.value;

	return *this;
}

} /* namespace base */
