#pragma once

#include <QWidget>
#include "ui_operationwindow.h"

class OperationWindow : public QWidget
{
	Q_OBJECT

public:
	OperationWindow(QWidget *parent = nullptr);
	~OperationWindow();

private:
	Ui::OperationWindowClass ui;
};
