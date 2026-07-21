#ifndef MAINGAME_H
#define MAINGAME_H


#include "assets.h"
#include "cards.h"
#include <SFML/Graphics.hpp>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <functional>


extern std::mutex guiMutexEvent, guiMutexDraw;
extern std::condition_variable guiCv;
extern std::optional<sf::Event> guiEvent;
extern std::vector<std::function<void()>> drawFunctions;  // to be managed by game loop


class MainGame {
public:
    MainGame(sf::RenderWindow&, Assets&);
    void run();
    bool isDone = false, mustQuit = false;  // to be checked by main thread
protected:
    // geometry information
    const sf::Vector2f posDeck{62, 60}, posRoom[4]{{318, 60}, {484, 60}, {650, 60}, {816, 60}},
        szCard{133, 200}, posAvoid{525, 300}, szAvoid{200, 50}, posWeapon{816, 430}, offKilledMonster{0, 40};
    const sf::FloatRect rectHealt{{64, 472}, {128, 128}};
    // geometry and data for dialog box
    const sf::Vector2f posDlg{250, 150}, szDlg{500, 240}, szBtn12{170, 30}, szBtnCancel{80, 30},
        posBtn1{260, 350}, posBtn2{570, 350}, posCancel{660, 160};
    std::string dlgText, dlgBtn1, dlgBtn2;
    bool dlgCancel;
    // graphics objects
    sf::RenderWindow &window;
    sf::View view;
    Assets &assets;
    sf::Sprite spriteBg, spriteBack;
    sf::Text txtDeck, txtAvoid, txtHealth, txtDialog, txtBtn1, txtBtn2, txtCancel;
    sf::RectangleShape rectAvoid{szAvoid}, rectDlg{szDlg}, rectBtn1{szBtn12}, rectBtn2{szBtn12}, rectCancel{szBtnCancel};
    //
    sf::Event waitEvent();  // automatically manages resize by calling onResize()
    enum class UserInput { Card, Avoid, Btn1, Btn2, Cancel, Esc };
    std::pair<UserInput,int> getInput();
    void onResize(sf::Vector2u);
    void drawTable();
    void drawAvoid();
    void drawDialog();
    void draw4Backs();
    static void matchAspectRatio(sf::View &view, sf::Vector2u winSize);
    void center(sf::Text&, const sf::FloatRect&, sf::Vector2f off = {0, 0}) const;
    void center(sf::Text&, const sf::Shape&, sf::Vector2f off = {0, 0}) const;
    int currentCards() const;
    UserInput showDialog(std::string const &text, std::string const &btn1, std::string const &btn2, bool cancel);
    int finalScore();
    // game state
    Deck deck;
    int health = 20;
    std::optional<Card> room[4], weapon, lastMonster;
    bool avoidedLast = false;
};


#endif // MAINGAME_H
