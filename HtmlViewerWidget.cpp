#include "stdafx.h"

#include "HtmlViewerWidget.h"
#include <QDebug>
#include <QApplication>
#include <QStyle>
#include <QPainter>
#include <QFont>
#include <QBrush>
#include <QPen>
#include <QRadialGradient>
#include <QLinearGradient>
#include <QPixmap>
#include <QIcon>
#include <QWebEngineView>

HtmlViewerWidget::HtmlViewerWidget(QWidget* parent)
	: QWidget(parent)
	, mainLayout(nullptr)
	, splitter(nullptr)
	, fileTreeWidget(nullptr)
	, contentDisplay(nullptr)
{
	setupUI();

	// 设置窗口初始尺寸
	resize(1400, 900);  // 宽度1400px，高度900px
	setMinimumSize(800, 600);  // 设置最小尺寸防止窗口过小

	// 设置窗口图标 - 改进版
	QIcon windowIcon = createWindowIcon();
	this->setWindowIcon(windowIcon);

	// 如果有父窗口且是窗口，也设置父窗口的图标
	QWidget* topWindow = this;
	while (topWindow->parentWidget()) {
		topWindow = topWindow->parentWidget();
		if (topWindow->isWindow()) {
			topWindow->setWindowIcon(windowIcon);
			break;
		}
	}
}

HtmlViewerWidget::~HtmlViewerWidget()
{
	// Qt会自动清理子组件
}

void HtmlViewerWidget::setupUI()
{
	// 创建主布局
	mainLayout = new QHBoxLayout(this);
	mainLayout->setContentsMargins(12, 12, 12, 12);
	mainLayout->setSpacing(12);

	// 创建分割器
	splitter = new QSplitter(Qt::Horizontal, this);
	splitter->setHandleWidth(3);
	splitter->setStyleSheet(
		"QSplitter::handle {"
		"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
		"                stop:0 #e0e4e8, stop:0.5 #f6f8fa, stop:1 #e0e4e8);"
		"    border: none;"
		"    border-radius: 2px;"
		"    margin: 2px 0;"
		"}"
		"QSplitter::handle:hover {"
		"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
		"                stop:0 #0366d6, stop:0.5 #4285f4, stop:1 #0366d6);"
		"}"
		"QSplitter::handle:pressed {"
		"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
		"                stop:0 #0256cc, stop:0.5 #3367d6, stop:1 #0256cc);"
		"}"
	);

	// 创建文件树
	fileTreeWidget = new QTreeWidget(splitter);
	fileTreeWidget->setHeaderHidden(true);  // 隐藏标题栏
	fileTreeWidget->setMinimumWidth(300);
	fileTreeWidget->setMaximumWidth(500);
	fileTreeWidget->setRootIsDecorated(true);
	fileTreeWidget->setAnimated(true);

	// 设置文件树现代化样式（增强版）
	fileTreeWidget->setStyleSheet(
		"QTreeWidget {"
		"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
		"                stop:0 #fafbfc, stop:0.3 #f6f8fa, stop:1 #f1f3f4);"
		"    border: 2px solid #e1e4e8;"
		"    border-radius: 12px;"
		"    font-size: 14px;"
		"    font-family: 'Microsoft YaHei UI', 'Segoe UI', sans-serif;"
		"    selection-background-color: #e3f2fd;"
		"    outline: none;"
		"    padding: 8px;"
		"}"
		"QTreeWidget::item {"
		"    padding: 12px 8px;"
		"    border: none;"
		"    margin: 2px 0px;"
		"    border-radius: 8px;"
		"    min-height: 20px;"
		"}"
		"QTreeWidget::item:hover {"
		"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
		"                stop:0 #f0f8ff, stop:1 #e6f3ff);"
		"    color: #1565c0;"
		"    border: 1px solid #bbdefb;"
		"}"
		"QTreeWidget::item:selected {"
		"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
		"                stop:0 #e3f2fd, stop:0.5 #e1f5fe, stop:1 #e0f2f1);"
		"    color: #0d47a1;"
		"    border: 1px solid #81d4fa;"
		"    font-weight: 500;"
		"}"
		"QTreeWidget::item:selected:hover {"
		"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
		"                stop:0 #e8f4fd, stop:0.5 #e3f2fd, stop:1 #e1f5fe);"
		"    border: 1px solid #64b5f6;"
		"}"
		"QTreeWidget::branch {"
		"    background: transparent;"
		"}"
		"QTreeWidget::branch:has-children:!has-siblings:closed,"
		"QTreeWidget::branch:closed:has-children:has-siblings {"
		"    border-image: none;"
		"    image: url(data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMTIiIGhlaWdodD0iMTIiIHZpZXdCb3g9IjAgMCAxMiAxMiIgZmlsbD0ibm9uZSIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj4KPHBhdGggZD0iTTQgMyA4IDYgNCA5VjNaIiBmaWxsPSIjMDM2NmQ2Ii8+Cjwvc3ZnPgo=);"
		"}"
		"QTreeWidget::branch:open:has-children:!has-siblings,"
		"QTreeWidget::branch:open:has-children:has-siblings {"
		"    border-image: none;"
		"    image: url(data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMTIiIGhlaWdodD0iMTIiIHZpZXdCb3g9IjAgMCAxMiAxMiIgZmlsbD0ibm9uZSIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj4KPHBhdGggZD0iTTMgNCA2IDggOSA0SDNaIiBmaWxsPSIjMDM2NmQ2Ii8+Cjwvc3ZnPgo=);"
		"}"
	);

	// 创建内容显示区域
	contentDisplay = new QWebEngineView(splitter);

	// 设置内容显示区域样式
	contentDisplay->setStyleSheet(
		"QWebEngineView {"
		"    background: #ffffff;"
		"    border: 2px solid #e1e4e8;"
		"    border-radius: 12px;"
		"    margin: 0px;"
		"}"
		"QWebEngineView:focus {"
		"    border: 3px solid #0366d6;"
		"    border-radius: 12px;"
		"}"
	);

	// 设置分割器比例
	splitter->addWidget(fileTreeWidget);
	splitter->addWidget(contentDisplay);
	splitter->setStretchFactor(0, 0);  // 文件树不拉伸
	splitter->setStretchFactor(1, 1);  // 内容区域拉伸
	splitter->setSizes({ 350, 1050 });   // 设置初始分割比例

	// 添加到主布局
	mainLayout->addWidget(splitter);

	// 连接信号槽
	connect(fileTreeWidget, &QTreeWidget::itemClicked,
		this, &HtmlViewerWidget::onFileItemClicked);

	// 设置整体样式（增强版）
	this->setStyleSheet(
		"HtmlViewerWidget {"
		"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
		"                stop:0 #f8f9fa, stop:0.3 #ffffff, stop:0.7 #f6f8fa, stop:1 #e9ecef);"
		"    border-radius: 15px;"
		"    border: 1px solid #e1e4e8;"
		"}"
	);
}

