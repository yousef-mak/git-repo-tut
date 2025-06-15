#include <QApplication>
#include "tictactoegame.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TicTacToeGame game;
    game.show();

    return app.exec();
}
