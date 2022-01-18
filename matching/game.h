#pragma

#include "ncurses.h"
#include "board.h"
#include <cassert>

// using a-z, supporting 26 * 2 cards
using namespace std;

class Game {
public:
    Game(int _height, int _width) {
        assert((_height * _width) % 2 == 0);

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
    }

    ~Game() {
        delete board;
        endwin();
    }

    void start() {
        while (true) {
            print_board();
            update_board();
        }
    }

private:
    Board* board;
    WINDOW* gamewin;

    int stdscr_h, stdscr_w;
    int gamewin_h;
    int gamewin_w;
    int gamewin_start_y;
    int gamewin_start_x;
    int cursor_y;
    int cursor_x;
    int key_hit;

    void update_board() {
        key_hit = wgetch(gamewin);

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
            case 10:
                if (board->n_peak == 2) {
                    board->fold();
                }
                board->peak(cursor_y, cursor_x);
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
};