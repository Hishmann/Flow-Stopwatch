#include "main.h"

void Timer::update_all_time() {

    if (work_mode)
    {
        long long elapsed =
            std::chrono::duration_cast<std::chrono::seconds>(
                Clock::now() - work_start
            ).count();

        work_bank = prev_work_bank + elapsed;
        work_left = std::max(0LL, work_set_time - work_bank);
        
        break_allowance = work_bank / break_ratio;

        if (work_left == 0) {
            stop_work();
            play_sound();
        }
    }
    if (break_mode)
    {
        long long elapsed =
            std::chrono::duration_cast<std::chrono::seconds>(
                Clock::now() - break_start
            ).count();

        break_bank = prev_break_bank + elapsed;
    }

}


void Timer::start_work()
{
    if (work_mode || work_left <= 0)
        return;

    work_mode = true;
    work_start = Clock::now();
}


void Timer::stop_work()
{
    if (!work_mode)
        return;

    long long elapsed =
        std::chrono::duration_cast<std::chrono::seconds>(
            Clock::now() - work_start
        ).count();

    work_bank = prev_work_bank + elapsed;
    work_left = work_set_time - work_bank;
    prev_work_bank = work_bank;
    work_mode = false;
}


void Timer::start_break()
{
    if (break_mode || !work_mode)
        return;

    break_mode = true;
    break_start = Clock::now();
}


void Timer::stop_break()
{
    if (!break_mode || !work_mode)
        return;

    long long elapsed =
        std::chrono::duration_cast<std::chrono::seconds>(
            Clock::now() - break_start
        ).count();

    break_bank = prev_break_bank + elapsed;
    prev_break_bank = break_bank;
    break_mode = false;
}


void Timer::reset_timer()
{
    work_mode = false;
    break_mode = false;

    work_bank = 0;
    break_bank = 0;
    prev_work_bank = 0;
    prev_break_bank = 0;

    work_set_time = 0;
    work_left = 0;
    break_allowance = 0;
}

void Timer::add_work_time(long long seconds)
{
    // work time cannot become negative
    if (work_set_time + seconds < 0)
        return;

    work_set_time += seconds;

    if (!work_mode) {
        work_left = work_set_time - work_bank;
        if (work_left < 0)
            work_left = 0;
    }
}

void Timer::play_sound() {

    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("./done_sound.wav")) {
        std::cerr << "Error loading sound file!" << std::endl;
        return; 
    }

    sf::Sound sound;
    sound.setBuffer(buffer);

    sound.setVolume(10.f);
    sound.setPitch(1.0f);

    sound.play();

    // Keep the program running while the sound plays (If the program exits immediately, you won't hear anything)
    while (sound.getStatus() == sf::Sound::Playing) {
        sf::sleep(sf::milliseconds(100)); // Prevent high CPU usage while waiting
    }

    return;
}

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({900, 700}),
        "work Timer"
    );

    window.setFramerateLimit(30);

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
                        if (timer.get_work_mode())
                            timer.stop_work();
                        else
                            timer.start_work();

                        break;
                    }

                    case sf::Keyboard::B:
                    {
                        if (timer.get_break_mode())
                            timer.stop_break();
                        else
                            timer.start_break();
                        break;
                    }

                    case sf::Keyboard::R:
                    {
                        timer.reset_timer();
                        break;
                    }

                    case sf::Keyboard::Q:
                    case sf::Keyboard::Escape:
                    {
                        window.close();
                        break;
                    }

                    case sf::Keyboard::Num1:
                    {
                        timer.add_work_time(60*60);
                        break;
                    }
                    case sf::Keyboard::Num2:
                    {
                        timer.add_work_time(60);
                        break;
                    }
                    case sf::Keyboard::Num3:
                    {
                        timer.add_work_time(1);
                        break;
                    }
                    case sf::Keyboard::Num4:
                    {
                        timer.add_break_ratio(1);
                        break;
                    }
                    case sf::Keyboard::Num5:
                    {
                      timer.add_break_ratio(-1);
                        break;
                    }

                    default:
                        break;
                }
            }

            if (event.type == sf::Event::Resized)
            {
                window.setView(sf::View(
                    sf::FloatRect(
                        0.f,
                        0.f,
                        event.size.width,
                        event.size.height
                    )
                ));
            }
        }

        timer.update_all_time();

        Draw(window, font, timer);
    }

    return 0;
}
