#include "startwidget.h"
#include "ui_startwidget.h"

StartWidget::StartWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StartWidget)
{
    ui->setupUi(this);
    ui->levelSlider->setRange(1,10);
}

StartWidget::~StartWidget()
{
    delete ui;
}

int StartWidget::GetLevel()
{
    return level;
}

void StartWidget::on_startButton_clicked()
{
    emit GameStarted();
    this->close();
}


void StartWidget::on_levelSlider_valueChanged(int value)
{
    level = value;
    QString s;
    ui->levelLabel->setText(s.setNum(level));
}
