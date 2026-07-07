#include "main.h"

long long Timer::CurrentFocusTime()
{
    long long total = totalFocusSeconds;
    if (focusing)
    {   
        long long new_count = std::chrono::duration_cast<std::chrono::seconds>(Clock::now() - focusStart).count();
        breakCalcSeconds = new_count / 4;
        total += new_count;
    }
    return total;
}

long long Timer::CurrentBreakBank()
{
    long long bank = breakBankSeconds;
    if (onBreak)
    {
        bank -= std::chrono::duration_cast<std::chrono::seconds>(Clock::now() - breakStart).count();
    }
    return bank;
}

long long Timer::CurrentBreakCalc()
{
    return breakCalcSeconds;
}

void Timer::StartFocus()
{
    if (focusing || onBreak)
        return;

    focusing = true;
    focusStart = Clock::now();
}

void Timer::StopFocus()
{
    if (!focusing)
        return;

    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(Clock::now() - focusStart).count();

    focusing = false;
    totalFocusSeconds += elapsed;
    breakBankSeconds += elapsed / 4;    // Every 4 minutes of focus earns 1 minute of break.
    breakCalcSeconds = 0;
}

void Timer::StartBreak()
{
    if (focusing || onBreak)
        return;

    onBreak = true;
    breakStart = Clock::now();
}

void Timer::StopBreak()
{
    if (!onBreak)
        return;

    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(Clock::now() - breakStart).count();

    breakBankSeconds -= elapsed;
    onBreak = false;
}

void Timer::Reset()
{
    focusing = false;
    onBreak = false;    
    totalFocusSeconds = 0;
    breakBankSeconds = 0;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode({900, 600}),"Focus Timer");

    window.setFramerateLimit(30); // Limit rendering to 30 FPS.

    //--------------------------------------------------
    // Load font
    //--------------------------------------------------

    sf::Font font;

    if (!font.loadFromFile("Roboto-Regular.ttf"))
    {
        return -1;
    }

    Timer timer;

    //--------------------------------------------------
    // Main loop
    //--------------------------------------------------

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code)
                {
                    case sf::Keyboard::F:
                    {
                        if (timer.get_focusing())
                            timer.StopFocus();
                        else
                            timer.StartFocus();

                        break;
                    }

                    case sf::Keyboard::B:
                    {
                        if (timer.get_onbreak())
                            timer.StopBreak();
                        else
                            timer.StartBreak();

                        break;
                    }

                    case sf::Keyboard::R:
                    {
                        timer.Reset();
                        break;
                    }

                    case sf::Keyboard::Q:
                    case sf::Keyboard::Escape:
                    {
                        window.close();
                        break;
                    }

                    default:
                        break;
                }
            }

            if (event.type == sf::Event::Resized)
            {
                window.setView(sf::View(
                    sf::FloatRect(0.f, 0.f,
                    event.size.width,
                    event.size.height)));
            }
        }

        Draw(window, font, timer);
    }

    return 0;
}