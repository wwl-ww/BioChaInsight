#ifndef ASSISTANTWIDGET_H
#define ASSISTANTWIDGET_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QSplitter>
#include <QListWidget>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QIODevice>
#include <QTimer>
#include <QScrollArea>
#include <QFrame>
#include <QSystemTrayIcon>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QGuiApplication>
#include <QScreen>
#include <QIcon>
#include <QPixmap>
#include <QSslConfiguration>
#include <QSslSocket>
#include <QNetworkProxy>
#include "HttpClient.h"

class AssistantWidget : public QMainWindow {
	Q_OBJECT

public:
	AssistantWidget(QWidget* parent = nullptr);
	~AssistantWidget();

	// 窗口同步方法
	void syncPositionWith(QMainWindow* mainWindow);
	void setMainWindow(QMainWindow* mainWindow);
	void startSyncWithMainWindow();
	void stopSyncWithMainWindow();

private slots:
	void onSendClicked();
	void onClearChat();
	void onReloadKnowledge();

private:
	// UI组件
	QWidget* centralWidget = nullptr;
	QSplitter* mainSplitter = nullptr;

	// 左侧知识库面板
	QFrame* knowledgePanel = nullptr;
	QLabel* knowledgeTitle = nullptr;
	QPushButton* reloadButton = nullptr;
	QLabel* statusLabel = nullptr;

	// 右侧聊天面板
	QFrame* chatPanel = nullptr;
	QTextEdit* chatDisplay = nullptr;
	QFrame* inputFrame = nullptr;
	QLineEdit* inputBox = nullptr;
	QPushButton* sendButton = nullptr;
	QPushButton* clearButton = nullptr;

	// 状态栏
	QFrame* statusBar = nullptr;
	QProgressBar* progressBar = nullptr;
	QLabel* connectionStatus = nullptr;

	// 数据成员
	QJsonObject config;
	QStringList knowledgeFiles;
	QString knowledgeContext;
	bool isLoading;
	HttpClient* httpClient;

	// 窗口同步相关
	QMainWindow* m_mainWindow = nullptr;
	QTimer* m_syncTimer = nullptr;
	QRect m_lastMainWindowGeometry;  // 记录主窗口上次的几何状态

	// 方法
	void setupUI();
	void setupStyles();
	void setupKnowledgePanel();
	void setupChatPanel();
	void setupInputArea();
	void setupStatusBar();
	void initializeNetwork();
	QIcon createApplicationIcon();
	QIcon loadApplicationIcon();
	void loadConfiguration();
	bool saveConfiguration();
	void loadKnowledgeBase();
	void appendMessage(const QString& role, const QString& content, const QString& timestamp = "");
	void updateStatus(const QString& message, bool isError = false);
	void setLoading(bool loading);
	QString formatMessage(const QString& role, const QString& content, const QString& timestamp);
	QString parseMarkdownToHtml(const QString& markdown);
	QString getCurrentTimestamp();

	// 知识库相关
	QStringList scanDirectory(const QString& path, const QStringList& extensions);
	QString loadTextFile(const QString& filePath);
	QString extractFileContent(const QString& filePath);
	void updateFileList();

	// API相关
	QString callAI(const QString& userMessage);
	QString buildSystemPrompt();

	// 样式相关
	QString getModernStyle();
	QString getChatBubbleStyle(const QString& role);
};

#endif // ASSISTANTWIDGET_H