void HtmlViewerWidget::setHtmlFolder(const QString& folderPath)
{
	if (folderPath.isEmpty()) {
		return;
	}

	QDir dir(folderPath);
	if (!dir.exists()) {
		qWarning() << "文件夹不存在:" << folderPath;
		return;
	}

	currentFolderPath = folderPath;
	populateFileTree(folderPath);
	emit folderChanged(folderPath);
}

void HtmlViewerWidget::populateFileTree(const QString& folderPath)
{
	fileTreeWidget->clear();

	QDir dir(folderPath);
	if (!dir.exists()) {
		return;
	}

	// 创建根节点
	QTreeWidgetItem* rootItem = new QTreeWidgetItem(fileTreeWidget);
	rootItem->setText(0, dir.dirName());
	rootItem->setIcon(0, this->style()->standardIcon(QStyle::SP_DirIcon));
	rootItem->setExpanded(true);

	// 递归添加文件和文件夹
	addHtmlFilesToTree(rootItem, dir);
}

void HtmlViewerWidget::addHtmlFilesToTree(QTreeWidgetItem* parentItem, const QDir& dir)
{
	// 获取所有条目
	QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot,
		QDir::Name | QDir::DirsFirst);

	foreach(const QFileInfo & info, entries) {
		if (info.isDir()) {
			// 处理子文件夹 - 先创建临时项来测试是否有内容
			QTreeWidgetItem* tempFolderItem = new QTreeWidgetItem();
			tempFolderItem->setText(0, info.fileName());
			tempFolderItem->setIcon(0, this->style()->standardIcon(QStyle::SP_DirIcon));
			tempFolderItem->setData(0, Qt::UserRole, info.absoluteFilePath());

			// 递归处理子文件夹
			QDir subDir(info.absoluteFilePath());
			addHtmlFilesToTree(tempFolderItem, subDir);

			// 只有当文件夹包含HTML文件时才添加到树中
			if (tempFolderItem->childCount() > 0) {
				parentItem->addChild(tempFolderItem);
				// 如果子文件夹包含文件，则展开它
				tempFolderItem->setExpanded(true);
			}
			else {
				// 如果没有子项，删除临时项
				delete tempFolderItem;
			}
		}
		else if (info.suffix().toLower() == "html" || info.suffix().toLower() == "htm") {
			// 处理HTML文件
			QTreeWidgetItem* fileItem = new QTreeWidgetItem(parentItem);
			fileItem->setText(0, info.baseName());  // 使用baseName()去掉后缀名
			fileItem->setIcon(0, this->style()->standardIcon(QStyle::SP_FileIcon));
			fileItem->setData(0, Qt::UserRole, info.absoluteFilePath());

			// 设置文件项的样式
			fileItem->setToolTip(0, info.absoluteFilePath());
		}
	}
}

void HtmlViewerWidget::onFileItemClicked(QTreeWidgetItem* item, int column)
{
	Q_UNUSED(column)

		if (!item) {
			return;
		}

	QString filePath = item->data(0, Qt::UserRole).toString();
	QFileInfo fileInfo(filePath);

	// 只处理HTML文件，不处理文件夹
	if (fileInfo.isFile() &&
		(fileInfo.suffix().toLower() == "html" || fileInfo.suffix().toLower() == "htm")) {
		loadHtmlFile(filePath);
		emit fileSelected(filePath);
	}
}

void HtmlViewerWidget::loadHtmlFile(const QString& filePath)
{
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QString errorHtml = "<html><body style='padding: 20px; font-family: Microsoft YaHei;'>"
			"<p style='color: red; font-size: 18px;'>❌ 无法打开文件: " + filePath + "</p>"
			"</body></html>";
		contentDisplay->setHtml(errorHtml);
		return;
	}

	QTextStream in(&file);
	in.setCodec("UTF-8");  // 确保正确处理中文
	QString htmlContent = in.readAll();
	file.close();

	// 增强HTML内容样式并显示
	QString enhancedHtml = enhanceHtmlContent(htmlContent);
	contentDisplay->setHtml(enhancedHtml);

	// 更新当前状态
	currentFilePath = filePath;
	currentContent = htmlContent;

	emit contentChanged(htmlContent);
}

