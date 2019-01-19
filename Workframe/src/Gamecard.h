/*
 * Gamecard.h
 *
 *  Created on: 19 nov 2018
 *      Author: m1r
 */

#ifndef GAMECARD_H_
#define GAMECARD_H_
/*
#include "Element.h"
#include <random>

namespace game {

class Card final: private base::Ensemble {
	base::Primitive<bool> covered;
	static const std::string cover;
	static const std::string face;
	static const std::string card;

	Element& operator [](base::Primitive<bool>) const;
protected:
	Card(Unique_ptr&&, Unique_ptr&&, base::Primitive<bool>,
			const Log* = nullptr, base::Fields = nullptr);
	friend class Deck;
	friend Unique_ptr;
public:
	Element& operator ()(const Log* = nullptr) const;
	base::Primitive<bool> is_covered(const Log* = nullptr) const;
	void operator ~();
	void faces(const Log* = nullptr);
	void covers(const Log* = nullptr);
	static Unique_ptr construct(Unique_ptr&&, Unique_ptr&&,
			base::Primitive<bool> = true, const Log* = nullptr, base::Fields =
					nullptr);

	~Card();
};
class Deck final: private base::Ensemble {
	base::Primitive<bool> covered;

	size_t randomly_gives(base::Primitive<bool>, const Log* = nullptr);
protected:
	Deck(base::Primitive<bool>, const Log* = nullptr, base::Fields = nullptr);
	friend Ensemble::Unique_ptr;
public:
	using Unique_ptr = base::Class<std::unique_ptr<Card>>;

	void generates_over(Ensemble::Unique_ptr&&, Ensemble::Unique_ptr&&,
			const Log* = nullptr, base::Fields = nullptr);
	void generates_under(Ensemble::Unique_ptr&&, Ensemble::Unique_ptr&&,
			const Log* = nullptr, base::Fields = nullptr);
	void randomly_generates(Ensemble::Unique_ptr&&, Ensemble::Unique_ptr&&,
			const Log* = nullptr, base::Fields = nullptr);
	void gets_over(Unique_ptr&&, const Log* = nullptr);
	void gets_under(Unique_ptr&&, const Log* = nullptr);
	void randomly_gets(Unique_ptr&&, const Log* = nullptr);
	Unique_ptr draws_over(const Log* = nullptr);
	Unique_ptr draws_under(const Log* = nullptr);
	Unique_ptr randomly_draws(const Log* = nullptr);
	base::Primitive<size_t> has_size(const Log* = nullptr) const;
	void self_clears(const Log* = nullptr);
	void shuffles(const Log* = nullptr);
	base::Primitive<bool> is_covered(const Log* = nullptr) const;
	void operator ~();
	void faces(const Log* = nullptr);
	void covers(const Log* = nullptr);
	static Ensemble::Unique_ptr construct(base::Primitive<bool> = true, const Log* =
			nullptr, base::Fields = nullptr);

	~Deck();
};

} /* namespace game *

namespace base {

template<> class Class<std::unique_ptr<game::Card>> : public Ensemble::Unique_ptr {
	Class(Ensemble::Unique_ptr&&);
public:
	static Class<std::unique_ptr<game::Card>> dynamicCast(
			Ensemble::Unique_ptr&& card);
	static Class<std::unique_ptr<game::Card>> construct(Ensemble::Unique_ptr&&,
			Ensemble::Unique_ptr&&, base::Primitive<bool> = true, const Log* =
					nullptr, base::Fields = nullptr);

	virtual ~Class() = default;
	Class(const Class<std::unique_ptr<game::Card>>&) = delete;
	Class(Class<std::unique_ptr<game::Card>> &&);
	Class<std::unique_ptr<game::Card>>& operator =(
			const Class<std::unique_ptr<game::Card>>&) = delete;
};

} /* namespace base */

#endif /* GAMECARD_H_ */

