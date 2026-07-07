#include "main.h"

long long Timer::CurrentFocusTime()
{
    long long total = totalFocusSeconds;
    if (focusing)
    {
        total += std::chrono::duration_cast<std::chrono::seconds>(Clock::now() - focusStart).count();
    }
    return total;
}

long long Timer::CurrentBreakBank()
{
    long long bank = breakBankSeconds;
    if (onBreak)
    {
        bank -= std::chrono::duration_cast<std::chrono::seconds>(Clock::now() - breakStart).count();
        if (bank < 0) bank = 0;
    }
    return bank;
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
}

void Timer::StartBreak()
{
    if (focusing || onBreak || breakBankSeconds <= 0)
        return;

    onBreak = true;
    breakStart = Clock::now();
}

void Timer::StopBreak()
{
    if (!onBreak)
        return;

    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(Clock::now() - breakStart).count();

    if (elapsed > breakBankSeconds) elapsed = breakBankSeconds;
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

void Draw(Timer& timer)
{
    werase(stdscr);

    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    bool focusing = timer.get_focusing();
    bool onBreak = timer.get_onbreak();
    long long focus = timer.CurrentFocusTime();
    long long bank = timer.CurrentBreakBank();


    const int width = 60;
    const int startX = (cols - width) / 2;
    const int startY = 2;

    // Border
    mvaddch(startY, startX, ACS_ULCORNER);
    mvhline(startY, startX + 1, ACS_HLINE, width - 2);
    mvaddch(startY, startX + width - 1, ACS_URCORNER);

    for (int y = 1; y < 19; y++)
    {
        mvaddch(startY + y, startX, ACS_VLINE);
        mvaddch(startY + y, startX + width - 1, ACS_VLINE);
    }

    mvaddch(startY + 19, startX, ACS_LLCORNER);
    mvhline(startY + 19, startX + 1, ACS_HLINE, width - 2);
    mvaddch(startY + 19, startX + width - 1, ACS_LRCORNER);

    // Title
    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(startY + 1, startX + 20, "FOCUS TIMER");
    attroff(COLOR_PAIR(2) | A_BOLD);

    mvhline(startY + 2, startX + 1, ACS_HLINE, width - 2);

    // Status
    attron(A_BOLD);
    mvprintw(startY + 4, startX + 3, "STATUS");
    attroff(A_BOLD);

    if (focusing)
    {
        attron(COLOR_PAIR(3) | A_BOLD);
        mvprintw(startY + 5, startX + 8, "FOCUSING");
        attroff(COLOR_PAIR(3) | A_BOLD);
    }
    else if (onBreak)
    {
        attron(COLOR_PAIR(4) | A_BOLD);
        mvprintw(startY + 5, startX + 8, "ON BREAK");
        attroff(COLOR_PAIR(4) | A_BOLD);
    }
    else
    {
        attron(COLOR_PAIR(5) | A_BOLD);
        mvprintw(startY + 5, startX + 8, "IDLE");
        attroff(COLOR_PAIR(5) | A_BOLD);
    }

    // Focus
    attron(A_BOLD);
    mvprintw(startY + 8, startX + 3, "FOCUS TIME");
    mvprintw(startY + 9, startX + 8, "%s", FormatTime(focus).c_str());
    attroff(A_BOLD);

    // Break
    attron(A_BOLD);
    mvprintw(startY + 12, startX + 3, "BREAK BANK");
    mvprintw(startY + 13, startX + 8, "%s", FormatTime(bank).c_str());
    attroff(A_BOLD);

    // Divider
    mvhline(startY + 16, startX + 1, ACS_HLINE, width - 2);

    attron(A_BOLD);
    mvprintw(startY + 17, startX + 3, "[F] Focus");
    mvprintw(startY + 17, startX + 22, "[B] Break");

    mvprintw(startY + 18, startX + 3, "[R] Reset");
    mvprintw(startY + 18, startX + 22, "[Q] Quit");
    attroff(A_BOLD);

    wnoutrefresh(stdscr);
    doupdate();
}

bool ExecuteCommand(const std::string& command, Timer& timer)
{
    if (command == "start")
    {
        timer.StartFocus();
    }
    else if (command == "stop")
    {
        timer.StopFocus();
    }
    else if (command == "break start")
    {
        timer.StartBreak();
    }
    else if (command == "break stop")
    {
        timer.StopBreak();  
    }
    else if (command == "quit")
    {
        return false;
    }

    return true;
}

int main()
{
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    if (has_colors())
    {
        start_color();

        init_pair(1, COLOR_WHITE, COLOR_BLACK);
        init_pair(2, COLOR_CYAN, COLOR_BLACK);
        init_pair(3, COLOR_GREEN, COLOR_BLACK);
        init_pair(4, COLOR_YELLOW, COLOR_BLACK);
        init_pair(5, COLOR_RED, COLOR_BLACK);
    }

    Timer timer;

    bool running = true;

    timeout(100);

    while (running)
    {
        Draw(timer);

        int ch = getch();

        switch (tolower(ch))
        {
            case 'f':
            {
                if (timer.get_focusing())
                    timer.StopFocus();
                else
                    timer.StartFocus();
                break;
            }
            case 'b':
            {
                if (timer.get_onbreak())
                    timer.StopBreak();
                else
                    timer.StartBreak();
                break;
            }
            case 'r':
            {
                timer.Reset();
                break;
            }
            case 'q':
            {
                running = false;
                break;
            }   
        }

        // napms(100); // Sleep for 100ms so CPU usage stays extremely low.
    }

    endwin();

    return 0;
}