QString HtmlViewerWidget::enhanceHtmlContent(const QString& html)
{
	QString enhancedHtml = html;

	// 检查HTML是否已经包含完整的HTML结构
	bool hasHtmlTag = html.contains("<html", Qt::CaseInsensitive);
	bool hasBodyTag = html.contains("<body", Qt::CaseInsensitive);

	if (hasHtmlTag && hasBodyTag) {
		// 如果已经是完整的HTML文档，检查是否包含MathJax
		QString processedContent = processEmojis(enhancedHtml);

		// 如果没有MathJax配置，添加它
		if (!processedContent.contains("MathJax", Qt::CaseInsensitive)) {
			// 在</head>前插入MathJax配置
			QString mathJaxScript =
				"<!-- MathJax配置 - 自动注入 -->"
				"<script type='text/x-mathjax-config'>"
				"MathJax.Hub.Config({"
				"  tex2jax: {"
				"    inlineMath: [['$','$'], ['\\\\(','\\\\)'], ['`$','$`']],"
				"    displayMath: [['$$','$$'], ['\\\\[','\\\\]'], ['```math','```']],"
				"    processEscapes: true,"
				"    processEnvironments: true,"
				"    skipTags: ['script', 'noscript', 'style', 'textarea', 'pre', 'code']"
				"  },"
				"  TeX: {"
				"    extensions: ['AMSmath.js', 'AMSsymbols.js', 'color.js', 'mhchem.js']"
				"  },"
				"  'HTML-CSS': { scale: 110 },"
				"  showMathMenu: false"
				"});"
				"</script>"
				"<script src='https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.9/MathJax.js?config=TeX-AMS-MML_HTMLorMML'></script>";

			processedContent.replace("</head>", mathJaxScript + "</head>", Qt::CaseInsensitive);
		}

		return processedContent;
	}
	else {
		// 如果只是HTML片段，包装在完整的HTML结构中
		QString enhancedTemplate =
			"<html><head>"
			"<meta charset='UTF-8'>"
			"<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
			"<!-- MathJax配置 - 全功能增强版 -->"
			"<script type='text/x-mathjax-config'>"
			"MathJax.Hub.Config({"
			"  tex2jax: {"
			"    inlineMath: [['$','$'], ['\\\\(','\\\\)'], ['\\\\begin{math}','\\\\end{math}'], ['\\\\(','\\\\)'], ['`$','$`']],"
			"    displayMath: [['$$','$$'], ['\\\\[','\\\\]'], ['\\\\begin{displaymath}','\\\\end{displaymath}'], ['```math','```']],"
			"    processEscapes: true,"
			"    processEnvironments: true,"
			"    processRefs: true,"
			"    skipTags: ['script', 'noscript', 'style', 'textarea', 'pre', 'code'],"
			"    ignoreClass: 'tex2jax_ignore|no-mathjax',"
			"    processClass: 'tex2jax_process|mathjax'"
			"  },"
			"  TeX: {"
			"    equationNumbers: { autoNumber: 'AMS' },"
			"    extensions: ['AMSmath.js', 'AMSsymbols.js', 'noErrors.js', 'noUndefined.js', 'color.js', 'mhchem.js', 'cancel.js'],"
			"    Macros: {"
			"      RR: '{\\\\mathbb{R}}',"
			"      CC: '{\\\\mathbb{C}}',"
			"      NN: '{\\\\mathbb{N}}',"
			"      ZZ: '{\\\\mathbb{Z}}',"
			"      QQ: '{\\\\mathbb{Q}}',"
			"      PP: '{\\\\mathbb{P}}',"
			"      EE: '{\\\\mathbb{E}}',"
			"      ii: '{\\\\mathrm{i}}',"
			"      dd: '{\\\\mathrm{d}}',"
			"      ee: '{\\\\mathrm{e}}',"
			"      vec: ['{\\\\boldsymbol{#1}}', 1],"
			"      norm: ['{\\\\left\\\\|#1\\\\right\\\\|}', 1],"
			"      abs: ['{\\\\left|#1\\\\right|}', 1],"
			"      set: ['{\\\\left\\\\{#1\\\\right\\\\}}', 1]"
			"    }"
			"  },"
			"  'HTML-CSS': {"
			"    availableFonts: ['STIX', 'TeX', 'Latin-Modern'],"
			"    preferredFont: 'STIX',"
			"    webFont: 'STIX-Web',"
			"    styles: {"
			"      '.MathJax_Display': {"
			"        'margin': '1.5em 0',"
			"        'text-align': 'center',"
			"        'background': 'linear-gradient(135deg, #f8f9fa 0%, #e9ecef 100%)',"
			"        'padding': '15px',"
			"        'border-radius': '8px',"
			"        'border': '1px solid #e1e4e8',"
			"        'box-shadow': '0 2px 4px rgba(0,0,0,0.05)'"
			"      },"
			"      '.MathJax': {"
			"        'color': '#24292e'"
			"      }"
			"    },"
			"    scale: 110,"
			"    linebreaks: { automatic: true, width: '80%' }"
			"  },"
			"  MathML: {"
			"    extensions: ['content-mathml.js']"
			"  },"
			"  SVG: {"
			"    scale: 110,"
			"    linebreaks: { automatic: true, width: '80%' },"
			"    styles: {"
			"      '.MathJax_SVG_Display': {"
			"        'margin': '1.5em 0',"
			"        'text-align': 'center',"
			"        'background': 'linear-gradient(135deg, #f8f9fa 0%, #e9ecef 100%)',"
			"        'padding': '15px',"
			"        'border-radius': '8px',"
			"        'border': '1px solid #e1e4e8',"
			"        'box-shadow': '0 2px 4px rgba(0,0,0,0.05)'"
			"      }"
			"    }"
			"  },"
			"  CommonHTML: {"
			"    scale: 110,"
			"    linebreaks: { automatic: true, width: '80%' }"
			"  },"
			"  showMathMenu: false,"
			"  showProcessingMessages: false,"
			"  messageStyle: 'none',"
			"  skipStartupTypeset: false"
			"});"
			"MathJax.Hub.Queue(['Typeset', MathJax.Hub]);"
			"</script>"
			"<script src='https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.9/MathJax.js?config=TeX-AMS-MML_HTMLorMML'></script>"
			"<!-- 化学公式支持 -->"
			"<script src='https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.9/extensions/TeX/mhchem.js'></script>"
			"<!-- Emoji支持 -->"
			"<style>"
			"* {"
			"    box-sizing: border-box;"
			"}"
			"body {"
			"    font-family: '等线', 'DengXian', 'Microsoft YaHei UI', 'Segoe UI', 'SF Pro Display', -apple-system, BlinkMacSystemFont, sans-serif;"
			"    line-height: 1.8;"
			"    margin: 0;"
			"    padding: 0;"
			"    color: #2c3e50;"
			"    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);"
			"    min-height: 100vh;"
			"}"
			".container {"
			"    max-width: 900px;"
			"    margin: 0 auto;"
			"    padding: 40px 20px;"
			"    background: rgba(255, 255, 255, 0.95);"
			"    backdrop-filter: blur(10px);"
			"    border-radius: 20px;"
			"    box-shadow: 0 20px 40px rgba(0, 0, 0, 0.1);"
			"    margin-top: 20px;"
			"    margin-bottom: 20px;"
			"    animation: slideInUp 0.6s ease-out;"
			"}"
			"@keyframes slideInUp {"
			"    from {"
			"        opacity: 0;"
			"        transform: translateY(30px);"
			"    }"
			"    to {"
			"        opacity: 1;"
			"        transform: translateY(0);"
			"    }"
			"}"
			"h1, h2, h3, h4, h5, h6 {"
			"    color: #2c3e50;"
			"    margin-top: 2em;"
			"    margin-bottom: 1em;"
			"    font-weight: 700;"
			"    line-height: 1.3;"
			"    position: relative;"
			"}"
			"h1 {"
			"    font-size: 2.5em;"
			"    text-align: center;"
			"    background: linear-gradient(135deg, #667eea, #764ba2);"
			"    -webkit-background-clip: text;"
			"    -webkit-text-fill-color: transparent;"
			"    background-clip: text;"
			"    margin-bottom: 1.5em;"
			"    padding-bottom: 20px;"
			"    border-bottom: 3px solid transparent;"
			"    border-image: linear-gradient(90deg, #667eea, #764ba2) 1;"
			"}"
			"h2 {"
			"    font-size: 2em;"
			"    color: #667eea;"
			"    padding-left: 20px;"
			"    border-left: 5px solid #667eea;"
			"    background: linear-gradient(90deg, rgba(102, 126, 234, 0.1), transparent);"
			"    padding: 15px 20px;"
			"    border-radius: 0 10px 10px 0;"
			"    margin: 30px 0 20px 0;"
			"}"
			"h3 {"
			"    font-size: 1.6em;"
			"    color: #764ba2;"
			"    border-bottom: 2px dotted #764ba2;"
			"    padding-bottom: 8px;"
			"}"
			"h4 {"
			"    font-size: 1.3em;"
			"    color: #5a67d8;"
			"}"
			"h5, h6 {"
			"    font-size: 1.1em;"
			"    color: #718096;"
			"}"
			"p {"
			"    margin-bottom: 1.5em;"
			"    font-size: 16px;"
			"    text-align: justify;"
			"    text-indent: 2em;"
			"    transition: all 0.3s ease;"
			"}"
			"p:hover {"
			"    transform: translateX(5px);"
			"    color: #1a202c;"
			"}"
			"code {"
			"    background: linear-gradient(135deg, #f7fafc 0%, #edf2f7 100%);"
			"    padding: 6px 10px;"
			"    border-radius: 8px;"
			"    font-family: '等线', 'DengXian', 'SF Mono', 'Monaco', 'Inconsolata', 'Roboto Mono', 'Consolas', monospace;"
			"    font-size: 0.9em;"
			"    color: #e53e3e;"
			"    border: 1px solid #e2e8f0;"
			"    box-shadow: 0 2px 4px rgba(0,0,0,0.05);"
			"    font-weight: 600;"
			"}"
			"pre {"
			"    background: linear-gradient(135deg, #1a202c 0%, #2d3748 100%);"
			"    padding: 25px;"
			"    border-radius: 15px;"
			"    overflow-x: auto;"
			"    border: none;"
			"    box-shadow: 0 10px 25px rgba(0,0,0,0.2);"
			"    margin: 25px 0;"
			"    position: relative;"
			"}"
			"pre::before {"
			"    content: '';"
			"    position: absolute;"
			"    top: 0;"
			"    left: 0;"
			"    right: 0;"
			"    height: 30px;"
			"    background: linear-gradient(90deg, #e53e3e, #f56500, #38a169);"
			"    border-radius: 15px 15px 0 0;"
			"    opacity: 0.8;"
			"}"
			"pre code {"
			"    background: transparent;"
			"    padding: 0;"
			"    border: none;"
			"    color: #e2e8f0;"
			"    font-size: 14px;"
			"    font-family: '等线', 'DengXian', 'SF Mono', 'Monaco', 'Consolas', monospace;"
			"    box-shadow: none;"
			"    font-weight: normal;"
			"    margin-top: 10px;"
			"    display: block;"
			"}"
			"blockquote {"
			"    border-left: 5px solid #667eea;"
			"    margin: 25px 0;"
			"    padding: 20px 25px;"
			"    background: linear-gradient(135deg, rgba(102, 126, 234, 0.1) 0%, rgba(118, 75, 162, 0.05) 100%);"
			"    color: #4a5568;"
			"    border-radius: 0 15px 15px 0;"
			"    box-shadow: 0 5px 15px rgba(102, 126, 234, 0.1);"
			"    font-style: italic;"
			"    position: relative;"
			"    overflow: hidden;"
			"}"
			"blockquote::before {"
			"    content: '💬';"
			"    position: absolute;"
			"    top: 15px;"
			"    right: 20px;"
			"    font-size: 24px;"
			"    opacity: 0.3;"
			"}"
			"blockquote p {"
			"    margin: 0;"
			"    text-indent: 0;"
			"    font-size: 17px;"
			"}"
			"ul, ol {"
			"    margin-bottom: 20px;"
			"    padding-left: 35px;"
			"}"
			"li {"
			"    margin-bottom: 12px;"
			"    line-height: 1.7;"
			"    position: relative;"
			"    transition: all 0.3s ease;"
			"}"
			"li:hover {"
			"    transform: translateX(8px);"
			"    color: #667eea;"
			"}"
			"ul li::marker {"
			"    color: #667eea;"
			"    content: '▶ ';"
			"    font-size: 0.8em;"
			"}"
			"ol li::marker {"
			"    color: #764ba2;"
			"    font-weight: bold;"
			"}"
			"a {"
			"    color: #667eea;"
			"    text-decoration: none;"
			"    border-bottom: 2px solid transparent;"
			"    transition: all 0.3s ease;"
			"    font-weight: 500;"
			"    position: relative;"
			"}"
			"a:hover {"
			"    color: #764ba2;"
			"    border-bottom: 2px solid #667eea;"
			"    background: linear-gradient(135deg, rgba(102, 126, 234, 0.1) 0%, rgba(118, 75, 162, 0.1) 100%);"
			"    padding: 3px 6px;"
			"    border-radius: 6px;"
			"    transform: translateY(-2px);"
			"    box-shadow: 0 4px 8px rgba(102, 126, 234, 0.2);"
			"}"
			"table {"
			"    border-collapse: collapse;"
			"    width: 100%;"
			"    margin: 25px 0;"
			"    box-shadow: 0 15px 35px rgba(0,0,0,0.1);"
			"    border-radius: 15px;"
			"    overflow: hidden;"
			"    background: white;"
			"}"
			"th, td {"
			"    padding: 15px 20px;"
			"    text-align: left;"
			"    border: none;"
			"}"
			"th {"
			"    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);"
			"    color: white;"
			"    font-weight: 700;"
			"    font-size: 15px;"
			"    text-transform: uppercase;"
			"    letter-spacing: 1px;"
			"}"
			"td {"
			"    background: #ffffff;"
			"    transition: all 0.3s ease;"
			"    border-bottom: 1px solid #f7fafc;"
			"}"
			"tr:hover td {"
			"    background: linear-gradient(135deg, rgba(102, 126, 234, 0.05) 0%, rgba(118, 75, 162, 0.02) 100%);"
			"    transform: scale(1.01);"
			"}"
			"tr:last-child td {"
			"    border-bottom: none;"
			"}"
			"/* 数学公式样式 - 现代化版本 */"
			".MathJax_Display {"
			"    margin: 2em 0;"
			"    text-align: center;"
			"    background: linear-gradient(135deg, #f7fafc 0%, #edf2f7 100%);"
			"    padding: 25px;"
			"    border-radius: 15px;"
			"    border: 1px solid #e2e8f0;"
			"    box-shadow: 0 10px 25px rgba(0,0,0,0.08);"
			"    position: relative;"
			"    overflow: hidden;"
			"}"
			".MathJax_Display::before {"
			"    content: '📐';"
			"    position: absolute;"
			"    top: 10px;"
			"    right: 15px;"
			"    font-size: 20px;"
			"    opacity: 0.3;"
			"}"
			".MathJax {"
			"    font-size: 1.3em;"
			"    color: #2d3748;"
			"}"
			".MathJax_CHTML {"
			"    outline: 0;"
			"}"
			"/* 内联数学公式 */"
			".MathJax_CHTML.MathJax_Element {"
			"    background: linear-gradient(135deg, rgba(102, 126, 234, 0.1) 0%, rgba(118, 75, 162, 0.05) 100%);"
			"    border-radius: 6px;"
			"    padding: 3px 6px;"
			"    margin: 0 3px;"
			"    border: 1px solid rgba(102, 126, 234, 0.2);"
			"}"
			"/* Emoji样式 - 增强版 */"
			".emoji {"
			"    display: inline-block;"
			"    font-size: 1.3em;"
			"    line-height: 1;"
			"    vertical-align: middle;"
			"    margin: 0 0.2em;"
			"    transition: all 0.3s ease;"
			"    filter: drop-shadow(0 2px 4px rgba(0,0,0,0.1));"
			"}"
			".emoji:hover {"
			"    transform: scale(1.2) rotate(5deg);"
			"    filter: drop-shadow(0 4px 8px rgba(0,0,0,0.2));"
			"}"
			"/* 强调样式 - 现代化 */"
			"strong {"
			"    color: #e53e3e;"
			"    font-weight: 700;"
			"    background: linear-gradient(135deg, rgba(229, 62, 62, 0.1) 0%, rgba(229, 62, 62, 0.05) 100%);"
			"    padding: 2px 4px;"
			"    border-radius: 4px;"
			"}"
			"em {"
			"    color: #764ba2;"
			"    font-style: italic;"
			"    font-weight: 500;"
			"    background: linear-gradient(135deg, rgba(118, 75, 162, 0.1) 0%, rgba(118, 75, 162, 0.05) 100%);"
			"    padding: 1px 3px;"
			"    border-radius: 3px;"
			"}"
			"/* 分隔线 - 美化版 */"
			"hr {"
			"    border: none;"
			"    height: 3px;"
			"    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);"
			"    margin: 40px 0;"
			"    border-radius: 2px;"
			"    position: relative;"
			"    overflow: hidden;"
			"}"
			"hr::after {"
			"    content: '';"
			"    position: absolute;"
			"    top: 0;"
			"    left: -100%;"
			"    width: 100%;"
			"    height: 100%;"
			"    background: linear-gradient(90deg, transparent, rgba(255,255,255,0.6), transparent);"
			"    animation: shimmer 2s infinite;"
			"}"
			"@keyframes shimmer {"
			"    0% { left: -100%; }"
			"    100% { left: 100%; }"
			"}"
			"/* 滚动条美化 - 现代版 */"
			"::-webkit-scrollbar {"
			"    width: 12px;"
			"    height: 12px;"
			"}"
			"::-webkit-scrollbar-track {"
			"    background: linear-gradient(135deg, #f7fafc 0%, #edf2f7 100%);"
			"    border-radius: 6px;"
			"}"
			"::-webkit-scrollbar-thumb {"
			"    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);"
			"    border-radius: 6px;"
			"    border: 2px solid #f7fafc;"
			"}"
			"::-webkit-scrollbar-thumb:hover {"
			"    background: linear-gradient(135deg, #5a67d8 0%, #6b46c1 100%);"
			"    box-shadow: 0 4px 8px rgba(102, 126, 234, 0.3);"
			"}"
			"</style>"
			"</head><body><div class='container'>%1</div></body></html>";

		// 处理emoji后再套用模板
		QString processedHtml = processEmojis(enhancedHtml);
		return enhancedTemplate.arg(processedHtml);
	}
}

