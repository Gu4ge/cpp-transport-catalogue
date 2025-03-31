#include <iostream>
#include <fstream>

#include "json_reader.h"
#include "request_handler.h"

int main() {
    using namespace std;

    json::Document input = json::Load(cin);

    JsonReader reader(std::move(input));

    TransportCatalogue::TransportCatalogue catalogue;
    reader.FillCatalogue(catalogue);

    renderer::MapRenderer renderer =
        reader.FillRenderSettings(reader.GetRenderSettings().AsMap());

    RequestHandler handler(catalogue, renderer);

    json::Array responses = handler.BuildResponses(reader.GetStatRequests());
    json::Print(json::Document{responses}, cout);

    return 0;
}

