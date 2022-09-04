#include <iostream>
#include <cassert>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>

typedef unsigned short BoardHash;

const unsigned char Empty = 0;
const unsigned char X = 1;
const unsigned char O = 2;

const unsigned char GameInProgress = 0;
const unsigned char XWon = 1;
const unsigned char OWon = 2;
const unsigned char DrawGame = 3;

const unsigned long long NumberOfGamesToUseForTraining = 1000000;
const unsigned long long NumberOfGamesToUseForVerification = 100;

const bool AIGoesFirst = true;

class Board
{
public:
    Board()
    {
        Reset();
    }

    Board(const Board& oldBoard)
    {
        memcpy(&m_board, &oldBoard.m_board, sizeof(m_board));
        m_gameState = oldBoard.m_gameState;
        m_moveCount = oldBoard.m_moveCount;
    }

    void Reset()
    {
        memset(&m_board, Empty, sizeof(m_board));
        m_gameState = GameInProgress;
        m_moveCount = 0;
    }
    
    bool XWonGame() const
    {
        return m_gameState == XWon;
    }

    bool OWonGame() const
    {
        return m_gameState == OWon;
    }

    bool IsDraw() const
    {
        return m_gameState == DrawGame;
    }

    bool IsGameOver() const
    {
        return m_gameState != GameInProgress;
    }

    bool IsLegalMove(const unsigned char i) const
    {
        assert(i < 9);
        return m_board[i] == Empty;
    }

    // 0 1 2
    // 3 4 5
    // 6 7 8
    static unsigned char GetBoardIndex(const unsigned char x, const unsigned char y)
    {
        assert(x < 3);
        assert(y < 3);
        return (y * 3) + x;
    }

    char GetStringAtBoardPosition(unsigned char i) const
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

    void CalculateGameState()
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

    void Move(const unsigned char x, const unsigned char y)
    {
        Move(GetBoardIndex(x, y));
    }

    void Move(const unsigned char movePosition)
    {
        assert(movePosition < 9);
        assert(m_board[movePosition] == 0);

        const unsigned char moveValue = TurnIsX() ? X : O;

        m_board[movePosition] = moveValue;

        m_moveCount++;

        CalculateGameState();
    }

    void PrintGameOutcome() const
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

    void PrintBoardInternal(const bool AddTabs) const
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

    void PrintBoardWithTabs() const
    {
        PrintBoardInternal(true);
    }

    void PrintBoard() const
    {
        PrintBoardInternal(false);
    }
    
    unsigned char CountMovesFromBoard() const
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

    bool TurnIsX() const
    {
        return (m_moveCount % 2) == 0;
    }

    BoardHash GetBoardHash() const
    {
        BoardHash bH = m_board[0] +
            3 * m_board[1] +
            9 * m_board[2] +
            27 * m_board[3] +
            81 * m_board[4] +
            243 * m_board[5] +
            729 * m_board[6] +
            2187 * m_board[7] +
            6561 * m_board[8];

        assert(bH < 20000);

        return bH;
    }

    void SetBoardFromHash(BoardHash hashValue)
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

        CalculateGameState();
    }

private:

    bool WonGame(const unsigned char p) const
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

