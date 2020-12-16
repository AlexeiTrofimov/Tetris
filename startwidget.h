#ifndef STARTWIDGET_H
#define STARTWIDGET_H

#include <QWidget>

namespace Ui {
class StartWidget;
}

class StartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StartWidget(QWidget *parent = nullptr);
    ~StartWidget();

    int GetLevel();
signals:
    void GameStarted();
private slots:
    void on_startButton_clicked();

    void on_levelSlider_valueChanged(int value);

private:
    Ui::StartWidget *ui;

    int level;
};

#endif // STARTWIDGET_H
