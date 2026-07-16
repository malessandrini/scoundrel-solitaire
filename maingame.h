#ifndef MAINGAME_H
#define MAINGAME_H


#include "gameloop.h"
#include "assets.h"
#include "cards.h"


class MainGame: public GameLoop {
public:
    MainGame(sf::RenderWindow&, Assets&);
    void run() override;
    //void onResize(sf::Vector2u) override;
protected:
    sf::View view;
    Assets &assets;
    sf::Sprite spriteBg;
    void drawTable();
    // game state
    Deck deck;
    int health = 20;
    std::optional<Card> room[4], weapon, lastMonster;
    bool avoidedLast = false;
    //
    int ccc = 0;
};


#endif // MAINGAME_H
