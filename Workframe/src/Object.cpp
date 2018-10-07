/*
 * Object.cpp
 *
 *  Created on: 30 set 2018
 *      Author: m1r
 */

#include "Object.h"

namespace base {

bool run = true;
bool is_running() {
	return run;
}
void ends_run() {
	run = false;
}
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
	return label;
}
const std::string& Object::has_logger() const {
	return logger;
}
std::string Object::makes_track(const Object* caller) {
	std::string result;

	if (caller)
		result = caller->logger + ".";

	return result + std::to_string(++tracker);
}

Object::Object(const Object* caller, std::string label) {
	// TODO Auto-generated constructor stub
	this->label = label;
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
	log(logging.has_logger(), "=" + returning.prints().str(), logging.open,
			&returning);
	logging.open = false;
}
void Log::notes(std::ostringstream message) const {
	if (open)
		std::clog << has_logger() << "  " << message.str() << std::endl;
}
Object& Log::returns(Object& returning) const {
	log_return(const_cast<Log&>(*this), returning);

	return returning;
}
Object&& Log::returns(Object&& returning) const {
	log_return(const_cast<Log&>(*this), returning);

	return std::move(returning);
}
void Log::logs_error(std::ostringstream message) const {
	std::cerr << has_logger() << ": " << has_label() << " " << message.str()
			<< std::endl;
}
std::ostringstream Log::prints() const {
	std::ostringstream result;

	result << has_label() << "{" << track << "}";

	return result;
}
Log Log::as_unary(const Log* caller, bool open, std::type_index type,
		std::string operation, const Object& object) {
	Log result(caller, operation, open);

	log_unary(result, type, object, nullptr);

	return result;
}
Log Log::as_unary(const Log* caller, bool open, std::type_index type,
		const Object& object, std::string operation) {
	Log result(caller, operation, open);

	log_unary(object, result, type, nullptr);

	return result;
}
Log Log::as_binary(const Log* caller, bool open, std::type_index type,
		const Object& lefthand, std::string operation,
		const Object& righthand) {
	Log result(caller, operation, open);

	log_binary(result, type, lefthand, righthand, nullptr);

	return result;
}
Log Log::as_unary(const Log* caller, std::string operation,
		std::type_index type) const {
	return as_unary(caller, open, type, operation, *this);
}
Log Log::as_unary(const Log* caller, std::type_index type,
		std::string operation) const {
	return as_unary(caller, open, type, *this, operation);
}
Log Log::as_binary(const Log* caller, std::type_index type,
		std::string operation, const Object& righthand) const {
	return as_binary(caller, open, type, *this, operation, righthand);
}

Log::Log(const Log* caller, std::string label, bool open) :
		Object(caller, label) {
	// TODO Auto-generated constructor stub
	track = std::stoull(substring_after(has_logger(), ".", true, 0));
	this->open = open;
}
Log::~Log() {
	// TODO Auto-generated destructor stub
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

} /* namespace base */
