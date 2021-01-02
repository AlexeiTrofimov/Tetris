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
    int x, y, type;
};


const std::vector<std::vector<blockInfo>> tetrominos{
    {   //Horizontal
        {5,-1,0},{7,-1,0},{6,-1,0},{4,-1,0}},
    {   //Left Corner
        {5,-1,1},{6, -1,1},{4, -1,1},{4, -2,1}},
    {   //Right corner
        {5,-1,2},{6, -1,2},{4, -1,2},{6, -2,2}},
    {   //Square
        {5, -1,3},{6, -2,3},{6, -1,3},{5, -2,3}},
    {   //Step up right
        {5, -1,4},{4, -1,4},{5, -2,4},{6, -2,4}},
    {   //Pyramid
        {5, -1,5},{6, -1,5},{5, -2,5},{4, -1,5}},
    {   //Step up left
        {5, -1,6},{6, -1,6},{5, -2,6},{4, -2,6}}
};
