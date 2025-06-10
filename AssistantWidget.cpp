#include "stdafx.h"

#include "AssistantWidget.h"
#include <QApplication>
#include <QMessageBox>
#include <QScrollBar>
#include <QTextCursor>
#include <QFont>
#include <QTimer>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <QJsonArray>
#include <QJsonParseError>
#include <QUrl>
#include <QEventLoop>
#include <QScreen>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QHeaderView>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDirIterator>
#include <QGuiApplication>
#include <QPainter>
#include <QRadialGradient>
#include <QBrush>
#include <QPen>
#include <QDebug>

// AssistantWidget类构造函数
AssistantWidget::AssistantWidget(QWidget* parent)
	: QMainWindow(parent), isLoading(false) {

	try {
		// 0. 初始化HTTP客户端
		httpClient = new HttpClient(this);
		qDebug() << "使用WinHTTP客户端（无需Qt SSL）";

		// 初始化窗口同步定时器
		m_syncTimer = new QTimer(this);
		m_syncTimer->setInterval(100); // 每100ms检查一次
		connect(m_syncTimer, &QTimer::timeout, this, [this]() {
			if (m_mainWindow) {
				QRect currentGeometry = m_mainWindow->frameGeometry();
				// 只有当主窗口几何发生变化时才同步
				if (currentGeometry != m_lastMainWindowGeometry) {
					m_lastMainWindowGeometry = currentGeometry;
					syncPositionWith(m_mainWindow);
				}
			}
			});

		// 1. 首先加载配置
		loadConfiguration();

		// 2. 设置基本窗口属性
		setWindowTitle("AI智能助手");
		setMinimumSize(600, 400);
		resize(800, 600);

		// 3. 设置窗口图标（使用简单方式）
		setWindowIcon(createApplicationIcon());

		// 4. 设置UI（这会创建所有UI组件）
		setupUI();
		setupStyles();

		// 5. 初始化网络组件（现在UI已创建，可以安全使用updateStatus）
		initializeNetwork();

		// 6. 加载知识库
		loadKnowledgeBase();

		// 7. 显示欢迎消息（现在UI已经完全创建）
		if (chatDisplay) {
			QJsonObject systemConfig = config.value("system").toObject();
			QString welcomeMsg = systemConfig.value("welcome_message").toString("你好！我是你的AI助手，有什么可以帮助你的吗？");
			appendMessage("AI助手", welcomeMsg);
		}

		qDebug() << "AssistantWidget构造完成";

	}
	catch (const std::exception& e) {
		qCritical() << "构造函数异常:" << e.what();
	}
	catch (...) {
		qCritical() << "构造函数发生未知异常";
	}
}

// 析构函数
AssistantWidget::~AssistantWidget() {
	saveConfiguration();
}

// 初始化网络组件
void AssistantWidget::initializeNetwork() {
	// 检查配置的连接方式
	QJsonObject apiConfig = config.value("api").toObject();
	QString baseUrl = apiConfig.value("base_url").toString();

	if (baseUrl.startsWith("http://")) {
		qDebug() << "使用HTTP连接";
		updateStatus("HTTP连接模式");
	}
	else if (baseUrl.startsWith("https://")) {
		qDebug() << "⚠️ 使用HTTPS连接";
		updateStatus("HTTPS连接模式");
	}
	else {
		qDebug() << "⚠️ 未知的连接协议:" << baseUrl;
		updateStatus("连接协议未知");
	}

	qDebug() << "网络组件初始化完成，连接地址:" << baseUrl;
}

// 创建应用程序图标
QIcon AssistantWidget::createApplicationIcon() {
	// 创建一个48x48的图标
	QPixmap pixmap(48, 48);
	pixmap.fill(Qt::transparent);

	QPainter painter(&pixmap);
	painter.setRenderHint(QPainter::Antialiasing);

	// 绘制圆形背景
	QRadialGradient gradient(24, 24, 20);
	gradient.setColorAt(0, QColor("#3498db"));
	gradient.setColorAt(1, QColor("#2980b9"));

	painter.setBrush(QBrush(gradient));
	painter.setPen(QPen(QColor("#2c3e50"), 2));
	painter.drawEllipse(4, 4, 40, 40);

	// 绘制AI图标 (简化的机器人头像)
	painter.setPen(QPen(Qt::white, 2));
	painter.setBrush(Qt::white);

	// 眼睛
	painter.drawEllipse(16, 18, 4, 4);
	painter.drawEllipse(28, 18, 4, 4);

	// 嘴巴
	painter.setPen(QPen(Qt::white, 2, Qt::SolidLine, Qt::RoundCap));
	painter.drawArc(18, 26, 12, 8, 0, -180 * 16);

	// 天线
	painter.drawLine(24, 8, 24, 14);
	painter.drawEllipse(22, 6, 4, 4);

	return QIcon(pixmap);
}

// 加载应用程序图标（优先外部文件）
QIcon AssistantWidget::loadApplicationIcon() {
	// 在构造函数期间，直接使用程序生成的图标，避免复杂操作
	if (config.isEmpty()) {
		qDebug() << "配置未加载，使用程序生成的图标";
		return createApplicationIcon();
	}

	// 首先检查配置文件中是否指定了图标路径
	QJsonObject uiConfig = config.value("ui").toObject();
	QString configIconPath = uiConfig.value("icon_path").toString();

	if (!configIconPath.isEmpty() && QFile::exists(configIconPath)) {
		QIcon configIcon(configIconPath);
		if (!configIcon.isNull()) {
			qDebug() << "成功加载配置文件指定的图标:" << configIconPath;
			return configIcon;
		}
	}

	// 可能的图标文件路径（按优先级排序）
	QStringList iconPaths = {
		"C:\\Users\\wwl\\source\\repos\\BioChaInsight\\Image\\1\\基因研究.png"
	};

	// 尝试加载外部图标文件
	for (const QString& path : iconPaths) {
		if (QFile::exists(path)) {
			QIcon externalIcon(path);
			if (!externalIcon.isNull()) {
				qDebug() << "成功加载外部图标:" << path;
				return externalIcon;
			}
		}
	}

	// 如果没有找到外部图标，使用程序生成的图标
	qDebug() << "未找到外部图标文件，使用程序生成的图标";
	return createApplicationIcon();
}

