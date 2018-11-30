/*
 * Object.cpp
 *
 *  Created on: 30 set 2018
 *      Author: m1r
 */

#include "Object.h"
#include <iostream>

namespace base {

bool run = true;
bool is_running() {
	return run;
}
void ends_run() {
	run = false;
}
std::string give_substring_after(std::string string, const std::string match,
		bool from_end = false, size_t number) {
	auto position = from_end ? string.find(match, 0) : string.rfind(match);
	auto result =
			((from_end = number ? from_end : !from_end)) ?
					string.substr(0, position) :
					string.substr(position + match.length());

	if (number > 1)
		return give_substring_after(result, match, from_end, --number);
	else
		return result;
}
std::string make_scopes(std::string label) {
	return label;
}

//Object
long long unsigned Object::tracker = 0;

const std::string& Object::has_label() const {
	return label;
}
const std::string& Object::has_logger() const {
	return logger;
}
std::string Object::make_track(const Object* caller) {
	std::string result;

	if (caller)
		result = caller->logger + ".";

	return result + std::to_string(++tracker);
}

Object::Object(const Object* caller, std::string label) {
	this->label = label;
	logger = make_track(caller);
}
Object::Object(const Object& copy) {
	label = copy.label;
	logger = make_track(nullptr);
}
Object& Object::operator =(const Object& copy) {
	label = copy.label;
	logger = copy.logger;

	return *this;
}
Object::Object(Object&& moving) {
	label = moving.label;
	logger = moving.logger;
}

//Log
std::ostringstream Log::log_arguments() {
	return std::ostringstream();
}
void Log::log(std::string logger, std::string function, bool open,
		const Object* returning) {
	logger += ": "
			+ (returning ?
					(open ? "  }=" : function + "=")
							+ returning->prints().str() :
					function + (open ? " {" : ""));

	std::clog << logger << std::endl;
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
void Log::log_return(Log& logging, Object& returning) {
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
Log Log::as_unary(std::string operation, const Object& object,
		const Log* caller, bool open, std::type_index type) {
	Log result(caller, operation, open);

	log_unary(result, type, object, nullptr);

	return result;
}
Log Log::as_unary(const Object& object, std::string operation,
		const Log* caller, bool open, std::type_index type) {
	Log result(caller, operation, open);

	log_unary(object, result, type, nullptr);

	return result;
}
Log Log::as_binary(const Object& lefthand, std::string operation,
		const Object& righthand, const Log* caller, bool open,
		std::type_index type) {
	Log result(caller, operation, open);

	log_binary(result, type, lefthand, righthand, nullptr);

	return result;
}

Log::Log(const Log* caller, std::string label, bool open) :
		Object(caller, label) {
	track = std::stoull(give_substring_after(has_logger(), ".", true, 0));
	this->open = open;
}
Log::~Log() {
	if (open)
		std::clog << has_logger() << "  }" << std::endl;
}
Log::Log(Log&& moving) :
		Object(std::forward<Log&&>(moving)) {
	track = moving.track;
	open = moving.open;
	moving.open = false;
}
Log& Log::operator =(Log&& moving) {
	Object::operator =(moving);
	track = moving.track;
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
Primitive<const char*>& Primitive<const char*>::operator =(
		Primitive<const char*> && moving) {
	value = moving.value;

	return *this;
}
Primitive<const char*>& Primitive<const char*>::operator =(const char* value) {
	this->value = value;

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
std::ostringstream Class<std::string>::prints() const {
	std::ostringstream result;

	result << "\"" << value << "\"";

	return result;
}

Class<std::string>::Class(std::string value, const Log* caller) :
		Object(caller, value) {
	this->value = value;
}
Class<std::string> Class<std::string>::operator =(
		Class<std::string> && moving) {
	value = moving.value;

	return *this;
}
Class<std::string>& Class<std::string>::operator =(std::string copy) {
	value = copy;

	return *this;
}

} /* namespace base */
