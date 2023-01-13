#ifndef ORGANISM_H
#define ORGANISM_H

#include <iostream>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

template <typename species_t, bool can_eat_meat, bool can_eat_plants>
requires std::equality_comparable<species_t>
class Organism {
   private:
    species_t species;
    uint64_t vitality;

   public:
    constexpr Organism(species_t const &species, uint64_t vitality)
        : species(species), vitality(vitality) {}

    constexpr const species_t &get_species() const { return species; }

    constexpr uint64_t get_vitality() const { return vitality; }

    constexpr bool is_dead() const { return vitality == 0; }
};

template <typename species_t>
using Carnivore = Organism<species_t, true, false>;

template <typename species_t>
using Omnivore = Organism<species_t, true, true>;

template <typename species_t>
using Herbivore = Organism<species_t, false, true>;

template <typename species_t>
using Plant = Organism<species_t, false, false>;

namespace {

// 4
template <typename species_t, bool sp_eats_m, bool sp_eats_p>
constexpr std::tuple<Organism<species_t, sp_eats_m, sp_eats_p>,
                     Organism<species_t, sp_eats_m, sp_eats_p>,
                     std::optional<Organism<species_t, sp_eats_m, sp_eats_p>>>
mating(Organism<species_t, sp_eats_m, sp_eats_p> organism1,
       Organism<species_t, sp_eats_m, sp_eats_p> organism2) {
    uint64_t vitality =
        organism1.get_vitality() / 2 + organism2.get_vitality() / 2 +
        (organism1.get_vitality() % 2 + organism2.get_vitality() % 2) / 2;
    Organism<species_t, sp_eats_m, sp_eats_p> child(organism1.get_species(),
                                                    vitality);
    std::optional<Organism<species_t, sp_eats_m, sp_eats_p>> opt;
    opt = child;
    std::tuple<Organism<species_t, sp_eats_m, sp_eats_p>,
               Organism<species_t, sp_eats_m, sp_eats_p>,
               std::optional<Organism<species_t, sp_eats_m, sp_eats_p>>>
        parentsAndChildren(organism1, organism2, opt);
    return parentsAndChildren;
}

// 6
template <typename species_t, bool sp1_eats_p, bool sp2_eats_p>
constexpr std::tuple<Organism<species_t, true, sp1_eats_p>,
                     Organism<species_t, true, sp2_eats_p>,
                     std::optional<Organism<species_t, true, sp1_eats_p>>>
fight(Organism<species_t, true, sp1_eats_p> organism1,
      Organism<species_t, true, sp2_eats_p> organism2) {
    uint64_t vitality1 = 0, vitality2 = 0;
    if (organism1.get_vitality() > organism2.get_vitality()) {
        vitality1 = organism1.get_vitality() + organism2.get_vitality() / 2;
        vitality2 = 0;
    } else if (organism1.get_vitality() < organism2.get_vitality()) {
        vitality2 = organism2.get_vitality() + organism1.get_vitality() / 2;
        vitality1 = 0;
    } else {
        vitality1 = 0;
        vitality2 = 0;
    }
    Organism<species_t, true, sp1_eats_p> newOrganism1(organism1.get_species(),
                                                       vitality1);
    Organism<species_t, true, sp2_eats_p> newOrganism2(organism2.get_species(),
                                                       vitality2);
    std::optional<Organism<species_t, true, sp1_eats_p>> opt;
    std::tuple<Organism<species_t, true, sp1_eats_p>,
               Organism<species_t, true, sp2_eats_p>,
               std::optional<Organism<species_t, true, sp1_eats_p>>>
        result(newOrganism1, newOrganism2, opt);
    return result;
}

// 7
template <typename species_t, bool sp_eats_m>
constexpr std::tuple<Organism<species_t, sp_eats_m, true>,
                     Organism<species_t, false, false>,
                     std::optional<Organism<species_t, sp_eats_m, true>>>
eatingPlant(Organism<species_t, sp_eats_m, true> organism1,
            Organism<species_t, false, false> organism2) {
    uint64_t vitality = organism1.get_vitality() + organism2.get_vitality();
    Organism<species_t, sp_eats_m, true> newOrganism1(organism1.get_species(),
                                                      vitality);
    Organism<species_t, false, false> newOrganism2(organism2.get_species(), 0);
    std::optional<Organism<species_t, sp_eats_m, true>> opt;
    std::tuple<Organism<species_t, sp_eats_m, true>,
               Organism<species_t, false, false>,
               std::optional<Organism<species_t, sp_eats_m, true>>>
        result(newOrganism1, newOrganism2, opt);
    return result;
}

// 7
template <typename species_t, bool sp_eats_p>
constexpr std::tuple<Organism<species_t, true, sp_eats_p>,
                     Organism<species_t, false, true>,
                     std::optional<Organism<species_t, true, sp_eats_p>>>
eatingAnimal(Organism<species_t, true, sp_eats_p> organism1,
             Organism<species_t, false, true> organism2) {
    uint64_t vitality1 = 0, vitality2 = 0;
    if (organism1.get_vitality() <= organism2.get_vitality()) {
        vitality1 = organism1.get_vitality();
        vitality2 = organism2.get_vitality();
    } else {
        vitality1 = organism1.get_vitality() + organism2.get_vitality() / 2;
        vitality2 = 0;
    }
    Organism<species_t, true, sp_eats_p> newOrganism1(organism1.get_species(),
                                                      vitality1);
    Organism<species_t, false, true> newOrganism2(organism2.get_species(),
                                                   vitality2);
    std::optional<Organism<species_t, true, sp_eats_p>> opt;
    std::tuple<Organism<species_t, true, sp_eats_p>,
               Organism<species_t, false, true>,
               std::optional<Organism<species_t, true, sp_eats_p>>>
        result(newOrganism1, newOrganism2, opt);
    return result;
}

template <typename species_t, bool sp1_eats_m, bool sp1_eats_p, bool sp2_eats_m, bool sp2_eats_p, typename ... Args>
constexpr Organism<species_t, sp1_eats_m, sp1_eats_p>
encounter_series_fold(Organism<species_t, sp1_eats_m, sp1_eats_p> firstOrganism,
                      Organism<species_t, sp2_eats_m, sp2_eats_p> nextOrganism, 
                      Args ... args) {
    auto result = get<0>(encounter(firstOrganism, nextOrganism));
    if constexpr (sizeof...(args) > 0) {
        return encounter_series_fold(result, args...);
    }

    return result;
}
}

