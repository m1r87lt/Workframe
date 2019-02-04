/*
 * Gamecard.h
 *
 *  Created on: 19 nov 2018
 *      Author: m1r
 */

#ifndef GAMECARD_H_
#define GAMECARD_H_

#include "Existence.h"
#include <random>

namespace game {

class Card: private base::Ensemble {
	bool covered;

	Element& operator [](bool) const;
protected:
	Card(Unique_ptr&&, Unique_ptr&&, bool = true, Fields = Fields());
	friend class Deck;
	friend Ensemble;
public:
	Element& operator ()() const;
	Ensemble* is_ensemble(bool) const;
	void manages_piled(bool, bool);
	bool is_covered() const;
	void operator ~();
	void faces(bool);
	void covers(bool);
	virtual Fields shows() const;
	virtual std::string prints() const;
	static Element* cast(const Card*);
	static Unique_ptr construct(Unique_ptr&&, Unique_ptr&&, bool = true,
			Fields = Fields());
};
class Deck: private base::Ensemble {
	bool covered;

	size_t randomly_gives(bool);
protected:
	Deck();
	Deck(bool, Fields = Fields());
	friend Ensemble;
public:
	using Card = std::unique_ptr<Card>;

	void generates_over(Unique_ptr&&, Unique_ptr&&, Fields = Fields());
	void generates_under(Unique_ptr&&, Unique_ptr&&, Fields = Fields());
	void randomly_generates(Unique_ptr&&, Unique_ptr&&, Fields = Fields());
	void gets_over(Deck::Card&&);
	void gets_under(Deck::Card&&);
	void randomly_gets(Deck::Card&&);
	Deck::Card draws_over();
	Deck::Card draws_under();
	Deck::Card randomly_draws();
	size_t has_size() const;
	void self_clears();
	void shuffles();
	bool is_covered() const;
	void operator ~();
	void faces();
	void covers();
	virtual Fields shows() const;
	virtual std::string prints() const;
	static Unique_ptr cast(Deck::Card&&);
	static Deck::Card cast(Unique_ptr&&);
	static Element* cast(const Deck*);
	static Unique_ptr construct(bool = true, Fields = Fields());
};

} /* namespace game */

#endif /* GAMECARD_H_ */

