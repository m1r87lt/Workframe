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

class Trigger: public virtual base::Object {
	base::Element* generator;
protected:
	Trigger(base::Element*);
public:
	base::Element* gives_generator() const;
	virtual Fields shows() const;
	virtual bool operator ()() = 0;
	virtual ~Trigger() = default;
};

struct Simulator {
	class Choice: public virtual Trigger {
		std::set<std::string> validates_choices();
		friend Simulator;
	protected:
		virtual bool operator ()(std::string) = 0;
		virtual std::set<std::string> provides() = 0;
	public:
		virtual bool operator ()();
		Choice(base::Element*);
		virtual ~Choice();
	};
	friend Choice;
	static void execute();
	static std::map<Simulator::Choice*, std::set<std::string>> give_choices();
private:
	static std::set<Choice*> all;
	static std::map<Choice*, std::set<std::string>> valid;
};

} /* namespace game */

#endif /* LOGICS_H_ */
