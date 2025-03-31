#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "domain.h"

#include <set>
#include <string>
#include <optional>
#include <map>

class RequestHandler {
public:
    RequestHandler(const TransportCatalogue::TransportCatalogue& catalogue,
                   const renderer::MapRenderer& renderer)
        : catalogue_(catalogue), renderer_(renderer) {}

    json::Array BuildResponses(const json::Node& stat_requests) const;

    const json::Node PrintRoute(const json::Dict& request_map) const;
    const json::Node PrintStop(const json::Dict& request_map) const;

    std::optional<inforamtion::Info> GetBusStat(std::string_view bus_name) const;
    std::set<std::string_view> GetBusesByStop(std::string_view stop_name) const;

    svg::Document RenderMap() const;

private:
    const json::Node PrintMap(const json::Dict& request_map) const;
    const TransportCatalogue::TransportCatalogue& catalogue_;
    const renderer::MapRenderer& renderer_;
};