#pragma once

class PossibleMoves;
class Game;
class Weight;

class QLearner
{
private:
    const unsigned long long NumberOfGamesToUseForTraining = 1000000;

public:
    QLearner();
    void Learn();

    const unsigned char SelectBestMoveAndPrintDebug(PossibleMoves& moves) const;
    const unsigned char SelectBestMove(const Game& game) const;
    const unsigned char SelectBestMove(PossibleMoves& moves) const;
    const unsigned char SelectTrainingMove(PossibleMoves& moves) const;
    const unsigned char SelectMove(PossibleMoves& moves, const bool doRandomMove, const bool printMoves) const;

private:

    void Backpropagate(const Game& game);

    void GetWeights(PossibleMoves& moves) const;

private:
    Weight m_weights[20000];
};