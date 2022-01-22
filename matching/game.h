#pragma

#include "ncurses.h"
#include "board.h"
#include <cassert>

// using A-Z, supporting 26 * 2 cards
using namespace std;

class Game {
public:
    Game(int _height, int _width) : n_trial(0) {
        
        assert((_height * _width) % 2 == 0);
        assert((_height * _width) <= 26 * 2);

        // Create a board
        board = new Board(_height, _width);
        
        // Configure ncursor 
        initscr();
        noecho();
        cbreak();
        curs_set(0);
        
        // Color setting
        start_color();
        init_pair(Card::REVEALED, COLOR_RED, COLOR_BLACK);
        init_pair(Card::DONE, COLOR_GREEN, COLOR_BLACK);

        // Get the height and width of the standard screen
        getmaxyx(stdscr, stdscr_h, stdscr_w);
        
        // Welcome message
        game_title();
        
        // Set the height and width of the menu window;
        gamewin_h = board->height + 2;
        gamewin_w = board->width + 2;
        
        // Set the position for the standard screen
        gamewin_start_y = (stdscr_h - gamewin_h) / 2;
        gamewin_start_x = (stdscr_w - gamewin_w) / 2;

        // Set the current cursor position
        cursor_y = 0;
        cursor_x = 0;
        
        // Create a game window
        gamewin = newwin(gamewin_h, gamewin_w, gamewin_start_y, gamewin_start_x);
        box(gamewin, 0, 0);
        keypad(gamewin, true);
        refresh();

        // TODO create score win
        int scorewin_h = 5; 
        int scorewin_w = 20;
        int scorewin_start_y = gamewin_start_y;
        int scorewin_start_x = gamewin_start_x + 15;
        scorewin = newwin(scorewin_h, scorewin_w, scorewin_start_y, scorewin_start_x);
        box(scorewin, 0, 0);
        refresh();
    }

    ~Game() {
        delete board;
        endwin();
    }
    
    void start() {
        
        // TODO init score
        while (true) {
            print_board();
            print_score();
            update_board();
        }
    }

private:

    void print_score() {
        mvwprintw(scorewin, 1, 1, "Trials: %2d", n_trial);
        mvwprintw(scorewin, 2, 1, "Opened: %2d/%2d", board->n_done, board->n_card/2);
        mvwprintw(scorewin, 3, 1, "Left:   %2d/%2d", board->n_card/2 - board->n_done, board->n_card/2);
        wrefresh(scorewin);
    }
    
    Board* board;
    WINDOW* gamewin;
    WINDOW* scorewin;

    int stdscr_h, stdscr_w;
    int gamewin_h;
    int gamewin_w;
    int gamewin_start_y;
    int gamewin_start_x;
    int cursor_y;
    int cursor_x;
    int key_hit;
    int n_trial;

    void update_board() {
        key_hit = wgetch(gamewin);
        int n_pre_peek(0);
        switch (key_hit) {
            case KEY_UP:
                if (cursor_y > 0)
                    --cursor_y;
                break;
            case KEY_DOWN:
                if (cursor_y < board->height-1)
                    ++cursor_y;
                break;
            case KEY_LEFT:
                if (cursor_x > 0)
                    --cursor_x;
                break;
            case KEY_RIGHT:
                if (cursor_x < board->width-1)
                    ++cursor_x;
                break;
            // press enter
            case 10:
                if (board->n_peek == 2) {
                    board->fold();
                }
                
                n_pre_peek = board->n_peek;
                board->peek(cursor_y, cursor_x);
                if (board->n_peek == 1 && board->n_peek > n_pre_peek) {
                    ++n_trial;
                }
                break;
            default:
                break;
        }
    }

    void print_board() {
        int y(1);
        wmove(gamewin, y, 1);
        
        // Print board
        for (int idx=0; idx!=board->n_card; ++idx) {
            
            int _y = idx / board->width;
            int _x = idx % board->width;

            if (_y == cursor_y && _x == cursor_x) {
                wattron(gamewin, A_REVERSE);
            }
            
            switch (board->cards[idx].get_status()) {
                case Card::REVEALED:
                    wattron(gamewin, COLOR_PAIR(Card::REVEALED));
                    wprintw(gamewin, "%c", board->cards[idx].get_symbol());
                    wattroff(gamewin, COLOR_PAIR(Card::REVEALED));
                    break;
                case Card::UNREVEALED:
                    wprintw(gamewin, "?");
                    break;
                case Card::DONE:
                    wattron(gamewin, COLOR_PAIR(Card::DONE));
                    wprintw(gamewin, "%c", board->cards[idx].get_symbol());
                    wattroff(gamewin, COLOR_PAIR(Card::DONE));
                    break;
            }

            if (_y == cursor_y && _x == cursor_x) {
                wattroff(gamewin, A_REVERSE);
            }
            
            // New line
            if ((idx+1) % board->width == 0) {
                wmove(gamewin, ++y, 1);
            }
        }
        wrefresh(gamewin);
    }

    // TODO add some cool color
    void game_title(void) {
        int _y = stdscr_h/7, _x = stdscr_w/2 - 89/2;

        mvprintw(_y, _x, "\n"); _y++;
        mvprintw(_y, _x, "  __  __       _______ _____ _    _ _____ _   _  _____    _____          __  __ ______ \n"); _y++;
        mvprintw(_y, _x, " |  \\/  |   /\\|__   __/ ____| |  | |_   _| \\ | |/ ____|  / ____|   /\\   |  \\/  |  ____|\n"); _y++;
        mvprintw(_y, _x, " | \\  / |  /  \\  | | | |    | |__| | | | |  \\| | |  __  | |  __   /  \\  | \\  / | |__   \n"); _y++;
        mvprintw(_y, _x, " | |\\/| | / /\\ \\ | | | |    |  __  | | | | . ` | | |_ | | | |_ | / /\\ \\ | |\\/| |  __|  \n"); _y++;
        mvprintw(_y, _x, " | |  | |/ ____ \\| | | |____| |  | |_| |_| |\\  | |__| | | |__| |/ ____ \\| |  | | |____ \n"); _y++;
        mvprintw(_y, _x, " |_|  |_/_/    \\_\\_|  \\_____|_|  |_|_____|_| \\_|\\_____|  \\_____/_/    \\_\\_|  |_|______|\n"); _y++;
        mvprintw(_y, _x, "\n");

        refresh();
    }
};