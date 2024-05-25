#include "subtitle.h"

Subtitle::Subtitle(QWidget *parent)
	: QMainWindow(parent)
	, timer(new QTimer(this))
	, currentFramePositon(0)
	, operationWindow(new OperationWindow())
    , CurrentReplies(0)
{
	ui.setupUi(this);
	/* 禁用最大化 */
	setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
	setWindowTitle("Embed-Subtitle-Extractor-Lite");
	setWindowIcon(QIcon(":/icon.ico"));

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

	ui.textBrowser->setStyleSheet("QWidget#textBrowser{border-image:url(:/Image/TextBillboardUp.png);}");
	ui.centralWidget->setStyleSheet("QWidget#centralWidget{background-image:url(:/Image/Billboard.png);}");
	ui.HandlePage->setStyleSheet("QWidget#HandlePage{border-image:url(:/Image/rockBackground.png);}");
	ui.openVideoButton->setStyleSheet(buttonStyleSheet);
	ui.resquestButton->setStyleSheet(buttonStyleSheet);
	ui.stopExtractButton->setStyleSheet(buttonStyleSheet);
	ui.subtitleExportButton->setStyleSheet(buttonStyleSheet);
	 //链接跳转
	ui.textBrowser->setOpenExternalLinks(true);
	// 加载md文档
	ui.textBrowser->setSource(QUrl::fromLocalFile(":/Markdown/About.md"));

	ui.FrameScrollBar->setEnabled(false);
	//ui.subtitleExportButton->setEnabled(false);
	/* 视频处理页面 */
	handleLabel = new QLabel(ui.HandlePage);
	upperLabel = new QLabel(ui.HandlePage);
	upperLabel->setStyleSheet("border:0;background-color: rgba(154, 154, 154, 0.6);");
	upperLabel->raise();

	/* MoveAreaSlider调节upperLabel区域的y值
	从 0 到  ui.HandlePage->height() - upperLabel->height()*/
	ui.MoveAreaSlider->setRange(0, ui.HandlePage->height() - upperLabel->height());
	ui.MoveAreaSlider->setValue(ui.HandlePage->height() - upperLabel->height());
	ui.MoveAreaSlider->setInvertedAppearance(true);
	/* AdjustHeightSlider调节upperLabel高度 */
	ui.AdjustHeightSlider->setRange(40, 100);
	ui.AdjustHeightSlider->setValue(100);
	ui.AdjustHeightSlider->setInvertedAppearance(true);

	// 创建定时器，延迟设置窗口的最小和最大尺寸
	QTimer::singleShot(0, [this]() {
		setMinimumSize(this->size());
		setMaximumSize(this->size());
		upperLabel->setGeometry(0, ui.HandlePage->height() - ui.AdjustHeightSlider->value(), ui.HandlePage->width(), ui.AdjustHeightSlider->value());
	});
	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget(handleLabel);
	layout->setContentsMargins(0, 0, 0, 0);
	ui.HandlePage->setLayout(layout);

	QPixmap pixmap(":/Image/videoBackground.png");
	QPixmap scaledPixmap = pixmap.scaled(ui.HandlePage->width(), ui.HandlePage->height(), Qt::KeepAspectRatio);
	handleLabel->setPixmap(pixmap);
	/* 初始化连接 */
	InitConnect();

}

