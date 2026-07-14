#ifndef GAMELOOP_H
#define GAMELOOP_H

#include <SFML/Graphics.hpp>


class GameLoop {
public:
    GameLoop(sf::RenderWindow&);
    virtual ~GameLoop() {}
    //virtual void free() {};
    virtual int run() = 0;
protected:
    sf::RenderWindow &window;
    std::optional<sf::Event> pollEvent();  // manages window-close event
    static void matchAspectRatio(sf::View &view, sf::Vector2u winSize);
};


#endif // GAMELOOP_H
