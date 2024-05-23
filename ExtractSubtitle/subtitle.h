#pragma once

#include <QtWidgets/QMainWindow>
#include <ui_subtitle.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <opencv2/opencv.hpp>
#include <QPainter>
#include <QtWidgets\QLabel>
#include <qtimer.h>
#include <QDir>
#include <QFileDialog>
#include <QtWebSockets/QWebSocket>
#include <QJsonArray>
#include <QBuffer>
#include <QMessageBox>
#include <chrono>
#include <operationwindow.h>

#define INTERVALTIME 350 // ÿ��IntervalTime����һ֡

class Subtitle : public QMainWindow
{
	Q_OBJECT

public:
	Subtitle(QWidget *parent = nullptr);
	~Subtitle();
	void OpenVideoFile(const QString& VideoFilePath); // ����Ƶ�ļ�
	void ChooseSubtitleFrame(); // ����ui.FrameScrollBar��ѡ����Ļ֡
	void InitConnect(); // ��ʼ��ʱ��Ҫ���ӵ��źźͲ�
signals:
	void CroppedFrameData(const QImage& croppedImage);

public slots:
	void StartExtractSubTitle(); // ��ʼ��ȡ��Ļ
	void handleFrame(); // ����֡����
	void SendCroppedFrameData(const QImage& croppedImage); // ��������֡
	void ExportSubtitle(); // ������Ļ

private:
	Ui::SubtitleClass ui;
	QString VideoFilePath; // ��Ƶ�ļ�·��
	QLabel* handleLabel; // ����ҳ����ʾ��ͼ��
	QLabel* upperLabel; // ����label

	QTimer* timer; // ��ʱ��Ĭ��1s����һ��
	cv::VideoCapture cap;
	int currentFramePositon; // ��ǰ֡��λ��
	cv::Mat croppedFrame; // ���ü���֡ͼ��
	QVector<QImage> CroppedFrameVector; // һϵ��֡
	std::chrono::time_point<std::chrono::high_resolution_clock> start; // ������ȡ��Ƶ��Ļ��ʱ��
	int ImageSubTitleIndex; // ͼƬ��Ļ����
	OperationWindow* operationWindow; // �û�����Ļ�ı�����
};
