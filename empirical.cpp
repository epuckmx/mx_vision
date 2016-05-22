#include "empirical.hpp"
#include <string>
#include <cstdlib>

namespace Empirical {

int post(std::string message) {
    return system(("echo " + message).c_str());
}

} // namespace Empirical
