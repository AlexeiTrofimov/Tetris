/* Header: Tetromino
 * ----------
 * TIE-02201/TIE-02207 SPRING 2020
 * ----------
 * Initialization coordinates and color
 * for every single block of every tetromino.
 * Blocks are initialized so that first coordinates are rotation axis for
 * given teromino.
 *
 * */

#ifndef TETROMINO_HH
#define TETROMINO_HH
#include <QGraphicsScene>
#include <vector>
#endif // TETROMINO_HH


struct blockInfo{
    int x, y;
    QBrush color;
};

QBrush yellow(Qt::yellow);
QBrush purple(QColor(186,85,211));
QBrush blue(Qt::blue);
QBrush lightBlue(QColor(0,191,255));
QBrush green(Qt::green);
QBrush red(Qt::red);
QBrush orange(QColor(255,165,0));

const std::vector<std::vector<blockInfo>> tetrominos{
    {   //Horizontal
        {5,-1,lightBlue},{7,-1,lightBlue},{6,-1,lightBlue},{4,-1,lightBlue}},
    {   //Left Corner
        {5,-1,blue},{6, -1,blue},{4, -1,blue},{4, -2,blue}},
    {   //Right corner
        {5,-1,orange},{6, -1,orange},{4, -1,orange},{6, -2,orange}},
    {   //Square
        {5, -1,yellow},{6, -2,yellow},{6, -1,yellow},{5, -2,yellow}},
    {   //Step up right
        {5, -1,green},{4, -1,green},{5, -2,green},{6, -2,green}},
    {   //Pyramid
        {5, -1,purple},{6, -1,purple},{5, -2,purple},{4, -1,purple}},
    {   //Step up left
        {5, -1,red},{6, -1,red},{5, -2,red},{4, -2,red}}
};