// 窗口同步方法
void AssistantWidget::syncPositionWith(QMainWindow* mainWindow) {
	if (!mainWindow) return;

	// 获取主窗口的完整几何信息（包含标题栏和边框）
	QRect mainFrameGeometry = mainWindow->frameGeometry();
	QRect mainGeometry = mainWindow->geometry();

	qDebug() << "主窗口frameGeometry:" << mainFrameGeometry;
	qDebug() << "主窗口geometry:" << mainGeometry;

	// 计算标题栏和边框的偏移
	int titleBarHeight = mainFrameGeometry.top() - mainGeometry.top();
	int leftBorder = mainGeometry.left() - mainFrameGeometry.left();
	int rightBorder = mainFrameGeometry.right() - mainGeometry.right();
	int bottomBorder = mainFrameGeometry.bottom() - mainGeometry.bottom();

	qDebug() << "主窗口装饰 - 标题栏高度:" << titleBarHeight << "左边框:" << leftBorder << "右边框:" << rightBorder << "底边框:" << bottomBorder;

	// 设置助手窗口与主窗口Frame几何相同的高度
	QSize newSize = this->size();
	newSize.setHeight(mainFrameGeometry.height());  // 使用frameGeometry的高度

	// 应用新尺寸
	this->resize(newSize);

	// 强制更新窗口几何信息
	this->show();
	QApplication::processEvents();

	// 获取助手窗口的frame几何信息
	QRect assistantFrameGeometry = this->frameGeometry();
	QRect assistantGeometry = this->geometry();

	qDebug() << "助手窗口frameGeometry:" << assistantFrameGeometry;
	qDebug() << "助手窗口geometry:" << assistantGeometry;

	// 计算新位置 - 使用frameGeometry进行对齐
	QPoint newPosition;

	// 放在主窗口右侧，frameGeometry完全对齐
	newPosition.setX(mainFrameGeometry.x() + mainFrameGeometry.width() + 10);
	newPosition.setY(mainFrameGeometry.y());  // frameGeometry的Y坐标

	// 使用现代API获取屏幕信息
	QScreen* screen = QGuiApplication::screenAt(mainWindow->frameGeometry().center());
	if (!screen) {
		screen = QGuiApplication::primaryScreen();
	}
	QRect screenGeometry = screen->availableGeometry();

	// 如果放在右侧会超出屏幕，就放在左侧
	if (newPosition.x() + assistantFrameGeometry.width() > screenGeometry.right()) {
		newPosition.setX(mainFrameGeometry.x() - assistantFrameGeometry.width() - 10);
	}

	// 确保窗口不会超出屏幕顶部或底部
	if (newPosition.y() < screenGeometry.top()) {
		newPosition.setY(screenGeometry.top());
		// 如果顶部被裁剪，调整高度
		int availableHeight = screenGeometry.bottom() - screenGeometry.top();
		if (assistantFrameGeometry.height() > availableHeight) {
			// 需要重新计算内容区域高度
			int newContentHeight = availableHeight - (assistantFrameGeometry.height() - assistantGeometry.height());
			newSize.setHeight(newContentHeight);
			this->resize(newSize);
		}
	}

	if (newPosition.y() + assistantFrameGeometry.height() > screenGeometry.bottom()) {
		newPosition.setY(screenGeometry.bottom() - assistantFrameGeometry.height());
		// 如果还是超出，调整高度
		if (newPosition.y() < screenGeometry.top()) {
			newPosition.setY(screenGeometry.top());
			int availableHeight = screenGeometry.bottom() - screenGeometry.top();
			int newContentHeight = availableHeight - (assistantFrameGeometry.height() - assistantGeometry.height());
			newSize.setHeight(newContentHeight);
			this->resize(newSize);
		}
	}

	// 确保窗口不会超出屏幕左右边界
	if (newPosition.x() < screenGeometry.left()) {
		newPosition.setX(screenGeometry.left());
	}

	// 移动窗口到新位置（这里移动的是整个窗口frame）
	QPoint windowOffset = assistantGeometry.topLeft() - assistantFrameGeometry.topLeft();
	QPoint actualMovePosition = newPosition - windowOffset;

	this->move(actualMovePosition);

	qDebug() << "最终移动位置:" << actualMovePosition;
	qDebug() << "期望frame位置:" << newPosition;
	qDebug() << "窗口偏移:" << windowOffset;
}

