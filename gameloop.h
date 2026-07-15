#ifndef GAMELOOP_H
#define GAMELOOP_H

#include <SFML/Graphics.hpp>


class GameLoop {
public:
    GameLoop(sf::RenderWindow&);
    virtual ~GameLoop() {}
    virtual int run() = 0;
protected:
    sf::RenderWindow &window;
    std::optional<sf::Event> pollEvent();  // manages window-close and resize events
    static void matchAspectRatio(sf::View &view, sf::Vector2u winSize);
    virtual void onResize(sf::Vector2u) {};
};


#endif // GAMELOOP_H
