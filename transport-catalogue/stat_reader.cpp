#include "stat_reader.h"

void ParseAndPrintStat(const TransportCatalogue::TransportCatalogue &transport_catalogue, std::string_view request,
                       std::ostream &output)
{
    // Реализуйте самостоятельно

    auto it = request.find_first_of(' ');
    std::string name = std::string(request.substr(it + 1));

    if (request.substr(0, it) == "Bus")
    {
        if (transport_catalogue.findTrain(name) == true)
        {

            auto info = transport_catalogue.getInfo(name);

            output << request << ": " << info.all_station << " stops on route, " << info.unique_station << " unique stops, " << info.distance << " route length\n";
        }
        else
        {
            output << request << ": not found\n";
        }
    }
    else
    {
        
        if (transport_catalogue.findStation(name) == nullptr)
        {
            output << request << ": not found\n";
        }
        else
        {
            std::set<std::string> routes = transport_catalogue.getTrains(name);
            if (routes.size() == 0)
            {
                output << request << ": no buses\n";
            }
            else
            {
                output << request << ": buses";
                for (const auto &i : routes)
                {
                    output << " " << i;
                }
                output << "\n";
            }
        }
    }

    // Bus X: R stops on route, U unique stops, L route length
}
