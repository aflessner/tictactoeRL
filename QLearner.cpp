
#include "cstdafx.h"

#include "Board.h"
#include "Game.h"
#include "QLearner.h"

QLearner::QLearner()
{
    memset(&m_weights, 0, sizeof(m_weights));
}

const unsigned char QLearner::SelectBestMoveAndPrintDebug(PossibleMoves& moves) const
{
    return SelectMove(moves, false, true);
}

const unsigned char QLearner::SelectBestMove(const Game& game) const
{
    PossibleMoves moves;
    game.GetPossibleMoves(moves);
    return SelectMove(moves, false, false);
}

const unsigned char QLearner::SelectBestMove(PossibleMoves& moves) const
{
    return SelectMove(moves, false, false);
}

const unsigned char QLearner::SelectTrainingMove(PossibleMoves& moves) const
{
    const int percentRandom = 33;
    const int randomNumber = rand() % 100;
    const bool doRandomMove = randomNumber < percentRandom;
    return SelectMove(moves, doRandomMove, false);
}

const unsigned char QLearner::SelectMove(PossibleMoves& moves, const bool doRandomMove, const bool printMoves) const
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

void QLearner::Learn()
{
    PossibleMoves moves;
    Game g;

    for (unsigned long long i = 0; i < NumberOfGamesToUseForTraining; i++)
    {
        g.Reset();
        while (!g.IsGameOver())
        {
            g.GetPossibleMoves(moves);
            g.SelectMove(SelectTrainingMove(moves));
        }
        Backpropagate(g);
    }
}

void QLearner::Backpropagate(const Game& game)
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

void QLearner::GetWeights(PossibleMoves& moves) const
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