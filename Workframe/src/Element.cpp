/*
 * Element.cpp
 *
 *  Created on: 7 ott 2018
 *      Author: m1rma
 */

#include "Element.h"

namespace base {

//Element
std::set<Element*> Element::everything;

std::ostringstream print_std__chrono__system_clock__time_point(
		const std::chrono::system_clock::time_point& object) {
	std::ostringstream result;
	auto tp = std::chrono::system_clock::to_time_t(object);

	result << ctime(&tp);

	return result;
}
template<> std::function<
		std::ostringstream(const std::chrono::system_clock::time_point&)> Class<
		const std::chrono::system_clock::time_point>::printer =
		print_std__chrono__system_clock__time_point;
Class<const std::chrono::system_clock::time_point> Element::exists_from(
		const Log* caller) const {
	return method_class(
			std::forward<const std::chrono::system_clock::time_point>(creation),
			*this, __func__, caller);
}
void Element::is_modified(const Log* caller) {
	as_method<false>(__func__, caller);
	modification = std::chrono::system_clock::now();
}

template<> std::function<
		std::ostringstream(const std::map<std::string, std::string>&)> Class<
		const std::map<std::string, std::string>>::printer = print_std__map<
		std::string, std::string>;
Class<const std::map<std::string, std::string>> Element::gives_attributes(
		const Log* caller) const {
	return method_class(
			std::forward<const std::map<std::string, std::string>>(attributes),
			*this, __func__, caller);
}
void Element::gets_attributes(Fields attributes, const Log* caller) {
	auto log = as_method(__func__, caller, typeid(void), attributes);

	last_attributes = this->attributes;
	this->attributes = attributes.is();
	is_modified(caller);
}
std::ostringstream class_std__set_Element___(const std::set<Element*>& set) {
	return Container_Printer(set, "\t{", "}")();
}
template<> std::function<std::ostringstream(const std::set<Element*>&)> Class<
		std::set<Element*>>::printer = class_std__set_Element___;
Class<std::set<Element*>> Element::give_everything(const Log* caller) {
	return method_class(std::move(everything),
			make_scopes(__func__, "base", typeid(Element).name()), caller);
}

std::ostringstream print_modifications(const Element::Modifications& object) {
	std::ostringstream result("{");

	for (auto pair : object)
		result << "\n\t" << pair.first << ": " << pair.second.first << " -> "
				<< pair.second.second;
	if (result.str() == "{")
		result << " }";
	else
		result << "\n}";

	return result;
}
template<> std::function<std::ostringstream(const Element::Modifications&)> Class<
		Element::Modifications>::printer = print_modifications;
Class<Element::Modifications> Element::gives_modifications(const Log* caller) {
	Modifications result;
	using Mods = Class<decltype(result)>;
	auto log = as_method(__func__, caller, typeid(Mods));
	auto end = attributes.end();
	auto last_end = last_attributes.end();

	for (auto last_attribute : last_attributes) {
		auto last_first = last_attribute.first;
		auto last_second = last_attribute.second;

		if (attributes.find(last_first) == end)
			result.emplace(last_first, std::make_pair(last_second, ""));
		else {
			auto second = attributes.at(last_first);

			if (second != last_second)
				result.emplace(last_first, std::make_pair(last_second, second));
		}
	}
	for (auto attribute : attributes) {
		auto first = attribute.first;

		if (last_attributes.find(first) == last_end)
			result.emplace(attribute.first,
					std::make_pair("", attribute.second));
	}

	return log.returns(Mods(&log, result));
}

Element::Element(Class<std::string> label, const Log* caller, Fields attributes) :
		Object(caller, label.is()), Log(caller, label.is(), false) {
	as_constructor<false>("base", __func__, caller, attributes);
	modification = creation = std::chrono::system_clock::now();
	position = nullptr;
	this->attributes = attributes.is();
	everything.emplace(this);
}
Element::~Element() {
	as_destructor("base", __func__);
	if (is_running())
		everything.erase(this);
}
Element::Element(Element&& moving) :
		Object(std::move(moving)), Log(std::move(moving)) {
	creation = moving.creation;
	modification = moving.modification;
	position = moving.position;
	attributes = moving.attributes;
	last_attributes = moving.last_attributes;
}

//Class<std::unique_ptr<Element>>
std::unique_ptr<Element> Class<std::unique_ptr<Element>>::is_from(
		Class<std::unique_ptr<Element>> && instance) {
	return std::move(instance.value);
}
std::ostringstream Class<std::unique_ptr<Element>>::prints() const {
	std::ostringstream result;

	result << value.get();

	return result;
}
const Element& Class<std::unique_ptr<Element>>::operator *() const {
	return *value;
}
Element& Class<std::unique_ptr<Element>>::operator *() {
	return *value;
}
const Element* Class<std::unique_ptr<Element>>::operator ->() const {
	return value.operator ->();
}
Element* Class<std::unique_ptr<Element>>::operator ->() {
	return value.operator ->();
}
Class<std::unique_ptr<Element>>::operator const Element*() const {
	return value.get();
}
Class<std::unique_ptr<Element>>::operator Element*() {
	return value.get();
}

Class<std::unique_ptr<Element>>::Class(
		Class<std::unique_ptr<Element>> && moving) :
		Object(moving), value(std::move(moving.value)) {
}
Class<std::unique_ptr<Element>>& Class<std::unique_ptr<Element>>::operator =(
		Class<std::unique_ptr<Element>> && assigning) {
	Object::operator =(assigning);
	value.reset(assigning.value.release());

	return *this;
}
Class<std::unique_ptr<Element>>::Class(std::unique_ptr<Element>&& instance,
		const Log* caller) :
		Object(caller, __func__), value(std::move(instance)) {
}

//Ensemble
std::string log_out_of_range_0(Primitive<size_t> position,
		Primitive<size_t> size, const Log& log, bool warning = true) {
	std::ostringstream message;

	message << (warning ? "WARNING" : "ERROR");
	message << ": " << position.prints().str();
	if (position)
		message << " > " + size.prints().str();
	message << ".";

	return log.logs_error(std::move(message));
}
std::out_of_range throw_out_of_range_0(Primitive<size_t> position,
		Primitive<size_t> size, const Log& log) {

	return std::out_of_range(log_out_of_range_0(position, size, log, false));
}
std::invalid_argument throw_invalid_argument_null(Object& pointer,
		const Log& log) {
	return std::invalid_argument(
			log.logs_error(
					std::ostringstream(
							"ERROR: " + pointer.prints().str() + ".")));
}
std::string log_root_element(const Element& instance, const Log& log,
		bool warning = true) {
	return log.logs_error(
			std::ostringstream(
					std::string(warning ? "WARNING" : "ERROR") + ": "
							+ instance.prints().str() + " is a root Element."));
}
std::domain_error throw_root_element(const Element& instance, const Log& log) {
	return std::domain_error(log_root_element(instance, log, false));
}

Element& Ensemble::operator [](Primitive<size_t> position) const {
	auto log = as_binary("[]", position, nullptr, typeid(Element&));
	auto current = localizes(position, &log);

	if (current == container.end())
		throw throw_out_of_range_0(position,
				Primitive<size_t>(container.size(), &log), log);

	return log.returns(*current->second);
}

template<> std::function<std::ostringstream(const std::map<size_t, Element*>&)> Class<
		std::map<size_t, Element*>>::printer = print_std__map<size_t, Element*>;
Class<std::map<size_t, Element*>> Ensemble::operator [](
		Class<std::string> name) const {
	std::map<size_t, Element*> result;
	using Result = Class<decltype(result)>;
	auto log = as_binary("[]", name, nullptr, typeid(Result));
	size_t position = 0;
	auto current = container.begin();

	for (auto size = container.size(); position < size; ++current)
		if (names(name.is(), current->first))
			result[++position] = current->second.get();
		else
			++position;

	return log.returns(Result(result, &log));
}
Primitive<size_t> Ensemble::which_is(Class<std::string> name,
		const Log* caller) const {
	auto log = as_method(__func__, caller, typeid(Primitive<size_t> ), name);

	return log.returns(
			Primitive<size_t>(localizes(name.is(), &log).first, &log));
}
Class<std::string> Ensemble::who_is(Primitive<size_t> position,
		const Log* caller) const {
	std::string result;
	using Result = Class<decltype(result)>;
	auto log = as_method(__func__, caller, typeid(Result), position);

	return log.returns(Result(localizes(position, &log)->first, &log));
}
Unique_ptr Ensemble::gives(Primitive<size_t> position, const Log* caller) {
	auto log = as_method(__func__, caller, typeid(Unique_ptr), position);

	return log.returns(Unique_ptr(gives(localizes(position, &log), &log), &log));
}
Unique_ptr Ensemble::gives(Class<std::string> name, const Log* caller) {
	auto log = as_method(__func__, caller, typeid(Unique_ptr), name);

	return log.returns(
			Unique_ptr(gives(localizes(name.is(), &log).second, &log), &log));
}
void Ensemble::gets(Class<std::string> name, Unique_ptr && instance,
		Primitive<size_t> position = 0, const Log* caller) {
	auto log = as_method(__func__, caller, typeid(void), name, instance,
			position);
	auto current = dynamic_cast<Ensemble*>((Element*) instance);
	auto iterator = localizes(position, &log);

	if (current) {
		if (iterator == container.end())
			throw throw_out_of_range_0(position,
					Primitive<size_t>(container.size(), &log), log);
		container.emplace(iterator, names(name.is(), &log),
				Unique_ptr::is_from(std::move(instance)));
		current->position = this;
		current->is_modified(&log);
		is_modified(&log);
	} else
		throw throw_invalid_argument_null(instance, log);
}
void Ensemble::takes(Primitive<Ensemble*> ensemble, Primitive<size_t> source,
		Primitive<size_t> destination = 0, const Log* caller) {
	auto log = as_method(__func__, caller, typeid(void), ensemble, source,
			destination);
	Ensemble* giver = ensemble;

	if (giver) {
		auto target = giver->localizes(source, &log);

		if (target == giver->container.end())
			throw throw_out_of_range_0(source,
					Primitive<size_t>(giver->container.size()), log);
		gets(target->first, Unique_ptr(giver->gives(target, &log), &log),
				destination, &log);
	} else
		throw throw_invalid_argument_null(ensemble, log);
}
void Ensemble::takes(Primitive<Ensemble*> ensemble, Class<std::string> source,
		Primitive<size_t> destination, const Log* caller) {
	auto log = as_method(__func__, caller, typeid(void), ensemble, source,
			destination);
	Ensemble* giver = ensemble;

	if (giver)
		gets(source,
				Unique_ptr(
						giver->gives(giver->localizes(source.is(), &log).second,
								&log), &log), destination, &log);
	else
		throw throw_invalid_argument_null(ensemble, log);
}
Primitive<size_t> Ensemble::has_size(const Log* caller) const {
	return method_primitive(container.size(), *this, __func__, caller);
}
void Ensemble::self_clears(const Log* caller) {
	auto log = as_method(__func__, caller);

	container.clear();
	is_modified(&log);
}
Unique_ptr Ensemble::pop(Element& instance, const Log* caller) {
	auto log = as_method(make_scopes(__func__, "base", typeid(Ensemble).name()),
			true, caller, typeid(Unique_ptr), instance);
	Ensemble* ensemble = nullptr;
	auto current = find(instance, &log);

	if (current.first) {
		if ((ensemble = current.first)) {
			if (current.second == ensemble->container.end())
				throw std::domain_error(
						log.logs_error(
								std::ostringstream(
										"ERROR: " + instance.has_label()
												+ "'s position{"
												+ ensemble->has_label()
												+ "} should be set to null.")));
			else
				return log.returns(
						Unique_ptr(ensemble->gives(current.second, &log), &log));
		} else
			throw throw_root_element(instance, log);
	} else {
		Primitive<Ensemble*> primitive_ensemble(ensemble, &log);

		throw throw_invalid_argument_null(primitive_ensemble, log);
	}
}
void Ensemble::take(Primitive<Ensemble*> ensemble, Primitive<size_t> position,
		Element& instance, const Log* caller) {
	auto log = as_method(make_scopes(__func__, "base", typeid(Ensemble).name()),
			true, caller, typeid(void), ensemble, position, instance);
	auto current = localize(instance, &log).is();

	if (ensemble)
		throw throw_invalid_argument_null(ensemble, log);
	((Ensemble*) ensemble)->takes(std::get<0>(current), std::get<1>(current),
			position, &log);
}

std::ostringstream class_std__tuple_Ensemble__size_t__std__string__(
		const std::tuple<Ensemble*, size_t, std::string>& position) {
	std::ostringstream result("{ ");

	result << std::get<0>(position) << ": " << std::get<1>(position) << ", \""
			<< std::get<2>(position) << "\" }";

	return result;
}
template<> std::function<
		std::ostringstream(const std::tuple<Ensemble*, size_t, std::string>&)> Class<
		std::tuple<Ensemble*, size_t, std::string>>::printer =
		class_std__tuple_Ensemble__size_t__std__string__;
Class<std::tuple<Ensemble*, size_t, std::string>> Ensemble::localize(
		const Element& instance, const Log* caller) {
	auto log = as_method(make_scopes(__func__, "base", typeid(Ensemble).name()),
			true, caller,
			typeid(Class<std::tuple<Ensemble*, size_t, std::string>> ),
			instance);
	auto ensemble = dynamic_cast<Ensemble*>(instance.position);
	std::string name;
	size_t position = 0;

	if (ensemble) {
		auto current = ensemble->container.begin();
		auto size = ensemble->container.size();

		while (position++ < size && current->second.get() != &instance)
			++current;
		if (position > size)
			position = 0;
		else if (position)
			name = current->first;
		else
			log.logs_error(
					std::ostringstream(
							"WARNING: " + instance.prints().str()
									+ " not exists in "
									+ ensemble->prints().str() + "."));
	} else
		log_root_element(instance, log);

	return log.returns(
			Class<std::tuple<Ensemble*, size_t, std::string>>(
					std::make_tuple(ensemble, position, name), &log));
}
std::ostringstream class_std__vector_std__string__(
		const std::vector<std::string> sequence) {
	return Container_Printer(sequence, "\t")();
}
template<> std::function<std::ostringstream(const std::vector<std::string>&)> Class<
		std::vector<std::string>>::printer = class_std__vector_std__string__;
Class<std::vector<std::string>> Ensemble::have_path(const Element& instance,
		const Log* caller) {
	std::vector<std::string> result;
	using Result = Class<decltype(result)>;
	auto log = as_method(make_scopes(__func__, "base", typeid(Ensemble).name()),
			true, caller, typeid(Result), instance);
	auto current = localize(instance, &log).is();

	if (std::get<0>(current)) {
		result = have_path(*std::get<0>(current), &log).becomes();
		result.emplace_back(std::get<2>(current));
	}

	return log.returns(Result(result, &log));
}
std::string Ensemble::names(std::string name, const Log* caller) const {
	Class<std::string> candidate(name, caller);
	auto log = as_method(__func__, caller, typeid(std::string), candidate);
	std::set<std::string> found;
	auto current = container.begin();

	for (auto end = container.end(); current != end; ++current)
		if (names(name, current->first))
			found.emplace(current->first);
	if (found.size()) {
		if (found.size() > 1)
			candidate.is() += std::to_string(
					std::stoull((--found.end())->substr(name.length() + 1))
							+ 1);
		else
			candidate.is() += "_0";
	}

	return log.returns(candidate).becomes();
}
bool Ensemble::names(std::string name, std::string candidate,
		const Log* caller) const {
	Class<std::string> evaluated(candidate, caller);
	decltype(evaluated) base(name, caller);
	auto log = as_method(__func__, caller, typeid(bool), base, evaluated);
	auto length = name.length();

	if (length) {
		if (length < candidate.length())
			try {
				std::stoull(candidate.substr(length + 1));
			} catch (std::invalid_argument& exception) {
				candidate = "";
			}

		return log.returns(
				Primitive<bool>(candidate.substr(0, length + 1) == name + "_",
						&log));
	} else
		throw std::invalid_argument(
				log.logs_error(
						std::ostringstream(
								"ERROR: " + evaluated.prints().str() + ".")));
}
std::ostringstream class_Container__iterator_(
		const Ensemble::Container::iterator& position) {
	std::ostringstream result("{ ");

	result << &position << " }";

	return result;
}
template<> std::function<
		std::ostringstream(const Ensemble::Container::iterator&)> Class<
		Ensemble::Container::iterator>::printer = class_Container__iterator_;
Ensemble::Container::iterator Ensemble::localizes(size_t position,
		const Log* caller) const {
	auto result = const_cast<Container&>(container).begin();
	Primitive<decltype(position)> counter(position, caller);
	auto log = as_method(__func__, caller, typeid(decltype(result)), counter);
	auto size = container.size();

	if (position <= size && position)
		while (--position)
			++result;
	else {
		result = const_cast<Container&>(container).end();
		log_out_of_range_0(position, Primitive<decltype(size)>(size, &log),
				log);
	}

	return log.returns(Class<decltype(result)>(result, &log)).becomes();
}
template<typename First> std::ostringstream class_std__pair__Container__iterator__(
		const std::pair<First, Ensemble::Container::iterator>& position) {
	std::ostringstream result("{ ");

	result << position.first << "; " << &position.second << " }";

	return result;
}
template<> std::function<
		std::ostringstream(
				const std::pair<size_t, Ensemble::Container::iterator>&)> Class<
		std::pair<size_t, Ensemble::Container::iterator>>::printer =
		class_std__pair__Container__iterator__<size_t>;
std::pair<size_t, Ensemble::Container::iterator> Ensemble::localizes(
		std::string name, const Log* caller) const {
	Class<std::string> candidate(name, caller);
	auto log = as_method(__func__, caller,
			typeid(std::pair<size_t, Container::iterator>), candidate);
	auto size = const_cast<Container&>(container).size();
	decltype(size) result = 0;
	auto current = const_cast<Container&>(container).begin();

	while (result++ < size && !names(name, current->first))
		++current;

	return log.returns(
			Class<std::pair<size_t, Container::iterator>>(
					std::make_pair(result < size ? result : 0, current), &log)).becomes();
}
std::unique_ptr<Element> Ensemble::gives(Container::iterator position,
		const Log* caller) {
	Class<Container::iterator> iterator(position, caller);
	using Result = std::unique_ptr<Element>;
	auto log = as_method(__func__, caller, typeid(Result), iterator);
	Result result;

	if (position == container.end())
		throw std::invalid_argument(
				log.logs_error(
						std::ostringstream(
								"ERROR: invalid position{end of container}.")));
	result.swap(position->second);
	result->position = nullptr;
	result->is_modified(&log);
	container.erase(position);
	is_modified(&log);

	return Unique_ptr::is_from(log.returns(Unique_ptr(std::move(result), &log)));
}
template<> std::function<
		std::ostringstream(
				const std::pair<Ensemble*, Ensemble::Container::iterator>&)> Class<
		std::pair<Ensemble*, Ensemble::Container::iterator>>::printer =
		class_std__pair__Container__iterator__<Ensemble*>;
std::pair<Ensemble*, Ensemble::Container::iterator> Ensemble::find(
		Element& instance, const Log* caller) {
	auto log = as_method(make_scopes(__func__, "base", typeid(Ensemble).name()),
			true, caller, typeid(std::pair<Ensemble*, Container::iterator>),
			instance);
	Container::iterator current;
	auto ensemble = dynamic_cast<Ensemble*>(instance.position);

	if (ensemble) {
		auto end = ensemble->container.end();

		current = ensemble->container.begin();
		while (current != end && current->second.get() != &instance)
			++current;
	} else
		log_root_element(instance, log);

	return log.returns(
			Class<std::pair<Ensemble*, Container::iterator>>(
					std::make_pair(ensemble, current), &log)).becomes();
}

Ensemble::Ensemble(Class<std::string> label, const Log* caller,
		Fields attributes) :
		Object(caller, label.is()), Log(caller, label.is(), false), Element(
				label, caller, attributes) {
	as_constructor<false>("base", __func__, caller, attributes);
}
Ensemble::~Ensemble() {
	as_destructor("base", __func__);
}

} /* namespace base */
