#pragma once

#include <QtWidgets/qwidget.h>
#include <x64/Debug/uic/ui_OperationWindow.h>
#include <qmessagebox.h>
#include <fstream>
#include <QFileInfo>
#include <QDir>
#include <qdebug.h>

class OperationWindow : public QWidget
{
	Q_OBJECT

public:
	OperationWindow(QWidget *parent = nullptr);
	~OperationWindow();
	void ReceiveFilePath(const QString& VideoFilePath);
	void ReceiveSubtitle(const QStringList& SubtitleList);
	void ReceiveEmptyText();

private:
	Ui::OperationWindowClass ui;
	QString VideoFilePath;
	QStringList LastTextList;
};
