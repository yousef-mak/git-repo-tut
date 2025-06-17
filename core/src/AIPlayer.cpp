#include "../include/AIPlayer.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

AIPlayer::AIPlayer(char aiSym, char humanSym, DifficultyLevel difficulty)
    : aiSymbol(aiSym), humanSymbol(humanSym), currentDifficulty(difficulty) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

void AIPlayer::setDifficulty(DifficultyLevel difficulty) {
    currentDifficulty = difficulty;
}

void AIPlayer::pushAIMove(int row, int col) {
    aiMoveHistory.push({row, col});
}

std::pair<int, int> AIPlayer::popAIMove() {
    if (!aiMoveHistory.empty()) {
        std::pair<int, int> move = aiMoveHistory.top();
        aiMoveHistory.pop();
        return move;
    }
    return {-1, -1};
}

bool AIPlayer::hasAIMoveHistory() {
    return !aiMoveHistory.empty();
}

void AIPlayer::clearAIMoveHistory() {
    while (!aiMoveHistory.empty()) {
        aiMoveHistory.pop();
    }
}

std::pair<int, int> AIPlayer::getBestMove(const GameBoard& board) {
    std::pair<int, int> move = findBestMove(board);

    if (move.first != -1 && move.second != -1) {
        pushAIMove(move.first, move.second);
    }

    return move;
}

std::pair<int, int> AIPlayer::findBestMove(const GameBoard& board, int forcedChance) {
    // Get all available moves
    std::vector<std::pair<int, int>> moves;
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            if (board.getCell(r, c) == ' ') {  // Assuming empty cells are ' '
                moves.emplace_back(r, c);
            }
        }
    }

    if (moves.empty()) {
        return {-1, -1};
    }

    // Generate random chance if not forced
    int chance = (forcedChance >= 0) ? forcedChance : (std::rand() % 100);

    if (currentDifficulty == DifficultyLevel::EASY) {
        // 80% random, 20% shallow Minimax (depth=1)
        if (chance < 80) {
            return moves[std::rand() % moves.size()];
        }

        // Use limited minimax with depth 1
        int bestVal = std::numeric_limits<int>::min();
        std::pair<int, int> bestMove = moves[0];

        for (const auto& [r, c] : moves) {
            GameBoard tmp = board;
            tmp.makeMove(r, c, aiSymbol);
            int val = minimaxLimited(tmp, 0, false,std::numeric_limits<int>::min(),std::numeric_limits<int>::max(),1);

            if (val > bestVal) {
                bestVal = val;
                bestMove = {r, c};
            }
        }
        return bestMove;
    }
    else if (currentDifficulty == DifficultyLevel::MEDIUM) {
        // 50% random, 50% limited minimax (depth=2)
        if (chance < 50) {
            return moves[std::rand() % moves.size()];
        }

        int bestVal = std::numeric_limits<int>::min();
        std::pair<int, int> bestMove = moves[0];

        for (const auto& [r, c] : moves) {
            GameBoard tmp = board;
            tmp.makeMove(r, c, aiSymbol);
            int val = minimaxLimited(tmp, 0, false,std::numeric_limits<int>::min(),std::numeric_limits<int>::max(),2);
            if (val > bestVal) {
                bestVal = val;
                bestMove = {r, c};
            }
        }
        return bestMove;
    }
    else { // HARD difficulty
        // 10% random, 90% full minimax
        if (chance < 10) {
            return moves[std::rand() % moves.size()];
        }

        // Use full minimax for optimal play
        int bestVal = std::numeric_limits<int>::min();
        std::pair<int, int> bestMove = moves[0];

        for (const auto& [r, c] : moves) {
            GameBoard tmp = board;
            tmp.makeMove(r, c, aiSymbol);
            int val = minimax(tmp, 0, false,std::numeric_limits<int>::min(),std::numeric_limits<int>::max());
            if (val > bestVal) {
                bestVal = val;
                bestMove = {r, c};
            }
        }
        return bestMove;
    }
}

int AIPlayer::minimax(GameBoard board, int depth, bool isMax, int alpha, int beta) {
    // Check terminal states
    GameResult result = board.checkWin();

    if (result != GameResult::ONGOING) {
        if (result == GameResult::TIE) {
            return 0;
        }

        // Determine who won
        bool aiWon = ((result == GameResult::PLAYER1_WIN && aiSymbol == 'X') ||(result == GameResult::PLAYER2_WIN && aiSymbol == 'O'));

        if (aiWon) {
            return 10 - depth;  // AI wins, prefer shorter paths
        } else {
            return depth - 10;  // Human wins, prefer longer paths
        }
    }

    if (isMax) {
        int best = std::numeric_limits<int>::min();
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                if (board.getCell(r, c) == ' ') {
                    GameBoard tmp = board;
                    tmp.makeMove(r, c, aiSymbol);
                    int val = minimax(tmp, depth + 1, false, alpha, beta);
                    best = std::max(best, val);
                    alpha = std::max(alpha, best);
                    if (beta <= alpha) break;
                }
            }
            if (beta <= alpha) break;
        }
        return best;
    } else {
        int best = std::numeric_limits<int>::max();
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                if (board.getCell(r, c) == ' ') {
                    GameBoard tmp = board;
                    tmp.makeMove(r, c, humanSymbol);
                    int val = minimax(tmp, depth + 1, true, alpha, beta);
                    best = std::min(best, val);
                    beta = std::min(beta, best);
                    if (beta <= alpha) break;
                }
            }
            if (beta <= alpha) break;
        }
        return best;
    }
}

int AIPlayer::minimaxLimited(GameBoard board, int depth, bool isMax,int alpha, int beta, int maxDepth) {
    // Check terminal states
    GameResult result = board.checkWin();

    if (result != GameResult::ONGOING) {
        if (result == GameResult::TIE) {
            return 0;
        }

        bool aiWon = ((result == GameResult::PLAYER1_WIN && aiSymbol == 'X') || (result == GameResult::PLAYER2_WIN && aiSymbol == 'O'));
        if (aiWon) {
            return 10 - depth;
        } else {
            return depth - 10;
        }
    }

    // If we've reached max depth, return neutral evaluation
    if (depth >= maxDepth) {
        return 0;  // Could implement a heuristic here
    }

    if (isMax) {
        int best = std::numeric_limits<int>::min();
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                if (board.getCell(r, c) == ' ') {
                    GameBoard tmp = board;
                    tmp.makeMove(r, c, aiSymbol);
                    int val = minimaxLimited(tmp, depth + 1, false, alpha, beta, maxDepth);
                    best = std::max(best, val);
                    alpha = std::max(alpha, best);
                    if (beta <= alpha) break;
                }
            }
            if (beta <= alpha) break;
        }
        return best;
    } else {
        int best = std::numeric_limits<int>::max();
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                if (board.getCell(r, c) == ' ') {
                    GameBoard tmp = board;
                    tmp.makeMove(r, c, humanSymbol);
                    int val = minimaxLimited(tmp, depth + 1, true, alpha, beta, maxDepth);
                    best = std::min(best, val);
                    beta = std::min(beta, best);
                    if (beta <= alpha) break;
                }
            }
            if (beta <= alpha) break;
        }
        return best;
    }
}

std::string AIPlayer::difficultyToString(DifficultyLevel diff) {
    switch (diff) {
    case DifficultyLevel::EASY: return "easy";
    case DifficultyLevel::MEDIUM: return "medium";
    case DifficultyLevel::HARD: return "hard";
    default: return "hard";
    }
}
