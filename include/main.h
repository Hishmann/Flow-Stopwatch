#pragma once

#include <chrono>
#include <thread>

#include <iomanip>
#include <sstream>
#include <iostream>
#include <string>

#include <SFML/Graphics.hpp>


using Clock = std::chrono::steady_clock;


class Timer
{
    private:
        bool focusing = false;
        bool onBreak = false;

        Clock::time_point focusStart;
        Clock::time_point breakStart;

        long long totalFocusSeconds = 0;
        long long breakBankSeconds = 0;
        long long breakCalcSeconds = 0;

    public:

        bool get_focusing() { return focusing; }
        bool get_onbreak() { return onBreak; }
        long long CurrentFocusTime();
        long long CurrentBreakBank();
        long long CurrentBreakCalc();
        void StartFocus();
        void StopFocus();
        void StartBreak();
        void StopBreak();
        void Reset();
};


std::string FormatTime(long long seconds)
{

    std::string sign = (seconds < 0) ? "-" : "";
    long long total_seconds = std::abs(seconds);

    long long h = total_seconds / 3600; 
    total_seconds %= 3600;

    long long m = total_seconds / 60;
    long long s = total_seconds % 60;

    std::stringstream ss;

    ss << sign; 
    ss << std::setfill('0') 
       << std::setw(2) << h << ":"
       << std::setw(2) << m << ":"
       << std::setw(2) << s;

    return ss.str();
}


void CenterText(sf::Text& text, float y, const sf::RenderWindow& window)
{
    sf::FloatRect bounds = text.getLocalBounds();

    text.setOrigin(
        bounds.left + bounds.width / 2.f,
        bounds.top + bounds.height / 2.f
    );

    text.setPosition(
        window.getSize().x / 2.f,
        y
    );
}


void Draw(sf::RenderWindow& window, sf::Font& font, Timer& timer)
{
    window.clear(sf::Color(28,28,30));

    //------------------------------------
    // Background panel
    //------------------------------------

    sf::RectangleShape panel({700.f,500.f});

    panel.setFillColor(sf::Color(40,40,45));

    panel.setOutlineThickness(3);

    panel.setOutlineColor(sf::Color(80,80,90));

    const sf::Vector2f panelSize(760.f, 540.f);

    panel.setSize(panelSize);

    panel.setPosition(
        (window.getSize().x - panelSize.x) / 2.f,
        (window.getSize().y - panelSize.y) / 2.f
    );

    window.draw(panel);

    float panelTop = panel.getPosition().y;

    //------------------------------------
    // Title
    //------------------------------------

    sf::Text title;
    title.setFont(font);

    title.setString("FOCUS TIMER");

    title.setCharacterSize(42);

    title.setStyle(sf::Text::Bold);

    title.setFillColor(sf::Color::Cyan);

    CenterText(title, panelTop + 55.f, window);

    window.draw(title);

    //------------------------------------
    // Status
    //------------------------------------

    sf::Text status;
    status.setFont(font);

    status.setCharacterSize(30);

    if(timer.get_focusing())
    {
        status.setString("FOCUSING");
        status.setFillColor(sf::Color::Green);
    }
    else if(timer.get_onbreak())
    {
        status.setString("ON BREAK");
        status.setFillColor(sf::Color::Yellow);
    }
    else
    {
        status.setString("IDLE");
        status.setFillColor(sf::Color(230,90,90));
    }

    CenterText(status, panelTop + 115.f, window);

    window.draw(status);

    //------------------------------------
    // Focus timer
    //------------------------------------

    sf::Text focus;
    focus.setFont(font);

    focus.setCharacterSize(72);

    focus.setStyle(sf::Text::Bold);

    focus.setFillColor(sf::Color::White);

    focus.setString(FormatTime(
        timer.CurrentFocusTime()));

    CenterText(focus, panelTop + 230.f, window);

    window.draw(focus);

    //------------------------------------
    // Break bank
    //------------------------------------

    sf::Text bank;
    bank.setFont(font);

    bank.setCharacterSize(42);

    bank.setFillColor(sf::Color(180,180,180));
    
    long long bank_time = timer.CurrentBreakBank();


    if (bank_time >= 0) {
        bank.setString("BREAK BANK: " + FormatTime(bank_time) + " [" + FormatTime(timer.CurrentBreakCalc()) + "]");
        bank.setFillColor(sf::Color::Green);
    } else {
        bank.setString("BREAK DEBT: " + FormatTime(bank_time) + " [" + FormatTime(timer.CurrentBreakCalc()) + "]");
        bank.setFillColor(sf::Color(220, 70, 70));
    }

    CenterText(bank, panelTop + 340.f, window);

    window.draw(bank);

    //------------------------------------
    // Controls
    //------------------------------------

    sf::Text controls;
    controls.setFont(font);

    controls.setCharacterSize(24);

    controls.setFillColor(sf::Color(160,160,160));

    controls.setString(
        "[F] Focus    "
        "[B] Break    "
        "[R] Reset    "
        "[Q] Quit");

    CenterText(controls, panelTop + 450.f, window);

    window.draw(controls);

    window.display();
}