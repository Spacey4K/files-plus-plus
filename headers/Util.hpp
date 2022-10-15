#include <filesystem>
#include <string>
namespace fs = std::filesystem;

namespace Util {

std::string prettyBytes(uintmax_t bytes, int decimalPlaces);
uintmax_t getPathSize(fs::path dir);
template <class TP>
std::string formatTime(TP tp);
std::string countItems(fs::path dir, std::string type);
int handleFileError();

} // namespace Util
