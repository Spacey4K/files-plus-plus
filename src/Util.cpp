#include "../headers/util.hpp"

#include <chrono>
#include <cmath>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

int Util::handleFileError() {
    std::cerr << "Error while loading font" << '\n';
    return EXIT_FAILURE;
}

std::string Util::countItems(fs::path dir, std::string type) {
    using fp = bool (*)(const fs::path &);
    int count;
    if (type == "folders") {
        count = std::count_if(
            fs::directory_iterator(dir),
            fs::directory_iterator{},
            fp(fs::is_directory));
    } else if (type == "files") {
        count = std::count_if(
            fs::directory_iterator(dir),
            fs::directory_iterator{},
            fp(fs::is_regular_file));
    } else
        count = std::distance(fs::directory_iterator(dir), fs::directory_iterator{});
    return std::to_string(count) + ' ' + type;
};

uintmax_t Util::getPathSize(fs::path dir) {
    if (!fs::is_directory(dir)) return fs::file_size(dir);
    uintmax_t size = 0;
    for (fs::directory_entry const &entry : fs::directory_iterator(dir)) {
        if (entry.is_directory()) {
            size += getPathSize(entry.path());
        } else {
            size += entry.file_size();
        }
    }
    return size;
}

std::string Util::prettyBytes(uintmax_t bytes, int decimalPlaces) {
    if (bytes == 0) return "0 B";

    std::string prefixes = "BKMGT";
    double decimals = pow(10, decimalPlaces);

    double mantissa = bytes;
    int i;
    for (i = 0; mantissa >= 1024.; ++i) {
        mantissa /= 1024.;
    }
    mantissa = std::round(mantissa * decimals) / decimals;
    std::string d = std::to_string(mantissa);
    std::string size = d.substr(0, d.find('.') + decimalPlaces + 1) + ' ' + prefixes[i];
    size = i == 0 ? size : size + 'B';
    return size;
}

template <class TP>
std::string Util::formatTime(TP tp) {
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now() + system_clock::now());
    std::time_t tt = system_clock::to_time_t(sctp);
    std::stringstream buffer;
    buffer << std::put_time(std::localtime(&tt), "%d.%m.%Y %H:%M");
    return buffer.str();
}

template std::string Util::formatTime<fs::file_time_type>(fs::file_time_type); // ! important