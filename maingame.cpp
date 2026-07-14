#include "maingame.h"


MainGame::MainGame(sf::RenderWindow &w):
    GameLoop(w), view(window.getDefaultView())
{}


int MainGame::run() {
    matchAspectRatio(view, window.getSize());

    // remove pending events but manage resize events if pending
    // (note: window.getSize() is not updated it resize event not processed!)
    while(const auto event = pollEvent())
        if (const auto *e = event->getIf<sf::Event::Resized>()) {
            matchAspectRatio(view, e->size);
        }

    while (window.isOpen()) {
        // process events
        while (const auto event = pollEvent()) {
            if (const auto *e = event->getIf<sf::Event::Resized>()) {
                matchAspectRatio(view, e->size);
            }
            else if (const auto *e = event->getIf<sf::Event::KeyPressed>()) {
                if (e->scancode == sf::Keyboard::Scancode::Escape)
                    return -1;  // terminate program
            }
        }

        // draw
        window.clear();
        window.setView(view);
        window.display();
    }
    return -1;  // if here, window has been closed
}
