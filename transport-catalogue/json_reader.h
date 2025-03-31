#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "geo.h"
#include "domain.h"

#include <iostream>
#include <tuple>
#include <map>
#include <string_view>

class JsonReader
{
public:
    JsonReader(json::Document doc) : input_(std::move(doc)) {}
    JsonReader(std::istream &input)
        : input_(json::Load(input)) {}

    const json::Node &GetBaseRequests() const;
    const json::Node &GetStatRequests() const;
    const json::Node &GetRenderSettings() const;

    void FillCatalogue(TransportCatalogue::TransportCatalogue &catalogue);
    renderer::MapRenderer FillRenderSettings(const json::Dict &request_map) const;

private:
    json::Document input_;
    json::Node dummy_ = nullptr;

    std::tuple<std::string_view, geo::Coordinates, std::map<std::string_view, int>>
    FillStop(const json::Dict &request_map) const;

    void FillStopDistances(TransportCatalogue::TransportCatalogue &catalogue) const;

    std::tuple<std::string_view, std::vector<std::string_view>, bool>
    FillRoute(const json::Dict &request_map) const;
};