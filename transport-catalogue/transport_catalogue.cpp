#include "transport_catalogue.h"
#include "geo.h"

#include <iostream>
#include <unordered_set>

namespace TransportCatalogue
{

    void TransportCatalogue::addStation(std::string_view name, geo::Coordinates coordinates)
    {
        all_stops_.emplace_back(train::Stop{std::string(name), coordinates});
        const train::Stop *stop_ptr = &all_stops_.back();
        stops_map_[stop_ptr->name] = stop_ptr;
    }

    void TransportCatalogue::addTrain(std::string_view name, const std::vector<std::string_view> &stop_names, bool is_circle)
    {
        train::Bus bus;
        bus.name = std::string(name);
        bus.is_circle = is_circle;

        for (const auto &stop_name : stop_names)
        {
            if (const auto stop = findStation(stop_name))
            {
                bus.stops.push_back(stop);
            }
        }

        all_buses_.emplace_back(std::move(bus));
        const train::Bus *bus_ptr = &all_buses_.back();
        trains_[bus_ptr->name] = bus_ptr;
    }

    const train::Stop *TransportCatalogue::findStation(std::string_view name_station) const
    {
        if (auto it = stops_map_.find(name_station); it != stops_map_.end())
        {
            return it->second;
        }
        return nullptr;
    }

    const train::Bus *TransportCatalogue::findTrain(std::string_view name_train) const
    {
        if (auto it = trains_.find(name_train); it != trains_.end())
        {
            return it->second;
        }
        return nullptr;
    }

    std::set<std::string_view> TransportCatalogue::getTrains(std::string_view stop_name) const
    {
        std::set<std::string_view> result;
        for (const auto &[bus_name, bus] : trains_)
        {
            for (const train::Stop *stop : bus->stops)
            {
                if (stop->name == stop_name)
                {
                    result.insert(bus_name);
                    break;
                }
            }
        }
        return result;
    }

    const std::unordered_map<std::string_view, const train::Bus *> &TransportCatalogue::getAllTrains() const
    {
        return trains_;
    }

    int TransportCatalogue::getDistance(const train::Stop *from, const train::Stop *to) const
    {
        if (auto it = abstands_.find({from, to}); it != abstands_.end())
        {
            return it->second;
        }
        else if (auto rev = abstands_.find({to, from}); rev != abstands_.end())
        {
            return rev->second;
        }
        return 0;
    }

    void TransportCatalogue::addAbstands(const std::unordered_map<std::string_view, std::vector<std::pair<std::string, std::string>>> &stops)
    {
        for (const auto &[from, to_list] : stops)
        {
            const train::Stop *from_ptr = findStation(from);
            if (!from_ptr)
                continue;

            for (const auto &[to_name, dist_str] : to_list)
            {
                const train::Stop *to_ptr = findStation(to_name);
                if (to_ptr && abstands_.count({from_ptr, to_ptr}) == 0)
                {
                    add(from_ptr, to_ptr, std::stoi(dist_str));
                }
            }
        }
    }

    void TransportCatalogue::add(const train::Stop *from, const train::Stop *to, int dist)
    {
        abstands_[{from, to}] = dist;
    }

    inforamtion::Info TransportCatalogue::getInfo(std::string_view bus_name) const
    {
        const train::Bus *bus = findTrain(bus_name);
        if (!bus)
            throw std::invalid_argument("Bus not found");

        inforamtion::Info info;
        info.name_ = std::string(bus_name);
        info.all_station = bus->is_circle ? bus->stops.size()
                                          : bus->stops.size() * 2 - 1;

        std::unordered_set<std::string_view> unique;
        for (const auto *stop : bus->stops)
        {
            unique.insert(stop->name);
        }
        info.unique_station = unique.size();

        info.distance = static_cast<double>(getLength(bus_name));

        double geo_length = 0.0;
        for (size_t i = 1; i < bus->stops.size(); ++i)
        {
            geo_length += geo::ComputeDistance(bus->stops[i - 1]->coordinates, bus->stops[i]->coordinates);
        }
        if (!bus->is_circle)
        {
            geo_length *= 2;
        }

        info.curvature = geo_length > 0.0 ? info.distance / geo_length : 0.0;
        return info;
    }

    int TransportCatalogue::getLength(std::string_view bus_name) const
    {
        const train::Bus *bus = findTrain(bus_name);
        if (!bus)
            return 0;

        int length = 0;
        for (size_t i = 1; i < bus->stops.size(); ++i)
        {
            length += getDistance(bus->stops[i - 1], bus->stops[i]);
        }

        if (!bus->is_circle)
        {
            for (size_t i = bus->stops.size(); i-- > 1;)
            {
                length += getDistance(bus->stops[i], bus->stops[i - 1]);
            }
        }

        return length;
    }

} // namespace TransportCatalogue
