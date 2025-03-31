#include "request_handler.h"
#include "json.h"
#include <iostream>
#include <sstream>
#include <string>
using namespace std::string_literals;

json::Array RequestHandler::BuildResponses(const json::Node& stat_requests) const {
    json::Array result;

    for (const auto& request : stat_requests.AsArray()) {
        const auto& request_map = request.AsMap();
        const auto& type = request_map.at("type").AsString();

        if (type == "Stop") {
            result.push_back(PrintStop(request_map));
        } else if (type == "Bus") {
            result.push_back(PrintRoute(request_map));
        } else if (type == "Map") {
            result.push_back(PrintMap(request_map));
        }
    }

    return result;
}


const json::Node RequestHandler::PrintRoute(const json::Dict& request_map) const {
    json::Dict result;
    const std::string& route_number = request_map.at("name").AsString();
    result["request_id"] = request_map.at("id").AsInt();

    auto info = GetBusStat(route_number);
    if (!info) {
        result["error_message"] = "not found"s;
    } else {
        result["curvature"] = info->curvature;
        result["route_length"] = info->distance;
        result["stop_count"] = static_cast<int>(info->all_station);
        result["unique_stop_count"] = static_cast<int>(info->unique_station);
    }

    return json::Node{result};
}

const json::Node RequestHandler::PrintStop(const json::Dict& request_map) const {
    json::Dict result;
    const std::string& stop_name = request_map.at("name").AsString();
    result["request_id"] = request_map.at("id").AsInt();

    auto stop = catalogue_.findStation(stop_name);
    if (!stop) {
        result["error_message"] = "not found"s;
    } else {
        json::Array buses_array;
        for (const auto& bus : GetBusesByStop(stop_name)) {
            buses_array.emplace_back(std::string(bus));
        }
        result["buses"] = std::move(buses_array);
    }

    return json::Node{result};
}

const json::Node RequestHandler::PrintMap(const json::Dict& request_map) const {
    json::Dict result;
    result["request_id"] = request_map.at("id").AsInt();

    std::ostringstream map_stream;
    svg::Document doc = RenderMap();
    doc.Render(map_stream);

    result["map"] = map_stream.str();
    return json::Node{result};
}

std::optional<inforamtion::Info> RequestHandler::GetBusStat(std::string_view bus_name) const {
    try {
        return catalogue_.getInfo(bus_name);
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

std::set<std::string_view> RequestHandler::GetBusesByStop(std::string_view stop_name) const {
    return catalogue_.getTrains(stop_name);
}

svg::Document RequestHandler::RenderMap() const {
    std::map<std::string_view, const train::Bus*> bus_map;
    for (const auto& [name, bus_ptr] : catalogue_.getAllTrains()) {
        bus_map[name] = bus_ptr;
    }
    return renderer_.GetSVG(bus_map);
}
