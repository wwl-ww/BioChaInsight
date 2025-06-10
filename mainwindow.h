#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"
#include "table.h"

#include "AssistantWidget.h"
#include "HtmlViewerWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindowClass; };
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

	// 主界面
	void init_main();
	void init_menu();
	void init_status();
	void init_tool();
	void init_readme();

	// 序列匹配
	void alignment_init();
	void alignment_run();

	// 降维
	void pca_init();
	void pca_run();

	// 聚类
	void km_init();
	void km_run();

	// 表达对比
	void exp_init();

	// 矩阵可视化
	void array_init();

	// 差异表达/筛选
	void find_init();
	void find_run();

	// 数据预处理
	void pro_init();
	void pro_run();
	void st_run();

	// 助手窗口
	void initAssistant();
	void initReader();

	// 子窗口事件同步
	void moveEvent(QMoveEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
	void closeEvent(QCloseEvent* event) override;

private:
	Ui::MainWindowClass* ui;
	TableWidget* tbl = nullptr;  // 表格
	AssistantWidget* assistant = nullptr; // 助手窗口
	HtmlViewerWidget* mdReader = nullptr; // Markdown阅读器
};
