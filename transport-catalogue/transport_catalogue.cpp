#include "transport_catalogue.h"

#include <iostream>

void TransportCatalogue::TransportCatalogue::addStation(std::string_view name, Coordinates coordinates)
{
	const train::Stop stop{std::string(name), coordinates};
	all_stops_.push_front(std::move(stop));
	stops_map_[all_stops_.front().name] = &all_stops_.front();
}

void TransportCatalogue::TransportCatalogue::addTrain(std::unordered_map<std::string_view, std::vector<std::string_view>> trains)
{
	for (auto &train : trains)
	{
		if (findTrain(train.first) == nullptr)
		{
			train::Bus bus;
			bus.name = std::string(train.first);

			for (auto &station : train.second)
			{
				bus.stops.push_back(findStation(station));
			}
			all_buses_.push_back(std::move(bus));
			trains_[all_buses_.back().name] = &all_buses_.back();
		}
	}
}

const TransportCatalogue::train::Stop *TransportCatalogue::TransportCatalogue::findStation(std::string_view name_station) const
{
	auto it = stops_map_.find(name_station);
	if (it != stops_map_.end())
	{
		return it->second;
	}
	return nullptr;
}

const TransportCatalogue::train::Bus *TransportCatalogue::TransportCatalogue::findTrain(std::string_view name_train) const
{
	auto it = trains_.find(name_train);
	if (it != trains_.end())
	{
		return it->second;
	}
	return nullptr;
}

std::set<std::string_view> TransportCatalogue::TransportCatalogue::getTrains(std::string_view name_station) const
{
	std::set<std::string_view> result;
	for (const auto &train : trains_)
	{
		for (const auto &stop : train.second->stops)
		{
			if (stop->name == name_station && result.count(train.first) == 0)
			{
				result.insert(train.first);
				break;
			}
		}
	}
	return result;
}

int TransportCatalogue::TransportCatalogue::getLength(std::string_view bus_number) const
{
	int len = 0;
	auto stops = findTrain(bus_number)->stops;

	for (size_t i = 0; i < stops.size() - 1; ++i)
	{
		auto curr_station = findStation(stops[i]->name);
		auto next_station = findStation(stops[i + 1]->name);

		len += getDistance(curr_station, next_station);
	}
	return len;
}

TransportCatalogue::inforamtion::Info TransportCatalogue::TransportCatalogue::getInfo(std::string_view name) const
{
	checkIter(name);
	inforamtion::Info info;

	std::unordered_set<std::string> unique;
	for (auto &i : trains_.find(name)->second->stops)
	{
		unique.insert(i->name);
	}

	info.unique_station = unique.size();
	info.all_station = trains_.find(name)->second->stops.size();
	info.distance = static_cast<double>(getLength(name));

	auto &tmp = trains_.find(name)->second->stops;
	const size_t size = tmp.size();
	double geo_abstand = 0.0;
	for (size_t i = 1; i < size; i++)
	{
		geo_abstand += ComputeDistance(tmp[i - 1]->coordinates, tmp[i]->coordinates);
	}

	info.curvature = info.distance / geo_abstand;

	return info;
}

void TransportCatalogue::TransportCatalogue::add(const train::Stop *stop1, const train::Stop *stop2, int distance)
{
	abstands_.insert({std::make_pair(stop1, stop2), distance});
}

int TransportCatalogue::TransportCatalogue::getDistance(const train::Stop *stop1, const train::Stop *stop2) const
{
	if (abstands_.count(std::make_pair(stop1, stop2)))
		return abstands_.at(std::make_pair(stop1, stop2));
	else if (abstands_.count(std::make_pair(stop2, stop1)))
		return abstands_.at(std::make_pair(stop2, stop1));
	else
		return 0;
}

void TransportCatalogue::TransportCatalogue::addAbstands(const std::unordered_map<std::string_view, std::vector<std::pair<std::string, std::string>>> &stops)
{
	for (const auto &station : stops)
	{
		auto main_stop = findStation(station.first);
		for (const auto &value : station.second)
		{
			auto abstand_stop = findStation(value.first);
			if (getDistance(main_stop, abstand_stop) != std::stoi(value.second) && abstands_.count({main_stop, abstand_stop}) == 0)
			{
				add(main_stop, abstand_stop, std::stoi(value.second));
			}
		}
	}
}

void TransportCatalogue::TransportCatalogue::checkIter(std::string_view name_train) const
{
	if (trains_.find(name_train) == trains_.end())
	{
		throw std::invalid_argument("invalid train name");
	}
}

