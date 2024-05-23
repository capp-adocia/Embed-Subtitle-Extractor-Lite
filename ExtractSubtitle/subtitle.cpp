#include "subtitle.h"

Subtitle::Subtitle(QWidget *parent)
	: QMainWindow(parent)
	, timer(new QTimer(this))
	, currentFramePositon(0)
	, ImageSubTitleIndex(0)
	, operationWindow(new OperationWindow())
{
	ui.setupUi(this);
	/* ������� */
	setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);

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

	ui.textBrowserWidget->setStyleSheet("QWidget#textBrowserWidget{background-image:url(:/Image/BillboardUp.png);}");
	ui.textBrowser->setStyleSheet("QWidget#textBrowser{border-image:url(:/Image/TextBillboardUp.png);}");
	ui.centralWidget->setStyleSheet("QWidget#centralWidget{border-image:url(:/Image/BillboardUp.png);}");
	//// ������ת
	ui.textBrowser->setOpenExternalLinks(true);
	// ����md�ĵ�
	ui.textBrowser->setSource(QUrl::fromLocalFile(":/Markdown/About.md"));

	ui.FrameScrollBar->setEnabled(false);
	/* ��Ƶ����ҳ�� */
	handleLabel = new QLabel(ui.HandlePage);
	upperLabel = new QLabel(ui.HandlePage);
	upperLabel->setStyleSheet("border:0;background-color:rgba(0, 180, 120, 0.6);"); // ��ɫ����
	upperLabel->raise();

	/* MoveAreaSlider����upperLabel�����yֵ
	�� 0 ��  ui.HandlePage->height() - upperLabel->height()*/
	ui.MoveAreaSlider->setRange(0, ui.HandlePage->height() - upperLabel->height());
	ui.MoveAreaSlider->setValue(ui.HandlePage->height() - upperLabel->height());
	ui.MoveAreaSlider->setInvertedAppearance(true);
	/* AdjustHeightSlider����upperLabel�߶� */
	ui.AdjustHeightSlider->setRange(40, 100);
	ui.AdjustHeightSlider->setValue(100);
	ui.AdjustHeightSlider->setInvertedAppearance(true);

	// ������ʱ�����ӳ����ô��ڵ���С�����ߴ�
	QTimer::singleShot(0, [this]() {
		setMinimumSize(this->size());
		setMaximumSize(this->size());
		upperLabel->setGeometry(0, ui.HandlePage->height() - ui.AdjustHeightSlider->value(), ui.HandlePage->width(), ui.AdjustHeightSlider->value());
	});
	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget(handleLabel);
	layout->setContentsMargins(0, 0, 0, 0);
	ui.HandlePage->setLayout(layout);
	/* ��ʼ������ */
	InitConnect();

}

void Subtitle::InitConnect()
{
	// ͼƬ��ȡ
	connect(ui.resquestButton, &QPushButton::clicked, this, &Subtitle::StartExtractSubTitle);

	connect(ui.MoveAreaSlider, &QSlider::valueChanged, [&] {
		if (ui.MoveAreaSlider->value() + ui.AdjustHeightSlider->maximum() > ui.HandlePage->height())
			// ��ֹ���Ƚ�ȡ
			ui.AdjustHeightSlider->setEnabled(false);
		else
			ui.AdjustHeightSlider->setEnabled(true);

		ui.MoveAreaSlider->setMaximum(ui.HandlePage->height() - upperLabel->height());
		upperLabel->move(0, ui.MoveAreaSlider->value());

	});
	connect(ui.AdjustHeightSlider, &QSlider::valueChanged, [&] {
		upperLabel->resize(ui.HandlePage->width(), ui.AdjustHeightSlider->value());
	});
	// ����ʱʱ����ͼ������
	connect(timer, &QTimer::timeout, this, &Subtitle::handleFrame);
	// ѡ��ĳһ��Ļ֡
	connect(ui.FrameScrollBar, &QScrollBar::valueChanged, this, &Subtitle::ChooseSubtitleFrame);

	/* ����Ƶ�ļ� */
	connect(ui.actionOpen, &QAction::triggered, [&] {
		// ��
		OpenVideoFile(QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("ѡ����Ƶ�ļ�"), QDir::currentPath(), QString::fromLocal8Bit("��Ƶ�ļ� (*.mp4 *.avi)")));
	});
	connect(ui.openVideoButton, &QPushButton::clicked, [&] {
		OpenVideoFile(QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("ѡ����Ƶ�ļ�"), QDir::currentPath(), QString::fromLocal8Bit("��Ƶ�ļ� (*.mp4 *.avi)")));
	});
	/* ֹͣ��ȡ��Ļ */
	connect(ui.stopExtractButton, &QPushButton::clicked, [&] {
		if (timer->isActive())
		{
			ImageSubTitleIndex = 0;
			ui.MoveAreaSlider->setEnabled(true);
			ui.FrameScrollBar->setEnabled(true);
			ui.resquestButton->setEnabled(true);
			timer->stop();
		}
	});
	/* ������Ļ */
	connect(ui.subtitleExportButton, &QPushButton::clicked, this, &Subtitle::ExportSubtitle);



	// ���ӷ��Ͳü��õ�Mat
	connect(this, &Subtitle::CroppedFrameData, this, &Subtitle::SendCroppedFrameData);
}

