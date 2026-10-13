#pragma once

#include <chrono>
#include <thread>

#include <iomanip>
#include <sstream>
#include <iostream>
#include <string>
#include <cmath>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>


using Clock = std::chrono::steady_clock;


class Timer
{
    private:
        bool work_mode = false;
        bool break_mode = false;

        Clock::time_point work_start;
        Clock::time_point break_start;

        // Total time spent working/breaking
        long long work_bank = 0;
        long long prev_work_bank = 0;
        long long break_bank = 0;
        long long prev_break_bank = 0;

        // Time set for the work session
        long long work_set_time = 0;
        long long work_left = 0;

        // Break ratio (ex. 5 means 1 minute of break for every 5 minutes of work)
        long long break_ratio = 5;
        long long break_allowance = 0;

    public:

        bool get_work_mode() { return work_mode; }
        bool get_break_mode() { return break_mode; }

        long long get_work_bank() { return work_bank; }
        long long get_break_bank() { return break_bank; }

        long long get_work_left() { return work_left; }
        long long get_break_allowance() { return break_allowance; }
        long long get_break_ratio() { return break_ratio; }

        void update_all_time();
        void add_work_time(long long seconds);

        void add_break_ratio(long long ratio)
        {
            if (break_ratio + ratio <= 0)
                return;
            break_ratio += ratio;
        }

        void start_work();
        void stop_work();

        void start_break();
        void stop_break();

        void reset_timer();

        void play_sound();
};


std::string format_time(long long seconds)
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

    sf::RectangleShape panel({0.f,0.f});

    panel.setFillColor(sf::Color(40,40,45));

    panel.setOutlineThickness(3);

    panel.setOutlineColor(sf::Color(80,80,90));

    const sf::Vector2f panelSize(760.f, 600.f);

    panel.setSize(panelSize);

    panel.setPosition(
        (window.getSize().x - panelSize.x) / 2.f,
        (window.getSize().y - panelSize.y) / 2.f
    );

    window.draw(panel);

    float panelTop = panel.getPosition().y;
    float panelCovered = panelTop;

    //------------------------------------
    // Title
    //------------------------------------

    sf::Text title;
    title.setFont(font);
    title.setString("WORK TIME");
    title.setCharacterSize(42);
    title.setStyle(sf::Text::Bold);
    title.setFillColor(sf::Color::Cyan);

    CenterText(title, panelCovered + 55.f, window);
    panelCovered += 55.f;

    window.draw(title);

    //------------------------------------
    // Idea
    //------------------------------------

    sf::Text idea;
    idea.setFont(font);
    idea.setString("Focus on the work and let the time pass as a byproduct");
    idea.setCharacterSize(20);
    idea.setStyle(sf::Text::Italic);
    idea.setFillColor(sf::Color::White);

    CenterText(idea, panelCovered + 55.f, window);
    panelCovered += 55.f;

    window.draw(idea);

    idea.setString("Visualize Success Visualize Consequence");

    CenterText(idea, panelCovered + 25.f, window);
    panelCovered += 25.f;

    window.draw(idea);

    idea.setString("Make it Interesting Fun ");

    CenterText(idea, panelCovered + 25.f, window);
    panelCovered += 25.f;

    window.draw(idea);

    //------------------------------------
    // Status
    //------------------------------------

    sf::Text status;
    status.setFont(font);

    status.setCharacterSize(30);

    if(timer.get_work_mode())
    {
        if (timer.get_break_mode())
        {
            status.setString("ON BREAK");
            status.setFillColor(sf::Color::Yellow);
        } else {
            status.setString("WORKING");
            status.setFillColor(sf::Color::Green);
        }
    }
    else
    {
        status.setString("IDLE");
        status.setFillColor(sf::Color(230,90,90));
    }

    CenterText(status, panelCovered + 50.f, window);
    panelCovered += 50.f;

    window.draw(status);

    //------------------------------------
    // Work countdown
    //------------------------------------

    sf::Text work;
    work.setFont(font);
    work.setCharacterSize(72);
    work.setStyle(sf::Text::Bold);
    work.setFillColor(sf::Color::White);

    work.setString(format_time(timer.get_work_left()));

    CenterText(work, panelCovered + 75.f, window);
    panelCovered += 75.f;

    window.draw(work);

    //------------------------------------
    // Break total
    //------------------------------------

    sf::Text bank;
    bank.setFont(font);
    bank.setCharacterSize(72);
    bank.setFillColor(sf::Color::Green);

    bank.setString(
        format_time(timer.get_break_bank())
    );

    CenterText(bank, panelCovered + 95.f, window);
    panelCovered += 95.f;

    window.draw(bank);

    //------------------------------------
    // Break reference
    //------------------------------------

    sf::Text reference;
    reference.setFont(font);
    reference.setCharacterSize(30);
    reference.setFillColor(sf::Color::White);

    std::stringstream ref;
    ref << "Break Reference (Ratio= " << timer.get_break_ratio() << "): " <<
        format_time(timer.get_break_allowance());
    reference.setString(ref.str());

    CenterText(reference, panelCovered + 65.f, window);
    panelCovered += 65.f;

    window.draw(reference);

    //------------------------------------
    // Controls
    //------------------------------------

    sf::Text controls;
    controls.setFont(font);

    controls.setCharacterSize(24);

    controls.setFillColor(sf::Color(160,160,160));

    controls.setString(
        "[F] Work    "
        "[B] Break    "
        "[R] Reset    "
        "[Q] Quit");

    CenterText(controls, panelCovered + 75.f, window);
    panelCovered += 75.f;

    window.draw(controls);

    controls.setString(
    "[1] +1h    "
    "[2] +1m    "
    "[3] +1s    "
    "[4] +1 Ratio    "
    "[5] -1 Ratio");

    CenterText(controls, panelCovered + 45.f, window);
    panelCovered += 45.f;

    window.draw(controls);

    window.display();
}