#include "stat_reader.h"

void ParseAndPrintStat(const TransportCatalogue::TransportCatalogue &transport_catalogue, std::string_view request, std::ostream &output)
{
    auto it = request.find_first_of(' ');
    std::string_view name = request.substr(it + 1);

    if (request.substr(0, it) == "Bus")
    {
        const auto *bus = transport_catalogue.findTrain(name);
        if (bus)
        {
            auto info = transport_catalogue.getInfo(name);
            output << request << ": "
                   << info.all_station << " stops on route, "
                   << info.unique_station << " unique stops, "
                   << std::setprecision(6)
                   << info.distance << " route length, "
                   << info.curvature << " curvature\n";
        }
        else
        {
            output << request << ": not found\n";
        }
    }
    else
    {
        const auto *stop = transport_catalogue.findStation(name);
        if (!stop)
        {
            output << request << ": not found\n";
        }
        else
        {
            auto routes = transport_catalogue.getTrains(name);
            if (routes.empty())
            {
                output << request << ": no buses\n";
            }
            else
            {
                output << request << ": buses";
                for (const auto &route : routes)
                {
                    output << " " << route;
                }
                output << "\n";
            }
        }
    }
}
