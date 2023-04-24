#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

#include <date/date.h>
#include <nlohmann/json.hpp>

#include "internship.h"

using json = nlohmann::json;

namespace internship {

    struct OSInfo {
        std::string name;
        std::string version;
        int supportPeriod;
    };

    int daysBetweenDates(const std::string &date1, const std::string &date2) {
        std::tm tm1{}, tm2{};
        std::istringstream ss1(date1);
        std::istringstream ss2(date2);
        ss1 >> std::get_time(&tm1, "%Y-%m-%d");
        ss2 >> std::get_time(&tm2, "%Y-%m-%d");

        if (ss1.fail() || ss2.fail()) {
            return -1;
        }

        std::time_t time1 = std::mktime(&tm1);
        std::time_t time2 = std::mktime(&tm2);
        return (time2 - time1) / (60 * 60 * 24);
    }

    void fetchOS(const std::string& jsonFileName, int elementsCount) {
        std::ifstream f(jsonFileName);
        json data = json::parse(f);

        std::vector<OSInfo> osList;

        for (const auto &os : data) {
            auto os_it = os.find("os");
            if (os_it == os.end()) {
                continue;
            }
            if (!os_it->is_boolean() || !*os_it) {
                continue;
            }

            auto name_it = os.find("name");
            if (name_it == os.end()) {
                continue;
            }

            std::string name = *name_it;

            auto versions_it = os.find("versions");
            if (versions_it == os.end()) {
                continue;
            }

            for (const auto &version : *versions_it) {
                auto releaseDate_it = version.find("releaseDate");
                auto eol_it = version.find("eol");
                auto cycle_it = version.find("cycle");

                if (releaseDate_it == version.end() || eol_it == version.end() || cycle_it == version.end()) {
                    continue;
                }

                if (!releaseDate_it->is_string() || !eol_it->is_string() || !cycle_it->is_string()) {
                    continue;
                }

                std::string releaseDate = *releaseDate_it;
                std::string eol = *eol_it;
                std::string cycle = *cycle_it;
                int supportPeriod = daysBetweenDates(releaseDate, eol);

                if (supportPeriod == -1) {
                    continue;
                }

                osList.push_back({name, cycle, supportPeriod});
            }
        }

        std::partial_sort(osList.begin(), osList.begin() + elementsCount, osList.end(),
                      [](const OSInfo &a, const OSInfo &b) {
                          return a.supportPeriod > b.supportPeriod;
                      });

        for (int i = 0; i < elementsCount; ++i) {
            std::cout << osList[i].name << " " << osList[i].version << " " << osList[i].supportPeriod << std::endl;
        }
    }

    // do not remove this function
    void solution(const std::string& jsonFileName, int elementsCount) {
        fetchOS(jsonFileName, elementsCount);
    }
}