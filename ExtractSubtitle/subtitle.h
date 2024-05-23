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

#define INTERVALTIME 350 // 每隔IntervalTime发送一帧

class Subtitle : public QMainWindow
{
	Q_OBJECT

public:
	Subtitle(QWidget *parent = nullptr);
	~Subtitle();
	void OpenVideoFile(const QString& VideoFilePath); // 打开视频文件
	void ChooseSubtitleFrame(); // 调节ui.FrameScrollBar来选择字幕帧
	void InitConnect(); // 初始化时需要连接的信号和槽
signals:
	void CroppedFrameData(const QImage& croppedImage);

public slots:
	void StartExtractSubTitle(); // 开始提取字幕
	void handleFrame(); // 处理帧数据
	void SendCroppedFrameData(const QImage& croppedImage); // 发送数据帧
	void ExportSubtitle(); // 导出字幕

private:
	Ui::SubtitleClass ui;
	QString VideoFilePath; // 视频文件路径
	QLabel* handleLabel; // 处理页面显示的图像
	QLabel* upperLabel; // 矩形label

	QTimer* timer; // 定时器默认1s调用一次
	cv::VideoCapture cap;
	int currentFramePositon; // 当前帧的位置
	cv::Mat croppedFrame; // 被裁剪的帧图像
	QVector<QImage> CroppedFrameVector; // 一系列帧
	std::chrono::time_point<std::chrono::high_resolution_clock> start; // 计算提取视频字幕的时间
	int ImageSubTitleIndex; // 图片字幕索引
	OperationWindow* operationWindow; // 用户对字幕文本操作
};
