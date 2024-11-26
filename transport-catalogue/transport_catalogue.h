#pragma once

#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#include "geo.h"

namespace TransportCatalogue
{
	namespace inforamtion
	{
		struct Info
		{
			Info() = default;
			Info(std::string &name) : name_(name) {}
			std::string name_;
			size_t unique_station;
			size_t all_station;
			double distance;
		};
	}

	namespace train
	{
		struct Stop
		{
			std::string name;
			double x = 0.0, y = 0.0;
		};

		struct Bus
		{
			std::string name;
			std::vector<Stop *> stops;
		};
	}

	class TransportCatalogue
	{
		// Реализуйте класс самостоятельно
	public:
		TransportCatalogue() = default;

		void addStation(std::string &name, double x = 0.0, double y = 0.0);

		void addTrain(std::string &name, const std::vector<std::string_view> stops);

		bool findTrain(std::string &name) const noexcept;

		bool existStation(std::string &name) const noexcept;

		train::Stop *findStation(std::string &name) const;

		size_t getUniqueSize(std::string &name) const;

		size_t getAllSize(std::string &name) const;

		double getDistance(std::string &name) const;

		std::set<std::string> getTrains(std::string &name) const;

		inforamtion::Info getInfo(std::string &name) const;

	private:
		std::unordered_map<std::string, const train::Bus *> trains_;
		std::unordered_map<std::string, train::Stop *> stops_map_;
		std::deque<train::Stop> all_stops_;
		std::deque<train::Bus> all_buses_;
	};
}