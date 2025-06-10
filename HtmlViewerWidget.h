#ifndef HTMLVIEWERWIDGET_H
#define HTMLVIEWERWIDGET_H

#include <QWidget>
#include <QSplitter>
#include <QTreeWidget>
#include <QWebEngineView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDir>
#include <QTextStream>
#include <QFileInfo>
#include <QHeaderView>
#include <QLabel>
#include <QTreeWidgetItem>
#include <QRegularExpression>
#include <QIcon>
#include <QPixmap>
#include <QPainter>

class HtmlViewerWidget : public QWidget
{
	Q_OBJECT

public:
	explicit HtmlViewerWidget(QWidget* parent = nullptr);
	~HtmlViewerWidget();

	// 公共接口方法
	void setHtmlFolder(const QString& folderPath);
	void refreshFileList();
	QString getCurrentFilePath() const;
	QString getCurrentContent() const;
	void loadFile(const QString& filePath);

	// 设置界面样式
	void setTreeWidgetMinimumWidth(int width);
	void setContentEditReadOnly(bool readOnly = true);

	// 获取窗口图标
	QIcon getWindowIcon() const;

signals:
	void fileSelected(const QString& filePath);
	void contentChanged(const QString& content);
	void folderChanged(const QString& folderPath);

private slots:
	void onFileItemClicked(QTreeWidgetItem* item, int column);

private:
	void setupUI();
	void populateFileTree(const QString& folderPath);
	void loadHtmlFile(const QString& filePath);
	QString enhanceHtmlContent(const QString& html);
	QString processEmojis(const QString& html);
	void addHtmlFilesToTree(QTreeWidgetItem* parentItem, const QDir& dir);
	QIcon createWindowIcon();

	// UI组件
	QHBoxLayout* mainLayout;
	QSplitter* splitter;
	QTreeWidget* fileTreeWidget;
	QWebEngineView* contentDisplay;

	// 数据
	QString currentFolderPath;
	QString currentFilePath;
	QString currentContent;
};

#endif // 