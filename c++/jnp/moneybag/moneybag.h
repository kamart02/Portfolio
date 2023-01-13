#ifndef MONEYBAG_H
#define MONEYBAG_H

#include <cstdio>
#include <iostream>
#include <stdexcept>

class Moneybag {

    public:
        typedef size_t coin_number_t;

    private:
        coin_number_t livre;
        coin_number_t solidus;
        coin_number_t denier;
        const static coin_number_t MAX_SIZE = (size_t) - 1;

    public :
        constexpr Moneybag(coin_number_t livre, coin_number_t solidus, coin_number_t denier):
            livre(livre),
            solidus(solidus),
            denier(denier) {}

        //copy creator
        constexpr Moneybag(const Moneybag & other):
            livre(other.livre),
            solidus(other.solidus),
            denier(other.denier) {}

        //copy assignment
        constexpr Moneybag & operator=(const Moneybag & other) {
            if (this == &other) {
                return *this;
            }
            livre = other.livre;
            solidus = other.solidus;
            denier = other.denier;
            return *this;
        }

        constexpr coin_number_t livre_number() const {
            return livre;
        }
        constexpr coin_number_t solidus_number() const {
            return solidus;
        }
        constexpr coin_number_t denier_number() const {
            return denier;
        }

        constexpr Moneybag & operator+=(const Moneybag & other) {
            if (MAX_SIZE - livre < other.livre) {
                throw(std::out_of_range("Suma liwrow za duza"));
            }
            if (MAX_SIZE - solidus < other.solidus) {
                throw(std::out_of_range("Suma solidow za duza"));
            }
            if (MAX_SIZE - denier < other.denier) {
                throw(std::out_of_range("Suma denarow za duza"));
            }
            livre += other.livre;
            solidus += other.solidus;
            denier += other.denier;
            return *this;
        }

        constexpr const Moneybag operator+(const Moneybag & other) const {
            Moneybag ret = *this;
            ret += other;
            return ret;
        }

        constexpr Moneybag & operator-=(const Moneybag & other) {
            if (livre < other.livre) {
                throw(std::out_of_range("Odjemna livrow jest mniejsza niz odjemnik livrow"));
            }
            if (solidus < other.solidus) {
                throw(std::out_of_range("Odjemna solidow jest mniejsza niz odjemnik solidow"));
            }
            if (denier < other.denier) {
                throw(std::out_of_range("Odjemna denarow jest mniejsza niz odjemnik denarow"));
            }
            livre -= other.livre;
            solidus -= other.solidus;
            denier -= other.denier;
            return *this;
        }

        constexpr const Moneybag operator-(const Moneybag & other) const {
            Moneybag ret = *this;
            ret -= other;
            return ret;
        }


        constexpr Moneybag & operator*=(const coin_number_t x) {
            if (x != 0) {
                if (MAX_SIZE / x < livre) {
                    throw(std::out_of_range("Iloczyn lirow za duzy"));
                }
                if (MAX_SIZE / x < solidus) {
                    throw(std::out_of_range("Iloczyn solidow za duzy"));
                }
                if (MAX_SIZE / x < denier) {
                    throw(std::out_of_range("Iloczyn denarow za duzy"));
                }
            }
            livre *= x;
            solidus *= x;
            denier *= x;
            return *this;
        }

        constexpr const Moneybag operator*(const coin_number_t x) const {
            Moneybag ret = *this;
            ret *= x;
            return ret;
        }

        constexpr const friend Moneybag operator*(const coin_number_t x, const Moneybag bag) {
            return bag * x;
        }

        constexpr auto operator<=>(const Moneybag & bag) {
            if (this->denier == bag.denier && this->livre == bag.livre && this->solidus == bag.solidus) {
                return std::partial_ordering::equivalent;
            }
            if (this->denier <= bag.denier && this->livre <= bag.livre && this->solidus <= bag.solidus) {
                return std::partial_ordering::less;
            }
            if (this->denier >= bag.denier && this->livre >= bag.livre && this->solidus >= bag.solidus) {
                return std::partial_ordering::greater;
            }

            return std::partial_ordering::unordered;
        }

        constexpr friend bool operator==(const Moneybag & bag1, const Moneybag & bag2) {
            return bag1.denier == bag2.denier && bag1.livre == bag2.livre && bag1.solidus == bag2.solidus;
        }

        constexpr explicit operator bool() const {
            return denier != 0 || solidus != 0 || livre != 0;
        }

        friend std::ostream& operator<<(std::ostream& os, const Moneybag& bag);
};

class Value {
private:
    __uint128_t denier;
    static constexpr uint64_t denier_in_solidus = 12;
    static constexpr uint64_t denier_in_livre = 240;
public:
    constexpr Value(size_t num):
        denier(num) {}

    constexpr Value():
        denier(0) {}

    constexpr Value(const Moneybag & bag):
        denier(bag.denier_number() + bag.solidus_number() * denier_in_solidus + bag.livre_number() * denier_in_livre) {}


    constexpr Value(const Value & v):
        denier(v.denier) {}

    constexpr Value & operator=(const Value & v) {
        if (this == &v) {
            return *this;
        }
        denier = v.denier;
        return *this;
    }

    explicit operator std::string() const;

    constexpr friend bool operator==(const Value & v1, const Value & v2) {
        return v1.denier == v2.denier;
    }

    constexpr friend bool operator==(size_t v1, const Value & v2) {
        return v1 == v2.denier;
    }

    constexpr friend bool operator==(const Value & v1, size_t v2) {
        return v1.denier == v2;
    }

    constexpr auto operator<=> (const Value & v) const {
        if (this->denier == v.denier) {
            return std::strong_ordering::equal;
        }
        if (this->denier < v.denier) {
            return std::strong_ordering::less;
        }
        return std::strong_ordering::greater;
    }

    constexpr auto operator<=> (size_t v) const {
        if (this->denier == v) {
            return std::strong_ordering::equal;
        }
        if (this->denier < v) {
            return std::strong_ordering::less;
        }
        return std::strong_ordering::greater;
    }
};

constinit const Moneybag Denier(0, 0, 1);
constinit const Moneybag Solidus(0, 1, 0);
constinit const Moneybag Livre(1, 0, 0);

#endif