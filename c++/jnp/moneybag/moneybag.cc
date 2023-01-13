#include "moneybag.h"

std::ostream& operator<<(std::ostream& os, const Moneybag& bag) {
    os << "("<< bag.livre;
    if (bag.livre == 1) {
        os << " livr, ";
    }
    else {
        os << " livres, ";
    }
    os << bag.solidus;
    if (bag.solidus == 1) {
        os << " solidus, ";
    }
    else {
        os << " soliduses, ";
    }
    os << bag.denier;
    if (bag.denier == 1) {
        os << " denier)";
    }
    else {
        os << " deniers)";
    }
    return os;
}

Value::operator std::string() const {
    __uint128_t val = denier;
    std::string output = "";
    if (val == 0) {
        output += '0';
        return output;
    }
    else {
        while (val > 0) {
            output += '0' + val % 10;
            val /= 10;
        }
    }

    return std::string(output.rbegin(), output.rend());
}