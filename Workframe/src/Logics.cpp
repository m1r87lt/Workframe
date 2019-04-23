/*
 * Logics.cpp
 *
 *  Created on: 17 apr 2019
 *      Author: m1r
 */

#include "Logics.h"

namespace game {

//Conditional
base::Element* Conditional::gives_generator() const {
	return generator;
}

virtual Conditional::Fields Conditional::shows() const {
	auto result = Object::shows();

	result.insert(VARIABLE(generator));

	return result;
}

Conditional::Conditional(base::Element* generator) {
	this->generator = generator;
}

//Eventual
base::Element* Eventual::gives_generator() const {
	return generator;
}

virtual Eventual::Fields Eventual::shows() const {
	auto result = Object::shows();

	result.insert(VARIABLE(generator));

	return result;
}

Eventual::Eventual(base::Element* generator) {
	this->generator = generator;
}

Trigger::~Trigger() {
	// TODO Auto-generated destructor stub
}

} /* namespace game */