// 6 (o+o, c+c)
template <typename species_t, bool sp_eats_p>
constexpr std::tuple<Organism<species_t, true, sp_eats_p>,
                     Organism<species_t, true, sp_eats_p>,
                     std::optional<Organism<species_t, true, sp_eats_p>>>
encounter(Organism<species_t, true, sp_eats_p> organism1,
          Organism<species_t, true, sp_eats_p> organism2) {
    if (organism1.is_dead() || organism2.is_dead()) {  // 3
        std::optional<Organism<species_t, true, sp_eats_p>> opt;
        std::tuple<Organism<species_t, true, sp_eats_p>,
                   Organism<species_t, true, sp_eats_p>,
                   std::optional<Organism<species_t, true, sp_eats_p>>>
            result(organism1, organism2, opt);
        return result;
    } else if (organism1.get_species() == organism2.get_species()) {
        return mating(organism1, organism2);
    } else {
        return fight(organism1, organism2);
    }
}

// 5 (h+h)
template <typename species_t>
constexpr std::tuple<Organism<species_t, false, true>,
                     Organism<species_t, false, true>,
                     std::optional<Organism<species_t, false, true>>>
encounter(Organism<species_t, false, true> organism1,
          Organism<species_t, false, true> organism2) {
    if (organism1.is_dead() || organism2.is_dead()) {  // 3
        std::optional<Organism<species_t, false, true>> opt;
        std::tuple<Organism<species_t, false, true>,
                   Organism<species_t, false, true>,
                   std::optional<Organism<species_t, false, true>>>
            result(organism1, organism2, opt);
        return result;
    } else if (organism1.get_species() == organism2.get_species()) {
        return mating(organism1, organism2);
    } else {
        std::optional<Organism<species_t, false, true>> opt;
        std::tuple<Organism<species_t, false, true>,
                   Organism<species_t, false, true>,
                   std::optional<Organism<species_t, false, true>>>
            result(organism1, organism2, opt);
        return result;
    }
}

// 6 (c+o, o+c)
template <typename species_t, bool sp_eats_p>
constexpr std::tuple<Organism<species_t, true, sp_eats_p>,
                     Organism<species_t, true, !sp_eats_p>,
                     std::optional<Organism<species_t, true, sp_eats_p>>>
encounter(Organism<species_t, true, sp_eats_p> organism1,
          Organism<species_t, true, !sp_eats_p> organism2) {
    if (organism1.is_dead() || organism2.is_dead()) {  // 3
        std::optional<Organism<species_t, true, sp_eats_p>> opt;
        std::tuple<Organism<species_t, true, sp_eats_p>,
                   Organism<species_t, true, !sp_eats_p>,
                   std::optional<Organism<species_t, true, sp_eats_p>>>
            result(organism1, organism2, opt);
        return result;
    } else {
        return fight(organism1, organism2);
    }
}

