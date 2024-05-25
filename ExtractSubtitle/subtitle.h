#pragma once

#include <QtWidgets/QMainWindow>
#include <x64/Debug/uic/ui_subtitle.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtCore/qurl.h>
#include <QtCore/qdebug.h>
#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qfile.h>
#include <opencv2/opencv.hpp>
#include <QtGui/qpainter.h>
#include <QtWidgets\QLabel>
#include <QtCore/qtimer.h>
#include <QtCore/qdir.h>
#include <QtWidgets/qfiledialog.h>
#include <QtCore/qbuffer.h>
#include <QtWidgets/qmessagebox.h>
#include <chrono>
#include <operationwindow.h>

#define INTERVALTIME 400 // ÿ��IntervalTime����һ֡

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
	void SendFilePath(const QString& VideoFilePath);
	void CroppedFrameData(const QImage& croppedImage);
	void SendSubtitle(const QString& Subtitle); // ��༭���ڷ�����Ļ�ź�

public slots:
	void StartExtractSubTitle(); // ��ʼ��ȡ��Ļ
	void handleFrame(); // ����֡����
	void SendCroppedFrameData(const QImage& croppedImage); // ��������֡

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
	OperationWindow* operationWindow; // �û�����Ļ�ı�����
	int CurrentReplies; // ��¼��ǰ��������ĸ���
};
