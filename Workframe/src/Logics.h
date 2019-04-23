/*
 * Logics.h
 *
 *  Created on: 17 apr 2019
 *      Author: m1r
 */

#ifndef LOGICS_H_
#define LOGICS_H_

#include "Existence.h"

namespace game {

struct Simulator: public base::Object {
	class Case {
		friend Simulator;
		Case();
	};
	static Case consider(Case*);
};

class Conditional: public virtual base::Object {
	base::Element* generator;
protected:
	Conditional(base::Element*);
public:
	base::Element* gives_generator() const;
	virtual Fields shows() const;
	virtual bool operator ()(Simulator*) = 0;
	virtual ~Conditional() = default;
};
class Eventual: public virtual base::Object {
	base::Element* generator;
protected:
	Eventual(base::Element*);
public:
	base::Element* gives_generator() const;
	virtual Fields shows() const;
	virtual Simulator::Case operator ()(Simulator*) = 0;
	virtual ~Eventual() = default;
};

class Trigger: public base::Object {
public:
	virtual ~Trigger();
};

} /* namespace game */
#endif /* LOGICS_H_ */

