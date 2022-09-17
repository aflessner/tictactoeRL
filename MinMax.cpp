
#include <stdlib.h>
#include <iostream>
#include <Windows.h>

#include "Board.h"
#include "Game.h"
#include "MinMax.h"

const unsigned char XWonWeight = 100;
const unsigned char OWonWeight = 0;
const unsigned char DrawGameWeight = 50;

MinMax::MinMax()
{
    memset(&m_boards, 0, sizeof(m_boards));
}

unsigned char MinMax::SelectBestMove(const Game& g) const
{
    // To make it more interesting randomize the first move
    // since they all result in draws anyway

    if (g.GetMoveIndex() == 0)
    {
        return rand() % 9;
    }

    unsigned char currentMax = 0;
    unsigned char currentMoveIndex = UCHAR_MAX;
    for (unsigned char i = 0; i < 9; i++)
    {
        if (g.IsLegalMove(i))
        {
            const BoardHash bH = g.GetCurrentBoardHashOfMoveIndex(i);
            if (currentMoveIndex == UCHAR_MAX ||
                currentMax < m_boards[bH].m_weight)
            {
                currentMoveIndex = i;
                currentMax = m_boards[bH].m_weight;
            }
        }
    }
    return currentMoveIndex;
}

void MinMax::Learn()
{
    Game root;
    GenerateAllBoards(root);

    unsigned int draws = 0;
    unsigned int xWins = 0;
    unsigned int oWins = 0;
    unsigned short count = 0;

    for (unsigned short i = 0; i < 20000; i++)
    {
        if (m_boards[i].m_outcome == XWon)
        {
            xWins++;
            count++;

        }
        else if (m_boards[i].m_outcome == OWon)
        {
            oWins++;
            count++;
        }
        else if (m_boards[i].m_outcome == DrawGame)
        {
            draws++;
            count++;
        }
    }

    printf("Found %u boards!\n", count);
    printf("X Wins: %u\n", xWins);
    printf("O Wins: %u\n", oWins);
    printf("Draws: %u\n", draws);
}


void MinMax::GenerateAllBoards(Game& g1)
{
    // explore all subtrees of legal moves
    // and label terminal game states with outcomes and weights
    for (unsigned char i = 0; i < 9; i++)
    {
        if (g1.IsLegalMove(i))
        {
            Game g2;
            g2 = g1;
            g2.SelectMove(i);
            if (g2.IsGameOver())
            {
                //g2.PrintCurrentBoard();
                const BoardHash bH = g2.GetCurrentBoardHash();
                if (g2.XWonGame())
                {
                    m_boards[bH].m_outcome = XWon;
                    m_boards[bH].m_weight = XWonWeight;
                }
                else if (g2.OWonGame())
                {
                    m_boards[bH].m_outcome = OWon;
                    m_boards[bH].m_weight = OWonWeight;
                }
                else
                {
                    m_boards[bH].m_outcome = DrawGame;
                    m_boards[bH].m_weight = DrawGameWeight;
                }
            }
            else
            {
                GenerateAllBoards(g2);
            }
        }
    }

    const bool bIsMax = g1.TurnIsX();
    unsigned char currentWeight = bIsMax ? 0 : UCHAR_MAX;
    for (unsigned char i = 0; i < 9; i++)
    {
        if (g1.IsLegalMove(i))
        {
            const BoardHash bH = g1.GetCurrentBoardHashOfMoveIndex(i);
            currentWeight = bIsMax ? max(currentWeight, m_boards[bH].m_weight) : min(currentWeight, m_boards[bH].m_weight);
        }
    }

    const BoardHash bH = g1.GetCurrentBoardHash();
    m_boards[bH].m_weight = currentWeight;
}