/*
 * Element.cpp
 *
 *  Created on: 7 ott 2018
 *      Author: m1rma
 */

#include "Element.h"
#include <chrono>

namespace base {

std::set<Element*> Element::everything;

time_t Element::exists_from() const {
	return creation;
}

Element::~Element() {
	// TODO Auto-generated destructor stub
	if (is_running())
		everything.erase(this);
}

Element::Element(const Log* caller, std::string label):
		Log(caller, label, 4) {
	// TODO Auto-generated constructor stub
	creation = std::chrono::system_clock::now();
}

} /* namespace base */
