#include "mainwindow.hh"
#include "ui_mainwindow.h"
#include <QGraphicsRectItem>
#include <QTimer>
#include <QKeyEvent>
#include <QDir>



#include <tetromino.hh>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    l_pressed_(false),
    r_pressed_(false),
    d_pressed_(false)
{
    ui->setupUi(this);

    //Graphics scenes in which to draw tetrominos.
    scene_ = new QGraphicsScene(this);
    next_block_ = new QGraphicsScene(this);
    hold_block_ = new QGraphicsScene(this);


    // The graphicsView objects are placed on the window
    // at the following coordinates
    int top_margin = 150; // y coordinate
    int hold_left_margin = 25; // x coordinate
    int gameboard_left_margin = hold_left_margin + SQUARE_SIDE*4 + 50;
    int next_left_margin = BORDER_RIGHT+gameboard_left_margin+100;

    // The width of the graphicsView is BORDER_RIGHT added by 2,
    // since the borders take one pixel on each side
    // (1 on the left, and 1 on the right).
    // Similarly, the height of the graphicsView is BORDER_DOWN added by 2.
    ui->graphicsView->setGeometry(gameboard_left_margin, top_margin,
                                  BORDER_RIGHT + 2, BORDER_DOWN + 2);
    ui->graphicsView->setScene(scene_);

    // The width of the scene_ is BORDER_RIGHT decreased by 1 and
    // the height of it is BORDER_DOWN decreased by 1, because
    scene_->setSceneRect(0, 0, BORDER_RIGHT - 1, BORDER_DOWN - 1);

    // Similarily to graphicsView object, 2 is added to width and length to
    // compensate for borders.
    ui->nextPieceView->setGeometry(next_left_margin, top_margin, SQUARE_SIDE*5+2,
                                   SQUARE_SIDE*4+2);
    ui->nextPieceView->setScene(next_block_);
    next_block_->setSceneRect(SQUARE_SIDE*4, SQUARE_SIDE*4, 0, 0);

    // Similarily to graphicsView object, 2 is added to width and length to
    // compensate for borders.
    ui->holdPieceView->setGeometry(hold_left_margin, top_margin, SQUARE_SIDE*5+2,
                                   SQUARE_SIDE*4+2);
    ui->holdPieceView->setScene(hold_block_);
    hold_block_->setSceneRect(0, 0 , 0, 0);

    //Creating loop for background music and assigning it from resources


    playlist = new QMediaPlaylist();
    playlist->addMedia(QUrl("qrc:/sounds/bcmusic.mp3"));
    playlist->setPlaybackMode(QMediaPlaylist::Loop);

    music = new QMediaPlayer();
    line_clear_sound = new QMediaPlayer();
    line_clear_sound->setMedia(QUrl("qrc:/sounds/clearedSound.mp3"));

    music->setVolume(5);
    music->setPlaylist(playlist);




    // Setting random engine ready for the first real call.
    int seed = time(0); // You can change seed value for testing purposes
    randomEng.seed(seed);
    distr = std::uniform_int_distribution<int>(0, NUMBER_OF_TETROMINOS - 1);
    distr(randomEng); // Wiping out the first random number (which is almost always 0)

    game_started = false;
    paused = false;

    // initializing all used variables.
    points = 0;
    tetrises = 0;
    all_cleared_lines = 0;
    seconds = 0;
    minutes = 0;
    current_piece = distr(randomEng);

    // Setting timers for clock and falling
    fall_timer_.setSingleShot(false);
    clock_timer_.setSingleShot(false);
    move_timer_.setSingleShot(false);
    connect(&fall_timer_, &QTimer::timeout, this, &MainWindow::tetrominoFall);
    connect(&clock_timer_, &QTimer::timeout, this, &MainWindow::gameTimer);
    connect(&move_timer_, &QTimer::timeout, this, &MainWindow::moveFunction);



}

MainWindow::~MainWindow()
{
    delete ui;
    scene_->clear();
    hold_block_->clear();
    next_block_->clear();
    delete scene_;
    delete hold_block_;
    delete next_block_;

    for (Block* block : fallen_blocks){
        delete block;
    }
}


MainWindow::Piece MainWindow::newTetromino(int tetromino,
                                           QGraphicsScene* scene)
{
    Piece piece;
    for (blockInfo block : tetrominos.at(tetromino)){
        piece.push_back({block.x, block.y,
                         createBlock(block.x, block.y, block.color,scene)});
    }

    return piece;
}

void MainWindow::keyPressEvent(QKeyEvent* event) {

    if(!game_started || paused){
        return;
    }

    switch (event->key()) {

        case Qt::Key_A :
            l_pressed_ = true;
            break;
        case Qt::Key_S :
            d_pressed_ = true;
            break;
        case Qt::Key_W :
            rotate();
            break;
        case Qt::Key_D :
            r_pressed_ = true;
            break;
        case Qt::Key_Shift :
            while (canMoveDown()){
                tetrominoFall();
            }
            tetrominoFall();
            break;
        case Qt::Key_C :
            holdPiece();
            break;
     }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key()) {

        case Qt::Key_A :
            l_pressed_ = false;
            break;
        case Qt::Key_S :
            d_pressed_ = false;
            break;
        case Qt::Key_D :
            r_pressed_ = false;
            break;
        }
}

