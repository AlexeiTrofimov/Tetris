/* Teris
 * ----------
 * Desc:
 * Program contains game of tetris with most of the commonly found features of
 * this game implemented. For in depth description of this program
 * check instructions.txt.
 * -----------
 * Program author:
 *
 * Name: Alexei Trofimov
 * Student number: 281986
 * UserID: trofimov
 * E-Mail: alexei.trofimov@tuni.fi
 * */


#include "mainwindow.hh"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