// 设置UI界面
void AssistantWidget::setupUI() {
	// 获取配置中的窗口设置
	QJsonObject uiConfig = config.value("ui").toObject();
	int width = uiConfig.value("window_width").toInt(800);
	int height = uiConfig.value("window_height").toInt(600);
	QString title = uiConfig.value("window_title").toString("AI智能助手");

	setWindowTitle(title);
	setMinimumSize(600, 400);
	resize(width, height);

	// 创建中央widget
	centralWidget = new QWidget(this);
	setCentralWidget(centralWidget);

	// 创建主分割器
	mainSplitter = new QSplitter(Qt::Horizontal, this);

	// 创建左侧知识库面板（简化版）
	setupKnowledgePanel();

	// 创建右侧聊天面板
	setupChatPanel();

	// 设置分割器比例 - 让聊天区域更大
	mainSplitter->addWidget(knowledgePanel);
	mainSplitter->addWidget(chatPanel);
	mainSplitter->setStretchFactor(0, 1);
	mainSplitter->setStretchFactor(1, 4);

	// 创建简化的状态栏
	setupStatusBar();

	// 主布局
	QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
	mainLayout->setContentsMargins(5, 5, 5, 5);
	mainLayout->setSpacing(5);
	mainLayout->addWidget(mainSplitter, 1);
	mainLayout->addWidget(statusBar);
}

// 设置知识库面板
void AssistantWidget::setupKnowledgePanel() {
	knowledgePanel = new QFrame();
	knowledgePanel->setFrameStyle(QFrame::NoFrame);
	knowledgePanel->setMinimumWidth(180);
	knowledgePanel->setMaximumWidth(200);

	QVBoxLayout* layout = new QVBoxLayout(knowledgePanel);
	layout->setContentsMargins(10, 15, 10, 15);
	layout->setSpacing(15);

	// 知识库图标和标题
	QHBoxLayout* titleLayout = new QHBoxLayout();
	QLabel* kbIcon = new QLabel("📚");
	kbIcon->setStyleSheet("font-size: 20px;");

	knowledgeTitle = new QLabel("知识库");
	knowledgeTitle->setStyleSheet("font-weight: bold; font-size: 14px; color: #2c3e50;");

	titleLayout->addWidget(kbIcon);
	titleLayout->addWidget(knowledgeTitle);
	titleLayout->addStretch();

	// 知识库统计信息
	QFrame* statsFrame = new QFrame();
	statsFrame->setStyleSheet("background-color: #ecf0f1; border-radius: 8px; padding: 10px;");
	QVBoxLayout* statsLayout = new QVBoxLayout(statsFrame);

	statusLabel = new QLabel("正在加载...");
	statusLabel->setAlignment(Qt::AlignCenter);
	statusLabel->setStyleSheet("color: #7f8c8d; font-size: 12px;");

	QLabel* infoLabel = new QLabel("📊 文档统计");
	infoLabel->setStyleSheet("font-weight: bold; color: #34495e; font-size: 12px;");

	// 知识库状态指示器
	QHBoxLayout* statusLayout = new QHBoxLayout();
	QLabel* statusIcon = new QLabel("●");
	statusIcon->setStyleSheet("color: #27ae60; font-size: 12px;");
	QLabel* statusText = new QLabel("已就绪");
	statusText->setStyleSheet("color: #27ae60; font-size: 11px;");

	statusLayout->addWidget(statusIcon);
	statusLayout->addWidget(statusText);
	statusLayout->addStretch();

	statsLayout->addWidget(infoLabel);
	statsLayout->addWidget(statusLabel);
	statsLayout->addLayout(statusLayout);

	// 重新加载按钮
	reloadButton = new QPushButton("🔄 重新加载");
	reloadButton->setStyleSheet(
		"QPushButton {"
		"   background-color: #3498db;"
		"   color: white;"
		"   border: none;"
		"   border-radius: 6px;"
		"   padding: 8px 12px;"
		"   font-size: 11px;"
		"}"
		"QPushButton:hover {"
		"   background-color: #2980b9;"
		"}"
	);

	// 帮助信息
	QLabel* helpLabel = new QLabel("💡 知识库包含您的私人文档，内容已加密处理");
	helpLabel->setWordWrap(true);
	helpLabel->setStyleSheet("color: #95a5a6; font-size: 10px; margin-top: 10px;");

	layout->addLayout(titleLayout);
	layout->addWidget(statsFrame);
	layout->addWidget(reloadButton);
	layout->addStretch();
	layout->addWidget(helpLabel);

	// 连接信号
	connect(reloadButton, &QPushButton::clicked, this, &AssistantWidget::onReloadKnowledge);
}

// 设置聊天面板
void AssistantWidget::setupChatPanel() {
	chatPanel = new QFrame();
	chatPanel->setFrameStyle(QFrame::StyledPanel);

	QVBoxLayout* layout = new QVBoxLayout(chatPanel);

	// 聊天显示区域
	chatDisplay = new QTextEdit();
	chatDisplay->setReadOnly(true);

	// 设置自动换行模式
	chatDisplay->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
	chatDisplay->setLineWrapMode(QTextEdit::WidgetWidth);

	// 设置字体和大小
	QJsonObject uiConfig = config.value("ui").toObject();
	QString fontFamily = uiConfig.value("font_family").toString("等线");
	int fontSize = uiConfig.value("font_size").toInt(12);
	chatDisplay->setFont(QFont(fontFamily, fontSize));

	// 输入区域
	setupInputArea();

	layout->addWidget(chatDisplay, 1);
	layout->addWidget(inputFrame);
}

