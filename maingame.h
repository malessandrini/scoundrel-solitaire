#ifndef MAINGAME_H
#define MAINGAME_H


#include "gameloop.h"
#include "assets.h"
#include "cards.h"


class MainGame: public GameLoop {
public:
    MainGame(sf::RenderWindow&, Assets&);
    void run() override;
protected:
    sf::View view;
    Assets &assets;
    sf::Sprite spriteBg, spriteBack;
    void onResize(sf::Vector2u) override;
    void drawTable();
    // game state
    Deck deck;
    int health = 20;
    std::optional<Card> room[4], weapon, lastMonster;
    bool avoidedLast = false;
    // geometry information
    sf::Vector2f posDeck{62, 60}, posRoom[4]{{318, 60}, {484, 60}, {650, 60}, {816, 60}},
        szCars{133, 200};
    //
    int ccc = 0;
};


#endif // MAINGAME_H
