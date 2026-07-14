#ifndef MAINGAME_H
#define MAINGAME_H


#include "gameloop.h"


class MainGame: public GameLoop {
public:
    MainGame(sf::RenderWindow&);
    int run() override;
protected:
    sf::View view;
};



#endif // MAINGAME_H
