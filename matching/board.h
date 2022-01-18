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
    void set_symbol(char c) {
        symbol = c;
    }
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
    Board(int _height, int _width) : n_peak(0), pre_idx(-1), height(_height), width(_width), n_card(_height * _width) {
        char symbol('A');
        for (int idx=0; idx!=n_card/2; ++idx) {
            cards.push_back(Card(symbol));
            cards.push_back(Card(symbol));
            ++symbol;
        }
        default_random_engine rng;
        std::shuffle(begin(cards), end(cards), rng);
    }

    void peak(int y, int x) {
        
        if (cards[width * y + x].get_status() == Card::UNREVEALED) {
            ++n_peak;
            cards[width * y + x].set_status(Card::REVEALED);
        }

        if (n_peak == 1) {
            pre_idx = width * y + x;
        }
        
        if (n_peak == 2) {
            if (cards[width * y + x].get_symbol() == cards[pre_idx].get_symbol()) {
                cards[pre_idx].set_status(Card::DONE);
                cards[width * y + x].set_status(Card::DONE);
                n_peak = 0;
            }
        }
    }

    void fold() {
        for (int idx=0; idx!=n_card; ++idx) {
            if (cards[idx].get_status() == Card::REVEALED) {
                cards[idx].set_status(Card::UNREVEALED);
            }
        }
        n_peak = 0;
        pre_idx = -1;
    }

    vector<Card> cards;
    int n_peak;
    int pre_idx;
    int height;
    int width;
    int n_card;
};