/*
 * Object.cpp
 *
 *  Created on: 30 set 2018
 *      Author: m1r
 */

#include "Object.h"

namespace base {

const std::string ns = "base";
std::string substring_after(std::string string, const std::string match,
		bool from_end = false, size_t number) {
	auto position = from_end ? string.find(match, 0) : string.rfind(match);
	auto result =
			((from_end = number ? from_end : !from_end)) ?
					string.substr(0, position) :
					string.substr(position + match.length());

	if (number > 1)
		return substring_after(result, match, from_end, --number);
	else
		return result;
}

//Object
long long unsigned Object::tracker = 0;

const std::string& Object::has_label() const {
	return ns + "::" + label;
}
const std::string& Object::has_logger() const {
	return logger;
}
std::string Object::makes_track(const Object* caller,
		long long unsigned* track) {
	std::string result;
	auto tracking = ++tracker;

	if (caller)
		result = caller->logger + ".";
	if (track)
		*track = tracking;

	return result + std::to_string(tracking);
}

Object::Object(const Object* caller) {
	// TODO Auto-generated constructor stub
	label = ns + "::" + __func__;
	logger = makes_track(caller);
}
Object::Object(const Object& copy) {
	label = copy.label;
	logger = makes_track(nullptr);
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
void Log::log(std::string logger, std::string message, bool open,
		const Object* returning) {
	logger += ": ";

	if (returning) {
		if (open)
			std::clog << logger << "  }=" << returning->prints().str()
					<< std::endl;
		else
			std::clog << logger << message << "=" << returning->prints().str()
					<< std::endl;
	} else if (open)
		std::clog << logger << message << " {" << std::endl;
}
void Log::log_unary(const Log& logging, std::type_index type,
		const Object& operand, const Object* returning) {
	log(logging.logger,
			logging.prints().str() + "<" + type.name() + ">"
					+ operand.prints().str(), open, returning);
}
void Log::log_unary(const Object& operand, const Log& logging,
		std::type_index type, const Object* returning) {
	log(logging.logger,
			operand.prints().str() + "<" + type.name() + ">"
					+ logging.prints().str(), logging.open, returning);
}
void Log::log_binary(const Log& logging, std::type_index type,
		const Object& lefthand, const Object& righthand,
		const Object* returning) {
	log(logging.logger,
			type.name() + " " + lefthand.prints().str()
					+ (logging.label == "[]" ?
							"[" + righthand.prints().str() + "]" :
							" " + logging.prints().str() + " "
									+ righthand.prints().str()), logging.open,
			returning);
}
void Log::log_return(const Log& logging, Object& returning) {
	log(logging.logger, "=" + returning.prints().str(), logging.open,
			&returning);
	logging.open = false;
}
Log Log::as_unary(const Object* caller, bool open, std::type_index type,
		std::string operation, const Object& object) {
	Log result(caller, operation, open);

	log_unary(result, type, object, nullptr);

	return result;
}
Log Log::as_unary(const Object* caller, bool open, std::type_index type,
		const Object& object, std::string operation) {
	Log result(caller, operation, open);

	log_unary(object, result, type, nullptr);

	return result;
}
Log Log::as_binary(const Object* caller, bool open, std::type_index type,
		const Object& lefthand, std::string operation,
		const Object& righthand) {
	Log result(caller, operation, open);

	log_binary(result, type, lefthand, righthand, nullptr);

	return result;
}
void Log::notes(std::ostringstream message) const {
	if (open)
		std::clog << logger << "  " << message.str() << std::endl;
}
Object& Log::returns(Object& returning) const {
	log_return(*this, returning);

	return returning;
}
Object&& Log::returns(Object&& returning) const {
	log_return(*this, returning);

	return std::move(returning);
}
std::ostringstream Log::prints() const {
	std::ostringstream result;

	result << label << "{" << track << "}";

	return result;
}
void Log::logs_error(std::ostringstream message) const {
	std::cerr << logger << ": " << label << " " << message << std::endl;
}
Log Log::as_unary(const Object* caller, std::string operation,
		std::type_index type) const {
	return as_unary(caller, open, type, operation, *this);
}
Log Log::as_unary(const Object* caller, std::type_index type,
		std::string operation) const {
	return as_unary(caller, open, type, *this, operation);
}
Log Log::as_binary(const Object* caller, std::type_index type,
		std::string operation, const Object& righthand) const {
	return as_binary(caller, open, type, *this, operation, righthand);
}

Log::Log(const Object* caller, std::string label, bool open) {
	// TODO Auto-generated constructor stub
	logger = makes_track(caller, &track);
	this->label = label;
	this->open = open;
}
Log::~Log() {
	// TODO Auto-generated destructor stub
	if (open)
		std::clog << logger << "  }" << std::endl;
}
Log::Log(Log&& moving):
		Object(std::forward<Log&&>(moving)){
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

} /* namespace base */
