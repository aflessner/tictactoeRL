
#include "pch.h"

#include "Board.h"
#include "Game.h"
#include "QLearner.h"
#include "MinMax.h"

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
QLearner theQLearner;
MinMax theMinMax;

int main()
{
    const unsigned long long NumberOfGamesToUseForTraining = 1000000;
    const unsigned long long NumberOfGamesToUseForVerification = 10000;
    const bool AIGoesFirst = true;

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