// 7 (o+p, h+p)
template <typename species_t, bool sp_eats_m>
constexpr std::tuple<Organism<species_t, sp_eats_m, true>,
                     Organism<species_t, false, false>,
                     std::optional<Organism<species_t, sp_eats_m, true>>>
encounter(Organism<species_t, sp_eats_m, true> organism1,
          Organism<species_t, false, false> organism2) {
    if (organism1.is_dead() || organism2.is_dead()) {  // 3
        std::optional<Organism<species_t, sp_eats_m, true>> opt;
        std::tuple<Organism<species_t, sp_eats_m, true>,
                   Organism<species_t, false, false>,
                   std::optional<Organism<species_t, sp_eats_m, true>>>
            result(organism1, organism2, opt);
        return result;
    } else {
        return eatingPlant(organism1, organism2);
    }
}

// 7 (p+h, p+o)
template <typename species_t, bool sp_eats_m>
constexpr std::tuple<Organism<species_t, false, false>,
                     Organism<species_t, sp_eats_m, true>,
                     std::optional<Organism<species_t, false, false>>>
encounter(Organism<species_t, false, false> organism1,
          Organism<species_t, sp_eats_m, true> organism2) {
    auto reverse = encounter(organism2, organism1);
    std::optional<Organism<species_t, false, false>> opt;
    std::tuple<Organism<species_t, false, false>,
               Organism<species_t, sp_eats_m, true>,
               std::optional<Organism<species_t, false, false>>>
        result(get<1>(reverse), get<0>(reverse), opt);
    return result;
}

// 8 (c+h, o+h)
template <typename species_t, bool sp_eats_p>
constexpr std::tuple<Organism<species_t, true, sp_eats_p>,
                     Organism<species_t, false, true>,
                     std::optional<Organism<species_t, true, sp_eats_p>>>
encounter(Organism<species_t, true, sp_eats_p> organism1,
          Organism<species_t, false, true> organism2) {
    if (organism1.is_dead() || organism2.is_dead()) {  // 3
        std::optional<Organism<species_t, true, sp_eats_p>> opt;
        std::tuple<Organism<species_t, true, sp_eats_p>,
                   Organism<species_t, false, true>,
                   std::optional<Organism<species_t, true, sp_eats_p>>>
            result(organism1, organism2, opt);
        return result;
    } else {
        return eatingAnimal(organism1, organism2);
    }
}

// 8 (h+c, h+o)
template <typename species_t, bool sp_eats_p>
constexpr std::tuple<Organism<species_t, false, true>,
                     Organism<species_t, true, sp_eats_p>,
                     std::optional<Organism<species_t, false, true>>>
encounter(Organism<species_t, false, true> organism1,
          Organism<species_t, true, sp_eats_p> organism2) {
    auto reverse = encounter(organism2, organism1);
    std::optional<Organism<species_t, false, true>> opt;
    std::tuple<Organism<species_t, false, true>,
               Organism<species_t, true, sp_eats_p>,
               std::optional<Organism<species_t, false, true>>>
        result(get<1>(reverse), get<0>(reverse), opt);
    return result;
}
// (c + p)
template <typename species_t>
constexpr std::tuple<Organism<species_t, true, false>,
                     Organism<species_t, false, false>,
                     std::optional<Organism<species_t, true, false>>>
encounter(Organism<species_t, true, false> organism1,
          Organism<species_t, false, false> organism2) {
    std::optional<Organism<species_t, true, false>> opt;
    std::tuple<Organism<species_t, true, false>,
               Organism<species_t, false, false>,
               std::optional<Organism<species_t, true, false>>>
        result(organism1, organism2, opt);
    return result;
}
// (p + c)
template <typename species_t>
constexpr std::tuple<Organism<species_t, false, false>,
                     Organism<species_t, true, false>,
                     std::optional<Organism<species_t, false, false>>>
encounter(Organism<species_t, false, false> organism1,
          Organism<species_t, true, false> organism2) {
    std::optional<Organism<species_t, false, false>> opt;
    std::tuple<Organism<species_t, false, false>,
               Organism<species_t, true, false>,
               std::optional<Organism<species_t, false, false>>>
        result(organism1, organism2, opt);
    return result;
}

template <typename species_t, bool sp1_eats_m, bool sp1_eats_p, typename ... Args>
constexpr Organism<species_t, sp1_eats_m, sp1_eats_p>
encounter_series(Organism<species_t, sp1_eats_m, sp1_eats_p> organism1, Args ... args) {
    if constexpr (sizeof...(args) > 0) {
        return encounter_series_fold(organism1, args...);
    }
    return organism1;
}


#endif