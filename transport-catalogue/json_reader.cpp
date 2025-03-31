
#include "json_reader.h"
#include "svg.h"

const json::Node &JsonReader::GetBaseRequests() const
{
    const auto &root = input_.GetRoot();
    if (!root.IsMap())
        return dummy_;
    const auto &map = root.AsMap();
    if (map.count("base_requests") && map.at("base_requests").IsArray())
    {
        return map.at("base_requests");
    }
    return dummy_;
}

const json::Node &JsonReader::GetStatRequests() const
{
    const auto &root = input_.GetRoot();
    if (!root.IsMap())
        return dummy_;
    const auto &map = root.AsMap();
    if (map.count("stat_requests") && map.at("stat_requests").IsArray())
    {
        return map.at("stat_requests");
    }
    return dummy_;
}

const json::Node &JsonReader::GetRenderSettings() const
{
    const auto &root = input_.GetRoot();
    if (!root.IsMap())
        return dummy_;
    const auto &map = root.AsMap();
    if (map.count("render_settings") && map.at("render_settings").IsMap())
    {
        return map.at("render_settings");
    }
    return dummy_;
}

void JsonReader::FillCatalogue(TransportCatalogue::TransportCatalogue &catalogue)
{
    const auto &requests = GetBaseRequests();
    if (!requests.IsArray())
        return;
    const json::Array &arr = requests.AsArray();

    std::unordered_map<std::string_view, std::vector<std::pair<std::string, std::string>>> distances;

    for (const auto &request : arr)
    {
        if (!request.IsMap())
            continue;
        const auto &map = request.AsMap();
        if (!map.count("type") || !map.at("type").IsString())
            continue;
        const std::string &type = map.at("type").AsString();
        if (type == "Stop")
        {
            auto [stop_name, coordinates, stop_distances] = FillStop(map);
            catalogue.addStation(stop_name, coordinates);
            for (const auto &[to, dist] : stop_distances)
            {
                distances[stop_name].emplace_back(std::string(to), std::to_string(dist));
            }
        }
    }

    catalogue.addAbstands(distances);

    for (const auto &request : arr)
    {
        if (!request.IsMap())
            continue;
        const auto &map = request.AsMap();
        if (!map.count("type") || !map.at("type").IsString())
            continue;
        const std::string &type = map.at("type").AsString();
        if (type == "Bus")
        {
            auto [bus_number, stops, is_circle] = FillRoute(map);
            catalogue.addTrain(bus_number, stops, is_circle);
        }
    }
}

std::tuple<std::string_view, geo::Coordinates, std::map<std::string_view, int>>
JsonReader::FillStop(const json::Dict &request_map) const
{
    std::string_view name = request_map.at("name").AsString();
    double lat = request_map.at("latitude").AsDouble();
    double lng = request_map.at("longitude").AsDouble();
    std::map<std::string_view, int> stop_distances;
    if (request_map.count("road_distances") && request_map.at("road_distances").IsMap())
    {
        for (const auto &[to, node] : request_map.at("road_distances").AsMap())
        {
            if (node.IsInt())
            {
                stop_distances[to] = node.AsInt();
            }
        }
    }
    return {name, {lat, lng}, stop_distances};
}

std::tuple<std::string_view, std::vector<std::string_view>, bool>
JsonReader::FillRoute(const json::Dict &request_map) const
{
    std::string_view bus_name = request_map.at("name").AsString();
    std::vector<std::string_view> stops;
    for (const auto &stop : request_map.at("stops").AsArray())
    {
        stops.push_back(stop.AsString());
    }
    bool is_circle = request_map.at("is_roundtrip").AsBool();
    return {bus_name, stops, is_circle};
}

renderer::MapRenderer JsonReader::FillRenderSettings(const json::Dict &request_map) const
{
    using namespace renderer;
    RenderSettings rs;

    rs.width = request_map.at("width").AsDouble();
    rs.height = request_map.at("height").AsDouble();
    rs.padding = request_map.at("padding").AsDouble();
    rs.line_width = request_map.at("line_width").AsDouble();
    rs.stop_radius = request_map.at("stop_radius").AsDouble();
    rs.bus_label_font_size = request_map.at("bus_label_font_size").AsInt();

    const auto &bus_offset = request_map.at("bus_label_offset").AsArray();
    rs.bus_label_offset = {bus_offset[0].AsDouble(), bus_offset[1].AsDouble()};

    rs.stop_label_font_size = request_map.at("stop_label_font_size").AsInt();
    const auto &stop_offset = request_map.at("stop_label_offset").AsArray();
    rs.stop_label_offset = {stop_offset[0].AsDouble(), stop_offset[1].AsDouble()};

    const auto &under_color = request_map.at("underlayer_color");
    if (under_color.IsString())
    {
        rs.underlayer_color = under_color.AsString();
    }
    else if (under_color.IsArray())
    {
        const auto &arr = under_color.AsArray();
        if (arr.size() == 3)
        {
            rs.underlayer_color = svg::Rgb(arr[0].AsInt(), arr[1].AsInt(), arr[2].AsInt());
        }
        else if (arr.size() == 4)
        {
            rs.underlayer_color = svg::Rgba(arr[0].AsInt(), arr[1].AsInt(), arr[2].AsInt(), arr[3].AsDouble());
        }
    }

    rs.underlayer_width = request_map.at("underlayer_width").AsDouble();

    for (const auto &color : request_map.at("color_palette").AsArray())
    {
        if (color.IsString())
        {
            rs.color_palette.emplace_back(color.AsString());
        }
        else if (color.IsArray())
        {
            const auto &c = color.AsArray();
            if (c.size() == 3)
            {
                rs.color_palette.emplace_back(svg::Rgb(c[0].AsInt(), c[1].AsInt(), c[2].AsInt()));
            }
            else if (c.size() == 4)
            {
                rs.color_palette.emplace_back(svg::Rgba(c[0].AsInt(), c[1].AsInt(), c[2].AsInt(), c[3].AsDouble()));
            }
        }
    }

    return MapRenderer(rs);
}