// 设置输入区域
void AssistantWidget::setupInputArea() {
	inputFrame = new QFrame();
	inputFrame->setFrameStyle(QFrame::StyledPanel);
	inputFrame->setMaximumHeight(80);

	QHBoxLayout* layout = new QHBoxLayout(inputFrame);

	// 输入框
	inputBox = new QLineEdit();
	inputBox->setPlaceholderText("请输入您的问题...");

	QJsonObject uiConfig = config.value("ui").toObject();
	QString fontFamily = uiConfig.value("font_family").toString("等线");
	int fontSize = uiConfig.value("font_size").toInt(12);
	inputBox->setFont(QFont(fontFamily, fontSize));

	// 发送按钮
	sendButton = new QPushButton("发送");
	sendButton->setMinimumWidth(80);

	// 清空按钮
	clearButton = new QPushButton("清空");
	clearButton->setMinimumWidth(80);

	layout->addWidget(inputBox, 1);
	layout->addWidget(sendButton);
	layout->addWidget(clearButton);

	// 连接信号
	connect(sendButton, &QPushButton::clicked, this, &AssistantWidget::onSendClicked);
	connect(clearButton, &QPushButton::clicked, this, &AssistantWidget::onClearChat);
	connect(inputBox, &QLineEdit::returnPressed, this, &AssistantWidget::onSendClicked);
}

// 设置状态栏
void AssistantWidget::setupStatusBar() {
	statusBar = new QFrame();
	statusBar->setFrameStyle(QFrame::NoFrame);
	statusBar->setMaximumHeight(30);
	statusBar->setStyleSheet("background-color: #34495e; border-top: 1px solid #bdc3c7;");

	QHBoxLayout* layout = new QHBoxLayout(statusBar);
	layout->setContentsMargins(10, 5, 10, 5);

	// 连接状态指示器
	QHBoxLayout* connectionLayout = new QHBoxLayout();
	QLabel* connIcon = new QLabel("🌐");
	connIcon->setStyleSheet("font-size: 12px;");
	connectionStatus = new QLabel("已连接");
	connectionStatus->setStyleSheet("color: #1abc9c; font-size: 11px; font-weight: bold;");

	connectionLayout->addWidget(connIcon);
	connectionLayout->addWidget(connectionStatus);
	connectionLayout->setSpacing(5);

	// 分隔符
	QLabel* separator = new QLabel("|");
	separator->setStyleSheet("color: #7f8c8d;");

	// 版本信息
	QLabel* versionLabel = new QLabel("AI助手 v1.0");
	versionLabel->setStyleSheet("color: #95a5a6; font-size: 10px;");

	// 进度条（隐藏状态）
	progressBar = new QProgressBar();
	progressBar->setVisible(false);
	progressBar->setMaximumHeight(3);
	progressBar->setTextVisible(false);
	progressBar->setStyleSheet(
		"QProgressBar {"
		"   border: none;"
		"   background-color: transparent;"
		"   border-radius: 1px;"
		"}"
		"QProgressBar::chunk {"
		"   background-color: #3498db;"
		"   border-radius: 1px;"
		"}"
	);

	layout->addLayout(connectionLayout);
	layout->addWidget(separator);
	layout->addWidget(versionLabel);
	layout->addStretch();
	layout->addWidget(progressBar);
}

// 加载配置文件
void AssistantWidget::loadConfiguration() {
	QFile configFile("config.json");
	if (!configFile.open(QIODevice::ReadOnly)) {
		QMessageBox::critical(this, "配置错误", "无法找到config.json配置文件！\n请确保配置文件存在。");
		return;
	}

	QByteArray data = configFile.readAll();
	QJsonParseError error;
	QJsonDocument doc = QJsonDocument::fromJson(data, &error);

	if (error.error != QJsonParseError::NoError) {
		QMessageBox::warning(this, "配置错误", "配置文件格式错误: " + error.errorString());
		return;
	}

	config = doc.object();
	configFile.close();
}

// 保存配置文件
bool AssistantWidget::saveConfiguration() {
	QFile configFile("config.json");
	if (!configFile.open(QIODevice::WriteOnly)) {
		return false;
	}

	QJsonDocument doc(config);
	configFile.write(doc.toJson());
	return true;
}

// 加载知识库
void AssistantWidget::loadKnowledgeBase() {
	setLoading(true);
	updateStatus("正在加载知识库...");

	QJsonObject kbConfig = config.value("knowledge_base").toObject();
	QString kbPath = kbConfig.value("path").toString("docs");
	QJsonArray extensions = kbConfig.value("supported_extensions").toArray();

	QStringList extList;
	for (const QJsonValue& ext : extensions) {
		extList << ext.toString();
	}

	// 扫描目录
	knowledgeFiles = scanDirectory(kbPath, extList);

	// 加载文件内容
	knowledgeContext.clear();
	int loadedCount = 0;

	for (const QString& filePath : knowledgeFiles) {
		QString content = loadTextFile(filePath);
		if (!content.isEmpty()) {
			knowledgeContext += QString("\n=== %1 ===\n%2\n").arg(
				QFileInfo(filePath).fileName(), content);
			loadedCount++;
		}
	}

	// 更新文件列表显示
	updateFileList();

	// 更新状态
	updateStatus(QString("已加载 %1 个文件").arg(loadedCount));
	setLoading(false);
}

// 扫描目录获取文件列表
QStringList AssistantWidget::scanDirectory(const QString& path, const QStringList& extensions) {
	QStringList files;
	QDir dir(path);

	if (!dir.exists()) {
		updateStatus("知识库目录不存在: " + path, true);
		return files;
	}

	// 递归扫描所有子目录
	QDirIterator iterator(path, QDirIterator::Subdirectories);
	while (iterator.hasNext()) {
		QString filePath = iterator.next();
		QFileInfo fileInfo(filePath);

		if (fileInfo.isFile()) {
			QString suffix = "." + fileInfo.suffix().toLower();
			if (extensions.contains(suffix)) {
				files << filePath;
			}
		}
	}

	return files;
}

// 加载文本文件
QString AssistantWidget::loadTextFile(const QString& filePath) {
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return QString();
	}

	QTextStream stream(&file);
	stream.setCodec("UTF-8");
	return stream.readAll();
}

