#pragma

#include <vector>
#include <algorithm>
#include <random>

using namespace std;

class Card {
public:
    enum Status {UNREVEALED, REVEALED, DONE};
    Card() : status(UNREVEALED), symbol(0) {}
    Card(char _symbol) : status(UNREVEALED), symbol(_symbol) {}
    const Status get_status() const {
        return status;
    }
    const char get_symbol() const {
        return symbol;
    }
    void set_status(Status _status) {
        status = _status;
    }

private:
    Status status;
    char symbol;
};

class Board {
public:
    Board(int _height, int _width) : n_peek(0), pre_idx(-1), height(_height), width(_width), n_done(0), n_card(_height * _width) {
        
        // 33 to 126, supporting 94 symbols
        vector<char> ascii_printables;
        for (int i=33; i<=126; i++) {
            ascii_printables.push_back(i);
        }

        default_random_engine rng;
        std::shuffle(begin(ascii_printables), end(ascii_printables), rng);
        
        for (int idx=0; idx!=n_card/2; ++idx) {
            cards.push_back(Card(ascii_printables[idx]));
            cards.push_back(Card(ascii_printables[idx]));
        }
        std::shuffle(begin(cards), end(cards), rng);
    }

    void peek(int y, int x) {
        
        if (cards[width * y + x].get_status() == Card::UNREVEALED) {
            ++n_peek;
            cards[width * y + x].set_status(Card::REVEALED);
        }

        if (n_peek == 1) {
            pre_idx = width * y + x;
        }
        
        if (n_peek == 2) {
            // Successful pair

            if (cards[width * y + x].get_symbol() == cards[pre_idx].get_symbol()) {
                cards[pre_idx].set_status(Card::DONE);
                cards[width * y + x].set_status(Card::DONE);
                ++n_done;
            }
        }
    }

    void fold() {
        for (int idx=0; idx!=n_card; ++idx) {
            if (cards[idx].get_status() == Card::REVEALED) {
                cards[idx].set_status(Card::UNREVEALED);
            }
        }
        n_peek = 0;
        pre_idx = -1;
    }

    vector<Card> cards;
    int n_peek;
    int pre_idx;
    int height;
    int width;

    int n_done;
    int n_card;
};