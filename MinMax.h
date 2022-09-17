#pragma once

class MinMax
{
private:
    
    struct BoardState
    {
        unsigned char m_outcome;
        unsigned char m_weight;
    };

public:

    MinMax();
    void Learn();

    unsigned char SelectBestMove(const Game& g) const;

private:

    void GenerateAllBoards(Game& g1);

private:
    BoardState m_boards[20000];
};