// 更新文件列表显示
void AssistantWidget::updateFileList() {
	// 计算文件统计
	int totalFiles = knowledgeFiles.size();
	qint64 totalSize = 0;

	for (const QString& filePath : knowledgeFiles) {
		QFileInfo fileInfo(filePath);
		totalSize += fileInfo.size();
	}

	// 格式化大小显示
	QString sizeStr;
	if (totalSize > 1024 * 1024) {
		sizeStr = QString::number(totalSize / (1024 * 1024)) + " MB";
	}
	else if (totalSize > 1024) {
		sizeStr = QString::number(totalSize / 1024) + " KB";
	}
	else {
		sizeStr = QString::number(totalSize) + " B";
	}

	// 更新状态标签
	statusLabel->setText(QString("📄 %1 个文档\n💾 总大小: %2")
		.arg(totalFiles)
		.arg(sizeStr));
}

// 添加消息到聊天显示
void AssistantWidget::appendMessage(const QString& role, const QString& content, const QString& timestamp) {
	if (!chatDisplay) {
		qDebug() << "警告: chatDisplay为空，无法添加消息:" << role << content;
		return;
	}

	QString time = timestamp.isEmpty() ? getCurrentTimestamp() : timestamp;
	QString formattedMsg = formatMessage(role, content, time);

	chatDisplay->append(formattedMsg);

	// 滚动到底部
	QScrollBar* scrollBar = chatDisplay->verticalScrollBar();
	if (scrollBar) {
		scrollBar->setValue(scrollBar->maximum());
	}
}

// 解析Markdown格式到HTML
QString AssistantWidget::parseMarkdownToHtml(const QString& markdown) {
	QString html = markdown;

	// 首先处理代码块，避免其内容被其他规则影响
	// 1. 代码块处理（三个反引号）
	QRegExp codeBlockRegex("```([a-zA-Z]*)\n([\\s\\S]*?)\n```");
	codeBlockRegex.setMinimal(true);
	html.replace(codeBlockRegex,
		"<div class='code-block' style='background-color: #2d3748; color: #e2e8f0; padding: 10px; border-radius: 6px; margin: 6px 0; font-family: \"Consolas\", \"Monaco\", monospace; font-size: 13px; overflow-x: auto;'>"
		"<div style='color: #68d391; font-size: 11px; margin-bottom: 4px;'>\\1</div>"
		"<pre style='margin: 0; white-space: pre-wrap;'>\\2</pre>"
		"</div>");

	// 2. 内联代码处理（单个反引号）
	QRegExp inlineCodeRegex("`([^`]+)`");
	html.replace(inlineCodeRegex,
		"<code style='background-color: #f7fafc; color: #d69e2e; padding: 1px 3px; border-radius: 3px; font-family: \"Consolas\", \"Monaco\", monospace; font-size: 0.9em;'>\\1</code>");

	// 逐行处理，避免跨行匹配问题
	QStringList lines = html.split('\n');
	QStringList processedLines;

	for (int i = 0; i < lines.size(); i++) {
		QString line = lines[i];

		// 3. 标题处理 - 使用简单的字符串匹配
		if (line.startsWith("### ")) {
			line = QString("<h3 style='color: #4a5568; font-size: 16px; font-weight: bold; margin: 8px 0 4px 0; font-family: \"等线\", \"DengXian\", \"Microsoft YaHei\", sans-serif;'>%1</h3>")
				.arg(line.mid(4).trimmed());
		}
		else if (line.startsWith("## ")) {
			line = QString("<h2 style='color: #2d3748; font-size: 18px; font-weight: bold; margin: 10px 0 6px 0; font-family: \"等线\", \"DengXian\", \"Microsoft YaHei\", sans-serif;'>%1</h2>")
				.arg(line.mid(3).trimmed());
		}
		else if (line.startsWith("# ")) {
			line = QString("<h1 style='color: #1a202c; font-size: 20px; font-weight: bold; margin: 12px 0 8px 0; border-bottom: 2px solid #e2e8f0; padding-bottom: 6px; font-family: \"等线\", \"DengXian\", \"Microsoft YaHei\", sans-serif;'>%1</h1>")
				.arg(line.mid(2).trimmed());
		}

		// 4. 引用块处理
		else if (line.startsWith("> ")) {
			line = QString("<blockquote style='border-left: 4px solid #cbd5e0; padding-left: 12px; margin: 4px 0; color: #718096; font-style: italic; background-color: #f7fafc; font-family: \"等线\", \"DengXian\", \"Microsoft YaHei\", sans-serif;'>%1</blockquote>")
				.arg(line.mid(2).trimmed());
		}

		// 5. 分割线处理
		else if (line.trimmed() == "---" || line.trimmed() == "***") {
			line = "<hr style='border: none; border-top: 2px solid #e2e8f0; margin: 12px 0;'>";
		}

		processedLines.append(line);
	}

	html = processedLines.join('\n');

	// 6. 粗体处理
	QRegExp boldRegex("\\*\\*([^*]+)\\*\\*");
	html.replace(boldRegex, "<strong style='color: #2d3748; font-weight: bold; font-family: \"等线\", \"DengXian\", \"Microsoft YaHei\", sans-serif;'>\\1</strong>");

	QRegExp boldRegex2("__([^_]+)__");
	html.replace(boldRegex2, "<strong style='color: #2d3748; font-weight: bold; font-family: \"等线\", \"DengXian\", \"Microsoft YaHei\", sans-serif;'>\\1</strong>");

	// 7. 斜体处理
	QRegExp italicRegex("\\*([^*]+)\\*");
	html.replace(italicRegex, "<em style='color: #4a5568; font-style: italic; font-family: \"等线\", \"DengXian\", \"Microsoft YaHei\", sans-serif;'>\\1</em>");

	QRegExp italicRegex2("_([^_]+)_");
	html.replace(italicRegex2, "<em style='color: #4a5568; font-style: italic; font-family: \"等线\", \"DengXian\", \"Microsoft YaHei\", sans-serif;'>\\1</em>");

	// 8. 链接处理
	QRegExp linkRegex("\\[([^\\]]+)\\]\\(([^\\)]+)\\)");
	html.replace(linkRegex, "<a href='\\2' style='color: #3182ce; text-decoration: underline; font-family: \"等线\", \"DengXian\", \"Microsoft YaHei\", sans-serif;'>\\1</a>");

	// 9. 列表处理 - 重新分行处理
	lines = html.split('\n');
	QStringList finalLines;
	bool inUL = false;
	bool inOL = false;

	for (int i = 0; i < lines.size(); i++) {
		QString line = lines[i];

		// 无序列表
		QRegExp ulRegex("^[\\s]*[-*+] (.+)$");
		if (ulRegex.indexIn(line) != -1) {
			if (!inUL && !inOL) {
				finalLines.append("<ul style='margin: 4px 0; padding-left: 20px; font-family: \"等线\", \"DengXian\", \"Microsoft YaHei\", sans-serif;'>");
				inUL = true;
			}
			else if (inOL) {
				finalLines.append("</ol>");
				finalLines.append("<ul style='margin: 4px 0; padding-left: 20px; font-family: \"等线\", \"DengXian\", \"Microsoft YaHei\", sans-serif;'>");
				inOL = false;
				inUL = true;
			}
			QString item = ulRegex.cap(1);
			finalLines.append(QString("<li style='margin: 2px 0; color: #4a5568; font-family: \"等线\", \"DengXian\", \"Microsoft YaHei\", sans-serif;'>%1</li>").arg(item));
			continue;
		}

		// 有序列表
		QRegExp olRegex("^[\\s]*\\d+\\. (.+)$");
		if (olRegex.indexIn(line) != -1) {
			if (!inOL && !inUL) {
				finalLines.append("<ol style='margin: 4px 0; padding-left: 20px; font-family: \"等线\", \"DengXian\", \"Microsoft YaHei\", sans-serif;'>");
				inOL = true;
			}
			else if (inUL) {
				finalLines.append("</ul>");
				finalLines.append("<ol style='margin: 4px 0; padding-left: 20px; font-family: \"等线\", \"DengXian\", \"Microsoft YaHei\", sans-serif;'>");
				inUL = false;
				inOL = true;
			}
			QString item = olRegex.cap(1);
			finalLines.append(QString("<li style='margin: 2px 0; color: #4a5568; font-family: \"等线\", \"DengXian\", \"Microsoft YaHei\", sans-serif;'>%1</li>").arg(item));
			continue;
		}

		// 非列表行
		if (inUL) {
			finalLines.append("</ul>");
			inUL = false;
		}
		if (inOL) {
			finalLines.append("</ol>");
			inOL = false;
		}

		finalLines.append(line);
	}

	// 关闭未关闭的列表
	if (inUL) finalLines.append("</ul>");
	if (inOL) finalLines.append("</ol>");

	// 最终清理：合并连续的空行
	QStringList cleanedFinalLines;
	bool lastWasEmpty = false;

	for (const QString& line : finalLines) {
		QString trimmedLine = line.trimmed();
		if (trimmedLine.isEmpty()) {
			if (!lastWasEmpty) {
				cleanedFinalLines.append("");
				lastWasEmpty = true;
			}
			// 跳过连续的空行
		}
		else {
			cleanedFinalLines.append(line);
			lastWasEmpty = false;
		}
	}

	return cleanedFinalLines.join('\n');
}

