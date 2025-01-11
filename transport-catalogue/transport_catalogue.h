#pragma once

#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <string>
#include <string_view>
#include <vector>
#include <stdexcept>

#include "geo.h"

namespace TransportCatalogue
{

	namespace inforamtion
	{
		struct Info
		{
			Info() = default;
			Info(const std::string &name) : name_(name) {}
			std::string name_;
			size_t all_station = 0;
			size_t unique_station = 0;
			double distance = 0.0;
		};
	}

	namespace train
	{
		struct Stop
		{

			std::string name;
			Coordinates coordinates;
		};

		struct Bus
		{
			std::string name;
			std::vector<const Stop *> stops;
		};
	}

	class TransportCatalogue
	{
		// Реализуйте класс самостоятельно
	public:
		TransportCatalogue() = default;

		void addStation(std::string_view name, Coordinates coordinates);

		void addTrain(std::unordered_map<std::string_view, std::vector<std::string_view>> trains);

		const train::Stop *findStation(std::string_view name_station) const;

		const train::Bus *findTrain(std::string_view name_train) const;

		std::set<std::string_view> getTrains(std::string_view name_station) const;

		inforamtion::Info getInfo(std::string_view name) const; 

	private:

		void checkIter(std::string_view name_train) const;

		std::unordered_map<std::string_view, const train::Bus *> trains_;
		std::unordered_map<std::string_view, const train::Stop *> stops_map_;
		std::deque<train::Stop> all_stops_;
		std::deque<train::Bus> all_buses_;
	};
}