void Subtitle::InitConnect()
{
	// 图片提取
	connect(ui.resquestButton, &QPushButton::clicked, this, &Subtitle::StartExtractSubTitle);

	connect(ui.MoveAreaSlider, &QSlider::valueChanged, [&] {
		if (ui.MoveAreaSlider->value() + ui.AdjustHeightSlider->maximum() > ui.HandlePage->height())
			// 防止过度截取
			ui.AdjustHeightSlider->setEnabled(false);
		else
			ui.AdjustHeightSlider->setEnabled(true);

		ui.MoveAreaSlider->setMaximum(ui.HandlePage->height() - upperLabel->height());
		upperLabel->move(0, ui.MoveAreaSlider->value());

	});
	connect(ui.AdjustHeightSlider, &QSlider::valueChanged, [&] {
		upperLabel->resize(ui.HandlePage->width(), ui.AdjustHeightSlider->value());
	});
	// 当超时时处理图像数据
	connect(timer, &QTimer::timeout, this, &Subtitle::handleFrame);
	// 选择某一字幕帧
	connect(ui.FrameScrollBar, &QScrollBar::valueChanged, this, &Subtitle::ChooseSubtitleFrame);

	/* 打开视频文件 */
	connect(ui.openVideoButton, &QPushButton::clicked, [&] {
		OpenVideoFile(QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择视频文件"), QDir::currentPath(), QString::fromLocal8Bit("视频文件 (*.mp4 *.avi)")));
	});

	/* 导出字幕 */
	connect(ui.subtitleExportButton, &QPushButton::clicked, [&] {
		// 对字幕进行预览和编辑
		operationWindow->show();
	});
	/* 停止提取字幕 */
	connect(ui.stopExtractButton, &QPushButton::clicked, [&] {
		if (timer->isActive())
		{
			CurrentReplies = 0;
			ui.MoveAreaSlider->setEnabled(true);
			ui.FrameScrollBar->setEnabled(true);
			ui.resquestButton->setEnabled(true);
			timer->stop();
		}
	});
	// 连接发送裁剪好的图像帧数据
	connect(this, &Subtitle::CroppedFrameData, this, &Subtitle::SendCroppedFrameData);
	// 连接ReceiveFilePath 发送视频路径信息
	connect(this, &Subtitle::SendFilePath, operationWindow, &OperationWindow::ReceiveFilePath);
	// 连接ReceiveSubtitle 发送字幕信息
	connect(this, &Subtitle::SendSubtitle, operationWindow, &OperationWindow::ReceiveSubtitle);
	// 连接ReceiveEmptyText 发送清空文本
	connect(this, &Subtitle::SendEmptyText, operationWindow, &OperationWindow::ReceiveEmptyText);
}

void Subtitle::handleFrame()
{
	int frameRate = cap.get(cv::CAP_PROP_FPS); // 获取视频帧率
	int totalFrames = cap.get(cv::CAP_PROP_FRAME_COUNT); // 获取视频总帧数
	/* 视频帧提取完毕 */

	if ((currentFramePositon + frameRate) > totalFrames)
	{
		ui.MoveAreaSlider->setEnabled(true);
		ui.FrameScrollBar->setEnabled(true);
		ui.resquestButton->setEnabled(true);
		ui.openVideoButton->setEnabled(true);
		ui.FrameScrollBar->setValue(ui.FrameScrollBar->maximum());
		timer->stop();
		return;
	}
	currentFramePositon += (frameRate -  1);
	cap.set(cv::CAP_PROP_POS_FRAMES, currentFramePositon); // 设置视频位置为当前帧
	cv::Mat frame;
	cap.read(frame);
	cv::resize(frame, frame, cv::Size(ui.HandlePage->width(), ui.HandlePage->height()));

	cv::Rect roi(0, upperLabel->y(), upperLabel->width(), upperLabel->height()); // (x, y, width, height)
	// 裁剪图像
	this->croppedFrame = frame(roi);
	if (frame.empty()) {
		std::cerr << "Empty frame at position " << currentFramePositon << std::endl;
	}
	// 显示
	QImage qImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_BGR888);
	QImage croppedImage(croppedFrame.data, croppedFrame.cols, croppedFrame.rows, croppedFrame.step, QImage::Format_BGR888);

	/* 发送图像数据信号 */
	emit CroppedFrameData(croppedImage);

	handleLabel->setPixmap(QPixmap::fromImage(qImage));
}

void Subtitle::OpenVideoFile(const QString& VideoFilePath)
{
	if (!VideoFilePath.isEmpty()) {
		// 发送当前的视频文件路径
		this->VideoFilePath = VideoFilePath;
		emit SendFilePath(VideoFilePath);
		cap.release();

		cap.open(VideoFilePath.toStdString());
		if (!cap.isOpened()) {
			std::cerr << "Error open video file" << std::endl;
		}

		cap.set(cv::CAP_PROP_POS_FRAMES, 0); // 设置视频初始位置为当前帧
		cv::Mat frame;
		cap.read(frame);
		cv::resize(frame, frame, cv::Size(ui.HandlePage->width(), ui.HandlePage->height()));
		if (frame.empty()) {
			std::cerr << "Empty frame at position " << 0 << std::endl;
		}
		// 显示
		QImage qImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_BGR888);
		handleLabel->setPixmap(QPixmap::fromImage(qImage));

		int totalFrames = cap.get(cv::CAP_PROP_FRAME_COUNT); // 获取视频总帧数
		ui.FrameScrollBar->setRange(0, totalFrames - 1);
		ui.FrameScrollBar->setSingleStep(15);
		ui.FrameScrollBar->setEnabled(true);
	}
}