// 格式化消息
QString AssistantWidget::formatMessage(const QString& role, const QString& content, const QString& timestamp) {
	QString color;
	QString icon;
	QString bgColor;

	if (role == "用户") {
		color = "#1a365d";
		icon = "👤";
		bgColor = "#ebf4ff";
	}
	else if (role == "AI助手") {
		color = "#1a202c";
		icon = "🤖";
		bgColor = "#f0fff4";
	}
	else {
		color = "#4a5568";
		icon = "ℹ️";
		bgColor = "#f7fafc";
	}

	// 将Markdown转换为HTML
	QString htmlContent = parseMarkdownToHtml(content);

	// 智能换行处理：避免HTML标签后的多余换行
	QStringList lines = htmlContent.split('\n');
	QStringList cleanedLines;

	for (int i = 0; i < lines.size(); i++) {
		QString line = lines[i].trimmed();

		// 跳过空行，但保留段落间的适当间距
		if (line.isEmpty()) {
			// 只有当前面不是HTML块级元素结束标签时，才添加空行
			if (!cleanedLines.isEmpty()) {
				QString lastLine = cleanedLines.last();
				if (!lastLine.endsWith("</h1>") &&
					!lastLine.endsWith("</h2>") &&
					!lastLine.endsWith("</h3>") &&
					!lastLine.endsWith("</div>") &&
					!lastLine.endsWith("</ul>") &&
					!lastLine.endsWith("</ol>") &&
					!lastLine.endsWith("</blockquote>") &&
					!lastLine.endsWith("</hr>") &&
					!lastLine.isEmpty()) {
					cleanedLines.append("<br>");
				}
			}
		}
		else {
			cleanedLines.append(line);
		}
	}

	htmlContent = cleanedLines.join('\n');

	// 清理多余的换行符
	htmlContent.replace('\r', "");
	htmlContent.replace('\n', " "); // 将剩余的换行符转为空格

	return QString(
		"<div style='margin: 8px 0; padding: 12px; background: linear-gradient(135deg, %1 0%, rgba(255,255,255,0.8) 100%); "
		"border-radius: 10px; word-wrap: break-word; overflow-wrap: break-word; "
		"box-shadow: 0 2px 6px rgba(0,0,0,0.08); border: 1px solid rgba(0,0,0,0.05); "
		"font-family: \"等线\", \"DengXian\", \"Microsoft YaHei\", sans-serif;'>"

		"<div style='color: %2; font-weight: bold; margin-bottom: 6px; display: flex; align-items: center; "
		"border-bottom: 1px solid rgba(0,0,0,0.08); padding-bottom: 4px; "
		"font-family: \"等线\", \"DengXian\", \"Microsoft YaHei\", sans-serif;'>"
		"<span style='font-size: 16px; margin-right: 6px;'>%3</span>"
		"<span style='font-size: 14px;'>%4</span>"
		"<span style='font-size: 11px; color: #a0aec0; margin-left: auto; font-weight: normal;'>%5</span>"
		"</div>"

		"<div style='color: #2d3748; line-height: 1.5; word-wrap: break-word; overflow-wrap: break-word; "
		"font-size: 14px; font-family: \"等线\", \"DengXian\", \"Microsoft YaHei\", sans-serif;'>%6</div>"
		"</div>"
	).arg(bgColor, color, icon, role, timestamp, htmlContent);
}

