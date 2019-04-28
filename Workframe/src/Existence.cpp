/*
 * Existence.cpp
 *
 *  Created on: 01 jan 2019
 *      Author: m1r
 */

#include "Existence.h"

namespace base {
//Process
bool Process::run = true;

void Process::end() {
	run = false;
}
bool Process::running() {
	return run;
}

//Variable
Variable::operator Variable::Field() const {
	return std::make_pair(name, (std::string) *this);
}
std::string Variable::operator ()() const {
	return name + "< " + type + " >{ " + ((std::string) *this) + " }";
}

Variable::Variable(std::string name, std::type_index type) {
	this->name = name;
	this->type = type.name();
}

//Class
template<> std::function<std::string(const Object::Instant*)> Class<
		Object::Instant>::printer = [](const Object::Instant* instant) {
	return std::to_string((*instant - Object::start).count());
};
std::string string_printer(const std::string* string) {
	return "\"" + *string + "\"";
}
template<> std::function<std::string(const std::string*)> Class<
		const std::string>::printer = string_printer;
template<> std::function<std::string(const std::string*)> Class<std::string>::printer =
		string_printer;
template<> std::function<std::string(const Object::Fields*)> Class<
		Object::Fields>::printer = print_std__map<std::string, std::string>;
template<> std::function<std::string(Element* const *)> Class<Element*>::printer =
		[](Element* const * element) {
			auto pointer = *element;

			if (pointer)
			return "->" + (pointer)->prints();
			else
			return std::string("'nullptr'");
		};
template<> std::function<std::string(const std::set<Element*>*)> Class<
		std::set<Element*>>::printer = print_std__set<Element*>;
template<> std::function<
		std::string(
				const std::list<
						std::pair<std::string, std::unique_ptr<Element> > >*)> Class<
		std::list<std::pair<std::string, std::unique_ptr<Element> > > >::printer =
		[](const Ensemble::Container* container) {
			std::string result = "{";
			std::ostringstream list;
			auto end = container->end();

			for (auto current = container->begin(); current != end; ++current)
			list << "\n\t" << current->first << ": #" << current->second.get() << "#;";

			if ((result += list.str()) == "{")
			result += " ";
			else
			result += "\n";

			return result + "}";
		};
template<> std::function<std::string(const std::invalid_argument*)> Class<
		std::invalid_argument>::printer =
		[](const std::invalid_argument* exception) {
			return exception->what();
		};
template<> std::function<std::string(const std::logic_error*)> Class<
		std::logic_error>::printer = [](const std::logic_error* exception) {
	return exception->what();
};
template<> std::function<
		std::string(
				const std::map<std::string,
						std::pair<const std::string, std::string>>*)> Class<
		std::map<std::string, std::pair<const std::string, std::string>>>::printer =
		print_std__map<std::string, std::pair<const std::string, std::string>>;
template<> std::function<std::string(const bool*)> Class<bool>::printer =
		print_fundamental<bool>;
template<> std::function<std::string(Ensemble* const *)> Class<Ensemble*>::printer =
		print_fundamental<Ensemble*>;
template<> std::function<std::string(const long long unsigned*)> Class<
		long long unsigned>::printer = print_fundamental<long long unsigned>;
template<> std::function<std::string(const int*)> Class<int>::printer =
		print_fundamental<int>;
template<> std::function<
		std::string(const std::pair<const std::string, std::string>*)> Class<
		std::pair<const std::string, std::string>>::printer =
		[](const std::pair<const std::string, std::string>* pair) {
			return "{ " + pair->first + "; " + pair->second + " }";
		};

//Object
std::chrono::steady_clock::time_point Object::start =
		std::chrono::steady_clock::now();

Object::Instant Object::exists_from() const {
	return creation;
}
Object::Instant Object::is_since() const {
	return modification;
}
Object::Fields Object::gives_attributes() const {
	return attributes;
}
void Object::gets_attributes(Fields attributes) {
	last_attributes = this->attributes;
	this->attributes = attributes;
	is_modified();
}
Object::Modifications Object::gives_modifications() {
	Modifications result;
	auto new_end = attributes.end();
	auto last_end = last_attributes.end();

	for (auto last_attribute : last_attributes) {
		auto last_name = last_attribute.first;
		auto last_value = last_attribute.second;

		if (attributes.find(last_name) == new_end)
			result.emplace(last_name, std::make_pair(last_value, ""));
		else {
			auto new_value = attributes.at(last_name);

			if (new_value != last_value)
				result.emplace(last_name,
						std::make_pair(last_value, new_value));
		}
	}
	for (auto attribute : attributes) {
		auto new_name = attribute.first;

		if (last_attributes.find(new_name) == last_end)
			result.emplace(attribute.first,
					std::make_pair("", attribute.second));
	}

	return result;
}
bool Object::operator ==(const Object& righthand) {
	return typeid(*this) == typeid(righthand);
}
bool Object::operator !=(const Object& righthand) {
	return !operator ==(righthand);
}
void Object::initializes(Fields attributes) {
	this->attributes = attributes;
	is_modified();
	creation = savage = modification;
}
void Object::is_modified() {
	modification = std::chrono::steady_clock::now();
}

Object::Fields Object::shows() const {
	Fields result;

	result.insert(VARIABLE(creation));
	result.insert(VARIABLE(modification));
	result.insert(VARIABLE(attributes));
	result.insert(VARIABLE(last_attributes));

	return result;
}

Object::Object() {
	initializes();
}
Object::Object(Fields attributes) {
	initializes(attributes);
}
Object::Object(const Object& copy) {
	initializes(copy.attributes);
}
Object& Object::operator =(const Object& copy) {
	gets_attributes(copy.attributes);

	return *this;
}

//Element
std::set<Element*> Element::everything;

void Element::initializes() {
	position = nullptr;
	everything.emplace(this);
}

Object::Fields Element::shows() const {
	auto result = Object::shows();

	result.insert(VARIABLE(position));

	return result;
}

std::set<Element*> Element::give_everything() {
	return everything;
}
Variable::Field Element::shows_everything() {
	auto result = CLASS(base::Element::everything);

	return std::make_pair(result(), ((Variable::Field) result).second);
}

Element::Element() {
	initializes();
}
Element::Element(Fields attributes) :
		Object(attributes) {
	initializes();
}
Element::~Element() {
	if (Process::running())
		everything.erase(this);
}
Element::Element(const Element& copy) :
		Object(copy) {
	initializes();
}
Element& Element::operator =(const Element& copy) {
	Object::operator =(copy);

	return *this;
}
Element::Element(Element&& moving) :
		Object(std::move(moving)) {
	initializes();
	position = moving.position;
}

//Ensemble
Ensemble::Container Ensemble::nowhere;

Element& Ensemble::operator [](size_t position) const {
	return *localizes(position)->second;
}
std::map<size_t, Element*> Ensemble::operator [](std::string name) const {
	std::map<size_t, Element*> result;
	size_t position = 0;
	auto current = container.begin();

	for (auto size = container.size(); position < size; ++current)
		if (names(name, current->first))
			result[++position] = current->second.get();
		else
			++position;

	return result;
}
size_t Ensemble::which_is(std::string name) const {
	return localizes(name).first;
}
std::string Ensemble::who_is(size_t position) const {
	return localizes(position)->first;
}
Ensemble::Unique_ptr Ensemble::gives(size_t position) {
	return gives(localizes(position));
}
Ensemble::Unique_ptr Ensemble::gives(std::string name) {
	return gives(localizes(name).second);
}
void Ensemble::gets(std::string name, Unique_ptr&& element, size_t position) {
	auto current = element.get();
	auto iterator = localizes(position);

	if (current) {
		container.emplace(iterator, names(name), std::move(element));
		current->position = this;
		current->is_modified();
		is_modified();
	} else
		throw Throw::null_argument(__func__, 2);
}
void Ensemble::takes(size_t current_position, Ensemble& current_ensemble,
		size_t new_position) {
	takes(current_ensemble.localizes(current_position), current_position,
			current_ensemble, new_position);
}
void Ensemble::takes(std::string current_name, Ensemble& current_ensemble,
		size_t new_position) {
	auto current = current_ensemble.localizes(current_name);

	takes(current.second, current.first, current_ensemble, new_position);
}
void Ensemble::takes(Ensemble& ensemble) {
	auto size = ensemble.has_size();
	auto end = has_size();

	while (size--)
		takes(0, ensemble, ++end);
}
void gives_back() {
	d;
}
size_t Ensemble::has_size() const {
	return container.size();
}
void Ensemble::self_clears() {

	container.clear();
	is_modified();
}
void resumes() {

}
std::string Ensemble::names(std::string candidate) const {
	std::set<std::string> found;
	auto current = container.begin();
	std::set<std::string>::iterator last;
	auto length = candidate.length();

	if (candidate.empty())
		candidate = "_";
	for (auto end = container.end(); current != end; ++current)
		if (names(candidate, current->first))
			found.emplace(current->first);
	last = found.end();
	if (found.find(candidate) != last)
		if ((--last)->length() > length)
			candidate += "_"
					+ std::to_string(std::stoull(last->substr(length + 1)) + 1);

	return candidate;
}
bool Ensemble::names(std::string name, std::string candidate) const {
	size_t position = 0;
	auto prefix = name.length() + 1;

	try {
		std::stoull(candidate.substr(prefix), &position);
		if (position < candidate.length()
				|| name + "_" != candidate.substr(0, prefix))
			throw std::exception();
	} catch (...) {
		return name == candidate;
	}

	return true;
}
Ensemble::Container::iterator Ensemble::localizes(size_t position) const {
	auto result = const_cast<Container&>(container).begin();
	auto size = container.size() + 1;

	if (position <= size && position)
		while (--position)
			++result;
	else {
		result = const_cast<Container&>(container).end();
		std::clog << log_out_of_range_0(position, size) << std::endl;
	}

	return result;
}
std::pair<size_t, Ensemble::Container::iterator> Ensemble::localizes(
		std::string name) const {
	auto current = const_cast<Container&>(container).begin();
	auto end = container.end();
	size_t position = 0;

	while (current != end)
		++position;

	return std::make_pair(position, current);
}
Ensemble::Unique_ptr Ensemble::gives(Container::iterator iterator) {
	Unique_ptr result;

	if (iterator == container.end())
		throw throw_out_of_range_0(container.size(), container.size());
	else {
		result.swap(iterator->second);
		is_modified();
		result->position = nullptr;
		result->is_modified();
		container.erase(iterator);
	}

	return std::move(result);
}
void Ensemble::takes(Container::iterator current, size_t current_position, Ensemble& current_ensemble,
		size_t new_position) {
	if (current == current_ensemble.container.end())
		throw throw_out_of_range_0(current_position,
				current_ensemble.container.size());
	if (Process::saving())
		inbounds.emplace_back(current_ensemble, current_position,
				current->first, current->second.get());
	gets(current->first, current_ensemble.gives(current), new_position);
}

Object::Fields Ensemble::shows() const {
	auto result = Element::shows();

	result.insert(VARIABLE(container));

	return result;
}

std::tuple<Ensemble*, size_t, std::string> Ensemble::localize(
		const Element& element) {
	auto ensemble = dynamic_cast<Ensemble*>(element.position);
	std::string name;
	size_t position = 0;

	if (ensemble) {
		auto current = ensemble->container.begin();
		auto size = ensemble->container.size();

		while (position++ < size && current->second.get() != &element)
			++current;
		if (position > size)
			position = 0;
		else if (position)
			name = current->first;
		else
			throw_wrong_position(element, ensemble);
	} else {
		auto current = buffer.begin();
		auto size = buffer.size();

		while (position++ < size && current->second.get() != &element)
			++current;
		if (position > size)
			position = 0;
		else if (position)
			name = current->first;
		else
			std::clog << log_root_element(element) << std::endl;
	}

	return std::make_tuple(ensemble, position, name);
}
Ensemble::Unique_ptr Ensemble::pop(Element& element) {
	auto current = localize(element);
	auto ensemble = std::get<0>(current);

	if (ensemble)
		return ensemble->gives(std::get<1>(current));
	else
		throw throw_root_element(element);
}
void Ensemble::take(Element& element, Ensemble& new_ensemble,
		size_t new_position) {
	auto current = localize(element);
	auto current_ensemble = std::get<0>(current);

	if (current_ensemble)
		throw throw_root_element(element);
	new_ensemble.gets(std::get<2>(current),
			current_ensemble->gives(std::get<1>(current)), new_position);
}
std::vector<std::string> Ensemble::have_path(const Element& element) {
	std::vector<std::string> result;
	auto current = localize(element);
	auto position = std::get<0>(current);

	if (position) {
		result = have_path(*position);
		result.emplace_back(std::get<2>(current));
	} else {
		std::ostringstream string;

		string << &element;
		result.emplace_back(string.str());
	}

	return result;
}
std::string Ensemble::log_root_element(const Element& element) {
	return element.prints() + " is a root.";
}
std::string Ensemble::log_out_of_range_0(size_t position, size_t size) {
	std::string result = "position(" + std::to_string(position);

	if (position)
		result += ") > size(" + std::to_string(size) + ")";
	else
		result += ") = 0";

	return result + ".";
}
std::out_of_range Ensemble::throw_out_of_range_0(size_t position, size_t size) {
	auto result = log_out_of_range_0(position, size);

	std::cerr << result << std::endl;

	return std::out_of_range(result);
}
std::domain_error Ensemble::throw_root_element(const Element& element) {
	auto result = log_root_element(element);

	std::cerr << result << std::endl;

	return std::domain_error(result);
}
std::runtime_error Ensemble::throw_wrong_position(const Element& element,
		const Ensemble* ensemble) {
	std::ostringstream result(element.prints() + " has the wrong position ");

	if (ensemble)
		result << ensemble->prints();
	else
		result << ensemble;
	result << ".";
	std::cerr << result.str() << std::endl;

	return std::runtime_error(result.str());
}

Ensemble::Ensemble(Fields attributes) :
		Element(attributes) {
}
Ensemble::Ensemble(const Ensemble& copy) :
		Element(copy.gives_attributes()) {

}

std::invalid_argument Throw::invalid_argument(const Object& object) {
	std::string result = object.prints() + " is an invalid argument.";

	std::cerr << result << std::endl;

	return std::invalid_argument(result);
}
std::invalid_argument Throw::null_argument(std::string function,
		size_t argument) {
	std::string result = "A null argument#" + std::to_string(argument)
			+ " is passed to function '" + function + "'.";

	std::cerr << result << std::endl;

	return std::invalid_argument(result);
}
std::logic_error Throw::not_allowed(std::string message) {
	message = "Operation not allowed" + (message.empty() ? "" : ": " + message);
	std::cerr << message << std::endl;

	return std::logic_error(message);
}

} /* namespace base */
