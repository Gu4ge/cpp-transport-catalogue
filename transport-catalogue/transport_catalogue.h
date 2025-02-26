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
			double curvature = 0.0;
		};
	}

	namespace train
	{
		struct Stop
		{
			std::string name;		 // название остановки
			Coordinates coordinates; // координаты остановки
		};

		struct Bus
		{
			std::string name;				 // номер автобуса
			std::vector<const Stop *> stops; // маршрут
		};
	}

	class TransportCatalogue
	{
	public:
		TransportCatalogue() = default;

		void addStation(std::string_view name, Coordinates coordinates);

		void addTrain(std::unordered_map<std::string_view, std::vector<std::string_view>> trains);

		void addAbstands(const std::unordered_map<std::string_view, std::vector<std::pair<std::string, std::string>>> &stops);

		const train::Stop *findStation(std::string_view name_station) const;

		const train::Bus *findTrain(std::string_view name_train) const;

		std::set<std::string_view> getTrains(std::string_view name_station) const;

		inforamtion::Info getInfo(std::string_view name) const;

		void add(const train::Stop* stop1, const train::Stop* stop2, int distance);

    	int getDistance(const train::Stop* stop1, const train::Stop* stop2) const;

	private:
		void checkIter(std::string_view name_train) const;

		int getLength(std::string_view bus_number) const;

		struct hash_pair
		{
			template <class T>
			std::size_t operator()(const std::pair<T, T> &p) const
			{
				auto hash1 = std::hash<T>{}(p.first);
				auto hash2 = std::hash<T>{}(p.second);
				return hash1 ^ (hash2 << 1);
			}
		};

		std::unordered_map<std::string_view, const train::Bus *> trains_;
		std::unordered_map<std::string_view, const train::Stop *> stops_map_;
		std::unordered_map<std::pair<const train::Stop *, const train::Stop *>, int, hash_pair> abstands_;
		std::deque<train::Stop> all_stops_;
		std::deque<train::Bus> all_buses_;
	};
}