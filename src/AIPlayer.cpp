#include "../include/AIPlayer.h"

AIPlayer::AIPlayer(char aiSym, char humanSym, DifficultyLevel difficulty)
    : aiSymbol(aiSym), humanSymbol(humanSym), currentDifficulty(difficulty) {}

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
    std::pair<int, int> move;

    switch (currentDifficulty) {
    case DifficultyLevel::EASY:
        move = getRandomMove(board);
        break;
    case DifficultyLevel::MEDIUM:
        move = getMediumMove(board);
        break;
    case DifficultyLevel::HARD:
    default:
        move = getHardMove(board);
        break;
    }

    if (move.first != -1 && move.second != -1) {
        pushAIMove(move.first, move.second);
    }

    return move;
}

std::pair<int, int> AIPlayer::getRandomMove(const GameBoard& board) {
    auto moves = board.getAvailableMoves();
    if (moves.empty()) {
        return {-1, -1};
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, moves.size() - 1);

    return moves[dis(gen)];
}

std::pair<int, int> AIPlayer::getMediumMove(const GameBoard& board) {
    auto criticalMove = getCriticalMove(board);
    if (criticalMove.first != -1) {
        return criticalMove;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 100);

    if (dis(gen) <= 40) {
        return getRandomMove(board);
    } else {
        return getLimitedMinimax(board, 3);
    }
}

std::pair<int, int> AIPlayer::getHardMove(const GameBoard& board) {
    GameBoard tempBoard = board;
    int bestScore = INT_MIN;
    std::pair<int, int> bestMove = {-1, -1};

    auto moves = board.getAvailableMoves();
    if (moves.empty()) {
        return bestMove;
    }

    for (const auto& move : moves) {
        GameBoard testBoard = board;
        testBoard.makeMove(move.first, move.second, aiSymbol);
        int score = minimax(testBoard, 0, false, INT_MIN, INT_MAX);

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }

    return bestMove;
}

std::pair<int, int> AIPlayer::getCriticalMove(const GameBoard& board) {
    auto moves = board.getAvailableMoves();

    for (const auto& move : moves) {
        GameBoard testBoard = board;
        testBoard.makeMove(move.first, move.second, aiSymbol);
        if (testBoard.checkWin() != GameResult::ONGOING) {
            return move;
        }
    }

    for (const auto& move : moves) {
        GameBoard testBoard = board;
        testBoard.makeMove(move.first, move.second, humanSymbol);
        if (testBoard.checkWin() != GameResult::ONGOING) {
            return move;
        }
    }

    return {-1, -1};
}

std::pair<int, int> AIPlayer::getLimitedMinimax(const GameBoard& board, int maxDepth) {
    int bestScore = INT_MIN;
    std::pair<int, int> bestMove = {-1, -1};

    auto moves = board.getAvailableMoves();
    if (moves.empty()) {
        return bestMove;
    }

    for (const auto& move : moves) {
        GameBoard testBoard = board;
        testBoard.makeMove(move.first, move.second, aiSymbol);
        int score = limitedMinimax(testBoard, 0, false, INT_MIN, INT_MAX, maxDepth);

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }

    return bestMove;
}

int AIPlayer::limitedMinimax(GameBoard& board, int depth, bool isMaximizing, int alpha, int beta, int maxDepth) {
    GameResult result = board.checkWin();

    if (depth >= maxDepth) {
        return evaluatePosition(board);
    }

    if (result == GameResult::PLAYER1_WIN || result == GameResult::PLAYER2_WIN) {
        if ((result == GameResult::PLAYER1_WIN && aiSymbol == 'X') ||
            (result == GameResult::PLAYER2_WIN && aiSymbol == 'O')) {
            return 10 - depth;
        } else {
            return depth - 10;
        }
    }

    if (result == GameResult::TIE) {
        return 0;
    }

    if (isMaximizing) {
        int maxEval = INT_MIN;
        auto moves = board.getAvailableMoves();

        for (const auto& move : moves) {
            board.makeMove(move.first, move.second, aiSymbol);
            int eval = limitedMinimax(board, depth + 1, false, alpha, beta, maxDepth);
            board.makeMove(move.first, move.second, ' ');

            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);

            if (beta <= alpha) {
                break;
            }
        }
        return maxEval;
    } else {
        int minEval = INT_MAX;
        auto moves = board.getAvailableMoves();

        for (const auto& move : moves) {
            board.makeMove(move.first, move.second, humanSymbol);
            int eval = limitedMinimax(board, depth + 1, true, alpha, beta, maxDepth);
            board.makeMove(move.first, move.second, ' ');

            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);

            if (beta <= alpha) {
                break;
            }
        }
        return minEval;
    }
}

