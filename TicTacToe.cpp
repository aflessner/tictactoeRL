#include <iostream>
#include <cassert>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>

#include "Board.h"
#include "Game.h"
#include "QLearner.h"

const unsigned long long NumberOfGamesToUseForTraining = 1000000;
const unsigned long long NumberOfGamesToUseForVerification = 10000;

const bool AIGoesFirst = true;

const unsigned char XWonWeight = 100;
const unsigned char OWonWeight = 0;
const unsigned char DrawGameWeight = 50;

struct BoardState
{
    unsigned char m_outcome;
    unsigned char m_weight;
};

// Neuron
// 
template <const unsigned long NumberOfWeights>
class Neuron
{
public:
    Neuron()
    {
        memset(m_weights, 0, sizeof(m_weights));
        m_bias = 0.0f;
    }

    const float CalculateOutput(const float inputs[NumberOfWeights]) const
    {
        float sum = m_bias;
        for (unsigned i = 0; i < NumberOfWeights; i++)
        {
            sum += inputs[i] * m_weights[i];
        }
        return Relu(sum);
    }

private:
    static float Relu(const float x)
    {
        return max(0.0f, x);
    }

private:
    float m_weights[NumberOfWeights];
    float m_bias;
};

template <const unsigned long InputLayerSize, const unsigned long OutputLayerSize>
class DenseLayer
{
public:
    DenseLayer()
    {}

    void Forward(const float inputs[InputLayerSize], float outputs[OutputLayerSize]) const
    {
        for (unsigned i = 0; i < OutputLayerSize; i++)
        {
            outputs[i] = m_neurons[i].CalculateOutput(inputs);
        }
    }

private:
    Neuron<InputLayerSize> m_neurons[OutputLayerSize];
};

static const ULONG TicTacToeBoardSize = 9;                              // 9 Board Positions
static const ULONG TicTacToeInputLayerSize = TicTacToeBoardSize * 3;    // board positions * 3 states per board position = 27
static const ULONG TicTacToeOutputLayerSize = TicTacToeBoardSize;       // histogram of softmax of output layer

class DeepNN
{
    const float WinReward = 1.0f;
    const float DrawReward = 0.0f;
    const float LoseReward = -1.0f;

public:

    DeepNN()
    {

    }

private:
    DenseLayer<TicTacToeInputLayerSize, TicTacToeOutputLayerSize> m_denseLayer;
};

DeepNN theDeepNN;

class MinMax
{
public:

    MinMax()
    {
        memset(&m_boards, 0, sizeof(m_boards));
    }

    unsigned char SelectBestMove(const Game& g) const
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

    void Learn()
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

private:

    void GenerateAllBoards(Game& g1)
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

private:
    BoardState m_boards[20000];
};

QLearner theQLearner;
MinMax theMinMax;

int main()
{
    const time_t t = time(NULL);
    const unsigned int tAsInt = static_cast<unsigned int>(t);
    srand(tAsInt);

    static_assert(sizeof(unsigned char) == 1);
    static_assert(sizeof(Board) == 12);

    theMinMax.Learn();

    printf("Simulating %llu games for training...\n", NumberOfGamesToUseForTraining);

    ULONGLONG startMs = GetTickCount64();

    theQLearner.Learn();

    ULONGLONG stopMs = GetTickCount64();
    ULONGLONG elapsedMs = stopMs - startMs;
    float seconds = elapsedMs / 1000.0f;

    printf("Done training, took %.1f seconds\n", seconds);

    printf("Simulating %llu games using Qlearning model playing against MinMax algorithm...\n", NumberOfGamesToUseForVerification);

    unsigned int draws = 0;
    unsigned int xWins = 0;
    unsigned int oWins = 0;

    PossibleMoves moves;
    Game g;

    for (unsigned int i = 0; i < NumberOfGamesToUseForVerification; i++)
    {
        g.Reset();
        while (!g.IsGameOver())
        {
            //g.PrintCurrentBoard();
            if (AIGoesFirst == g.TurnIsX())
            {
                g.SelectMove(theMinMax.SelectBestMove(g));
                //printf("\nMinMax selected %u!\n", moveIndex);
            }
            else
            {
                g.SelectMove(theQLearner.SelectBestMove(g));
                //printf("\nQ Learner selected %u!\n", moveIndex);
            }
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
        //g.PrintCurrentBoard();
    }

    printf("X Wins: %u\n", xWins);
    printf("O Wins: %u\n", oWins);
    printf("Draws: %u\n", draws);
}