void HtmlViewerWidget::refreshFileList()
{
	if (!currentFolderPath.isEmpty()) {
		populateFileTree(currentFolderPath);
	}
}

QString HtmlViewerWidget::getCurrentFilePath() const
{
	return currentFilePath;
}

QString HtmlViewerWidget::getCurrentContent() const
{
	return currentContent;
}

void HtmlViewerWidget::loadFile(const QString& filePath)
{
	loadHtmlFile(filePath);
}

void HtmlViewerWidget::setTreeWidgetMinimumWidth(int width)
{
	if (fileTreeWidget) {
		fileTreeWidget->setMinimumWidth(width);
	}
}

void HtmlViewerWidget::setContentEditReadOnly(bool readOnly)
{
	// QWebEngineView默认是只读的，无需设置
	// 这个函数保留以保持API兼容性
	Q_UNUSED(readOnly)
}

QString HtmlViewerWidget::processEmojis(const QString& html)
{
	QString processed = html;

	// GitHub emoji映射表 - 常用的emoji
	QMap<QString, QString> emojiMap;

	// 表情相关
	emojiMap[":smile:"] = "😄";
	emojiMap[":laughing:"] = "😆";
	emojiMap[":joy:"] = "😂";
	emojiMap[":grinning:"] = "😀";
	emojiMap[":grin:"] = "😁";
	emojiMap[":smiley:"] = "😃";
	emojiMap[":blush:"] = "😊";
	emojiMap[":relaxed:"] = "☺️";
	emojiMap[":wink:"] = "😉";
	emojiMap[":heart_eyes:"] = "😍";
	emojiMap[":kissing_heart:"] = "😘";
	emojiMap[":stuck_out_tongue:"] = "😛";
	emojiMap[":stuck_out_tongue_winking_eye:"] = "😜";
	emojiMap[":sunglasses:"] = "😎";
	emojiMap[":smirk:"] = "😏";
	emojiMap[":neutral_face:"] = "😐";
	emojiMap[":expressionless:"] = "😑";
	emojiMap[":confused:"] = "😕";
	emojiMap[":worried:"] = "😟";
	emojiMap[":frowning:"] = "😦";
	emojiMap[":anguished:"] = "😧";
	emojiMap[":cry:"] = "😢";
	emojiMap[":sob:"] = "😭";
	emojiMap[":angry:"] = "😠";
	emojiMap[":rage:"] = "😡";
	emojiMap[":triumph:"] = "😤";
	emojiMap[":sleepy:"] = "😪";
	emojiMap[":yum:"] = "😋";
	emojiMap[":mask:"] = "😷";
	emojiMap[":cold_sweat:"] = "😰";
	emojiMap[":sweat_smile:"] = "😅";
	emojiMap[":dizzy_face:"] = "😵";
	emojiMap[":astonished:"] = "😲";
	emojiMap[":worried:"] = "😟";

	// 手势和人物
	emojiMap[":thumbsup:"] = "👍";
	emojiMap[":+1:"] = "👍";
	emojiMap[":thumbsdown:"] = "👎";
	emojiMap[":-1:"] = "👎";
	emojiMap[":ok_hand:"] = "👌";
	emojiMap[":punch:"] = "👊";
	emojiMap[":fist:"] = "✊";
	emojiMap[":v:"] = "✌️";
	emojiMap[":wave:"] = "👋";
	emojiMap[":raised_hand:"] = "✋";
	emojiMap[":open_hands:"] = "👐";
	emojiMap[":point_up:"] = "☝️";
	emojiMap[":point_down:"] = "👇";
	emojiMap[":point_left:"] = "👈";
	emojiMap[":point_right:"] = "👉";
	emojiMap[":pray:"] = "🙏";
	emojiMap[":clap:"] = "👏";
	emojiMap[":muscle:"] = "💪";

	// 心形和爱情
	emojiMap[":heart:"] = "❤️";
	emojiMap[":broken_heart:"] = "💔";
	emojiMap[":two_hearts:"] = "💕";
	emojiMap[":heartbeat:"] = "💓";
	emojiMap[":heartpulse:"] = "💗";
	emojiMap[":sparkling_heart:"] = "💖";
	emojiMap[":cupid:"] = "💘";
	emojiMap[":gift_heart:"] = "💝";
	emojiMap[":revolving_hearts:"] = "💞";
	emojiMap[":heart_decoration:"] = "💟";
	emojiMap[":blue_heart:"] = "💙";
	emojiMap[":green_heart:"] = "💚";
	emojiMap[":yellow_heart:"] = "💛";
	emojiMap[":purple_heart:"] = "💜";

	// 符号和标志
	emojiMap[":star:"] = "⭐";
	emojiMap[":star2:"] = "🌟";
	emojiMap[":dizzy:"] = "💫";
	emojiMap[":boom:"] = "💥";
	emojiMap[":collision:"] = "💥";
	emojiMap[":anger:"] = "💢";
	emojiMap[":sweat_drops:"] = "💦";
	emojiMap[":droplet:"] = "💧";
	emojiMap[":zzz:"] = "💤";
	emojiMap[":dash:"] = "💨";
	emojiMap[":fire:"] = "🔥";
	emojiMap[":flame:"] = "🔥";
	emojiMap[":sparkles:"] = "✨";
	emojiMap[":tada:"] = "🎉";
	emojiMap[":confetti_ball:"] = "🎊";
	emojiMap[":balloon:"] = "🎈";
	emojiMap[":gift:"] = "🎁";

	// 工作和学习
	emojiMap[":memo:"] = "📝";
	emojiMap[":pencil:"] = "📝";
	emojiMap[":book:"] = "📖";
	emojiMap[":books:"] = "📚";
	emojiMap[":page_facing_up:"] = "📄";
	emojiMap[":newspaper:"] = "📰";
	emojiMap[":bookmark_tabs:"] = "📑";
	emojiMap[":bar_chart:"] = "📊";
	emojiMap[":chart_with_upwards_trend:"] = "📈";
	emojiMap[":chart_with_downwards_trend:"] = "📉";
	emojiMap[":clipboard:"] = "📋";
	emojiMap[":pushpin:"] = "📌";
	emojiMap[":round_pushpin:"] = "📍";
	emojiMap[":paperclip:"] = "📎";
	emojiMap[":straight_ruler:"] = "📏";
	emojiMap[":triangular_ruler:"] = "📐";
	emojiMap[":closed_book:"] = "📕";
	emojiMap[":green_book:"] = "📗";
	emojiMap[":blue_book:"] = "📘";
	emojiMap[":orange_book:"] = "📙";
	emojiMap[":notebook:"] = "📓";
	emojiMap[":notebook_with_decorative_cover:"] = "📔";
	emojiMap[":ledger:"] = "📒";

	// 科技和工具
	emojiMap[":computer:"] = "💻";
	emojiMap[":desktop_computer:"] = "🖥️";
	emojiMap[":keyboard:"] = "⌨️";
	emojiMap[":mouse:"] = "🖱️";
	emojiMap[":trackball:"] = "🖲️";
	emojiMap[":joystick:"] = "🕹️";
	emojiMap[":printer:"] = "🖨️";
	emojiMap[":fax:"] = "📠";
	emojiMap[":phone:"] = "📞";
	emojiMap[":telephone:"] = "☎️";
	emojiMap[":mobile_phone:"] = "📱";
	emojiMap[":calling:"] = "📲";
	emojiMap[":email:"] = "📧";
	emojiMap[":inbox_tray:"] = "📥";
	emojiMap[":outbox_tray:"] = "📤";
	emojiMap[":package:"] = "📦";
	emojiMap[":mailbox:"] = "📪";
	emojiMap[":mailbox_closed:"] = "📪";
	emojiMap[":mailbox_with_mail:"] = "📬";
	emojiMap[":mailbox_with_no_mail:"] = "📭";
	emojiMap[":postbox:"] = "📮";

	// 警告和提示
	emojiMap[":warning:"] = "⚠️";
	emojiMap[":no_entry:"] = "⛔";
	emojiMap[":no_entry_sign:"] = "🚫";
	emojiMap[":x:"] = "❌";
	emojiMap[":o:"] = "⭕";
	emojiMap[":white_check_mark:"] = "✅";
	emojiMap[":heavy_check_mark:"] = "✔️";
	emojiMap[":heavy_multiplication_x:"] = "✖️";
	emojiMap[":exclamation:"] = "❗";
	emojiMap[":grey_exclamation:"] = "❕";
	emojiMap[":question:"] = "❓";
	emojiMap[":grey_question:"] = "❔";
	emojiMap[":bangbang:"] = "‼️";
	emojiMap[":interrobang:"] = "⁉️";
	emojiMap[":sos:"] = "🆘";

	// 动物
	emojiMap[":dog:"] = "🐶";
	emojiMap[":cat:"] = "🐱";
	emojiMap[":mouse:"] = "🐭";
	emojiMap[":hamster:"] = "🐹";
	emojiMap[":rabbit:"] = "🐰";
	emojiMap[":bear:"] = "🐻";
	emojiMap[":panda_face:"] = "🐼";
	emojiMap[":koala:"] = "🐨";
	emojiMap[":tiger:"] = "🐯";
	emojiMap[":lion_face:"] = "🦁";
	emojiMap[":cow:"] = "🐮";
	emojiMap[":pig:"] = "🐷";
	emojiMap[":pig_nose:"] = "🐽";
	emojiMap[":frog:"] = "🐸";
	emojiMap[":octopus:"] = "🐙";
	emojiMap[":monkey_face:"] = "🐵";
	emojiMap[":see_no_evil:"] = "🙈";
	emojiMap[":hear_no_evil:"] = "🙉";
	emojiMap[":speak_no_evil:"] = "🙊";

	// 遍历映射表进行替换
	for (auto it = emojiMap.begin(); it != emojiMap.end(); ++it) {
		processed.replace(it.key(), QString("<span class='emoji'>%1</span>").arg(it.value()));
	}

	return processed;
}

