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
#include <utility>
#include <functional>

namespace base {
#define BASE "base"

bool is_running();
void ends_run();
std::string give_substring_after(std::string, std::string, bool, int = 1);
std::string make_scopes(std::string, std::string);
template<typename ... Arguments> std::string make_scopes(std::string scope0,
		std::string scope1, std::string scope2, Arguments&& ... scopes) {
	auto recursive = make_scopes(scope1, scope2, scopes ...);

	if (recursive.substr(0, 2) != "::")
		scope0 += "::";
	scope0 += recursive;

	return recursive;
}

class Object {
	long long unsigned track;
	std::string label;
	std::string logger;
	static long long unsigned tracker;
	friend class Log;
	void makes_track(const Object*);
	void makes_track(std::string);
	void constructs(std::string);
	void becomes(Object&&);
public:
	virtual std::ostringstream prints() const = 0;
protected:
	const std::string& has_label() const;
	const std::string& has_logger() const;

	Object(const Object*, std::string);
	virtual ~Object() = default;
	Object(const Object&);
	Object& operator =(const Object&) = delete;
	Object(Object&&);
	Object& operator =(Object&&);
};

class Log: virtual public Object {
	bool open;

	template<typename ... Arguments> static std::ostringstream log_arguments(
			const Object& argument, Arguments& ... arguments) {
		std::ostringstream result;

		result << argument.prints().str() << ", "
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
	static void log_return(Log&, const Object&);
public:
	void notes(std::ostringstream) const;
	template<typename Type> Type&& returns(Type&& returning) const {
		log_return(const_cast<Log&>(*this), returning);

		return std::forward<Type&&>(returning);
	}
	std::string logs_error(std::ostringstream) const;
	virtual std::ostringstream prints() const;
	static Log as_unary(std::string, const Object&, bool = true,
			std::type_index = typeid(void));
	static Log as_unary(const Object&, std::string, bool = true,
			std::type_index = typeid(void));
	static Log as_binary(const Object&, std::string, const Object&, bool = true,
			std::type_index = typeid(void));
	template<typename ... Arguments> static Log as_function(std::string label,
			bool open = true, const Log* caller = nullptr,
			std::type_index returning_type = typeid(void),
			Arguments& ... arguments) {
		Log result(caller, label, open);

		log_method(result, returning_type, nullptr, arguments ...);

		return result;
	}

	virtual ~Log();
	Log(const Log&) = delete;
	Log& operator =(const Log&) = delete;
	Log(Log&&);
	Log& operator =(Log&&);
protected:
	template<bool Open = true> Log as_unary(std::string operation,
			std::type_index returning_type = typeid(void)) const {
		return as_unary(operation, *this, Open, returning_type);
	}
	template<bool Open = true> Log as_unary(std::type_index returning_type,
			std::string operation) const {
		return as_unary(*this, operation, Open, returning_type);
	}
	template<bool Open = true> Log as_binary(std::string operation,
			const Object& righthand, std::type_index returning_type =
					typeid(void)) const {
		return as_binary(*this, operation, righthand, Open, returning_type);
	}
	template<bool Open = true, typename ... Arguments> Log as_method(
			std::string label, const Log* caller = nullptr,
			std::type_index returning_type = typeid(void),
			Arguments& ... arguments) const {
		return as_method(has_label() + "." + label, Open, caller,
				returning_type, arguments ...);
	}
	template<bool Open = true, typename ... Arguments> Log as_constructor(
			std::string ns, std::string label, const Log* caller = nullptr,
			Arguments& ... arguments) {
		Log result(caller, label, Open);

		log(has_logger(),
				make_scopes(ns, label, label) + "("
						+ log_arguments(arguments ...).str() + ")", Open,
				nullptr);

		return result;
	}
	template<bool Open = false, typename ... Arguments> Log as_destructor(
			std::string ns, std::string label) {
		Log result(nullptr, label, Open);

		log(has_logger(),
				make_scopes(ns, label,
						give_substring_after(label, "~", false, 1)) + "()",
				Open, nullptr);

		return result;
	}
	template<typename ... Arguments> static Log as_method(std::string label,
			bool open, const Log* caller = nullptr,
			std::type_index returning_type = typeid(void),
			Arguments& ... arguments) {
		Log result(caller, label, open);

		log_method(result, returning_type, nullptr, arguments ...);

		return result;
	}
	friend class Method;
	Log(const Log*, std::string, bool);
};

template<typename Type> class Primitive final: public Object {
	Type value;
public:
	template<typename Operand> Primitive<Type>& operator +=(
			Primitive<Operand> operand) {
		auto log = Log::as_binary(*this, __func__, operand, true,
				typeid(*this));

		value += (Type) operand.value;

		return log.returns(*this);
	}
	template<typename Operand> Primitive<Type>& operator -=(
			Primitive<Operand> operand) {
		auto log = Log::as_binary(*this, __func__, operand, true,
				typeid(*this));

		value -= (Type) operand.value;

		return log.returns(*this);
	}
	template<typename Operand> Primitive<Type>& operator *=(
			Primitive<Operand> operand) {
		auto log = Log::as_binary(*this, __func__, operand, true,
				typeid(*this));

		value *= (Type) operand.value;

		return log.returns(*this);
	}
	template<typename Operand> Primitive<Type>& operator /=(
			Primitive<Operand> operand) {
		auto log = Log::as_binary(*this, __func__, operand, true,
				typeid(*this));

		value /= (Type) operand.value;

		return log.returns(*this);
	}
	template<typename Operand> Primitive<Type>& operator %=(
			Primitive<Operand> operand) {
		auto log = Log::as_binary(*this, __func__, operand, true,
				typeid(*this));

		value %= (Type) operand.value;

		return log.returns(*this);
	}
	template<typename Operand> Primitive<Type>& operator &=(
			Primitive<Operand> operand) {
		auto log = Log::as_binary(*this, __func__, operand, true,
				typeid(*this));

		value &= (Type) operand.value;

		return log.returns(*this);
	}
	template<typename Operand> Primitive<Type>& operator |=(
			Primitive<Operand> operand) {
		auto log = Log::as_binary(*this, __func__, operand, true,
				typeid(*this));

		value |= (Type) operand.value;

		return log.returns(*this);
	}
	template<typename Operand> Primitive<Type>& operator ^=(
			Primitive<Operand> operand) {
		auto log = Log::as_binary(*this, __func__, operand, true,
				typeid(*this));

		value ^= (Type) operand.value;

		return log.returns(*this);
	}
	Primitive<Type>& operator ++() {
		auto log = Log::as_unary(__func__, *this, true, typeid(*this));

		++value;

		return log.returns(*this);
	}
	Primitive<Type> operator ++(int) {
		auto log = Log::as_unary(*this, __func__, true, typeid(*this));

		return log.returns(Primitive<Type>(++value, &log));
	}
	Primitive<Type>& operator --() {
		auto log = Log::as_unary(__func__, *this, true, typeid(*this));

		--value;

		return log.returns(*this);
	}
	Primitive<Type> operator --(int) {
		auto log = Log::as_unary(*this, __func__, true, typeid(*this));

		return log.returns(Primitive<Type>(--value, &log));
	}
	Primitive<Type> operator +() {
		return Log::as_unary(__func__, *this, false, typeid(*this)).returns(
				*this);
	}
	Primitive<Type> operator -() {
		auto log = Log::as_unary(__func__, *this, true, typeid(*this));

		return log.returns(Primitive<Type>(-value, &log));
	}
	template<typename Operand> Primitive<Type> operator +(
			Primitive<Operand> operand) {
		auto log = Log::as_binary(*this, __func__, operand, true,
				typeid(*this));

		return log.returns(Primitive<Type>(value + (Type) operand.value, &log));
	}
	template<typename Operand> Primitive<Type> operator -(
			Primitive<Operand> operand) {
		auto log = Log::as_binary(*this, __func__, operand, true,
				typeid(*this));

		return log.returns(Primitive<Type>(value - (Type) operand.value, &log));
	}
	template<typename Operand> Primitive<Type> operator *(
			Primitive<Operand> operand) {
		auto log = Log::as_binary(*this, __func__, operand, true,
				typeid(*this));

		return log.returns(Primitive<Type>(value * (Type) operand.value, &log));
	}
	template<typename Operand> Primitive<Type> operator /(
			Primitive<Operand> operand) {
		auto log = Log::as_binary(*this, __func__, operand, true,
				typeid(*this));

		return log.returns(Primitive<Type>(value / (Type) operand.value, &log));
	}
	template<typename Operand> Primitive<Type> operator %(
			Primitive<Operand> operand) {
		auto log = Log::as_binary(*this, __func__, operand, true,
				typeid(*this));

		return log.returns(Primitive<Type>(value % (Type) operand.value, &log));
	}
	Primitive<Type>& operator ~() {
		auto log = Log::as_unary(__func__, *this, true, typeid(*this));

		value = -value;

		return log.returns(*this);
	}
	template<typename Operand> Primitive<Type> operator &(
			Primitive<Operand> operand) {
		auto log = Log::as_binary(*this, __func__, operand, true,
				typeid(*this));

		return log.returns(Primitive<Type>(value & (Type) operand.value, &log));
	}
	template<typename Operand> Primitive<Type> operator |(
			Primitive<Operand> operand) {
		auto log = Log::as_binary(*this, __func__, operand, true,
				typeid(*this));

		return log.returns(Primitive<Type>(value | (Type) operand.value, &log));
	}
	template<typename Operand> Primitive<Type> operator ^(
			Primitive<Operand> operand) {
		auto log = Log::as_binary(*this, __func__, operand, true,
				typeid(*this));

		return log.returns(Primitive<Type>(value ^ (Type) operand.value, &log));
	}
	Primitive<Type> operator !() {
		auto log = Log::as_unary(__func__, *this, true, typeid(*this));

		return log.returns(Primitive<Type>(-value));
	}
	template<typename Operand> Primitive<bool> operator &&(
			Primitive<Operand> operand) {
		auto log = Log::as_binary(*this, __func__, operand, true,
				typeid(Primitive<bool> ));

		return log.returns(Primitive<bool>(value && operand.value, &log));
	}
	template<typename Operand> Primitive<bool> operator ||(
			Primitive<Operand> operand) {
		auto log = Log::as_binary(*this, __func__, operand, true,
				typeid(Primitive<bool> ));

		return log.returns(Primitive<bool>(value || operand.value, &log));
	}
	template<typename Operand> Primitive<bool> operator ==(
			Primitive<Operand> operand) {
		auto log = Log::as_binary(*this, __func__, operand, true,
				typeid(Primitive<bool> ));

		return log.returns(Primitive<bool>(value == operand.value, &log));
	}
	template<typename Operand> Primitive<bool> operator !=(
			Primitive<Operand> operand) {
		auto log = Log::as_binary(*this, __func__, operand, true,
				typeid(Primitive<bool> ));

		return log.returns(Primitive<bool>(value != operand.value, &log));
	}
	template<typename Operand> Primitive<bool> operator <(
			Primitive<Operand> operand) {
		auto log = Log::as_binary(*this, __func__, operand, true,
				typeid(Primitive<bool> ));

		return log.returns(Primitive<bool>(value < operand.value, &log));
	}
	template<typename Operand> Primitive<bool> operator >(
			Primitive<Operand> operand) {
		auto log = Log::as_binary(*this, __func__, operand, true,
				typeid(Primitive<bool> ));

		return log.returns(Primitive<bool>(value > operand.value, &log));
	}
	template<typename Operand> Primitive<bool> operator <=(
			Primitive<Operand> operand) {
		auto log = Log::as_binary(*this, __func__, operand, true,
				typeid(Primitive<bool> ));

		return log.returns(Primitive<bool>(value <= operand.value, &log));
	}
	template<typename Operand> Primitive<bool> operator >=(
			Primitive<Operand> operand) {
		auto log = Log::as_binary(*this, __func__, operand, true,
				typeid(Primitive<bool> ));

		return log.returns(Primitive<bool>(value >= operand.value, &log));
	}
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
	Primitive(const Primitive<Type>& copy) = default;
	Primitive<Type>& operator =(const Primitive<Type> & copy) {
		value = copy.value;

		return *this;
	}
	Primitive(Primitive<Type> &&) = default;
	Primitive<Type>& operator =(Primitive<Type> &&) = default;
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
	Primitive(const Primitive<const char*>&);
	Primitive<const char*>& operator =(const Primitive<const char*> &);
	Primitive(Primitive<const char*> &&) = default;
	Primitive<const char*>& operator =(Primitive<const char*> &&) = default;
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
	template<typename Operand> Primitive<bool> operator ==(
			Class<Operand> operand) const {
		auto log = Log::as_binary(*this, __func__, operand, true,
				typeid(Primitive<bool> ));

		return log.returns(Primitive<bool>(value == operand.value, &log));
	}
	template<typename Operand> Primitive<bool> operator !=(
			Class<Operand> operand) const {
		auto log = Log::as_binary(*this, __func__, operand, true,
				typeid(Primitive<bool> ));

		return log.returns(Primitive<bool>(value != operand.value, &log));
	}
	virtual std::ostringstream prints() const {
		return printer(value);
	}

	template<typename ... Arguments> Class(const Log* caller,
			Arguments&& ... arguments) :
			Object(caller, typeid(Type).name()), value(
					std::forward<Arguments&&>(arguments) ...) {
	}
	Class(const Class<Type>& copy) = default;
	Class<Type>& operator =(const Class<Type>& copy) {
		value = copy.value;

		return *this;
	}
	Class(Class<Type> &&) = default;
	Class<Type>& operator =(Class<Type> && moving) = default;
	Class(Type&& object, const Log* caller = nullptr) :
			Object(caller, typeid(Type).name()), value(
					std::forward<Type>(object)) {
	}
	Class<Type>& operator =(Type&& object) {
		value = std::forward<Type>(object);

		return *this;
	}
};
template<> class Class<std::string> final: public Object {
	std::string value;
public:
	const std::string& is() const;
	std::string& is();
	std::string&& becomes();
	Primitive<bool> operator ==(Class<std::string> operand) const {
		auto log = Log::as_binary(*this, __func__, operand, true,
				typeid(Primitive<bool> ));

		return log.returns(Primitive<bool>(value == operand.value, &log));
	}
	Primitive<bool> operator !=(Class<std::string> operand) const {
		auto log = Log::as_binary(*this, __func__, operand, true,
				typeid(Primitive<bool> ));

		return log.returns(Primitive<bool>(value != operand.value, &log));
	}
	virtual std::ostringstream prints() const;

	Class(std::string, const Log* = nullptr);
	Class(const Class<std::string>&) = default;
	Class<std::string>& operator =(const Class<std::string>&);
	Class(Class<std::string> &&) = default;
	Class<std::string>& operator =(Class<std::string> &&) = default;
	Class<std::string>& operator =(std::string);
};

template<typename Type> Primitive<Type> unary_primitive(std::string operation,
		const Object& object, Type returning) {
	auto log = Log::as_unary(operation, object, false, typeid(returning));

	return log.returns(Class<Type>(returning, &log));
}
template<typename Type> Class<Type> unary_class(std::string operation,
		const Object& object, Type&& returning) {
	auto log = Log::as_unary(operation, object, false, typeid(returning));

	return log.returns(Class<Type>(returning, &log));
}
template<typename Type> Primitive<Type> unary_primitive(const Object& object,
		std::string operation, Type returning) {
	auto log = Log::as_unary(object, operation, false, typeid(returning));

	return log.returns(Primitive<Type>(returning, &log));
}
template<typename Type> Class<Type> unary_class(const Object& object,
		std::string operation, Type&& returning) {
	auto log = Log::as_unary(object, operation, false, typeid(returning));

	return log.returns(Class<Type>(returning, &log));
}
template<typename Type> Primitive<Type> binary_primitive(const Object& lefthand,
		std::string operation, const Object& righthand, Type returning) {
	auto log = Log::as_binary(lefthand, operation, righthand, false,
			typeid(returning));

	return log.returns(Primitive<Type>(returning, &log));
}
template<typename Type> Class<Type> binary_class(const Object& lefthand,
		std::string operation, const Object& righthand, Type&& returning) {
	auto log = Log::as_binary(lefthand, operation, righthand, false,
			typeid(returning));

	return log.returns(Class<Type>(returning, &log));
}
template<typename Type, typename ... Arguments> Primitive<Type> function_primitive(
		Type returning, std::string label, const Log* caller = nullptr,
		Arguments& ... arguments) {
	auto log = Log::as_function(label, false, caller, typeid(returning),
			arguments ...);

	return log.returns(Primitive<Type>(returning, &log));
}
template<typename Type, typename ... Arguments> Class<Type> function_class(
		Type&& returning, std::string label, const Log* caller = nullptr,
		Arguments& ... arguments) {
	auto log = Log::as_function(label, false, caller, typeid(returning),
			arguments ...);

	return log.returns(Class<Type>(returning, &log));
}

struct Method {
	template<typename Type, typename ... Arguments> static Primitive<Type> return_primitive(
			Type returning, std::string label, const Log* caller = nullptr,
			Arguments& ... arguments) {
		auto log = Log::as_method(label, false, caller, typeid(returning),
				arguments ...);

		return log.returns(Primitive<Type>(returning, &log));
	}
	template<typename Type, typename ... Arguments> static Primitive<Type> return_primitive(
			Type returning, const Log& object, std::string label,
			const Log* caller = nullptr, Arguments& ... arguments) {
		auto log = object.as_method<false>(label, caller, typeid(returning),
				arguments ...);

		return log.returns(Primitive<Type>(returning, &log));
	}
	template<typename Type, typename ... Arguments> static Class<Type> return_class(
			Type&& returning, std::string label, const Log* caller = nullptr,
			Arguments& ... arguments) {
		auto log = Log::as_method(label, false, caller, typeid(returning),
				arguments ...);

		return log.returns(Class<Type>(std::forward<Type>(returning), &log));
	}
	template<typename Type, typename ... Arguments> static Class<Type> return_class(
			Type returning, const Log& object, std::string label,
			const Log* caller = nullptr, Arguments& ... arguments) {
		auto log = object.as_method<false>(label, caller, typeid(returning),
				arguments ...);

		return log.returns(Class<Type>(std::forward<Type>(returning), &log));
	}
};

} /* namespace base */

#endif /* OBJECT_H_ */

