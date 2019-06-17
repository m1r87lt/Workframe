/*
 * Logics.cpp
 *
 *  Created on: 17 apr 2019
 *      Author: m1r
 */

#include "Logics.h"

namespace game {

//Trigger
base::Element* Trigger::gives_generator() const {
	return generator;
}

Trigger::Fields Trigger::shows() const {
	auto result = Object::shows();

	result.insert(VARIABLE(generator));

	return result;
}

Trigger::Trigger(base::Element* generator) {
	this->generator = generator;
}

//Simulator
std::set<Simulator::Choice*> Simulator::all;
std::map<Simulator::Choice*, std::set<std::string>> Simulator::valid;

void Simulator::execute() {
	auto choices = all;

	valid.clear();
	for (auto choice : choices)
		(*choice)();
}
std::map<Simulator::Choice*, std::set<std::string>> Simulator::give_choices() {
	return valid;
}

//Simulator::Choice
std::set<std::string> Simulator::Choice::validates_choices() {
	std::set<std::string> result;

	for (auto option : provides()) {
		if (operator ()(option))
			result.emplace(option);
		reverts(option);
	}

	return result;
}

bool Simulator::Choice::operator ()() {
	auto choices = validates_choices();

	if (choices.empty()) {
		valid.erase(this);

		return false;
	} else {
		valid[this] = choices;

		return true;
	}
}

std::invalid_argument Simulator::Choice::throw_invalid_command(Choice* choice, std::string command) {
	return "\"" + command + "\" is not a valid command for " + choice->prints() + ".";
}

Simulator::Choice::Choice(base::Element* generator): Trigger(generator) {
	Simulator::all.emplace(this);
}
Simulator::Choice::~Choice() {
	Simulator::all.erase(this);
}

} /* namespace game */