void MainWindow::tetrominoFall()
{
    if (canMoveDown()){

        for(Block& block : current_tetromino){
            ++block.y;
            block.graphic->moveBy(0,STEP);
        }
    }
    else{
        for (Block& block: current_tetromino){
            Block* to_bottom = new Block{block};
            fallen_blocks.push_back(to_bottom);
        }

        if (gameFinished()){
            fall_timer_.stop();
            music->stop();
            clock_timer_.stop();
            game_started = false;
            createEndBox();
        }

        else{
            clearLine();
            spawnPiece();
        }
    }
    return;

}

void MainWindow::spawnPiece()
{

    if (current_tetromino.empty()){
        next_piece = distr(randomEng);
        next_tetromino = newTetromino(next_piece, next_block_);
        current_tetromino = newTetromino(current_piece, scene_);
    }
    else{
        for (Block block : next_tetromino){
            delete block.graphic;
        }
        next_tetromino.clear();
        current_piece = next_piece;
        current_tetromino = newTetromino(current_piece, scene_);
        next_piece = distr(randomEng);
        next_tetromino = newTetromino(next_piece, next_block_);
    }
    in_hold = false;
    speed = 1100 - 100*level;
    fall_timer_.start(speed);
}

QGraphicsRectItem* MainWindow::createBlock(int x, int y, QBrush color,QGraphicsScene* scene)
{
    QPen blackPen(Qt::black);
    blackPen.setWidth(2);
    block_ = scene->addRect(x*STEP,y*STEP,SQUARE_SIDE,SQUARE_SIDE,blackPen,color);

    return block_;
}

void MainWindow::holdPiece()
{

    if (!in_hold){
        for (Block block : current_tetromino ){
            delete block.graphic;
        }

        if (held_tetromino.empty()){
            held_tetromino = newTetromino(current_piece, hold_block_);
            held_piece = current_piece;
            spawnPiece();
        }
        else{
            for (Block block : held_tetromino ){
                delete block.graphic;
            }
            held_tetromino = newTetromino(current_piece, hold_block_);
            current_tetromino = newTetromino(held_piece, scene_);

            int temp = held_piece;
            held_piece = current_piece;
            current_piece = temp;

        }
        in_hold = true;
    }

}

void MainWindow::clearLine()
{
    if (fallen_blocks.empty()){
        return;
    }
    else{
        // Vector with information about blocks on every line
        std::vector<std::vector<Block*>> grid_line;
        int cleared_lines = 0;
        for (int i = 0 ; i <= 23;++i){
            grid_line.push_back({});
        }

        for (Block* block : fallen_blocks){
            grid_line.at(block->y).push_back(block);
        }

        for (int row = 0; row < ROWS; ++row){
            if (grid_line.at(row).size()==12){
                ++cleared_lines;
                for (Block* to_remove : grid_line.at(row)){
                    fallen_blocks.erase(std::find(fallen_blocks.begin(),fallen_blocks.end(),to_remove));

                    delete to_remove->graphic;
                    delete to_remove;
                }
                for (Block* block : fallen_blocks){
                    if (block->y < row){
                        ++block->y;
                        block->graphic->moveBy(0,STEP);
                    }
                }
            }
        }
        if (cleared_lines > 0){
            pointCounter(cleared_lines);
        }
        grid_line.clear();
    }
}

void MainWindow::pointCounter(int removed_lines)
{
    if (removed_lines == 1){
        points += 40*level;
    }
    else if (removed_lines == 2){
        points += 100*level;
    }
    else if(removed_lines == 3){
        points += 300*level;
    }
    else if(removed_lines == 4){
        points += 1200*level;
        ++tetrises;
    }

    // Difficulty gets higher every 15 cleared lines

    for (int i = 0; i <= removed_lines-1; ++i){
        if (all_cleared_lines % 15 == 0 && all_cleared_lines != 0 ){
            ++level;
            ui->lcdLevel->display(level);
        }
        ++all_cleared_lines;     
    }
    ui->lcdPoints->display(points);
}

void MainWindow::moveFunction()
{
    if (l_pressed_ && canMoveLeft()){
        for(Block& block : current_tetromino){
            --block.x;
            block.graphic->moveBy(-STEP,0);
        }
    }

    if (r_pressed_ && canMoveRight()){
        for(Block& block : current_tetromino){
            ++block.x;
            block.graphic->moveBy(STEP,0);
        }
    }
    if (d_pressed_){
        tetrominoFall();
    }
}

bool MainWindow::canMoveLeft()
{

    for (Block block: current_tetromino){
        if (block.x <= 0){
            return false;
        }
        else if (!fallen_blocks.empty()){
            for (Block* bottom_block: fallen_blocks){
                if (block.x == bottom_block->x+1 &&
                        block.y == bottom_block->y){
                    return false;
                }
            }
        }
    }
    return true;
}