void Subtitle::handleFrame()
{
	int frameRate = cap.get(cv::CAP_PROP_FPS); // ��ȡ��Ƶ֡��
	int totalFrames = cap.get(cv::CAP_PROP_FRAME_COUNT); // ��ȡ��Ƶ��֡��
	/* ��Ƶ֡��ȡ��� */

	if ((currentFramePositon + frameRate) > totalFrames)
	{
		// ��ȡ����ʱ���
		auto end = std::chrono::high_resolution_clock::now();
		// ����ִ��ʱ��
		std::chrono::duration<double> duration = end - start;
		QString msgTitle = QString::fromLocal8Bit("��Ļ��ȡ���!");
		QString msg = QString::fromLocal8Bit("�ܺ�ʱ��") + QString::number(duration.count()) + QString::fromLocal8Bit(" ��");
		QMessageBox::information(this, msgTitle, msg);

		ImageSubTitleIndex = 0;
		ui.MoveAreaSlider->setEnabled(true);
		ui.FrameScrollBar->setEnabled(true);
		ui.resquestButton->setEnabled(true);
		ui.FrameScrollBar->setValue(ui.FrameScrollBar->maximum());
		timer->stop();
		return;
	}
	currentFramePositon += (frameRate - 1);
	cap.set(cv::CAP_PROP_POS_FRAMES, currentFramePositon); // ������Ƶλ��Ϊ��ǰ֡
	cv::Mat frame;
	cap.read(frame);
	cv::resize(frame, frame, cv::Size(ui.HandlePage->width(), ui.HandlePage->height()));

	cv::Rect roi(0, upperLabel->y(), upperLabel->width(), upperLabel->height()); // (x, y, width, height)
	// �ü�ͼ��
	this->croppedFrame = frame(roi);
	if (frame.empty()) {
		std::cerr << "Empty frame at position " << currentFramePositon << std::endl;
	}
	// ��ʾ
	QImage qImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_BGR888);
	QImage croppedImage(croppedFrame.data, croppedFrame.cols, croppedFrame.rows, croppedFrame.step, QImage::Format_BGR888);

	/* ����ͼ�������ź� */
	emit CroppedFrameData(croppedImage);

	handleLabel->setPixmap(QPixmap::fromImage(qImage));
}

void Subtitle::OpenVideoFile(const QString& VideoFilePath)
{
	if (!VideoFilePath.isEmpty()) {
		// ���浱ǰѡ����ļ�
		this->VideoFilePath = VideoFilePath;
		cap.release();

		cap.open(VideoFilePath.toStdString());
		if (!cap.isOpened()) {
			std::cerr << "Error open video file" << std::endl;
		}

		cap.set(cv::CAP_PROP_POS_FRAMES, 0); // ������Ƶ��ʼλ��Ϊ��ǰ֡
		cv::Mat frame;
		cap.read(frame);
		cv::resize(frame, frame, cv::Size(ui.HandlePage->width(), ui.HandlePage->height()));
		if (frame.empty()) {
			std::cerr << "Empty frame at position " << 0 << std::endl;
		}
		// ��ʾ
		QImage qImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_BGR888);
		handleLabel->setPixmap(QPixmap::fromImage(qImage));

		int totalFrames = cap.get(cv::CAP_PROP_FRAME_COUNT); // ��ȡ��Ƶ��֡��
		ui.FrameScrollBar->setRange(0, totalFrames - 1);
		ui.FrameScrollBar->setSingleStep(15);
		ui.FrameScrollBar->setEnabled(true);
	}
}

/* ѡ����ʵ���Ļ֡ */
void Subtitle::ChooseSubtitleFrame()
{
	cap.set(cv::CAP_PROP_POS_FRAMES, ui.FrameScrollBar->value());
	cv::Mat frame;
	cap.read(frame);
	cv::resize(frame, frame, cv::Size(ui.HandlePage->width(), ui.HandlePage->height()));
	if (frame.empty()) {
		std::cerr << "Empty frame at position " << 0 << std::endl;
	}
	// ��ʾ
	QImage qImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_BGR888);
	handleLabel->setPixmap(QPixmap::fromImage(qImage));
}

// ��ʼ��ȡ��Ļ
void Subtitle::StartExtractSubTitle()
{
	/* ��ʼʱ�� */
	start = std::chrono::high_resolution_clock::now();
	ui.textBrowser->append(QString::fromLocal8Bit("*********��ʼ��ȡ��Ƶ��Ļ*********\n"));
	
	// ֻ�м�������Ƶ����ȡ
	if (VideoFilePath.isEmpty())
	{
		// ��������ʾ�����
		QString msgTitle = QString::fromLocal8Bit("����!");
		QString msg = QString::fromLocal8Bit("��ѡ�������Ƶ�ļ�");
		QMessageBox::warning(this, msgTitle, msg);
		return;
	}
	ui.resquestButton->setEnabled(false);
	ui.MoveAreaSlider->setEnabled(false);
	ui.FrameScrollBar->setEnabled(false);

	CroppedFrameVector.clear();
	currentFramePositon = 0;
	timer->start(INTERVALTIME); // ������ʱ��
}

// ������Ļ֡
void Subtitle::SendCroppedFrameData(const QImage& croppedImage)
{
	/* �ü����ͼ������ */
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
			if (jsonArray.isEmpty())
			{
				return;
			}
			++ImageSubTitleIndex;
			ui.textBrowser->append(QString::fromLocal8Bit("��") + QString::number(ImageSubTitleIndex) + QString::fromLocal8Bit("��ͼƬ����Ļ���:"));

			foreach(const QJsonValue &value, jsonArray)
			{
				ui.textBrowser->append("- " + value.toString());
			}
		}
		else {
			qDebug() << "Error: " << reply->errorString();
		}
		reply->deleteLater();
		manager->deleteLater();
	});
}

/* ������Ļ */
void Subtitle::ExportSubtitle()
{

}





Subtitle::~Subtitle()
{
	if (operationWindow)
	{
		delete operationWindow;
		operationWindow = nullptr;
	}
}