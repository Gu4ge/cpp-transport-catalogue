#include "input_reader.h"

#include <algorithm>
#include <cassert>
#include <iterator>

/**
 * Парсит строку вида "10.123,  -30.1837" и возвращает пару координат (широта, долгота)
 */
Coordinates ParseCoordinates(std::string_view str)
{
    static const double nan = std::nan("");

    auto not_space = str.find_first_not_of(' ');
    auto comma = str.find(',');

    if (comma == str.npos)
    {
        return {nan, nan};
    }

    auto not_space2 = str.find_first_not_of(' ', comma + 1);

    double lat = std::stod(std::string(str.substr(not_space, comma - not_space)));
    double lng = std::stod(std::string(str.substr(not_space2)));

    return {lat, lng};
}

/**
 * Удаляет пробелы в начале и конце строки
 */
std::string_view Trim(std::string_view string)
{
    const auto start = string.find_first_not_of(' ');
    if (start == string.npos)
    {
        return {};
    }
    return string.substr(start, string.find_last_not_of(' ') + 1 - start);
}

/**
 * Разбивает строку string на n строк, с помощью указанного символа-разделителя delim
 */
std::vector<std::string_view> Split(std::string_view string, char delim)
{
    std::vector<std::string_view> result;

    size_t pos = 0;
    while ((pos = string.find_first_not_of(' ', pos)) < string.length())
    {
        auto delim_pos = string.find(delim, pos);
        if (delim_pos == string.npos)
        {
            delim_pos = string.size();
        }
        if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty())
        {
            result.push_back(substr);
        }
        pos = delim_pos + 1;
    }

    return result;
}

/**
 * Парсит маршрут.
 * Для кольцевого маршрута (A>B>C>A) возвращает массив названий остановок [A,B,C,A]
 * Для некольцевого маршрута (A-B-C-D) возвращает массив названий остановок [A,B,C,D,C,B,A]
 */
std::vector<std::string_view> ParseRoute(std::string_view route)
{
    if (route.find('>') != route.npos)
    {
        return Split(route, '>');
    }

    auto stops = Split(route, '-');
    std::vector<std::string_view> results(stops.begin(), stops.end());
    results.insert(results.end(), std::next(stops.rbegin()), stops.rend());

    return results;
}

CommandDescription ParseCommandDescription(std::string_view line)
{
    auto colon_pos = line.find(':');
    if (colon_pos == line.npos)
    {
        return {};
    }

    auto space_pos = line.find(' ');
    if (space_pos >= colon_pos)
    {
        return {};
    }

    auto not_space = line.find_first_not_of(' ', space_pos);
    if (not_space >= colon_pos)
    {
        return {};
    }

    return {std::string(line.substr(0, space_pos)),
            std::string(line.substr(not_space, colon_pos - not_space)),
            std::string(line.substr(colon_pos + 1))};
}

void InputReader::ParseLine(std::string_view line)
{
    auto command_description = ParseCommandDescription(line);
    if (command_description)
    {
        commands_.push_back(std::move(command_description));
    }
}

std::pair<std::string, std::string> InputReader::GetCoordinatesAndAbstand(std::string_view line) const
{
    auto temp1 = line.find_first_of(",");
    auto temp2 = line.find_first_of(",", temp1 + 1);
    return {std::string(line.substr(0, temp2)), std::string(line.substr(temp2 + 2))}; // поделили на координаты и на расстояния до остановок
}

std::vector<std::pair<std::string, std::string>> InputReader::GetAbstands(std::string_view line) const
{
    std::vector<std::pair<std::string, std::string>> result;
    size_t pointer1 = 0;
    size_t pointer2 = line.find(' ');

    while (pointer1 < line.size() && pointer2 != std::string::npos)
    {
        auto abstand = line.substr(pointer1, pointer2 - pointer1);

        pointer1 = pointer2 + 4;
        pointer2 = line.find(',', pointer1);

        auto train = line.substr(pointer1, pointer2 - pointer1);

        
        result.push_back({std::string(train), std::string(abstand)});

        if (pointer2 != std::string::npos)
        {
            pointer1 = pointer2 + 2;
            pointer2 = line.find(' ', pointer1);
        }
    }

    return result; // поделили на пары остановка-расстояние 
}

void InputReader::ApplyCommands([[maybe_unused]] TransportCatalogue::TransportCatalogue &catalogue) const
{
    std::unordered_map<std::string_view, std::vector<std::string_view>> trains;
    std::unordered_map<std::string_view, std::vector<std::pair<std::string, std::string>>> stops;

    // Реализуйте метод самостоятельно
    for (auto &i : commands_)
    {
        std::string_view command = Trim(i.command); // убрали пробелы из команды

        if (command == "Stop")
        {
            std::string_view id = Trim(i.id);
            auto description = Trim(i.description);
            std::pair<std::string, std::string> temp;
            std::vector<std::pair<std::string, std::string>> abstand;

            if (std::count(description.begin(), description.end(), ',') != 1)
            {
                temp = GetCoordinatesAndAbstand(description);
                abstand = GetAbstands(temp.second);
                stops[id] = abstand;
            }

            auto coordinates = ParseCoordinates(Trim(temp.first)); // координаты
            catalogue.addStation(id, coordinates);
        }
        else if (command == "Bus")
        {
            std::string_view id = Trim(i.id);
            auto route = ParseRoute(Trim(i.description)); // маршрут
            trains.insert({id, route});
        }
    }
    catalogue.addTrain(trains);
    catalogue.addAbstands(stops);
}