bool MainWindow::canMoveRight()
{
    for (Block block: current_tetromino){
        if (block.x >= COLUMNS-1 ){
            return false;
        }
        else if (!fallen_blocks.empty()){
            for (Block* bottom_block: fallen_blocks){
                if (block.x == bottom_block->x-1 &&
                        block.y == bottom_block->y){
                    return false;
                }
            }
        }
    }
    return true;
}

bool MainWindow::canMoveDown()
{
    for (Block block: current_tetromino){
        if (block.y >= ROWS-1){
            fall_timer_.stop();
            return false;
        }

        else if (!fallen_blocks.empty()){
            for (Block* bottom_block: fallen_blocks){
                if (block.y == bottom_block->y-1 &&
                        block.x == bottom_block->x){
                    fall_timer_.stop();
                    return false;
                }
            }
        }
    }
    return true;
}

void MainWindow::rotate()
{
    if (current_piece == SQUARE){
        return;
    }

    std::vector<std::vector<int>> newCoordinates;
    for (int i = 0 ; i <= 3;++i){
        newCoordinates.push_back({});
    }

    int pivot_x = current_tetromino[0].x;
    int pivot_y = current_tetromino[0].y;
    int i = 0;
    // Every blocks coordiantes get multiplied by rotation matrix
    // [0,-1;1,0] with first block being the axis.
    for (Block& block : current_tetromino){
        int tempX = (block.x-pivot_x);
        int tempY = (block.y-pivot_y);
        int VX = 0*tempX+(-1)*tempY;
        int VY = 1*tempX+0*tempY;
        int newX = pivot_x + VX;
        int newY = pivot_y + VY;

        // Checking collision with left,right and bottom border
        if (newX < 0 || newX > COLUMNS-1 || newY >= ROWS){
            return ;
        }
        // Checking bollision with already fallen blocks
        for (Block* bottom_block: fallen_blocks){
            if (newX == bottom_block->x &&
                    newY == bottom_block->y){
                return;
            }
        }
        newCoordinates.at(i).push_back(newX);
        newCoordinates.at(i).push_back(newY);
        ++i;
    }
    i = 0;
    // If block doesn't collide with border or fallen pieces, rotated coordinates
    // are applied to tetromino.
    for (Block& block : current_tetromino){
        block.x = newCoordinates[i][0];
        block.y = newCoordinates[i][1];
        // Replacing graphical block to match rotated block.
        delete block.graphic;
        block.graphic = createBlock(newCoordinates[i][0],newCoordinates[i][1],
                tetrominos[current_piece].at(i).color,scene_);
        ++i;
    }
}

void MainWindow::createEndBox()
{
    QMessageBox msgBox;
    QString points_msg = QString("Your score was: \n %1 points").arg(points);
    msgBox.setWindowTitle("Game Finished");
    msgBox.setText(points_msg);
    double tetris_percent;
    if (all_cleared_lines == 0){
        tetris_percent = 0;
    }
    else{
        tetris_percent = (tetrises/(all_cleared_lines-tetrises*4))*100;
    }
    QString tetris_msg = QString("Percent of tetrises was: %1 %").arg(tetris_percent);
    msgBox.setInformativeText(tetris_msg);
    msgBox.exec();
    restartGame();
}

void MainWindow::levelSelect()
{
    int set_level = 0;
    QInputDialog level_select;

    while (set_level > 10 || set_level < 1){
        set_level = level_select.getInt(this, tr("Tetris"),
                                        tr("Select level (1-10):"));
    }
    level = set_level;
    ui->lcdLevel->display(level);


}

bool MainWindow::gameFinished()
{
    for (Block* block: fallen_blocks){
        if (block->y <= BORDER_UP || level == 11){
            return true;
        }
    }
    return false;
}

void MainWindow::restartGame()
{
    ui->startButton->setDisabled(false);
    points = 0;
    tetrises = 0;
    all_cleared_lines = 0;
    seconds = 0;
    minutes = 0;
    current_piece = distr(randomEng);

    scene_->clear();
    hold_block_->clear();
    next_block_->clear();

    for (Block* block : fallen_blocks){
        delete block;
    }

    current_tetromino.clear();
    held_tetromino.clear();
    next_tetromino.clear();
    fallen_blocks.clear();

}

void MainWindow::on_startButton_clicked()
{
    levelSelect();
    clock_timer_.start(1000);
    move_timer_.start(100);
    music->play();
    game_started = true;
    ui->startButton->setDisabled(true);
    spawnPiece();
}



void MainWindow::on_pauseButton_clicked()
{
    if (!game_started){
        return;
    }

    if (!paused){
        fall_timer_.stop();
        clock_timer_.stop();
        ui->pauseButton->setText("Resume");
        paused = true;
    }
    else{
        fall_timer_.start(speed);
        clock_timer_.start(1000);
        ui->pauseButton->setText("Pause");
        paused = false;
    }
}

void MainWindow::gameTimer()
{
    ++seconds;
    if (seconds == 60){
        ++minutes;
        seconds = 0;
        ui->lcdMinutes->display(minutes);
    }

    ui->lcdSeconds->display(seconds);
}
