#pragma once

struct BoardState
{
    unsigned char m_outcome;
    unsigned char m_weight;
};

class MinMax
{
public:

    MinMax();
    void Learn();

    unsigned char SelectBestMove(const Game& g) const;

private:

    void GenerateAllBoards(Game& g1);

private:
    BoardState m_boards[20000];
};