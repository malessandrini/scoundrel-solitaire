#include <SFML/Graphics.hpp>
#include "maingame.h"
#include "assets.h"
#include <thread>


int main(int, char**) {
    sf::RenderWindow window(sf::VideoMode({1024, 768}), "Scoundrel Solitaire");

    Assets assets;

    while (window.isOpen()) {
        MainGame mainGameLoop(window, assets);  // create new game every time it's restarted

        std::thread gameThread(&MainGame::run, &mainGameLoop);
        //mainGameLoop.onResize(window.getSize());  // may be needed the first time

        while (window.isOpen() && !mainGameLoop.isDone) {
            // process events
            auto event = window.pollEvent();
            if (event) {
                if (event->is<sf::Event::Closed>())
                    window.close();
                if (event->is<sf::Event::Closed>() || event->is<sf::Event::Resized>() || event->is<sf::Event::KeyPressed>() || event->is<sf::Event::MouseButtonPressed>()) {
                    // send event to thread, that's likely blocked on wait
                    std::lock_guard lk(guiMutexEvent);
                    guiEvent = event;
                }
                guiCv.notify_one();
            }
            // draw things
            {
                std::lock_guard lk(guiMutexDraw);
                for (auto &f: drawFunctions) f();
                window.display();
            }
        }

        // window has been closed or thread terminated
        gameThread.join();
        if (mainGameLoop.mustQuit) break;
    }
}
