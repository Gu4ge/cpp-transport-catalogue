#include "transport_catalogue.h"

void TransportCatalogue::TransportCatalogue::addStation(std::string& name, double x, double y)
{
	train::Stop *tmp = findStation(name);
	if (tmp != nullptr && (x != 0.0 && y != 0.0))
	{
		tmp->x = x;
		tmp->y = y;
	}
	else if (tmp == nullptr)
	{
		train::Stop stop;
		stop.name = name;
		stop.x = x;
		stop.y = y;

		all_stops_.push_back(std::move(stop));
		stops_map_.insert({all_stops_.back().name, &all_stops_.back()});
	}
}

void TransportCatalogue::TransportCatalogue::addTrain(std::string& name, const std::vector<std::string_view> stops)
{
	if (findTrain(name) == false)
	{
		train::Bus bus;
		bus.name = name;
		for (auto &i : stops)
		{
			std::string str = std::string(i);
			if (existStation(str) == false)
			{
				train::Stop stop;
				stop.name = str;
				stop.x = 0.0;
				stop.y = 0.0;

				all_stops_.push_back(std::move(stop));
				stops_map_.insert({all_stops_.back().name, &all_stops_.back()});
			}
			bus.stops.push_back(findStation(str));
		}
		all_buses_.push_back(std::move(bus));
		trains_.insert({all_buses_.back().name, &all_buses_.back()});
	}
}

bool TransportCatalogue::TransportCatalogue::findTrain(std::string& name) const noexcept
{
	if (trains_.find(name) != trains_.end())
	{
		return true;
	}
	return false;
}

bool TransportCatalogue::TransportCatalogue::existStation(std::string& name) const noexcept
{
	if (findStation(name) == nullptr)
	{
		return false;
	}
	return true;
}

TransportCatalogue::train::Stop *TransportCatalogue::TransportCatalogue::findStation(std::string& name) const
{
	if (stops_map_.count(name) != 0)
	{
		return stops_map_.find(name)->second;
	}
	return nullptr;
}

size_t TransportCatalogue::TransportCatalogue::getUniqueSize(std::string& name) const
{
	
	std::unordered_set<std::string> unique;
	for (auto &i : trains_.find(name)->second->stops)
	{
		unique.insert(i->name);
	}
	return unique.size();
}

size_t TransportCatalogue::TransportCatalogue::getAllSize(std::string& name) const
{
	return trains_.find(name)->second->stops.size();
}

double TransportCatalogue::TransportCatalogue::getDistance(std::string& name) const
{
	auto& tmp = trains_.find(name)->second->stops;
	double distance = 0.0;
	for (size_t i = 1; i < tmp.size(); i++)
	{
		Coordinates from;
		Coordinates to;
		from.lat = tmp[i - 1]->x;
		from.lng = tmp[i - 1]->y;
		to.lat = tmp[i]->x;
		to.lng = tmp[i]->y;
		distance += ComputeDistance(from, to);
	}

	return distance;
}

std::set<std::string> TransportCatalogue::TransportCatalogue::getTrains(std::string& name) const
	{
		std::set<std::string> result;
		for (const auto &train : trains_)
		{
			for (const auto &stop : train.second->stops)
			{
				if (stop->name == name)
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

	TransportCatalogue::inforamtion::Info TransportCatalogue::TransportCatalogue::getInfo(std::string& name) const{
		inforamtion::Info info(name);
		info.all_station = getAllSize(info.name_);
		info.unique_station = getUniqueSize(info.name_);
		info.distance = getDistance(info.name_);
		return info;
	}
