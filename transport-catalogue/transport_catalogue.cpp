#include "transport_catalogue.h"

void TransportCatalogue::TransportCatalogue::addStation(std::string_view name, Coordinates coordinates)
{
	train::Stop *tmp = findStation(name);
	if (tmp != nullptr && (coordinates.lat != 0.0 && coordinates.lng != 0.0))
	{
		tmp->coordinates.lat = coordinates.lat;
		tmp->coordinates.lng = coordinates.lng;
	}
	else if (tmp == nullptr)
	{
		train::Stop stop;
		stop.name = std::string(name);
		stop.coordinates.lat = coordinates.lat;
		stop.coordinates.lng = coordinates.lng;

		all_stops_.push_back(std::move(stop));
		stops_map_[all_stops_.back().name] = &all_stops_.back();
	}
}

void TransportCatalogue::TransportCatalogue::addTrain(std::string_view name, const std::vector<std::string_view> stops)
{
	if (findTrain(name) == nullptr )
	{
		train::Bus bus;
		bus.name =  std::string(name);;
		for (auto &i : stops)
		{
			
			if (findStation(i) == nullptr)
			{
				train::Stop stop;
				stop.name =  std::string(i);;
				stop.coordinates.lat = 0.0;
				stop.coordinates.lng = 0.0;

				all_stops_.push_back(std::move(stop));
				stops_map_[all_stops_.back().name] = &all_stops_.back();
			}
			bus.stops.push_back(findStation(i));
		}
		all_buses_.push_back(std::move(bus));
		trains_[all_buses_.back().name] =  &all_buses_.back();
	}
}

TransportCatalogue::train::Stop *TransportCatalogue::TransportCatalogue::findStation(std::string_view name_station) const
{
	if (stops_map_.count(name_station) != 0)
	{
		return stops_map_.at(name_station);
	}
	return nullptr;
}

const TransportCatalogue::train::Bus *TransportCatalogue::TransportCatalogue::findTrain(std::string_view name_train) const
{
	if (trains_.count(name_train) != 0)
	{
		return trains_.at(name_train);
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
			if (stop->name == name_station)
			{
				if (result.count(train.first) == 0)
				{
					result.insert(train.first);
					break;
				}
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
	for (size_t i = 1; i < tmp.size(); i++)
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