int AIPlayer::evaluatePosition(const GameBoard& board) {
    int aiScore = 0;

    for (int i = 0; i < 3; i++) {
        aiScore += evaluateLine(board.getCell(i, 0), board.getCell(i, 1), board.getCell(i, 2));
        aiScore += evaluateLine(board.getCell(0, i), board.getCell(1, i), board.getCell(2, i));
    }

    aiScore += evaluateLine(board.getCell(0, 0), board.getCell(1, 1), board.getCell(2, 2));
    aiScore += evaluateLine(board.getCell(0, 2), board.getCell(1, 1), board.getCell(2, 0));

    return aiScore;
}

int AIPlayer::evaluateLine(char cell1, char cell2, char cell3) {
    int aiCount = 0;
    int humanCount = 0;

    if (cell1 == aiSymbol) aiCount++;
    else if (cell1 == humanSymbol) humanCount++;

    if (cell2 == aiSymbol) aiCount++;
    else if (cell2 == humanSymbol) humanCount++;

    if (cell3 == aiSymbol) aiCount++;
    else if (cell3 == humanSymbol) humanCount++;

    if (aiCount > 0 && humanCount > 0) return 0;

    if (aiCount == 3) return 100;
    if (aiCount == 2) return 10;
    if (aiCount == 1) return 1;

    if (humanCount == 3) return -100;
    if (humanCount == 2) return -10;
    if (humanCount == 1) return -1;

    return 0;
}

int AIPlayer::minimax(GameBoard& board, int depth, bool isMaximizing, int alpha, int beta) {
    GameResult result = board.checkWin();

    if (result == GameResult::PLAYER1_WIN || result == GameResult::PLAYER2_WIN) {
        if ((result == GameResult::PLAYER1_WIN && aiSymbol == 'X') ||
            (result == GameResult::PLAYER2_WIN && aiSymbol == 'O')) {
            return 10 - depth;
        } else {
            return depth - 10;
        }
    }

    if (result == GameResult::TIE) {
        return 0;
    }

    if (isMaximizing) {
        int maxEval = INT_MIN;
        auto moves = board.getAvailableMoves();

        for (const auto& move : moves) {
            board.makeMove(move.first, move.second, aiSymbol);
            int eval = minimax(board, depth + 1, false, alpha, beta);
            board.makeMove(move.first, move.second, ' ');

            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);

            if (beta <= alpha) {
                break;
            }
        }
        return maxEval;
    } else {
        int minEval = INT_MAX;
        auto moves = board.getAvailableMoves();

        for (const auto& move : moves) {
            board.makeMove(move.first, move.second, humanSymbol);
            int eval = minimax(board, depth + 1, true, alpha, beta);
            board.makeMove(move.first, move.second, ' ');

            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);

            if (beta <= alpha) {
                break;
            }
        }
        return minEval;
    }
}

int AIPlayer::evaluateBoard(const GameBoard& board) {
    GameResult result = board.checkWin();

    if (result == GameResult::PLAYER1_WIN || result == GameResult::PLAYER2_WIN) {
        if ((result == GameResult::PLAYER1_WIN && aiSymbol == 'X') ||
            (result == GameResult::PLAYER2_WIN && aiSymbol == 'O')) {
            return 10;
        } else {
            return -10;
        }
    }

    return 0;
}
