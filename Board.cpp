
#include "cstdafx.h"

#include "Board.h"

Board::Board()
{
    Reset();
}

Board::Board(const Board& oldBoard)
{
    memcpy(&m_board, &oldBoard.m_board, sizeof(m_board));
    m_gameState = oldBoard.m_gameState;
    m_moveCount = oldBoard.m_moveCount;
}

void Board::Reset()
{
    memset(&m_board, Empty, sizeof(m_board));
    m_gameState = GameInProgress;
    m_moveCount = 0;
}

bool Board::XWonGame() const
{
    return m_gameState == XWon;
}

bool Board::OWonGame() const
{
    return m_gameState == OWon;
}

bool Board::IsDraw() const
{
    return m_gameState == DrawGame;
}

bool Board::IsGameOver() const
{
    return m_gameState != GameInProgress;
}

bool Board::IsLegalMove(const unsigned char i) const
{
    assert(i < 9);
    return m_board[i] == Empty;
}


unsigned char Board::GetBoardIndex(const unsigned char x, const unsigned char y)
{
    // 0 1 2
    // 3 4 5
    // 6 7 8
    assert(x < 3);
    assert(y < 3);
    return (y * 3) + x;
}

char Board::GetStringAtBoardPosition(unsigned char i) const
{
    assert(i <= 9);
    unsigned char BoardValue = m_board[i];

    assert(0 <= BoardValue && BoardValue <= O);

    if (BoardValue == X)
    {
        return 'X';
    }
    else if (BoardValue == O)
    {
        return 'O';
    }
    else
    {
        return '-';
    }
}

void Board::CalculateAndSetGameState()
{
    assert(m_gameState == GameInProgress);

    if (m_moveCount < 5)
    {
        return;
    }
    if (WonGame(X))
    {
        m_gameState = XWon;
    }
    else if (WonGame(O))
    {
        m_gameState = OWon;
    }
    else if (m_moveCount == 9)
    {
        m_gameState = DrawGame;
    }
}

void Board::Move(const unsigned char x, const unsigned char y)
{
    Move(GetBoardIndex(x, y));
}

void Board::Move(const unsigned char movePosition)
{
    assert(movePosition < 9);
    assert(m_board[movePosition] == 0);

    const unsigned char moveValue = TurnIsX() ? X : O;

    m_board[movePosition] = moveValue;

    m_moveCount++;

    CalculateAndSetGameState();
}

void Board::PrintGameOutcome() const
{
    if (XWonGame())
    {
        printf("X Won the Game!\n");
    }
    else if (OWonGame())
    {
        printf("O Won the Game!\n");
    }
    else if (IsDraw())
    {
        printf("Game is a draw!\n");
    }
}

void Board::PrintBoardInternal(const bool AddTabs) const
{
    printf("\n");
    if (AddTabs)
    {
        printf("    ");
    }
    for (unsigned char i = 0; i < 9; i++)
    {
        printf("%c ", GetStringAtBoardPosition(i));
        if (i == 2 || i == 5 || i == 8)
        {
            printf("\n");
            if (AddTabs)
            {
                printf("    ");
            }
        }
    }
    PrintGameOutcome();
}

void Board::PrintBoardWithTabs() const
{
    PrintBoardInternal(true);
}

void Board::PrintBoard() const
{
    PrintBoardInternal(false);
}

unsigned char Board::CountMovesFromBoard() const
{
    unsigned char totalMoves = 0;

    for (unsigned char i = 0; i < 9; i++)
    {
        if (m_board[i] != Empty)
        {
            totalMoves++;
        }
    }

    return totalMoves;
}

bool Board::TurnIsX() const
{
    return (m_moveCount % 2) == 0;
}

BoardHash Board::GetBoardHash() const
{
    return m_board[0] +
        3 * m_board[1] +
        9 * m_board[2] +
        27 * m_board[3] +
        81 * m_board[4] +
        243 * m_board[5] +
        729 * m_board[6] +
        2187 * m_board[7] +
        6561 * m_board[8];
}

BoardHash Board::GetBoardHashOfMoveIndex(const unsigned char moveIndex) const
{
    assert(moveIndex < 9);
    const BoardHash currentBoardHash = GetBoardHash();
    unsigned short NumberToAdd = TurnIsX() ? X : O;
    for (unsigned short i = 0; i < moveIndex; i++)
    {
        NumberToAdd *= 3;
    }
    return currentBoardHash + NumberToAdd;
}

void Board::SetBoardFromHash(BoardHash hashValue)
{
    assert(hashValue < 20000);

    Reset();

    unsigned char i = 0;
    while (0 < hashValue)
    {
        unsigned char r = hashValue % 3;
        m_board[i] = r;
        hashValue /= 3;
        i++;
    }

    m_moveCount = CountMovesFromBoard();

    CalculateAndSetGameState();
}


bool Board::WonGame(const unsigned char p) const
{
    assert(p == X || p == O);

    return
        (m_board[0] == p && m_board[1] == p && m_board[2] == p) ||
        (m_board[3] == p && m_board[4] == p && m_board[5] == p) ||
        (m_board[6] == p && m_board[7] == p && m_board[8] == p)
        ||
        (m_board[0] == p && m_board[3] == p && m_board[6] == p) ||
        (m_board[1] == p && m_board[4] == p && m_board[7] == p) ||
        (m_board[2] == p && m_board[5] == p && m_board[8] == p)
        ||
        (m_board[0] == p && m_board[4] == p && m_board[8] == p) ||
        (m_board[6] == p && m_board[4] == p && m_board[2] == p);
}
