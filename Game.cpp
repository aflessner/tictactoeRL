
#include "pch.h"

#include "Board.h"
#include "Game.h"

Weight::Weight()
{
    Reset();
}

void Weight::Reset()
{
    m_value = 0.0f;
    m_count = 0;
}

PossibleMoves::PossibleMoves()
{
    Reset();
}

void PossibleMoves::Reset()
{
    for (unsigned char i = 0; i < 9; i++)
    {
        m_isLegalMove[i] = false;
        m_boardHash[i] = 0;
        m_weights[i].Reset();
    }
}

unsigned char PossibleMoves::CountLegalMoves() const
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

Game::Game()
    : m_moveIndex(0)
{
    Reset();
}

void Game::Reset()
{
    m_moveIndex = 0;

    for (unsigned char i = 0; i < 9; i++)
    {
        m_boards[i].Reset();
    }
}

BoardHash Game::GetCurrentBoardHash() const
{
    return m_boards[m_moveIndex].GetBoardHash();
}

BoardHash Game::GetCurrentBoardHashOfMoveIndex(const unsigned char moveIndex) const
{
    return m_boards[m_moveIndex].GetBoardHashOfMoveIndex(moveIndex);
}

BoardHash Game::GetBoardHash(const unsigned char boardIndex) const
{
    assert(boardIndex < 9);
    assert(boardIndex <= m_moveIndex);
    return m_boards[boardIndex].GetBoardHash();
}

unsigned char Game::GetMoveIndex() const
{
    return m_moveIndex;
}

bool Game::TurnIsX() const
{
    return m_boards[m_moveIndex].TurnIsX();
}

bool Game::IsGameOver() const
{
    return m_boards[m_moveIndex].IsGameOver();
}

bool Game::XWonGame() const
{
    return m_boards[m_moveIndex].XWonGame();
}

bool Game::OWonGame() const
{
    return m_boards[m_moveIndex].OWonGame();
}

bool Game::IsDraw() const
{
    return m_boards[m_moveIndex].IsDraw();
}

void Game::GetPossibleMoves(PossibleMoves& moves) const
{
    moves.Reset();
    moves.m_turnIsX = TurnIsX();
    for (unsigned char i = 0; i < 9; i++)
    {
        if (m_boards[m_moveIndex].IsLegalMove(i))
        {
            moves.m_isLegalMove[i] = true;
            moves.m_boardHash[i] = GetCurrentBoardHashOfMoveIndex(i);
        }
        else
        {
            moves.m_isLegalMove[i] = false;
        }
    }
}

void Game::SelectMove(unsigned char i)
{
    m_boards[m_moveIndex].Move(i);
    if (!m_boards[m_moveIndex].IsGameOver())
    {
        m_moveIndex++;
        assert(m_moveIndex < 9);
        m_boards[m_moveIndex] = m_boards[m_moveIndex - 1];
    }
}

bool Game::IsLegalMove(const unsigned char i) const
{
    return i < 9 && m_boards[m_moveIndex].IsLegalMove(i);
}

void Game::PrintCurrentBoard() const
{
    m_boards[m_moveIndex].PrintBoard();
}

void Game::PrintGame() const
{
    for (unsigned char i = 0; i <= m_moveIndex; i++)
    {
        m_boards[i].PrintBoard();
    }
}

void Game::PrintGameOutcome() const
{
    m_boards[m_moveIndex].PrintGameOutcome();
}