// 获取当前时间戳
QString AssistantWidget::getCurrentTimestamp() {
	return QDateTime::currentDateTime().toString("hh:mm:ss");
}

// 更新状态
void AssistantWidget::updateStatus(const QString& message, bool isError) {
	if (!connectionStatus) {
		qDebug() << "警告: connectionStatus为空，无法更新状态:" << message;
		return;
	}

	if (isError) {
		connectionStatus->setText(message);
		connectionStatus->setStyleSheet("color: #e74c3c; font-size: 11px; font-weight: bold;");
	}
	else {
		connectionStatus->setText(message);
		connectionStatus->setStyleSheet("color: #1abc9c; font-size: 11px; font-weight: bold;");
	}
}

// 设置加载状态
void AssistantWidget::setLoading(bool loading) {
	isLoading = loading;
	if (!progressBar) {
		qDebug() << "警告: progressBar为空，无法设置加载状态";
		return;
	}

	progressBar->setVisible(loading);
	if (loading) {
		progressBar->setRange(0, 0); // 无限进度条
	}
}

// 发送按钮点击事件
void AssistantWidget::onSendClicked() {
	QString userInput = inputBox->text().trimmed();
	if (userInput.isEmpty() || isLoading) {
		return;
	}

	// 显示用户消息
	appendMessage("用户", userInput);

	// 清空输入框
	inputBox->clear();

	// 设置加载状态
	setLoading(true);
	sendButton->setText("思考中...");
	sendButton->setEnabled(false);

	// 异步调用AI
	QTimer::singleShot(100, [this, userInput]() {
		QString response = callAI(userInput);

		if (!response.isEmpty()) {
			appendMessage("AI助手", response);
		}
		else {
			appendMessage("系统", "抱歉，AI服务暂时不可用，请稍后重试。");
		}

		// 恢复按钮状态
		setLoading(false);
		sendButton->setText("发送");
		sendButton->setEnabled(true);
		inputBox->setFocus();
		});
}

// 清空聊天记录
void AssistantWidget::onClearChat() {
	int ret = QMessageBox::question(this, "确认", "确定要清空所有聊天记录吗？",
		QMessageBox::Yes | QMessageBox::No);
	if (ret == QMessageBox::Yes) {
		chatDisplay->clear();
		QJsonObject systemConfig = config.value("system").toObject();
		QString welcomeMsg = systemConfig.value("welcome_message").toString("欢迎使用AI助手");
		appendMessage("系统", welcomeMsg);
	}
}

// 重新加载知识库
void AssistantWidget::onReloadKnowledge() {
	loadKnowledgeBase();
}

