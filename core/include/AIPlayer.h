#ifndef AIPLAYER_H
#define AIPLAYER_H

#include "GameBoard.h"
#include <stack>
#include <random>
#include <climits>

enum class DifficultyLevel {
    EASY,
    MEDIUM,
    HARD
};

class AIPlayer {
public:
    AIPlayer(char aiSymbol, char humanSymbol, DifficultyLevel difficulty = DifficultyLevel::HARD);
    std::pair<int, int> getBestMove(const GameBoard& board);
    void setDifficulty(DifficultyLevel difficulty);
    void pushAIMove(int row, int col);
    std::pair<int, int> popAIMove();
    bool hasAIMoveHistory();
    void clearAIMoveHistory();

private:
    int minimax(GameBoard& board, int depth, bool isMaximizing, int alpha, int beta);
    int evaluateBoard(const GameBoard& board);
    std::pair<int, int> getRandomMove(const GameBoard& board);
    std::pair<int, int> getMediumMove(const GameBoard& board);
    std::pair<int, int> getHardMove(const GameBoard& board);
    std::pair<int, int> getCriticalMove(const GameBoard& board);
    std::pair<int, int> getLimitedMinimax(const GameBoard& board, int maxDepth);
    int limitedMinimax(GameBoard& board, int depth, bool isMaximizing, int alpha, int beta, int maxDepth);
    int evaluatePosition(const GameBoard& board);
    int evaluateLine(char cell1, char cell2, char cell3);

    char aiSymbol;
    char humanSymbol;
    DifficultyLevel currentDifficulty;
    static const int MAX_DEPTH = 9;
    std::stack<std::pair<int, int>> aiMoveHistory;
};

#endif // AIPLAYER_H
