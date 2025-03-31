#include "map_renderer.h"
#include <algorithm>
#include <set>

using namespace std::literals;

namespace renderer {

MapRenderer::MapRenderer(RenderSettings settings)
    : settings_(std::move(settings)) {}

svg::Document MapRenderer::GetSVG(const std::map<std::string_view, const train::Bus*>& buses) const {
    svg::Document doc;

    std::set<const train::Stop*> all_stops;
    for (const auto& [_, bus] : buses) {
        all_stops.insert(bus->stops.begin(), bus->stops.end());
    }

    std::vector<geo::Coordinates> coords;
    for (const auto* stop : all_stops) {
        coords.push_back(stop->coordinates);
    }
    SphereProjector projector(coords.begin(), coords.end(),
                               settings_.width, settings_.height, settings_.padding);

    size_t color_index = 0;
    for (const auto& [bus_name, bus] : buses) {
        if (bus->stops.empty()) continue;

        const svg::Color& color = settings_.color_palette[color_index++ % settings_.color_palette.size()];
        svg::Polyline polyline;

        for (const auto* stop : bus->stops) {
            polyline.AddPoint(projector(stop->coordinates));
        }

        if (!bus->is_circle) {
            for (size_t i = bus->stops.size() - 1; i > 0; --i) {
                polyline.AddPoint(projector(bus->stops[i - 1]->coordinates));
            }
        }

        polyline.SetStrokeColor(color)
                .SetFillColor("none")
                .SetStrokeWidth(settings_.line_width)
                .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        doc.Add(std::move(polyline));
    }

    std::vector<std::pair<std::string_view, const train::Bus*>> sorted_buses(buses.begin(), buses.end());
    std::sort(sorted_buses.begin(), sorted_buses.end());

    color_index = 0;
    for (const auto& [bus_name, bus] : sorted_buses) {
        if (bus->stops.empty()) continue;

        const svg::Color& color = settings_.color_palette[color_index++ % settings_.color_palette.size()];
        std::vector<const train::Stop*> label_stops{bus->stops.front()};
        if (!bus->is_circle && bus->stops.front() != bus->stops.back()) {
            label_stops.push_back(bus->stops.back());
        }

        for (const auto* stop : label_stops) {
            svg::Point pos = projector(stop->coordinates);

            svg::Text underlayer;
            underlayer.SetData(std::string(bus_name))
                      .SetPosition(pos)
                      .SetOffset(settings_.bus_label_offset)
                      .SetFontSize(settings_.bus_label_font_size)
                      .SetFontFamily("Verdana")
                      .SetFontWeight("bold")
                      .SetFillColor(settings_.underlayer_color)
                      .SetStrokeColor(settings_.underlayer_color)
                      .SetStrokeWidth(settings_.underlayer_width)
                      .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                      .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

            svg::Text label;
            label.SetData(std::string(bus_name))
                 .SetPosition(pos)
                 .SetOffset(settings_.bus_label_offset)
                 .SetFontSize(settings_.bus_label_font_size)
                 .SetFontFamily("Verdana")
                 .SetFontWeight("bold")
                 .SetFillColor(color);

            doc.Add(std::move(underlayer));
            doc.Add(std::move(label));
        }
    }

    std::vector<const train::Stop*> sorted_stops(all_stops.begin(), all_stops.end());
    std::sort(sorted_stops.begin(), sorted_stops.end(),
              [](const train::Stop* lhs, const train::Stop* rhs) {
                  return lhs->name < rhs->name;
              });

    for (const auto* stop : sorted_stops) {
        svg::Circle circle;
        circle.SetCenter(projector(stop->coordinates))
              .SetRadius(settings_.stop_radius)
              .SetFillColor("white");
        doc.Add(std::move(circle));
    }

    for (const auto* stop : sorted_stops) {
        svg::Point pos = projector(stop->coordinates);

        svg::Text underlayer;
        underlayer.SetData(std::string(stop->name))
                  .SetPosition(pos)
                  .SetOffset(settings_.stop_label_offset)
                  .SetFontSize(settings_.stop_label_font_size)
                  .SetFontFamily("Verdana")
                  .SetFillColor(settings_.underlayer_color)
                  .SetStrokeColor(settings_.underlayer_color)
                  .SetStrokeWidth(settings_.underlayer_width)
                  .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                  .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

        svg::Text label;
        label.SetData(std::string(stop->name))
             .SetPosition(pos)
             .SetOffset(settings_.stop_label_offset)
             .SetFontSize(settings_.stop_label_font_size)
             .SetFontFamily("Verdana")
             .SetFillColor("black");

        doc.Add(std::move(underlayer));
        doc.Add(std::move(label));
    }

    return doc;
}

} // namespace renderer
