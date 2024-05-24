#include "operationwindow.h"

OperationWindow::OperationWindow(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowTitle(QString::fromLocal8Bit("导出字幕"));
	QString buttonStyleSheet = "QPushButton{"
		"color:white;"
		"background-color: rgb(109, 109, 109);"
		"border-left:4px solid rgb(184, 184, 185);"
		"border-top:4px solid rgb(184, 184, 185);"
		"border-right:4px solid rgb(88, 88, 88);"
		"border-bottom:4px solid rgb(88, 88, 88);"
		"}"
		"QPushButton:hover{"
		"color:white;"
		"background-color: rgb(109, 109, 109);"
		"border:5px solid rgb(224, 226, 224);"
		"}";
	ui.centerWidget->setStyleSheet("QWidget#centerWidget{background-image:url(:/Image/Billboard.png);}");
	ui.subtitleEditArea->setStyleSheet("QWidget#subtitleEditArea{border-image:url(:/Image/TextBillboardUp.png);}");
	ui.portSRTButton->setStyleSheet(buttonStyleSheet);
	ui.portTXTButton->setStyleSheet(buttonStyleSheet);
	ui.subtitleEditArea->setStyleSheet("color: rgb(0, 0, 0);");

	/* .txt格式导出 */
	connect(ui.portTXTButton, &QPushButton::clicked, [&] {
		
		QFileInfo fileInfo(VideoFilePath);
		QDir parentDir = fileInfo.dir();
		// 构造路径
		QString siblingFilePath = parentDir.filePath(fileInfo.completeBaseName() + ".txt");

		QFile file(siblingFilePath);
		if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			QString textContent = ui.subtitleEditArea->toPlainText();
			QTextStream out(&file);
			out << textContent;
			file.close();
			QMessageBox::information(this, QString::fromLocal8Bit("成功保存！"), (QString::fromLocal8Bit("文本保存路径：") + siblingFilePath));
		}
		else QMessageBox::warning(this, QString::fromLocal8Bit("Failed"), QString::fromLocal8Bit("Failed to open file for writing"));

	});

	/* .srt格式导出 */
	connect(ui.portSRTButton, &QPushButton::clicked, [&] {
		QMessageBox::warning(this, QString::fromLocal8Bit("提示！"), QString::fromLocal8Bit("暂不支持导出该格式"));
	});
}

void OperationWindow::ReceiveFilePath(const QString& VideoFilePath)
{
	this->VideoFilePath = VideoFilePath;
}

void OperationWindow::ReceiveSubtitle(const QString& Subtitle)
{
	QString currentText = ui.subtitleEditArea->toPlainText();

	// 去重
	if (!currentText.contains(Subtitle)) ui.subtitleEditArea->append(Subtitle);
}

OperationWindow::~OperationWindow() {}