/* 选择合适的字幕帧 */
void Subtitle::ChooseSubtitleFrame()
{
	cap.set(cv::CAP_PROP_POS_FRAMES, ui.FrameScrollBar->value());
	cv::Mat frame;
	cap.read(frame);
	cv::resize(frame, frame, cv::Size(ui.HandlePage->width(), ui.HandlePage->height()));
	if (frame.empty()) {
		std::cerr << "Empty frame at position " << 0 << std::endl;
	}
	// 显示
	QImage qImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_BGR888);
	handleLabel->setPixmap(QPixmap::fromImage(qImage));
}

// 开始提取字幕
void Subtitle::StartExtractSubTitle()
{
	/* 起始时间 */
	start = std::chrono::high_resolution_clock::now();
	ui.textBrowser->append("\n*********" + QString::fromLocal8Bit("<b>开始提取视频字幕</b>") + "*********\n");
	CurrentReplies = 0;
	emit SendEmptyText();
	// 只有加载了视频才提取
	if (VideoFilePath.isEmpty())
	{
		// 创建并显示警告框
		QString msgTitle = QString::fromLocal8Bit("提示！");
		QString msg = QString::fromLocal8Bit("请选择你的视频文件");
		QMessageBox::warning(this, msgTitle, msg);
		return;
	}
	ui.openVideoButton->setEnabled(false);
	ui.resquestButton->setEnabled(false);
	ui.MoveAreaSlider->setEnabled(false);
	ui.FrameScrollBar->setEnabled(false);

	CroppedFrameVector.clear();
	currentFramePositon = 0;
	timer->start(INTERVALTIME); // 启动定时器
}

// 发送字幕帧
void Subtitle::SendCroppedFrameData(const QImage& croppedImage)
{
	/* 裁剪后的图像数据 */
	QByteArray imageData;
	QBuffer buffer(&imageData);
	buffer.open(QIODevice::WriteOnly);
	croppedImage.save(&buffer, "PNG");
	QString base64Image = QString(imageData.toBase64());

	QJsonObject imageObject;
	imageObject.insert("image", base64Image);

	QJsonObject json;
	json.insert("croppedImage", imageObject);
	QJsonDocument doc(json);

	QNetworkAccessManager* manager = new QNetworkAccessManager(this);

	QNetworkRequest request(QUrl("http://127.0.0.1:5000/receive"));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	QNetworkReply* reply = manager->post(request, doc.toJson());

	connect(reply, &QNetworkReply::finished, [this, manager, reply]() {
		if (reply->error() == QNetworkReply::NoError) {
			QByteArray responseData = reply->readAll();
			QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
			QJsonArray jsonArray = jsonDoc.array();
			
			if (jsonArray.isEmpty()) return;

			++CurrentReplies;
			QStringList SubtitleList;
			ui.textBrowser->append("<b>" + QString::fromLocal8Bit("结果") + QString::number(CurrentReplies) + ":</b>");
			foreach(const QJsonValue &value, jsonArray)
			{
				if (!value.toString().isEmpty())
				{
					ui.textBrowser->append("* " + value.toString());
					SubtitleList.append(value.toString());
				}
			}
			emit SendSubtitle(SubtitleList);
			// 获取结束时间点
			auto end = std::chrono::high_resolution_clock::now();
			// 计算执行时间
			std::chrono::duration<double> duration = end - start;
			QString msg = QString::fromLocal8Bit("耗时：") + QString::number(duration.count()) + QString::fromLocal8Bit(" 秒");
			ui.textBrowser->append("- " + msg);
		}
		else 
		{
			//qDebug() << "Error: " << reply->errorString();
			if (reply->errorString() == "Connection refused")
			{
				ui.textBrowser->append(QString::fromLocal8Bit("* 无法检测到文字！\n* 请检查后台服务是否开启"));
			}
			
		}
		reply->deleteLater();
		manager->deleteLater();
	});
}

Subtitle::~Subtitle()
{
	if (operationWindow)
	{
		delete operationWindow;
		operationWindow = nullptr;
	}
}