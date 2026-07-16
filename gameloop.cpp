#include "gameloop.h"


std::mutex guiMutexEvent, guiMutexDraw;
std::condition_variable guiCv;
std::optional<sf::Event> guiEvent;
std::vector<std::function<void()>> drawFunctions;


GameLoop::GameLoop(sf::RenderWindow &w): window(w)
{}


void GameLoop::matchAspectRatio(sf::View &view, sf::Vector2u winSize) {
    const float aspect_ratio = (float)view.getSize().x / view.getSize().y, new_asp = (float)winSize.x / winSize.y;
    if (new_asp > aspect_ratio) view.setViewport(sf::FloatRect({(1 - aspect_ratio / new_asp) / 2, 0}, {aspect_ratio / new_asp, 1}));
    else view.setViewport(sf::FloatRect({0, (1 - new_asp /aspect_ratio) / 2}, {1, new_asp / aspect_ratio}));
}


sf::Event GameLoop::waitEvent() {
    // Block on wait condition, letting the main GUI thread continue.
    // When notified, return the event to the game function. Gui is
    // immediately unlocked at the end of the function, so if the caller
    // must do GUI-affecting stuff in response to this event, like changing
    // the data used by the draw functions or those functions themselves, it
    // must acquire the draw mutex.
    std::unique_lock lk(guiMutexEvent);  // lock the mutex
    if (!guiEvent) guiCv.wait(lk, [](){ return guiEvent; });  // mutex is unlocked while waiting
    // an event has been sent from GUI thread
    // here we own the mutex again
    auto event = guiEvent.value();
    guiEvent.reset();
    return event;
}