private:
    unsigned char m_board[9];
    unsigned char m_gameState;
    unsigned char m_moveCount;
    unsigned char m_padding[1];
};

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
    Game()
        : m_moveIndex(0)
    {
        Reset();
    }

    void Reset()
    {
        m_moveIndex = 0;

        for (unsigned char i = 0; i < 9; i++)
        {
            m_boards[i].Reset();
        }
    }

    BoardHash GetBoardHash(const unsigned char boardIndex) const
    {
        assert(boardIndex < 9);
        assert(boardIndex <= m_moveIndex);
        return m_boards[boardIndex].GetBoardHash();
    }

    unsigned char GetMoveIndex() const
    {
        return m_moveIndex;
    }

    bool TurnIsX() const
    {
        return m_boards[m_moveIndex].TurnIsX();
    }

    bool IsGameOver() const
    {
        return m_boards[m_moveIndex].IsGameOver();
    }

    bool XWonGame() const
    {
        return m_boards[m_moveIndex].XWonGame();
    }

    bool OWonGame() const
    {
        return m_boards[m_moveIndex].OWonGame();
    }

    bool IsDraw() const
    {
        return m_boards[m_moveIndex].IsDraw();
    }

    void GetPossibleMoves(PossibleMoves& moves)
    {
        moves.m_turnIsX = TurnIsX();
        for (unsigned char i = 0; i < 9; i++)
        {
            if (m_boards[m_moveIndex].IsLegalMove(i))
            {
                Board tmpBoard(m_boards[m_moveIndex]);
                tmpBoard.Move(i);
                moves.m_isLegalMove[i] = true;
                moves.m_boardHash[i] = tmpBoard.GetBoardHash();
            }
            else
            {
                moves.m_isLegalMove[i] = false;
            }
        }
    }

    void SelectMove(unsigned char i)
    {
        m_boards[m_moveIndex].Move(i);
        if (!m_boards[m_moveIndex].IsGameOver())
        {
            m_moveIndex++;
            assert(m_moveIndex < 9);
            m_boards[m_moveIndex] = m_boards[m_moveIndex - 1];
        }
    }

    bool IsLegalMove(const unsigned char i) const
    {
        return i < 9 && m_boards[m_moveIndex].IsLegalMove(i);
    }

    void PrintCurrentBoard() const
    {
        m_boards[m_moveIndex].PrintBoard();
    }

    void PrintGame() const
    {
        for (unsigned char i = 0; i <= m_moveIndex; i++)
        {
            m_boards[i].PrintBoard();
        }
    }

    void PrintGameOutcome() const
    {
        m_boards[m_moveIndex].PrintGameOutcome();
    }

private:
    unsigned char m_moveIndex;
    Board m_boards[9];
};

class NN
{
public:
    NN()
    {
        memset(&m_weights, 0, sizeof(m_weights));
    }

    unsigned char SelectBestPossibleMoveAndPrintDebug(PossibleMoves& moves)
    {
        return SelectMove(moves, false, true);
    }

    unsigned char SelectBestPossibleMove(PossibleMoves& moves)
    {
        return SelectMove(moves, false, false);
    }

    unsigned char SelectTrainingMove(PossibleMoves& moves)
    {
        const int percentRandom = 33;
        const int randomNumber = rand() % 100;
        const bool doRandomMove = randomNumber < percentRandom;
        return SelectMove(moves, doRandomMove, false);
    }

    unsigned char SelectMove(PossibleMoves& moves, bool doRandomMove, bool printMoves)
    {
        GetWeights(moves);

        unsigned char moveIndex = UCHAR_MAX;

        if (doRandomMove)
        {
            const unsigned char legalMoveCount = moves.CountLegalMoves();
            assert(0 < legalMoveCount);
            const unsigned char moveTarget = rand() % legalMoveCount;

            unsigned char moveCount = 0;
            for (unsigned char i = 0; i < 9; i++)
            {
                if (moves.m_isLegalMove[i])
                {
                    if (moveCount == moveTarget)
                    {
                        moveIndex = i;
                        break;
                    }
                    moveCount++;
                }
            }
        }
        else
        {
            for (unsigned char i = 0; i < 9; i++)
            {
                if (moves.m_isLegalMove[i])
                {
                    if (printMoves)
                    {
                        printf("\n    Weight = %.2f (Count = %u)", moves.m_weights[i].m_value, moves.m_weights[i].m_count);
                        Board b;
                        b.SetBoardFromHash(moves.m_boardHash[i]);
                        b.PrintBoardWithTabs();
                    }

                    if (moveIndex == UCHAR_MAX ||
                        moves.m_weights[moveIndex].m_value < moves.m_weights[i].m_value)
                    {
                        moveIndex = i;
                    }
                }
            }
        }

        assert(moveIndex < 9);
        return moveIndex;
    }

