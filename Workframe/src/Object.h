/*
 * Object.h
 *
 *  Created on: 30 set 2018
 *      Author: m1r
 */

#ifndef OBJECT_H_
#define OBJECT_H_
#include <string>
#include <iostream>
#include <functional>

namespace base {

class Object {
	std::string logger;
	bool open;
	static long long unsigned tracker;
public:
	virtual ~Object();

	virtual Object& operator ()(std::initializer_list<Object&>) = 0;
protected:
	template<typename Argument, typename ... Argument> Object(bool open, const Object* caller, )

	Object(bool open, const Object* caller) :
			open(open) {
		std::clog << (logger = caller->logger + "." + std::to_string(++tracker))
				<< ": " << __func__ << "{" << this << (open ? "}() {" : "}()")
				<< std::endl;
	}
};

} /* namespace base */

#endif /* OBJECT_H_ */
