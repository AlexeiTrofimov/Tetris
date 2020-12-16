/* Class: MainWindow
 * ----------
 * TIE-02201/TIE-02207 SPRING 2020
 * ----------
 * Class that contains every function of tetris game. Impemented functions are:
 * Dropping piece,Collison detection, all four terominos, WASD-controls, game end
 * detection, clearing of full lines, showing next piece, hold-functionality,
 * rotating, points counter, game timer, leveling system, restart.
 *
 * */
#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QTimer>
#include <QInputDialog>
#include <QMessageBox>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <random>

#include <startwidget.h>



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private slots:
    void on_startButton_clicked();

    void on_pauseButton_clicked();

private:
    Ui::MainWindow *ui;

    // Scenes for game field, next block and block in hold
    QGraphicsScene* scene_;
    QGraphicsScene* next_block_;
    QGraphicsScene* hold_block_;

    QGraphicsRectItem* block_;

    QMediaPlaylist* playlist;
    QMediaPlayer* music;
    QMediaPlayer* line_clear_sound;

    QTimer fall_timer_;
    QTimer move_timer_;
    QTimer clock_timer_;

    StartWidget start_window_;


    bool l_pressed_;
    bool r_pressed_;
    bool d_pressed_;

    // Constants describing scene coordinates
    const int BORDER_UP = 0;
    const int BORDER_DOWN = 480;
    const int BORDER_LEFT = 0;
    const int BORDER_RIGHT = 240;
    const int NEXT_BORDER = BORDER_RIGHT+150;

    // Used to scale tetromino coordinate changes to match graphical change
    const int STEP = 20;

    // Size of a tetromino component
    const int SQUARE_SIDE = 20;
    // Number of horizontal cells (places for tetromino components)
    const int COLUMNS = BORDER_RIGHT / SQUARE_SIDE;
    // Number of vertical cells (places for tetromino components)
    const int ROWS = BORDER_DOWN / SQUARE_SIDE;

    // Constants for different tetrominos and the number of them
    enum Tetromino_kind {HORIZONTAL,
                         LEFT_CORNER,
                         RIGHT_CORNER,
                         SQUARE,
                         STEP_UP_RIGHT,
                         PYRAMID,
                         STEP_UP_LEFT,
                         NUMBER_OF_TETROMINOS};

    // For randomly selecting the next dropping tetromino
    std::default_random_engine randomEng;
    std::uniform_int_distribution<int> distr;

    // Initalizing clock
    int seconds;
    int minutes;
    void gameTimer();

    // Struct that stores coordinates and graphical interpretation of a single
    // block on a game grid.
    struct Block{
         int x,y;
         QGraphicsRectItem* graphic;

    };

    typedef std::vector<Block> Piece;

    //Stores information about next tetromino.
    Piece next_tetromino;

    //Stores information about currently falling tetromino.
    Piece current_tetromino;

    //Stores information about tetromino put in hold.
    Piece held_tetromino;

    // Restricts player from putting tetromino more than once in a row.
    bool in_hold;

    bool game_started;

    bool paused;

    // Difficulty level of a game.
    int level;

    // Speed of a falling tetromino. Depends on current level.
    int speed;

    // Points that player gets by clearing a line.
    int points;

    // Number of lines that are cleared during the game.
    int all_cleared_lines;

    // Number of tetrises that player gets during the game.
    double tetrises;

    //Vector of Block structs that contains information about already fallen blocks
    std::vector<Block*> fallen_blocks;

    //Index of first dropped tetromino that gets generated before start.
    int current_piece;

    //Index of next dropped tetromino that gets generated every new block.
    int next_piece;

    //Index of held tetromino that changes every time player puts tetromino in hold.
    int held_piece;

    /**
     * @brief tetrominoFall
     * Takes care of dropping tetromino every tick. Calls canmoveDown() to check
     * if piece has touched bottom or fallen tetrominos and then saves it to
     * fallen blocks. Also ends game if gameFinished returns true.
     */
    void tetrominoFall();

    /**
     * @brief spawnPiece
     * Spawns new falling tetromino by creating them using newTetromino. Also
     * creates next tetromino and then assigns it to current on next run.
     * Speed of a tetromino is also changed depending on current level.
     */
    void spawnPiece();

    /**
     * @brief newTetromino
     * @param tetromino, index of tetromino assigned in Tetromino_kind
     * @param scene, scene to which block is created
     * @return vector containing blocks that are assigned to given scene
     */
    Piece newTetromino(int tetromino,QGraphicsScene* scene);

    /**
     * @brief createBlock
     * @param x, x coordinate of a block
     * @param y, y coordinate of a block
     * @param scene, scene whre given block is created
     * @return pointer to graphical representation of a single block of a tetromino
     */
    QGraphicsRectItem* createBlock(int x, int y, QBrush,QGraphicsScene* scene);

    /**
     * @brief holdPiece
     * Allows player to put currently falling tetromino in hold. After tetromino
     * is set to hold, it won't be possible to put tetromino in hold before
     * retrieved tetromino falls completely.
     */
    void holdPiece();

    /**
     * @brief clearLine
     * Checks if any line is filled and removes it.Calss pointCounter to add
     * points for player.
     */
    void clearLine();

    /**
     * @brief pointCounter
     * @param removed_lines, number of lines that were removed by clearLine
     * Adds points for player and changes difficulty every 15 cleared lines.
     */
    void pointCounter(int removed_lines);

    void moveFunction();
    /**
     * @brief canMoveLeft
     * @return true if every block of a tetromino can move to the left.
     * Returns false otherwise.
     */
    bool canMoveLeft();

    /**
     * @brief canMoveRight
     * @return true if every block of a tetromino can move to the right.
     * Returns false otherwise.
     */
    bool canMoveRight();

    /**
     * @brief canMoveDown
     * @return true if every block of a tetromino can move down.
     * Returns false otherwise.
     * @note Also stops timer if tetromino hits bottom or fallen tetrominos.
     */
    bool canMoveDown();

    /**
     * @brief rotate
     * Rotates falling tetromino by multipying its coordinates with rotation matrix.
     * @note Also checks if rotation is possible before assigning new coordinates
     * for blocks.
     */
    void rotate();

    /**
     * @brief createEndBox
     * Creates pop-up window that informs about end of the game. Final score
     * of a player and percent of tetrominos are displayed.
     */
    void createEndBox();
    /**
     * @brief levelSelect
     * Creates pop-up window before starting the game to ask for
     * wanted difficulty level.
     */
    void levelSelect();

    /**
     * @brief gameFinished
     * @return True if tetrominos reach upper border or if player completes
     * level 10.
     */

    void StartGame();

    bool gameFinished();

    /**
     * @brief restartGame
     * Clears all variables in case player wants to play a new game.
     */
    void restartGame();
};

#endif // MAINWINDOW_HH
