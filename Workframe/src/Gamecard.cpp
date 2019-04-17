/*
 * Gamecard.cpp
 *
 *  Created on: 19 nov 2018
 *      Author: m1r
 */

#include "Gamecard.h"

namespace game {

// Card
base::Element& Card::operator [](bool cover) const {
	return Ensemble::operator [](cover ? 1 : 2);
}
void Card::manages_piled(bool covered, bool do_keep_piled) {
	auto face = is_ensemble(covered);

	if (face && do_keep_piled) {
		auto other = is_ensemble(!covered);

		if (other) {
			auto content = face->has_size();

			while (content)
				other->takes(content--, *face, 1);
		}
	} else if (face)
		face->self_clears();
}
base::Element& Card::operator ()() const {
	return operator [](covered);
}
base::Ensemble* Card::is_ensemble(bool cover) const {
	Ensemble* result = nullptr;
	Element& face = operator [](cover);

	try {
		result = &dynamic_cast<Ensemble&>(face);
	} catch (std::exception& e) {
		std::clog << prints() << "'s face " << face.prints()
				<< " is not Ensemble-derived." << std::endl;
	}

	return result;
}
bool Card::is_covered() const {
	return covered;
}
void Card::operator ~() {
	auto face = is_ensemble(covered);

	covered = !covered;
	if (face)
		face->self_clears();
}
void Card::faces(bool do_keep_piled) {
	if (covered) {
		manages_piled(covered, do_keep_piled);
		covered = false;
	}
}
void Card::covers(bool do_keep_piled) {
	if (!covered) {
		manages_piled(covered, do_keep_piled);
		covered = true;
	}
}

Card::Fields Card::shows() const {
	auto result = Ensemble::shows();

	result.insert(VARIABLE(covered));

	return result;
}
std::string Card::prints() const {
	std::ostringstream result;

	result << NAME(game::Card) << (covered ? "( " : "[ ") << this
			<< (covered ? " )" : " ]");

	return result.str();
}

base::Ensemble* Card::cast(const Card* pointer) {
	return reinterpret_cast<Ensemble*>(const_cast<Card*>(pointer));
}
game::Card* Card::cast(const Element* pointer) {
	return reinterpret_cast<Card*>(const_cast<Element*>(pointer));
}
Card::Unique_ptr Card::construct(Unique_ptr&& cover, Unique_ptr&& face,
		bool covered, Fields attributes) {
	return Unique_ptr(
			new Card(std::move(cover), std::move(face), covered, attributes));
}

Card::Card(Unique_ptr&& cover, Unique_ptr&& face, bool covered,
		Fields attributes) :
		Ensemble(attributes) {
	this->covered = covered;
	gets(TYPE(face), std::move(face));
	gets(TYPE(cover), std::move(cover));
}

// Deck
size_t Deck::randomly_gives(bool add) {
	std::default_random_engine generator;
	std::uniform_int_distribution<size_t> distribution(1,
			has_size() + (add ? 0 : 1));

	return distribution(generator);
}
void Deck::generates_over(Unique_ptr&& cover, Unique_ptr&& face,
		Fields attributes) {
	generates<game::Card>(NAME(game::Card), 1, std::move(cover),
			std::move(face), covered, attributes);
}
void Deck::generates_under(Unique_ptr&& cover, Unique_ptr&& face,
		Fields attributes) {
	generates<game::Card>(NAME(game::Card), Ensemble::has_size() + 1,
			std::move(cover), std::move(face), covered, attributes);
}
void Deck::randomly_generates(Unique_ptr&& cover, Unique_ptr&& face,
		Fields attributes) {
	generates<game::Card>(NAME(game::Card), randomly_gives(true),
			std::move(cover), std::move(face), covered, attributes);
}
void Deck::gets_over(Deck::Card&& card) {
	gets(NAME(game::Card), cast(std::move(card)));
}
void Deck::gets_under(Deck::Card&& card) {
	gets(NAME(game::Card), cast(std::move(card)), has_size() + 1);
}
void Deck::randomly_gets(Deck::Card&& card) {
	gets(NAME(game::Card), cast(std::move(card)), randomly_gives(true));
}
Deck::Card Deck::draws_over() {
	return cast(gives(1));
}
Deck::Card Deck::draws_under() {
	return cast(gives(has_size()));
}
Deck::Card Deck::randomly_draws() {
	return cast(gives(randomly_gives(false)));
}
size_t Deck::has_size() const {
	return Ensemble::has_size();
}
void Deck::self_clears() {
	Ensemble::self_clears();
}
void Deck::shuffles() {
	std::default_random_engine generator;
	auto size = has_size();

	if (size > 1)
		for (size_t index = 1; index <= size; ++index)
			takes(std::uniform_int_distribution<size_t>(index, size)(generator),
					*this);
}
bool Deck::is_covered() const {
	return covered;
}
void Deck::operator ~() {
	covered = !covered;
	for (auto content = has_size(); content; --content)
		~*game::Card::cast(&(operator [](content)));
}
void Deck::faces() {
	if (covered) {
		covered = false;
		for (auto content = has_size(); content; --content)
			~*game::Card::cast(&(operator [](content)));
	}
}
void Deck::covers() {
	if (!covered) {
		covered = true;
		for (auto content = has_size(); content; --content)
			~*game::Card::cast(&(operator [](content)));
	}
}

Deck::Fields Deck::shows() const {
	auto result = Ensemble::shows();

	result.insert(VARIABLE(covered));

	return result;
}
std::string Deck::prints() const {
	std::ostringstream result;

	result << NAME(game::Deck) << (covered ? "( " : "[ ") << this
			<< (covered ? " )" : " ]");

	return result.str();
}
Deck::Unique_ptr Deck::cast(Deck::Card&& unique_ptr) {
	return Deck::Unique_ptr(unique_ptr.release());
}
Deck::Card Deck::cast(Unique_ptr&& unique_ptr) {
	return Deck::Card(reinterpret_cast<game::Card*>(unique_ptr.release()));
}
base::Ensemble* Deck::cast(const Deck* pointer) {
	return reinterpret_cast<Ensemble*>(const_cast<Deck*>(pointer));
}
Deck* Deck::cast(const Element* pointer) {
	return reinterpret_cast<Deck*>(const_cast<Element*>(pointer));
}
Deck::Unique_ptr Deck::construct(bool covered, Fields attributes) {
	return Unique_ptr(new Deck(covered, attributes));
}

Deck::Deck() :
		Ensemble() {
	covered = true;
}
Deck::Deck(bool covered, Fields attributes) :
		Ensemble(attributes) {
	this->covered = covered;
}

} /* namespace game */

