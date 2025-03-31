#pragma once
#include <string>
#include <vector>
#include "geo.h"

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
		std::string name;			  // название остановки
		geo::Coordinates coordinates; // координаты остановки
	};

	struct Bus
	{
		std::string name;				 // номер автобуса
		std::vector<const Stop *> stops; // маршрут
		bool is_circle = false;
	};
}