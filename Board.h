#pragma once

const unsigned char Empty = 0;
const unsigned char X = 1;
const unsigned char O = 2;

const unsigned char GameInProgress = 0;
const unsigned char XWon = 1;
const unsigned char OWon = 2;
const unsigned char DrawGame = 3;

typedef unsigned short BoardHash;

class Board
{
private:
    static unsigned char GetBoardIndex(const unsigned char x, const unsigned char y);

public:
    Board();
    Board(const Board& oldBoard);
    void Reset();

    void Move(const unsigned char x, const unsigned char y);
    void Move(const unsigned char movePosition);
    void SetBoardFromHash(BoardHash hashValue);

    bool XWonGame() const;
    bool OWonGame() const;
    bool IsDraw() const;
    bool IsGameOver() const;
    bool IsLegalMove(const unsigned char i) const;
    void PrintGameOutcome() const;
    void PrintBoardInternal(const bool AddTabs) const;
    void PrintBoardWithTabs() const;
    void PrintBoard() const;
    unsigned char CountMovesFromBoard() const;
    bool TurnIsX() const;
    BoardHash GetBoardHash() const;
    BoardHash GetBoardHashOfMoveIndex(const unsigned char moveIndex) const;
    char GetStringAtBoardPosition(unsigned char i) const;

private:
    void CalculateAndSetGameState();

private:
    bool WonGame(const unsigned char p) const;

private:
    unsigned char m_board[9];
    unsigned char m_gameState;
    unsigned char m_moveCount;
    unsigned char m_padding[1];
};