    void Backpropagate(const Game& game)
    {
        float weightToAdd = 0.0f;
        
        if (game.XWonGame())
        {
            weightToAdd = 1.0f;
        }
        else if (game.OWonGame())
        {
            weightToAdd = -1.0f;
        }
        else
        {
            assert(game.IsDraw());
        }

        assert(game.GetMoveIndex() < 9);

        for (unsigned int i = 0; i <= game.GetMoveIndex(); i++)
        {
            BoardHash bH = game.GetBoardHash(i);
            assert(bH < 20000);
            if (i == game.GetMoveIndex() && game.XWonGame())
            {
                m_weights[bH].m_value = 100000.0f;
                m_weights[bH].m_count = ULONG_MAX;
            }
            else if (i == game.GetMoveIndex() && game.OWonGame())
            {
                m_weights[bH].m_value = -100000.0f;
                m_weights[bH].m_count = ULONG_MAX;
            }
            else if (m_weights[bH].m_count == 0)
            {
                m_weights[bH].m_value = weightToAdd;
                m_weights[bH].m_count++;
            }
            else if (m_weights[bH].m_count != ULONG_MAX)
            {
                float totalSum = (m_weights[bH].m_value * m_weights[bH].m_count) + weightToAdd;
                m_weights[bH].m_count++;
                m_weights[bH].m_value = totalSum / m_weights[bH].m_count;
            }
        }
    }

private:
    void GetWeights(PossibleMoves& moves) const
    {
        for (unsigned char i = 0; i < 9; i++)
        {
            if (moves.m_isLegalMove[i])
            {
                // Must invert the weight if it is the O players turn
                const float fMultiply = moves.m_turnIsX ? 1.0f : -1.0f;
                const BoardHash bH = moves.m_boardHash[i];
                moves.m_weights[i] = m_weights[bH];
                moves.m_weights[i].m_value *= fMultiply;
            }
        }
    }

private:
    Weight m_weights[20000];
};

NN theNN;

int main()
{
    const time_t t = time(NULL);
    const unsigned int tAsInt = static_cast<unsigned int>(t);
    srand(tAsInt);

    static_assert(sizeof(unsigned char) == 1);
    static_assert(sizeof(Board) == 12);

    PossibleMoves moves;
    Game g;

    printf("Simulating %llu games for training...\n", NumberOfGamesToUseForTraining);

    ULONGLONG startMs = GetTickCount64();

    for (unsigned long long i = 0; i < NumberOfGamesToUseForTraining; i++)
    {
        g.Reset();
        while (!g.IsGameOver())
        {
            moves.Reset();
            g.GetPossibleMoves(moves);
            unsigned char moveIndex = theNN.SelectTrainingMove(moves);
            g.SelectMove(moveIndex);
        }
        theNN.Backpropagate(g);
    }

    ULONGLONG stopMs = GetTickCount64();
    ULONGLONG elapsedMs = stopMs - startMs;
    float seconds = elapsedMs / 1000.0f;

    printf("Done training, took %.1f seconds\n", seconds);

    unsigned int draws = 0;
    unsigned int xWins = 0;
    unsigned int oWins = 0;

    printf("Simulating %llu games using inference based on model...\n", NumberOfGamesToUseForVerification);

    startMs = GetTickCount64();

    for (unsigned int i = 0; i < NumberOfGamesToUseForVerification; i++)
    {
        g.Reset();
        g.SelectMove(rand() % 8);
        while (!g.IsGameOver())
        {
            moves.Reset();
            g.GetPossibleMoves(moves);
            unsigned char moveIndex = theNN.SelectBestPossibleMove(moves);
            g.SelectMove(moveIndex);
        }
        if (g.XWonGame())
        {
            xWins++;
        }
        else if (g.OWonGame())
        {
            oWins++;
        }
        else
        {
            draws++;
        }
        g.PrintGame();
    }

    stopMs = GetTickCount64();
    elapsedMs = stopMs - startMs;
    seconds = elapsedMs / 1000.0f;

    printf("Done playing, took %.3f seconds (%llu ms)\n", seconds, elapsedMs);
    printf("X Wins: %u\n", xWins);
    printf("O Wins: %u\n", oWins);
    printf("Draws: %u\n", draws);

    while (true)
    {
        printf("Starting game against AI!\n");
        g.Reset();
        while (!g.IsGameOver())
        {
            g.PrintCurrentBoard();
            if (AIGoesFirst == g.TurnIsX())
            {
                moves.Reset();
                g.GetPossibleMoves(moves);
                unsigned char moveIndex = theNN.SelectBestPossibleMoveAndPrintDebug(moves);
                g.SelectMove(moveIndex);
            }
            else
            {
                moves.Reset();
                g.GetPossibleMoves(moves);
                (void)theNN.SelectBestPossibleMoveAndPrintDebug(moves);
                unsigned int moveIndex;
                do
                {
                    printf("Input Move: ");
                    scanf_s("%u", &moveIndex);
                } while (!g.IsLegalMove(moveIndex));
                g.SelectMove(moveIndex);
            }
        }
        g.PrintCurrentBoard();
    }
}

