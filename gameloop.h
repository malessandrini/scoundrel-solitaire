#ifndef GAMELOOP_H
#define GAMELOOP_H

#include <SFML/Graphics.hpp>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <functional>
#include <atomic>

extern std::mutex guiMutexEvent, guiMutexDraw;
extern std::condition_variable guiCv;
extern std::optional<sf::Event> guiEvent;
extern std::vector<std::function<void()>> drawFunctions;  // to be managed by game loop


// abstract class to represent the game logic, executed in another thread
//  so it can have a sequential operation instead of event-driven state machine.


class GameLoop {
public:
    GameLoop(sf::RenderWindow&);
    virtual ~GameLoop() {}
    virtual void run() = 0;  // executed in another thread
    bool isDone = false, mustQuit = false;
protected:
    sf::RenderWindow &window;
    static void matchAspectRatio(sf::View &view, sf::Vector2u winSize);
    sf::Event waitEvent();  // automatically manages resize by calling onResize()
    virtual void onResize(sf::Vector2u) {};
};


#endif // GAMELOOP_H
