#ifndef CARDS_H
#define CARDS_H

#include <cstdint>
#include <vector>
#include <random>


struct Card {
    char suit;
    uint8_t value;
    uint8_t sprite_index;
};


class Deck {
public:
    Deck();
    void shuffle();
    unsigned num_cards() const { return cards.size(); }
    const Card pick();
    void add(const Card&);
    void remove(const char suit, const uint8_t number);
    const std::vector<Card>& deck() const { return cards; }
protected:
    std::vector<Card> cards;
    static std::mt19937_64 gen;
};


#endif // CARDS_H
