#include "gameloop.h"


GameLoop::GameLoop(sf::RenderWindow &w): window(w)
{}


std::optional<sf::Event> GameLoop::pollEvent() {
    auto event = window.pollEvent();
    if (event && event->is<sf::Event::Closed>()) {
        window.close();
        return std::nullopt;
    }
    return event;
}


void GameLoop::matchAspectRatio(sf::View &view, sf::Vector2u winSize) {
    const float aspect_ratio = (float)view.getSize().x / view.getSize().y, new_asp = (float)winSize.x / winSize.y;
    if (new_asp > aspect_ratio) view.setViewport(sf::FloatRect({(1 - aspect_ratio / new_asp) / 2, 0}, {aspect_ratio / new_asp, 1}));
    else view.setViewport(sf::FloatRect({0, (1 - new_asp /aspect_ratio) / 2}, {1, new_asp / aspect_ratio}));
}
