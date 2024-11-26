#pragma once

#include <iosfwd>
#include <string_view>
#include <iostream>
#include <iomanip>

#include "transport_catalogue.h"

void ParseAndPrintStat(const TransportCatalogue::TransportCatalogue& tansport_catalogue, std::string_view request,
                       std::ostream& output);