// 调用AI接口 - 使用WinHTTP，无需Qt SSL
QString AssistantWidget::callAI(const QString& userMessage) {
	updateStatus("正在连接AI服务...");

	// 构建系统提示
	QString systemPrompt = buildSystemPrompt();

	// 准备请求数据
	QJsonObject apiConfig = config.value("api").toObject();
	QString apiUrl = apiConfig.value("base_url").toString();
	QString apiKey = apiConfig.value("api_key").toString();
	QString model = apiConfig.value("model").toString();
	double temperature = apiConfig.value("temperature").toDouble();
	int maxTokens = apiConfig.value("max_tokens").toInt();

	if (apiKey == "sk-你的API密钥") {
		updateStatus("请先配置API密钥", true);
		return "请先在 config.json 文件中配置您的API密钥。";
	}

	// 构建请求JSON
	QJsonObject requestJson;
	requestJson["model"] = model;
	requestJson["temperature"] = temperature;
	requestJson["max_tokens"] = maxTokens;

	QJsonArray messages;
	QJsonObject systemMsg;
	systemMsg["role"] = "system";
	systemMsg["content"] = systemPrompt;
	QJsonObject userMsg;
	userMsg["role"] = "user";
	userMsg["content"] = userMessage;
	messages.append(systemMsg);
	messages.append(userMsg);
	requestJson["messages"] = messages;

	qDebug() << "🚀 使用WinHTTP发送请求到:" << apiUrl;

	// 使用WinHTTP发送请求 - 完全绕过Qt SSL
	QString response = httpClient->post(apiUrl, requestJson, apiKey);

	if (response.isEmpty()) {
		QString errorMsg = httpClient->getLastError();
		qDebug() << "❌ WinHTTP请求失败:" << errorMsg;
		updateStatus("网络连接失败", true);
		return "网络请求失败: " + errorMsg;
	}

	// 解析响应
	QJsonParseError parseError;
	QJsonDocument responseDoc = QJsonDocument::fromJson(response.toUtf8(), &parseError);

	if (parseError.error != QJsonParseError::NoError) {
		qDebug() << "❌ JSON解析失败:" << parseError.errorString();
		updateStatus("响应解析失败", true);
		return "服务器响应格式错误: " + parseError.errorString();
	}

	QJsonObject responseObj = responseDoc.object();

	// 检查API响应
	if (responseObj.contains("error")) {
		QJsonObject error = responseObj.value("error").toObject();
		QString errorMsg = error.value("message").toString();
		qDebug() << "❌ API错误:" << errorMsg;
		updateStatus("API调用失败", true);
		return "API错误: " + errorMsg;
	}

	if (responseObj.contains("choices")) {
		QJsonArray choices = responseObj.value("choices").toArray();
		if (!choices.isEmpty()) {
			QJsonObject firstChoice = choices[0].toObject();
			QJsonObject message = firstChoice.value("message").toObject();
			QString content = message.value("content").toString();

			qDebug() << "成功获取AI响应，长度:" << content.length();
			updateStatus("已连接");
			return content;
		}
	}

	qDebug() << "❌ 响应格式不正确";
	updateStatus("响应格式错误", true);
	return "服务器响应格式不正确";
}

// 构建系统提示
QString AssistantWidget::buildSystemPrompt() {
	QJsonObject systemConfig = config.value("system").toObject();
	QString basePrompt = systemConfig.value("system_prompt").toString("你是一个专业的AI助手");

	if (knowledgeContext.isEmpty()) {
		return basePrompt + "\n\n注意：当前没有加载任何知识库文档。";
	}

	int maxLength = systemConfig.value("max_context_length").toInt(8000);
	QString context = knowledgeContext;

	// 如果上下文太长，截取前面部分
	if (context.length() > maxLength) {
		context = context.left(maxLength) + "\n\n[注意：知识库内容过长，已截取前面部分]";
	}

	return basePrompt + "\n\n以下是知识库内容：\n" + context;
}

// 设置样式
void AssistantWidget::setupStyles() {
	setStyleSheet(getModernStyle());
}

// 获取现代化样式
QString AssistantWidget::getModernStyle() {
	return R"(
        QMainWindow {
            background-color: #ecf0f1;
            color: #2c3e50;
        }
        
        QFrame {
            background-color: white;
            border: 1px solid #bdc3c7;
            border-radius: 10px;
        }
        
        QTextEdit {
            background-color: white;
            border: 2px solid #ecf0f1;
            border-radius: 12px;
            padding: 15px;
            font-family: '等线', 'DengXian', 'Microsoft YaHei', sans-serif;
            font-size: 13px;
            line-height: 1.6;
        }
        
        QTextEdit:focus {
            border-color: #3498db;
        }
        
        QLineEdit {
            background-color: white;
            border: 2px solid #3498db;
            border-radius: 25px;
            padding: 12px 20px;
            font-family: '等线', 'DengXian', 'Microsoft YaHei', sans-serif;
            font-size: 13px;
            color: #2c3e50;
        }
        
        QLineEdit:focus {
            border-color: #2980b9;
            box-shadow: 0 0 5px rgba(52, 152, 219, 0.3);
        }
        
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #3498db, stop:1 #2980b9);
            color: white;
            border: none;
            border-radius: 8px;
            padding: 10px 20px;
            font-family: '等线', 'DengXian', 'Microsoft YaHei', sans-serif;
            font-size: 12px;
            font-weight: bold;
            min-height: 16px;
        }
        
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #2980b9, stop:1 #1f4e79);
            transform: translateY(-1px);
        }
        
        QPushButton:pressed {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #1f4e79, stop:1 #2980b9);
        }
        
        QPushButton:disabled {
            background-color: #95a5a6;
            color: #ecf0f1;
        }
        
        QSplitter::handle {
            background-color: #bdc3c7;
            width: 3px;
            margin: 5px;
            border-radius: 1px;
        }
        
        QSplitter::handle:hover {
            background-color: #3498db;
        }
        
        QProgressBar {
            border: none;
            background-color: #ecf0f1;
            border-radius: 1px;
            text-align: center;
        }
        
        QProgressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #3498db, stop:1 #2980b9);
            border-radius: 1px;
        }
        
        QLabel {
            color: #2c3e50;
            font-family: '等线', 'DengXian', 'Microsoft YaHei', sans-serif;
        }
    )";
}

// 设置要同步的主窗口
void AssistantWidget::setMainWindow(QMainWindow* mainWindow) {
	m_mainWindow = mainWindow;
	if (m_mainWindow) {
		// 记录初始几何状态
		m_lastMainWindowGeometry = m_mainWindow->frameGeometry();
		// 立即同步一次
		syncPositionWith(m_mainWindow);
	}
}

// 开始持续同步
void AssistantWidget::startSyncWithMainWindow() {
	if (m_mainWindow && m_syncTimer) {
		m_syncTimer->start();
		qDebug() << "开始持续同步助手窗口与主窗口";
	}
}

// 停止持续同步
void AssistantWidget::stopSyncWithMainWindow() {
	if (m_syncTimer) {
		m_syncTimer->stop();
		qDebug() << "停止同步助手窗口";
	}
}