#pragma once

typedef unsigned short BoardHash;

class Board;

class Weight
{
public:
    Weight()
    {
        Reset();
    }

    void Reset()
    {
        m_value = 0.0f;
        m_count = 0;
    }

    float m_value;
    unsigned long m_count;
};

class PossibleMoves
{
public:
    PossibleMoves()
    {
        Reset();
    }

    void Reset()
    {
        for (unsigned char i = 0; i < 9; i++)
        {
            m_isLegalMove[i] = false;
            m_boardHash[i] = 0;
            m_weights[i].Reset();
        }
    }

    unsigned char CountLegalMoves() const
    {
        unsigned char legalMoves = 0;
        for (unsigned char i = 0; i < 9; i++)
        {
            if (m_isLegalMove[i])
            {
                legalMoves++;
            }
        }
        return legalMoves;
    }

    bool m_turnIsX;
    bool m_isLegalMove[9];
    BoardHash m_boardHash[9];
    Weight m_weights[9];
};

class Game
{
public:
    Game();
    void Reset();
    void SelectMove(unsigned char i);

    BoardHash GetCurrentBoardHash() const;
    BoardHash GetCurrentBoardHashOfMoveIndex(const unsigned char moveIndex) const;
    BoardHash GetBoardHash(const unsigned char boardIndex) const;
    unsigned char GetMoveIndex() const;
    bool TurnIsX() const;
    bool IsGameOver() const;
    bool XWonGame() const;
    bool OWonGame() const;
    bool IsDraw() const;
    void GetPossibleMoves(PossibleMoves& moves) const;
    bool IsLegalMove(const unsigned char i) const;
    void PrintCurrentBoard() const;
    void PrintGame() const;
    void PrintGameOutcome() const;

private:
    unsigned char m_moveIndex;
    Board m_boards[9];
};