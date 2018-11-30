/*
 * Gamecard.cpp
 *
 *  Created on: 19 nov 2018
 *      Author: m1rma
 */

#include "Gamecard.h"

namespace game {

// Card
base::Element& Card::operator [](bool cover) const {
	base::Primitive<bool> is_covered(cover, this);

	return as_binary("[]", is_covered, nullptr, typeid(Element&)).returns(
			Ensemble::operator [](cover ? 0 : 1));
}
base::Element& Card::operator ()(const Log* caller) const {
	return as_method("", caller, typeid(Element&)).returns(operator [](covered));
}
base::Primitive<bool> Card::is_covered(const Log* caller) const {
	return base::method_primitive(covered, *this, __func__, caller);
}
void Card::operator ~() {
	as_unary("~");
	covered = !covered;
}
void Card::faces(const Log* caller) {
	as_method<false>(__func__, caller);
	covered = false;
}
void Card::covers(const Log* caller) {
	as_method<false>(__func__, caller);
	covered = true;
}
base::Unique_ptr Card::construct(base::Unique_ptr&& cover,
		base::Unique_ptr&& face, base::Primitive<bool> covered,
		const Log* caller, base::Fields attributes) {
	auto log = as_method(
			base::make_scopes(__func__, "game", typeid(Card).name()), true,
			caller, typeid(base::Unique_ptr), cover, face, covered, attributes);

	return log.returns(
			base::Unique_ptr::construct<Card>(&log, std::move(cover),
					std::move(face), covered,
					base::Primitive<const Log*>(&log, &log), attributes));
}

Card::Card(base::Unique_ptr&& cover, base::Unique_ptr&& face,
		base::Primitive<bool> covered, const Log* caller,
		base::Fields attributes) :
		Object(caller, base::make_scopes(__func__, "a")), base::Log(caller,
				base::make_scopes(__func__, "a"), true), Ensemble(
				base::make_scopes(__func__, "a"), caller) {
	as_constructor("a", __func__, this, cover, face, covered, attributes);
	this->covered = covered;
	gets(base::Class<std::string>("face"), std::move(face), 1, this);
	gets(base::Class<std::string>("cover"), std::move(cover), 1, this);
}
Card::~Card() {
	as_destructor("a", __func__);
}

// Deck
size_t Deck::randomly_gives(bool add, const Log* caller) {
	base::Primitive<bool> insert(add, caller);
	auto log = as_method(__func__, caller, typeid(size_t), insert);
	std::default_random_engine generator;
	std::uniform_int_distribution<size_t> distribution(1,
			has_size(&log) + (add ? 0 : 1));

	return log.returns(base::Primitive<size_t>(distribution(generator), &log));
}
void Deck::generates_over(base::Unique_ptr&& cover, base::Unique_ptr&& face,
		const Log* caller, base::Fields attributes) {
	auto log = as_method(__func__, caller, typeid(void), cover, face,
			attributes);

	generates<Card>(base::Class<std::string>("card", &log),
			base::Primitive<size_t>(1, &log), &log, std::move(cover),
			std::move(face), base::Primitive<bool>(covered, &log),
			base::Primitive<const Log*>(&log, &log), attributes);
}
void Deck::generates_under(base::Unique_ptr&& cover, base::Unique_ptr&& face,
		const Log* caller, base::Fields attributes) {
	auto log = as_method(__func__, caller, typeid(void), cover, face,
			attributes);

	generates<Card>(base::Class<std::string>("card", &log),
			Ensemble::has_size(&log), &log, std::move(cover), std::move(face),
			base::Primitive<bool>(covered, &log),
			base::Primitive<const Log*>(&log, &log), attributes);
}
void Deck::randomly_generates(base::Unique_ptr&& cover, base::Unique_ptr&& face,
		const Log* caller, base::Fields attributes) {
	auto log = as_method(__func__, caller, typeid(void), cover, face,
			attributes);

	generates<Card>(base::Class<std::string>("card", &log),
			base::Primitive<size_t>(randomly_gives(true, &log), &log), &log,
			std::move(cover), std::move(face),
			base::Primitive<bool>(covered, &log),
			base::Primitive<const Log*>(&log, &log), attributes);
}
void Deck::gets_over(Unique_ptr&& card, const Log* caller) {
	auto log = as_method(__func__, caller);

	gets(base::Class<std::string>("card", &log), std::move(card.card),
			base::Primitive<size_t>(1, &log), &log);
}
void Deck::gets_under(Unique_ptr&& card, const Log* caller) {
	auto log = as_method(__func__, caller);

	gets(base::Class<std::string>("card", &log), std::move(card.card),
			has_size(&log), &log);
}
void Deck::randomly_gets(Unique_ptr&& card, const Log* caller) {
	auto log = as_method(__func__, caller);

	gets(base::Class<std::string>("card", &log), std::move(card.card),
			randomly_gives(true, &log), &log);
}
Deck::Unique_ptr Deck::draws_over(const Log* caller) {
	auto log = as_method(__func__, caller, typeid(Unique_ptr));

	return log.returns(
			Unique_ptr::dynamicCast(
					gives(base::Primitive<size_t>(1, &log), &log)));
}
Deck::Unique_ptr Deck::draws_under(const Log* caller) {
	auto log = as_method(__func__, caller, typeid(Unique_ptr));

	return log.returns(Unique_ptr::dynamicCast(gives(has_size(&log), &log)));
}
Deck::Unique_ptr Deck::randomly_draws(const Log* caller) {
	auto log = as_method(__func__, caller, typeid(Unique_ptr));

	return log.returns(
			Unique_ptr::dynamicCast(gives(randomly_gives(false, &log), &log)));
}
base::Primitive<size_t> Deck::has_size(const Log* caller) const {
	auto log = as_method(__func__, caller, typeid(base::Primitive<size_t>));

	return log.returns(Ensemble::has_size(&log));
}
void Deck::self_clears(const Log* caller) {
	auto log = as_method(__func__, caller);

	Ensemble::self_clears(&log);
}
void Deck::shuffles(const Log* caller) {
	auto log = as_method(__func__, caller);
	std::default_random_engine generator;
	auto size = has_size(&log);

	if (size > 1) {
		base::Primitive<size_t> front(1, &log);
		base::Primitive<Ensemble*> it(this, &log);

		for (size_t index = 1; index <= size; ++index)
			takes(it,
					base::Primitive<size_t>(
							std::uniform_int_distribution<size_t>(index, size)(
									generator), &log), front, &log);
	}
}
base::Primitive<bool> Deck::is_covered(const Log* caller) const {
	return base::method_primitive(covered, *this, __func__, caller);
}
void Deck::operator ~() {
	as_unary("~");
	covered = !covered;
}
void Deck::faces(const Log* caller) {
	as_method<false>(__func__, caller);
	covered = false;
}
void Deck::covers(const Log* caller) {
	as_method<false>(__func__, caller);
	covered = true;
}
base::Unique_ptr Deck::construct(base::Primitive<bool> covered,
		const Log* caller, base::Fields attributes) {
	auto log = as_method(
			base::make_scopes(__func__, "game", typeid(Deck).name()), true,
			caller, typeid(base::Unique_ptr), covered, attributes);

	return log.returns(
			base::Unique_ptr::construct<Deck>(&log, covered,
					base::Primitive<const Log*>(&log, &log), attributes));
}

Deck::Deck(base::Primitive<bool> covered, const Log* caller,
		base::Fields attributes) :
		Object(caller, base::make_scopes(__func__, "game")), Log(caller,
				base::make_scopes(__func__, "game"), true), Ensemble(
				base::make_scopes(__func__, "game"), caller, attributes) {
	as_constructor("game", __func__, this, covered, attributes);
	this->covered = covered;
}
Deck::~Deck() {
	as_destructor("game", __func__);
}

}
/* namespace game */

namespace base {

// Class<std::unique_ptr<game::Card>>
std::ostringstream Class<std::unique_ptr<game::Card>>::prints() const {
	return card.prints();
}
Class<std::unique_ptr<game::Card>> Class<std::unique_ptr<game::Card>>::dynamicCast(
		Unique_ptr&& card) {
	dynamic_cast<game::Card*>((Element*) card);

	return Class<std::unique_ptr<game::Card>>(std::move(card));
}

Class<std::unique_ptr<game::Card>>::Class(Unique_ptr&& card) :
		Object(std::move(card)), card(std::move(card)) {
}
Class<std::unique_ptr<game::Card>>::Class(
		Class<std::unique_ptr<game::Card>> && moving) :
		Object(std::move(moving)), card(std::move(moving.card)) {
}

} /* namespace base */
