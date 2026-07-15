#include "cards.h"
#include <ctime>
#include <algorithm>
#include <cassert>


std::mt19937_64 Deck::gen(time(nullptr));


Deck::Deck() {
    cards.reserve(52);
    uint8_t id = 0;
    for (char suit: {'s', 'h', 'd', 'c'})
        for (uint8_t n = 1; n <= 13; ++n)
            cards.push_back({suit, n, id++});
    // in this game, aces have value 14
    for (auto &c: cards) if (c.value == 1) c.value = 14;
}


void Deck::shuffle() {
    for (unsigned i = 0; i < cards.size(); ++i) {
        std::uniform_int_distribution<> dist(i, cards.size() - 1);
        const unsigned j = dist(gen);
        std::swap(cards[i], cards[j]);
    }
}


const Card Deck::pick() {
    assert(cards.size());
    auto c = cards.front();
    cards.erase(cards.begin());
    return c;
}


void Deck::add(const Card &c) {
    cards.push_back(c);
}


void Deck::remove(const char suit, const uint8_t number) {
    auto i = std::find_if(cards.begin(), cards.end(), [suit, number](const Card &card){ return card.suit == suit && card.value == number; });
    if (i != cards.end()) cards.erase(i);
}
