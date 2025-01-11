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

	auto &tmp = trains_.find(name)->second->stops;
	const size_t size = tmp.size();
	for (size_t i = 1; i < size; i++)
	{
		info.distance += ComputeDistance(tmp[i - 1]->coordinates, tmp[i]->coordinates);
	}

	return info;
}

void TransportCatalogue::TransportCatalogue::checkIter(std::string_view name_train) const
{
	if (trains_.find(name_train) == trains_.end())
	{
		throw std::invalid_argument("invalid train name");
	}
}