QIcon HtmlViewerWidget::createWindowIcon()
{
	// 创建48x48的高质量图标
	QPixmap pixmap(48, 48);
	pixmap.fill(Qt::transparent);

	QPainter painter(&pixmap);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::SmoothPixmapTransform);

	// 绘制现代化背景圆形渐变
	QRadialGradient backgroundGradient(24, 18, 24);
	backgroundGradient.setColorAt(0, QColor(255, 87, 34));   // 橙色 #ff5722
	backgroundGradient.setColorAt(0.3, QColor(255, 152, 0)); // 橙黄 #ff9800
	backgroundGradient.setColorAt(0.7, QColor(255, 193, 7)); // 黄色 #ffc107
	backgroundGradient.setColorAt(1, QColor(255, 235, 59));  // 亮黄 #ffeb3b

	painter.setBrush(QBrush(backgroundGradient));
	painter.setPen(QPen(QColor(230, 81, 0), 2)); // 深橙边框
	painter.drawEllipse(3, 3, 42, 42);

	// 绘制HTML5标志样式的 "<>" 符号
	painter.setPen(QPen(Qt::white, 3));
	painter.setFont(QFont("Arial", 12, QFont::Bold));

	// 绘制左尖括号 <
	QPainterPath leftBracket;
	leftBracket.moveTo(18, 15);
	leftBracket.lineTo(12, 24);
	leftBracket.lineTo(18, 33);
	painter.strokePath(leftBracket, QPen(Qt::white, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

	// 绘制右尖括号 >
	QPainterPath rightBracket;
	rightBracket.moveTo(30, 15);
	rightBracket.lineTo(36, 24);
	rightBracket.lineTo(30, 33);
	painter.strokePath(rightBracket, QPen(Qt::white, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

	// 绘制中间的 HTML 文字
	painter.setFont(QFont("Arial", 7, QFont::Bold));
	painter.setPen(QPen(Qt::white, 1));
	painter.drawText(QRect(19, 20, 10, 8), Qt::AlignCenter, "H");

	// 绘制装饰性光晕效果
	QRadialGradient glowGradient(24, 18, 20);
	glowGradient.setColorAt(0, QColor(255, 255, 255, 120));
	glowGradient.setColorAt(0.5, QColor(255, 255, 255, 60));
	glowGradient.setColorAt(1, QColor(255, 255, 255, 0));

	painter.setBrush(QBrush(glowGradient));
	painter.setPen(Qt::NoPen);
	painter.drawEllipse(4, 4, 40, 25);

	// 添加小点装饰
	painter.setBrush(QBrush(QColor(255, 255, 255, 180)));
	painter.setPen(Qt::NoPen);
	painter.drawEllipse(12, 38, 3, 3);
	painter.drawEllipse(33, 38, 3, 3);
	painter.drawEllipse(22, 42, 4, 4);

	return QIcon(pixmap);
}

QIcon HtmlViewerWidget::getWindowIcon() const
{
	return const_cast<HtmlViewerWidget*>(this)->createWindowIcon();
}