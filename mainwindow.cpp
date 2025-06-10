#pragma  execution_character_set("utf-8")
#include "stdafx.h"

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QToolButton>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QColorDialog>
#include <QPalette>
#include <QFontDialog>
#include <QApplication>
#include <QTextBrowser>
#include <QScrollArea>
#include <QPushButton>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QFile>
#include <QTextStream>
#include <QDockWidget>
#include <QFileDialog>
#include "Alignment.h"
#include <QWebEngineView>
#include <QWebEngineSettings>
#include "FASTA.h"
#include <qdebug.h>
#include "table.h"
#include <QVBoxLayout>

#include <iostream>
#include "BCarray.h"
#include "BCmatrix.h"
#include "matplotlibcpp.h"
#include "plot.h"
#include "hisvg.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

QString defaultDir = "C:\\Users\\wwl\\source\\repos\\BioChaInsight\\data";

BCmatrix* loadCSV(const string& path) {
	// 检查文件是否存在且可读
	ifstream in(path);
	if (!in.is_open())
		throw runtime_error("loadCSV 错误：无法打开文件 \"" + path + "\"");
	in.close();

	BCmatrix* data = new BCmatrix();
	data->load_data(path);

	// 检查是否是空矩阵
	if (data->getColumnCount() == 0) {
		delete data;
		throw runtime_error("loadCSV 错误：文件 \"" + path + "\" 中未读取到任何列 (getColumnCount() == 0)");
	}

	return data;
}

template<typename T>
bool saveVectorToCsv(const vector<T>& vec, const string& path) {
	ofstream ofs(path);
	if (!ofs.is_open()) {
		cerr << "Failed to open file: " << path << "\n";
		return false;
	}
	ostringstream oss;
	for (size_t i = 0; i < vec.size(); ++i) {
		oss << vec[i];
		if (i + 1 < vec.size())
			oss << ',';
	}
	ofs << oss.str();
	return true;
}

void applyFontToAllChildren(QWidget* widget, const QFont& font) {
	widget->setFont(font);
	for (QObject* child : widget->children()) {
		QWidget* childWidget = qobject_cast<QWidget*>(child);
		if (childWidget) {
			applyFontToAllChildren(childWidget, font);
		}
	}
}

void applyFontColorToAllChildren(QWidget* widget, const QColor& color) {
	QString style = QString("color: %1;").arg(color.name());
	widget->setStyleSheet(style);

	for (QObject* child : widget->children()) {
		QWidget* childWidget = qobject_cast<QWidget*>(child);
		if (childWidget) {
			applyFontColorToAllChildren(childWidget, color);
		}
	}
}

// 子窗口事件同步
void MainWindow::moveEvent(QMoveEvent* event) {
	QMainWindow::moveEvent(event);
	if (assistant) assistant->syncPositionWith(this);
}

void MainWindow::resizeEvent(QResizeEvent* event) {
	QMainWindow::resizeEvent(event);
	if (assistant) assistant->syncPositionWith(this);
}

void MainWindow::closeEvent(QCloseEvent* event) {
	if (assistant) assistant->close();
	QMainWindow::closeEvent(event);
}

void MainWindow::alignment_init()
{
	connect(ui->menu11, &QAction::triggered, this, [this]() {
		ui->viewStack->setCurrentWidget(ui->pageAlign);
		});
	connect(ui->menu11_2, &QAction::triggered, this, [this]() {
		ui->viewStack->setCurrentWidget(ui->pageAlign);
		});

	// 默认先展示空白
	ui->swSeq1Input->setCurrentWidget(ui->seq1White);
	ui->swSeq2Input->setCurrentWidget(ui->seq2White);

	// 文件读入 or 手动输入
	// Seq1 手动 / 文件 模式切换
	connect(ui->rbSeq1Manual, &QRadioButton::toggled, this, [this](bool checked) {
		if (checked) ui->swSeq1Input->setCurrentIndex(2);  // 手动 2
		});
	connect(ui->rbSeq1File, &QRadioButton::toggled, this, [this](bool checked) {
		if (checked) ui->swSeq1Input->setCurrentIndex(0);  // 文件 0
		});

	// Seq2 手动 / 文件 模式切换
	connect(ui->rbSeq2Manual, &QRadioButton::toggled, this, [this](bool checked) {
		if (checked) ui->swSeq2Input->setCurrentIndex(2);  // 手动 2
		});
	connect(ui->rbSeq2File, &QRadioButton::toggled, this, [this](bool checked) {
		if (checked) ui->swSeq2Input->setCurrentIndex(0);  // 文件 0
		});

	// 文件选择按钮
	connect(ui->pushButton_1, &QPushButton::clicked, this, [this]() {
		QString filePath = QFileDialog::getOpenFileName(
			this,
			tr("choose FASTA file"),
			defaultDir,
			tr("*")
		);
		if (!filePath.isEmpty()) {
			ui->lineEdit_1->setText(filePath);
		}
		});

	connect(ui->pushButton_2, &QPushButton::clicked, this, [this]() {
		QString filePath = QFileDialog::getOpenFileName(
			this,
			tr("choose FASTA file"),
			defaultDir,
			tr("*")
		);
		if (!filePath.isEmpty()) {
			ui->lineEdit_2->setText(filePath);
		}
		});

	// 执行按钮
	connect(ui->btnRun, &QPushButton::clicked,
		this, &MainWindow::alignment_run);
}

void MainWindow::alignment_run()
{
	// 清除掉之前的输入
	ui->teResult->clear();

	Sequence seq1, seq2;
	// 文件
	if (ui->swSeq1Input->currentIndex() == 0) {
		QString path1 = ui->lineEdit_1->text();
		if (path1.isEmpty()) {
			qDebug() << "xixi";
			ui->teResult->append("Error: 请先为序列1选择一个 FASTA 文件");
			return;
		}
		int idx1 = ui->spinBox_1->value();
		try {
			FASTAReader reader(path1.toStdString());
			seq1 = reader[idx1];
		}
		catch (const out_of_range&) {
			ui->teResult->append(
				QString("Error: FASTA 文件中不存在索引 %1 的序列").arg(idx1)
			);
			return;
		}
	}
	// 手动模式
	else {
		string s1 = ui->leSeq1Manual->text().toStdString();
		if (s1.empty()) {
			ui->teResult->append("Error: 请在序列1输入框中手动输入序列");
			return;
		}
		seq1 = Sequence(s1);
	}

	// 文件模式
	if (ui->swSeq2Input->currentIndex() == 0) {
		QString path2 = ui->lineEdit_2->text();
		if (path2.isEmpty()) {
			ui->teResult->append("Error: 请先为序列2选择一个 FASTA 文件");
			return;
		}
		int idx2 = ui->spinBox_2->value();
		try {
			FASTAReader reader(path2.toStdString());
			seq2 = reader[idx2];
		}
		catch (const out_of_range&) {
			ui->teResult->append(
				QString("Error: FASTA 文件中不存在索引 %1 的序列").arg(idx2)
			);
			return;
		}
	}
	// 手动模式
	else {
		string s2 = ui->leSeq2Manual->text().toStdString();
		if (s2.empty()) {
			ui->teResult->append("Error: 请在序列2输入框中手动输入序列");
			return;
		}
		seq2 = Sequence(s2);
	}

	AlignmentAlgorithm* alg = nullptr;
	QString algName = ui->cbAlgorithm->currentText();
	if (algName == "NeedlemanWunsch") {
		alg = new NeedlemanWunsch(seq1.getSequence(), seq2.getSequence());
	}
	else if (algName == "SmithWaterman") {
		alg = new SmithWaterman(seq1.getSequence(), seq2.getSequence());
	}
	else if (algName == "Gotoh") {
		alg = new Gotoh(seq1.getSequence(), seq2.getSequence());
	}
	else if (algName == "Hirschberg") {
		alg = new Hirschberg(seq1.getSequence(), seq2.getSequence());
	}
	else {
		ui->teResult->append("Error: 未知的对齐算法");
		return;
	}

	// 执行算法
	alg->align();

	// 显示逻辑
	{
		const string& s1 = alg->getAlignedSeq1();
		const string& s2 = alg->getAlignedSeq2();
		const vector<int>& st1 = alg->getSeq1State();
		const vector<int>& st2 = alg->getSeq2State();

		// 生成 HTML
		QString html;
		html += "<p><b>Alignment Result:</b></p>";
		html += "<pre style='font-family: JetBrains Mono; font-size: 18px; line-height:0.6;'>";

		// 第一条序列
		for (size_t i = 0; i < s1.size(); ++i) {
			QChar c(s1[i]);
			switch (st1[i]) {
			case AlignmentAlgorithm::MATCH:
				html += "<span style='background-color:lightgreen;'>" + QString(c) + "</span>";
				break;
			case AlignmentAlgorithm::FAIL:
				html += "<span style='background-color:lightcoral;'>" + QString(c) + "</span>";
				break;
			case AlignmentAlgorithm::GAP:
				html += "<span style='background-color:lightgray;'>" + QString(c) + "</span>";
				break;
			}
		}
		html += "\n\n";

		// 第二条序列
		for (size_t i = 0; i < s2.size(); ++i) {
			QChar c(s2[i]);
			switch (st2[i]) {
			case AlignmentAlgorithm::MATCH:
				html += "<span style='background-color:lightgreen;'>" + QString(c) + "</span>";
				break;
			case AlignmentAlgorithm::FAIL:
				html += "<span style='background-color:lightcoral;'>" + QString(c) + "</span>";
				break;
			case AlignmentAlgorithm::GAP:
				html += "<span style='background-color:lightgray;'>" + QString(c) + "</span>";
				break;
			}
		}
		html += "</pre>";

		// 显示到 QTextEdit
		ui->teResult->clear();
		ui->teResult->setHtml(html);
	}

	// 可选：统计图
	if (ui->chkDrawStats->isChecked()) {
		const string& s1 = alg->getAlignedSeq1();
		const string& s2 = alg->getAlignedSeq2();
		const Sequence seq1(s1);
		const Sequence seq2(s2);
		GenePlot::showTwoBaseCompositionPieDialog(seq1, seq2, this);
	}

	// 可选：轨迹可视化
	if (ui->chkVisualizeTrace->isChecked()) {
		const string& s1 = alg->getAlignedSeq1();
		const string& s2 = alg->getAlignedSeq2();
		double len = max(s1.size(), s2.size());
		auto mat = alg->getHighlightedMatrix(len);
		GenePlot::plot_heatmap(mat);
	}

	delete alg;
}

void MainWindow::pca_init() {
	// 进入按钮
	connect(ui->menu15, &QAction::triggered, this, [this]() {
		ui->viewStack->setCurrentWidget(ui->pagePCA);
		});
	connect(ui->menu15_2, &QAction::triggered, this, [this]() {
		ui->viewStack->setCurrentWidget(ui->pagePCA);
		});

	// 默认先展示空白
	ui->stDim->setCurrentWidget(ui->PCA);

	// 页面切换
	connect(ui->comboBoxDim, &QComboBox::currentTextChanged,
		this, [this](const QString& text) {
			if (text == "PCA")      ui->stDim->setCurrentWidget(ui->PCA);
			else if (text == "LLE") ui->stDim->setCurrentWidget(ui->LLE);
			else if (text == "TSNE") ui->stDim->setCurrentWidget(ui->tSNE);
		});

	// 文件选择
	connect(ui->pbDimIn, &QPushButton::clicked, this, [this]() {
		QString filePath = QFileDialog::getOpenFileName(
			this,
			tr("choose input csv file"),
			defaultDir,
			tr("*.csv")
		);
		if (!filePath.isEmpty()) {
			ui->leDimIn->setText(filePath);

			// 默认输出位置
			QString filePath = ui->leDimIn->text();
			int pos = filePath.lastIndexOf('.');
			if (pos != -1)
				filePath.insert(pos, "_out");         // 结果 "/path/to/xxx_out.csv"
			ui->leDimOut->setText(filePath);
		}
		});
	connect(ui->pbDimOut, &QPushButton::clicked, this, [this]() {
		QString filePath = QFileDialog::getOpenFileName(
			this,
			tr("choose output csv file"),
			defaultDir,
			tr("*.csv")
		);
		if (!filePath.isEmpty()) {
			ui->leDimOut->setText(filePath);
		}
		});

	// 设置按钮的默认值
	ui->pcaObjDim->setValue(2);
	ui->tsneObjDim->setValue(2);
	ui->tsneCon->setValue(30.0);
	ui->lleDimObj->setValue(2);
	ui->lleNei->setValue(3);
	ui->lleReg->setValue(0.01);

	// 显示文件界面
	ui->PCAtoFileWidget->setVisible(ui->PCAtoFile->isChecked());
	connect(ui->PCAtoFile,
		QOverload<bool>::of(&QRadioButton::toggled),
		ui->PCAtoFileWidget,
		&QWidget::setVisible);

	// 绑定运行按钮
	connect(ui->runDim, &QPushButton::clicked,
		this, &MainWindow::pca_run);

}

void MainWindow::pca_run() {
	// 加载数据
	string path = ui->leDimIn->text().toStdString();

	BCmatrix* data = nullptr;
	try {
		data = loadCSV(path);
	}
	catch (const exception& e) {
		QMessageBox::warning(
			this,
			tr("no data"),
			tr("no data provided, maybe no file or a blank file?")
		);
		return;
	}
	BCmatrix res;

	// 算法
	QString algName = ui->comboBoxDim->currentText();
	if (algName == "PCA") {
		res = data->performPCA(ui->pcaObjDim->value());
	}
	else if (algName == "LLE") {
		res = data->performLLE(ui->lleNei->value(), ui->lleDimObj->value(), ui->lleReg->value());
	}
	else if (algName == "TSNE") {
		res = data->performTSNE(ui->tsneObjDim->value(), ui->tsneCon->value());
	}

	// 输出
	if (ui->PCAtoFile->isChecked()) {
		string savePath = ui->leDimOut->text().toStdString();
		res.to_csv(savePath);
		// 提示
		QString qmsg = QString::fromStdString("Successfully saved to " + savePath);
		QMessageBox::information(this, tr("Save Complete"), qmsg);
	}
	else if (ui->PCAtoWin->isChecked()) {
		string savePath = "C:\\Users\\wwl\\source\\repos\\BioChaInsight\\_protected_file\\_tmp.csv";
		res.to_csv(savePath);

		// 跳转到表格
		ui->viewStack->setCurrentWidget(ui->pageArray);
		ui->delCSVBt->click(); // 防止已经有表格了
		QString qPath = QString::fromStdString(savePath);
		ui->leArray->setText(qPath);
		ui->csvReadBt->click();
	}
	else {
		QMessageBox::warning(
			this,
			tr("out mode choose"),
			tr("choose file/windows to save your results")
		);
		return;
	}
}

void MainWindow::km_init() {
	// 进入按钮
	connect(ui->menu16, &QAction::triggered, this, [this]() {
		ui->viewStack->setCurrentWidget(ui->pageKM);
		});
	connect(ui->menu16_2, &QAction::triggered, this, [this]() {
		ui->viewStack->setCurrentWidget(ui->pageKM);
		});

	// 算法选择
	ui->cbCluster->setCurrentIndex(0);
	connect(ui->cbCluster, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx) {
		// 层次聚类不需要聚类数
		if (idx == 2) {
			ui->lbClusters->hide();
			ui->nClusters->hide();
			ui->cluView->hide();
			ui->cluView->setChecked(false);
		}
		else {
			ui->lbClusters->show();
			ui->nClusters->show();
			ui->cluView->show();
		}
		});

	// 文件选择
	connect(ui->pbCluIn, &QPushButton::clicked, this, [this]() {
		QString filePath = QFileDialog::getOpenFileName(
			this,
			tr("choose input csv file"),
			defaultDir,
			tr("*.csv")
		);
		if (!filePath.isEmpty()) {
			ui->leCluIn->setText(filePath);

			// 默认输出位置
			QString filePath = ui->leCluIn->text();
			int pos = filePath.lastIndexOf('.');
			if (pos != -1)
				filePath.insert(pos, "_clusters");         // 结果 "/path/to/xxx_clusters.csv"
			ui->leCluOut->setText(filePath);
		}
		});
	connect(ui->pbCluOut, &QPushButton::clicked, this, [this]() {
		QString filePath = QFileDialog::getOpenFileName(
			this,
			tr("choose output csv file"),
			defaultDir,
			tr("*.csv")
		);
		if (!filePath.isEmpty()) {
			ui->leCluOut->setText(filePath);
		}
		});

	// 可选项
	// 一开始输出框不可见
	ui->outGp->hide();
	ui->cluOut->setChecked(false);
	connect(ui->cluOut, QOverload<bool>::of(&QCheckBox::toggled), ui->outGp, &QWidget::setVisible);

	// 绑定运行按钮
	connect(ui->kmRun, &QPushButton::clicked,
		this, &MainWindow::km_run);

}

void MainWindow::km_run() {
	// 加载数据
	string path = ui->leCluIn->text().toStdString();
	BCmatrix* raw_data = nullptr;
	try {
		raw_data = loadCSV(path);
	}
	catch (const exception& e) {
		QMessageBox::warning(
			this,
			tr("no data"),
			tr("no data provided, maybe no file or a blank file?")
		);
		return;
	}

	vector<vector<double>> data = raw_data->getPureValue();
	vector<int> res;
	string savePath = ui->leCluOut->text().toStdString();

	QString algName = ui->cbCluster->currentText();
	// 簇聚类
	if (algName == "k-means" || algName == "k-means++") {

		int N = ui->nClusters->value();
		if (N <= 1) {
			QMessageBox::warning(
				this,
				tr("invalid number"),
				tr("number of clusters should >= 1")
			);
			return;
		}

		if (algName == "k-means")
			res = StatTools::kMeans(data, N);
		else
			res = StatTools::kMeansPP(data, N);

		if (ui->cluView->isChecked() == 0 && ui->cluOut->isChecked() == 0) {
			QMessageBox::warning(
				this,
				tr("out mode choose"),
				tr("choose to output a picture or a file")
			);
			return;
		}

		if (ui->cluOut->isChecked())
		{
			saveVectorToCsv(res, savePath);
			// 提示
			QString qmsg = QString::fromStdString("Successfully saved to " + savePath);
			QMessageBox::information(this, tr("Save Complete"), qmsg);
		}

		if (ui->cluView->isChecked())
		{
			if (raw_data->getColumnCount() != 2) {
				QMessageBox::warning(
					this,
					tr("dim error"),
					tr("only data with 2 columns can be treated as a plot")
				);
				return;
			}
			GenePlot::plotClusterScatter(data, res);
		}

	}
	// 层次聚类
	else {
		const int N = 18;

		vector<string> all_genes = raw_data->getRowName();

		if (ui->cluOut->isChecked() == 0) {
			QMessageBox::warning(
				this,
				tr("out mode choose"),
				tr("choose to output a file")
			);
			return;
		}

		vector<string> genes;
		for (int i = 0; i < N; i++)
			genes.push_back(all_genes[i]);
		vector<vector<double>> head;
		for (int i = 0; i < N; i++)
			head.push_back(data[i]);

		cout << "data prepared" << endl;

		auto distMat = computeDistanceMatrix(head);
		cout << "dismat prepared" << endl;

		auto Z = hierarchicalClustering(distMat);
		cout << "clustering finished" << endl;

		double maxH;
		Node* root = buildDendrogram(Z, N, maxH);

		const double canvasW = 1000, canvasH = 600;
		writeSVG(root, N, canvasW, canvasH, savePath, genes);

		// 提示
		QString qmsg = QString::fromStdString("Successfully saved to " + savePath);
		QMessageBox::information(this, tr("Save Complete"), qmsg);

		cleanup(root);
	}

	delete raw_data;
}

void MainWindow::exp_init() {
	// 进入按钮
	connect(ui->menu13, &QAction::triggered, this, [this]() {
		ui->viewStack->setCurrentWidget(ui->pageGene);
		});
	connect(ui->menu13_2, &QAction::triggered, this, [this]() {
		ui->viewStack->setCurrentWidget(ui->pageGene);
		});

	// 文件选择按钮
	connect(ui->expPath, &QPushButton::clicked, this, [this]() {
		QString filePath = QFileDialog::getOpenFileName(
			this,
			tr("choose csv file"),
			defaultDir,
			tr("*.csv")
		);
		if (!filePath.isEmpty()) {
			ui->expFile->setText(filePath);
		}
		});

	// 选择模式
	connect(ui->singleMod, &QRadioButton::toggled,
		this, [this](bool on) {
			ui->singleWg->setVisible(on);   // 选单模 显示 singleWg
			ui->conWg->setVisible(!on);     // 隐藏 conWg
		});

	ui->singleMod->setChecked(true);            // 默认选单模
	ui->singleWg->setVisible(true);
	ui->conWg->setVisible(false);

	// btHist
	connect(ui->btHist, &QPushButton::clicked, this, [this]() {
		string path = ui->expFile->text().toStdString();
		BCmatrix* data = nullptr;
		try {
			data = loadCSV(path);
		}
		catch (const exception& e) {
			QMessageBox::warning(
				this,
				tr("no data"),
				tr("no data provided, maybe no file or a blank file?")
			);
			return;
		}

		int id = ui->geneID->value();
		int expN = ui->expN->value();
		int baseN = ui->baseN->value();

		if (expN + baseN != data->getColumnCount()) {
			QMessageBox::warning(
				this,
				tr("group error"),
				tr("now your experiment group num + control group num != total group num")
			);
			return;
		}

		if (ui->chooseExp->isChecked() == 0 && ui->chooseBase->isChecked() == 0) {
			QMessageBox::warning(
				this,
				tr("group error"),
				tr("choose experiment group or control group")
			);
			return;
		}

		BCarray<int> group;
		for (int i = 1; i <= baseN; i++)
			group.push_back(0);
		for (int i = 1; i <= expN; i++)
			group.push_back(1);

		BCarray<double> rowData = data->getRow(id);
		BCarray<double> baseData = rowData.split(group).first;
		BCarray<double> expData = rowData.split(group).second;

		if (ui->chooseExp->isChecked())
			GenePlot::plotHistogramKDE(expData, 0.3);
		else
			GenePlot::plotHistogramKDE(baseData, 0.3);

		delete data;
		});
	// bnKDE
	connect(ui->bnKDE, &QPushButton::clicked, this, [this]() {
		string path = ui->expFile->text().toStdString();
		BCmatrix* data = nullptr;
		try {
			data = loadCSV(path);
		}
		catch (const exception& e) {
			QMessageBox::warning(
				this,
				tr("no data"),
				tr("no data provided, maybe no file or a blank file?")
			);
			return;
		}

		int id = ui->geneID->value();
		int expN = ui->expN->value();
		int baseN = ui->baseN->value();

		if (expN + baseN != data->getColumnCount()) {
			QMessageBox::warning(
				this,
				tr("group error"),
				tr("now your experiment group num != control group num")
			);
			return;
		}

		if (ui->chooseExp->isChecked() == 0 && ui->chooseBase->isChecked() == 0) {
			QMessageBox::warning(
				this,
				tr("group error"),
				tr("choose experiment group or control group")
			);
			return;
		}

		BCarray<int> group;
		for (int i = 1; i <= baseN; i++)
			group.push_back(0);
		for (int i = 1; i <= expN; i++)
			group.push_back(1);

		BCarray<double> rowData = data->getRow(id);
		BCarray<double> baseData = rowData.split(group).first;
		BCarray<double> expData = rowData.split(group).second;

		if (ui->chooseExp->isChecked())
			GenePlot::plotKDE(expData, 0.3);
		else
			GenePlot::plotKDE(baseData, 0.3);
		delete data;
		});
	// plot
	connect(ui->bnPolt, &QPushButton::clicked, this, [this]() {
		string path = ui->expFile->text().toStdString();
		BCmatrix* data = nullptr;
		try {
			data = loadCSV(path);
		}
		catch (const exception& e) {
			QMessageBox::warning(
				this,
				tr("no data"),
				tr("no data provided, maybe no file or a blank file?")
			);
			return;
		}

		int id = ui->geneID->value();
		int expN = ui->expN->value();
		int baseN = ui->baseN->value();

		if (expN + baseN != data->getColumnCount()) {
			QMessageBox::warning(
				this,
				tr("group error"),
				tr("now your experiment group num != control group num")
			);
			return;
		}

		BCarray<int> group;
		for (int i = 1; i <= baseN; i++)
			group.push_back(0);
		for (int i = 1; i <= expN; i++)
			group.push_back(1);

		BCarray<double> rowData = data->getRow(id);
		BCarray<double> baseData = rowData.split(group).first;
		BCarray<double> expData = rowData.split(group).second;

		GenePlot::plot_two_lines(baseData, expData);
		delete data;
		});
	// scatter
	connect(ui->bnScatter, &QPushButton::clicked, this, [this]() {
		string path = ui->expFile->text().toStdString();
		BCmatrix* data = nullptr;
		try {
			data = loadCSV(path);
		}
		catch (const exception& e) {
			QMessageBox::warning(
				this,
				tr("no data"),
				tr("no data provided, maybe no file or a blank file?")
			);
			return;
		}

		int id = ui->geneID->value();
		int expN = ui->expN->value();
		int baseN = ui->baseN->value();

		if (expN + baseN != data->getColumnCount()) {
			QMessageBox::warning(
				this,
				tr("group error"),
				tr("now your experiment group num != control group num")
			);
			return;
		}

		BCarray<int> group;
		for (int i = 1; i <= baseN; i++)
			group.push_back(0);
		for (int i = 1; i <= expN; i++)
			group.push_back(1);

		BCarray<double> rowData = data->getRow(id);
		BCarray<double> baseData = rowData.split(group).first;
		BCarray<double> expData = rowData.split(group).second;

		GenePlot::plot_two_xy(baseData, expData);
		delete data;
		});
	// box
	connect(ui->bnBox, &QPushButton::clicked, this, [this]() {
		string path = ui->expFile->text().toStdString();
		BCmatrix* data = nullptr;
		try {
			data = loadCSV(path);
		}
		catch (const exception& e) {
			QMessageBox::warning(
				this,
				tr("no data"),
				tr("no data provided, maybe no file or a blank file?")
			);
			return;
		}

		int id = ui->geneID->value();
		int expN = ui->expN->value();
		int baseN = ui->baseN->value();

		if (expN + baseN != data->getColumnCount()) {
			QMessageBox::warning(
				this,
				tr("group error"),
				tr("now your experiment group num != control group num")
			);
			return;
		}

		BCarray<int> group;
		for (int i = 1; i <= baseN; i++)
			group.push_back(0);
		for (int i = 1; i <= expN; i++)
			group.push_back(1);

		BCarray<double> rowData = data->getRow(id);
		BCarray<double> baseData = rowData.split(group).first;
		BCarray<double> expData = rowData.split(group).second;

		GenePlot::plot_two_boxplot(baseData, expData);
		delete data;
		});

}

void MainWindow::array_init() {
	// 进入按钮
	connect(ui->menu12, &QAction::triggered, this, [this]() {
		ui->viewStack->setCurrentWidget(ui->pageArray);
		});
	connect(ui->menu12_2, &QAction::triggered, this, [this]() {
		ui->viewStack->setCurrentWidget(ui->pageArray);
		});

	// 文件选择按钮
	connect(ui->bnArray, &QPushButton::clicked, this, [this]() {
		QString filePath = QFileDialog::getOpenFileName(
			this,
			tr("choose csv file"),
			defaultDir,
			tr("*.csv")
		);
		if (!filePath.isEmpty()) {
			ui->leArray->setText(filePath);
		}
		});


	/* TableWidget* tbl = new TableWidget(this);
	 tbl->loadCSV("C:\\Users\\wwl\\source\\repos\\cpp_work\\cpp_work\\data.csv");
	 ui->tableLable->hide();
	 ui->tableArea->layout()->addWidget(tbl);*/

	 // 读取按钮
	connect(ui->csvReadBt, &QPushButton::clicked, this, [this]() {

		if (this->tbl) {
			QMessageBox::warning(
				this,
				tr("clear out"),
				tr("please clear out the current table before loading another")
			);
			return;
		}

		QString path = ui->leArray->text();

		if (path.isEmpty()) {
			QMessageBox::warning(this,
				tr("warning"),
				tr("A csv file is needed"));
			return;
		}

		ui->tableLable->hide();
		this->tbl = new TableWidget(this);
		tbl->loadCSV(path);
		ui->tableArea->layout()->addWidget(tbl);
		});

	// 清楚按钮
	connect(ui->delCSVBt, &QPushButton::clicked, this, [this]() {

		if (!(this->tbl))  // 防止重复按
			return;

		if (this->tbl) {
			ui->tableArea->layout()->removeWidget(this->tbl);
			this->tbl->deleteLater();
			this->tbl = nullptr;
		}
		ui->tableLable->show();
		});

	// 几个跳转按钮
	connect(ui->gotoProBt, &QPushButton::clicked, this, [this]() {
		ui->viewStack->setCurrentWidget(ui->pagePro);
		ui->leProIn->setText(ui->leArray->text());
		});
	connect(ui->gotoPCABt, &QPushButton::clicked, this, [this]() {
		ui->viewStack->setCurrentWidget(ui->pagePCA);
		ui->leDimIn->setText(ui->leArray->text());
		});
	connect(ui->gotoFinfBt, &QPushButton::clicked, this, [this]() {
		ui->viewStack->setCurrentWidget(ui->pageFind);
		ui->leFindIn->setText(ui->leArray->text());
		});
	connect(ui->gotoKMBt, &QPushButton::clicked, this, [this]() {
		ui->viewStack->setCurrentWidget(ui->pageKM);
		ui->leCluIn->setText(ui->leArray->text());
		});
	connect(ui->gotoExpBt, &QPushButton::clicked, this, [this]() {
		ui->viewStack->setCurrentWidget(ui->pageGene);
		ui->expFile->setText(ui->leArray->text());
		});

	// heatmap
	connect(ui->bnHeatMap, &QPushButton::clicked, this, [this]() {
		string path = ui->leArray->text().toStdString();
		BCmatrix* data = nullptr;
		try {
			data = loadCSV(path);
		}
		catch (const exception& e) {
			QMessageBox::warning(
				this,
				tr("no data"),
				tr("no data provided, maybe no file or a blank file?")
			);
			return;
		}
		GenePlot::plot_heatmap(data->getPureValue());
		delete data;
		});
}

void MainWindow::find_init() {
	// 进入按钮
	connect(ui->menu14, &QAction::triggered, this, [this]() {
		ui->viewStack->setCurrentWidget(ui->pageFind);
		});
	connect(ui->menu14_2, &QAction::triggered, this, [this]() {
		ui->viewStack->setCurrentWidget(ui->pageFind);
		});

	// 文件选择按钮
	connect(ui->pbFindin, &QPushButton::clicked, this, [this]() {
		QString filePath = QFileDialog::getOpenFileName(
			this,
			tr("choose csv file"),
			defaultDir,
			tr("*.csv")
		);
		if (!filePath.isEmpty()) {
			ui->leFindIn->setText(filePath);

			// 默认输出位置
			QString filePath = ui->leFindIn->text();
			int pos = filePath.lastIndexOf('.');
			if (pos != -1)
				filePath.insert(pos, "_analyze");         // 结果 "/path/to/xxx_analyze.csv"
			ui->leFindOut->setText(filePath);
		}
		});

	// 目前DESeq2不支持火山图
	ui->volCb->setVisible(ui->ttest->isChecked());
	connect(ui->ttest,
		QOverload<bool>::of(&QRadioButton::toggled),
		this, [this](bool checked) {
			ui->volCb->setVisible(checked);
			if (!checked) {
				// ttest 取消时，把 volCb 也取消勾选并隐藏 volWid
				ui->volCb->setChecked(false);
				ui->volWid->setVisible(false);
			}
		});

	// 输出到文件一开始时隐藏的
	ui->find2fileWidget->setVisible(ui->find2file->isChecked());
	connect(ui->find2file,
		QOverload<bool>::of(&QRadioButton::toggled),
		ui->find2fileWidget,
		&QWidget::setVisible);

	// 火山图选项一开始是隐藏的
	ui->find2fileWidget->setVisible(ui->volCb->isChecked());
	ui->volWid->hide();
	connect(ui->volCb, &QCheckBox::stateChanged,
		this, [this](int state) {
			ui->volWid->setVisible(state == Qt::Checked);
		});

	// 算法的默认值
	ui->foldchange->setValue(1.0);
	ui->p_v->setValue(0.05);

	// 绑定运行按钮
	connect(ui->findRun, &QPushButton::clicked,
		this, &MainWindow::find_run);
}

void MainWindow::find_run() {
	// 加载数据
	string path = ui->leFindIn->text().toStdString();
	BCmatrix* data = nullptr;
	try {
		data = loadCSV(path);
	}
	catch (const exception& e) {
		QMessageBox::warning(
			this,
			tr("no data"),
			tr("no data provided, maybe no file or a blank file?")
		);
		return;
	}

	int cols = data->getColumnCount();
	vector<int> vec(cols);
	int half = cols / 2;
	for (int i = 0; i < cols; ++i)
		vec[i] = (i < half) ? 0 : 1;
	data->set_group(vec);
	BCmatrix res;

	// 算法
	if (ui->ttest->isChecked())
		res = data->t_test();
	else if (ui->deseq->isChecked())
		res = data->deseq2();
	else {
		QMessageBox::warning(
			this,
			tr("no algorithm"),
			tr("choose one algorithm")
		);
		return;
	}

	// 输出
	if (ui->find2file->isChecked()) {
		string savePath = ui->leFindOut->text().toStdString();
		res.to_csv(savePath);
		// 提示
		QString qmsg = QString::fromStdString("Successfully saved to " + savePath);
		QMessageBox::information(this, tr("Save Complete"), qmsg);
	}
	else if (ui->find2win->isChecked()) {
		string savePath = "C:\\Users\\wwl\\source\\repos\\BioChaInsight\\_protected_file\\_tmp.csv";
		res.to_csv(savePath);

		// 跳转到表格
		ui->viewStack->setCurrentWidget(ui->pageArray);
		ui->delCSVBt->click(); // 防止已经有表格了
		QString qPath = QString::fromStdString(savePath);
		ui->leArray->setText(qPath);
		ui->csvReadBt->click();
	}
	else {
		QMessageBox::warning(
			this,
			tr("out mode choose"),
			tr("choose file/windows to save your results")
		);
		return;
	}

	// volcano
	if (ui->volCb->isChecked())
	{
		BCarray<double> log2fc = res("log2_fc", 'c');
		BCarray<double> p = res("p_value", 'c');
		double fc = ui->foldchange->value();
		double p_v = ui->p_v->value();

		GenePlot::volcano(log2fc, p, {}, fc, p_v);
		matplotlibcpp::show();
	}

	delete data;
}

void MainWindow::pro_init() {
	// 进入按钮
	connect(ui->menu17, &QAction::triggered, this, [this]() {
		ui->viewStack->setCurrentWidget(ui->pagePro);
		});
	connect(ui->menu17_2, &QAction::triggered, this, [this]() {
		ui->viewStack->setCurrentWidget(ui->pagePro);
		});

	// 文件选择按钮
	connect(ui->btProIn, &QPushButton::clicked, this, [this]() {
		QString filePath = QFileDialog::getOpenFileName(
			this,
			tr("choose csv file"),
			defaultDir,
			tr("*.csv")
		);
		if (!filePath.isEmpty()) {
			ui->leProIn->setText(filePath);

			// 默认输出位置
			QString filePath = ui->leProIn->text();
			int pos = filePath.lastIndexOf('.');
			if (pos != -1)
				filePath.insert(pos, "_processed");         // 结果 "/path/to/xxx_processed.csv"
			ui->leProOut->setText(filePath);

			filePath = ui->leProIn->text();
			pos = filePath.lastIndexOf('.');
			if (pos != -1)
				filePath.insert(pos, "_stat");         // 结果 "/path/to/xxx_stat.csv"
			ui->leProOut_2->setText(filePath);
		}
		});
	connect(ui->btProOut_2, &QPushButton::clicked, this, [this]() {
		QString filePath = QFileDialog::getOpenFileName(
			this,
			tr("choose csv file"),
			defaultDir,
			tr("*.csv")
		);
		if (!filePath.isEmpty()) {
			ui->leProOut_2->setText(filePath);
		}
		});
	connect(ui->btProOut, &QPushButton::clicked, this, [this]() {
		QString filePath = QFileDialog::getOpenFileName(
			this,
			tr("choose csv file"),
			defaultDir,
			tr("*.csv")
		);
		if (!filePath.isEmpty()) {
			ui->leProOut->setText(filePath);
		}
		});

	// 初始隐藏
	ui->gpLine->setVisible(ui->cbLine->isChecked());
	connect(ui->cbLine, &QCheckBox::toggled,
		ui->gpLine, &QWidget::setVisible);
	ui->gpNorm->setVisible(ui->cbNorm->isChecked());
	connect(ui->cbNorm, &QCheckBox::toggled,
		ui->gpNorm, &QWidget::setVisible);
	ui->gpSt->setVisible(ui->cbSt->isChecked());
	connect(ui->cbSt, &QCheckBox::toggled,
		ui->gpSt, &QWidget::setVisible);

	ui->fileWid->setVisible(ui->pro2file->isChecked());
	connect(ui->pro2file, &QRadioButton::toggled,
		ui->fileWid, &QWidget::setVisible);

	// 绑定运行按钮
	connect(ui->proBt, &QPushButton::clicked,
		this, &MainWindow::pro_run);
	connect(ui->stBt, &QPushButton::clicked,
		this, &MainWindow::st_run);
}

void MainWindow::pro_run() {
	// 加载数据
	string path = ui->leProIn->text().toStdString();
	BCmatrix* data = nullptr;
	try {
		data = loadCSV(path);
	}
	catch (const exception& e) {
		QMessageBox::warning(
			this,
			tr("no data"),
			tr("no data provided, maybe no file or a blank file?")
		);
		return;
	}
	BCmatrix res;

	// 切片
	if (ui->cbLine->isChecked()) {
		int s_id = ui->s_id->value();
		int e_id = ui->e_id->value();

		// 末尾必须大于开头
		if (s_id >= e_id) {
			QMessageBox::warning(this, tr("warning"), "end_index must > start_index");
			return;
		}
		// 不能超出索引
		if (e_id >= data->getRowCount()) {
			QMessageBox::warning(this, tr("warning"), "end_index out of range");
			return;
		}
		res = data->sliceRows(s_id, e_id);
	}
	else {
		res = *data;
	}

	// norm
	if (ui->cbNorm->isChecked())
	{
		int kind = ui->cbNormA->currentIndex();
		string method = "";
		if (kind == 0)
			method = "L2";
		else if (kind == 1)
			method = "minmax";
		else if (kind == 2)
			method = "zscore";
		else if (kind == 3)
			method = "centralize";

		string by = "";
		if (ui->byR2->isChecked()) {
			by = "row";
		}
		else if (ui->byC2->isChecked()) {
			by = "column";
		}
		else if (ui->byAll->isChecked()) {
			by = "all";
		}
		else {
			QMessageBox::warning(this, tr("warning"), "choose by row or by column or by all");
			return;
		}
		res.normalize(method, by);
	}

	// 输出
	if (ui->pro2file->isChecked()) {
		string savePath = ui->leProOut->text().toStdString();
		res.to_csv(savePath);
		// 提示
		QString qmsg = QString::fromStdString("Successfully saved to " + savePath);
		QMessageBox::information(this, tr("Save Complete"), qmsg);
	}
	else if (ui->pro2win->isChecked()) {
		string savePath = "C:\\Users\\wwl\\source\\repos\\BioChaInsight\\_protected_file\\_tmp.csv";
		res.to_csv(savePath);

		// 跳转到表格
		ui->viewStack->setCurrentWidget(ui->pageArray);
		ui->delCSVBt->click(); // 防止已经有表格了
		QString qPath = QString::fromStdString(savePath);
		ui->leArray->setText(qPath);
		ui->csvReadBt->click();
	}
	else {
		QMessageBox::warning(
			this,
			tr("out mode choose"),
			tr("choose file/windows to save your results")
		);
		return;
	}
	delete data;
}

void MainWindow::st_run() {
	if (ui->cbSt->isChecked()) {
		// 加载数据
		string path = ui->leProIn->text().toStdString();
		BCmatrix* data = nullptr;
		try {
			data = loadCSV(path);
		}
		catch (const exception& e) {
			QMessageBox::warning(
				this,
				tr("no data"),
				tr("no data provided, maybe no file or a blank file?")
			);
			return;
		}
		BCmatrix res;

		// 执行算法
		if (ui->byR1->isChecked()) {
			res = data->describe("row");
		}
		else if (ui->byC1->isChecked()) {
			res = data->describe("column");
		}
		else {
			QMessageBox::warning(this, tr("warning"), "choose by row or by column");
			return;
		}

		// 保存
		string savePath = ui->leProOut_2->text().toStdString();
		res.to_csv(savePath);
		QString qmsg = QString::fromStdString("Successfully saved to " + savePath);
		QMessageBox::information(this, tr("Save Complete"), qmsg);
		delete data;
	}
}

void MainWindow::initAssistant() {
	assistant = new AssistantWidget();
	assistant->show();
	assistant->syncPositionWith(this);    // 初始对齐
}

void MainWindow::initReader() {
	mdReader = new HtmlViewerWidget();
	mdReader->setHtmlFolder("C:\\Users\\wwl\\source\\repos\\BioChaInsight\\doc");
	mdReader->setWindowTitle("html Reader");
	mdReader->show();
}

// readme栏目 "C:\\Users\\wwl\\source\\repos\\BioChaInsight\\doc\\main_ui.html"
void MainWindow::init_readme()
{
	// 创建临时容器面板
	QWidget* readmePanel = new QWidget(this);
	QVBoxLayout* layout = new QVBoxLayout(readmePanel);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	// 将面板加入到 viewStack 并显示
	ui->viewStack->addWidget(readmePanel);
	ui->viewStack->setCurrentWidget(readmePanel);
	connect(ui->actionreadme, &QAction::triggered, this, [this, readmePanel]() {
		ui->viewStack->setCurrentWidget(readmePanel);
		});

	// 使用 QWebEngineView 来渲染 HTML（支持彩色 Emoji）
	QWebEngineView* webview = new QWebEngineView(readmePanel);
	webview->settings()->setAttribute(
		QWebEngineSettings::LocalContentCanAccessFileUrls, true);

	webview->setZoomFactor(1.25);  // 设置缩放比例为 1.25

	QString htmlPath = "C:/Users/wwl/source/repos/BioChaInsight/doc/main_ui.html";
	webview->load(QUrl::fromLocalFile(htmlPath));

	layout->addWidget(webview);
	readmePanel->setLayout(layout);
}

void MainWindow::init_main()
{
	// 设置窗口图标
	this->setWindowIcon(QIcon("C:\\Users\\wwl\\source\\repos\\BioChaInsight\\Image\\0"));
	// 设置窗口标题
	this->setWindowTitle("BioChaInsight");
	ui->setupUi(this);
	this->resize(1200, 741);

	// 把所有的 QSpinBox 最大值都调大
	constexpr int NEW_MAX = 1000000;
	for (auto* sb : this->findChildren<QSpinBox*>()) {
		sb->setMaximum(NEW_MAX);
	}
}

void MainWindow::init_menu()
{
	connect(ui->actioncolor, &QAction::triggered, this, [=]() {
		QColor color = QColorDialog::getColor(Qt::white, this, "选择主题颜色");

		if (color.isValid()) {
			QString colorName = color.name();
			QString style = QString(R"(
            QMenuBar, QToolBar, QStatusBar {
                background-color: %1;
            }
            QMenuBar::item, QToolButton {
                background: transparent;
                color: white;
            }
            QMenu {
                background-color: %1;
                color: white;
            }
            QMenu::item:selected {
                background-color: #555555;
            }
        )").arg(colorName);

			ui->menubar->setStyleSheet(style);
			ui->toolBar->setStyleSheet(style);
			ui->statusbar->setStyleSheet(style);
		}
		});

	connect(ui->actionfont, &QAction::triggered, this, [=]() {
		bool ok;
		QFont font = QFontDialog::getFont(&ok, qApp->font(), this, "选择全局字体");
		if (ok) {
			qApp->setFont(font);  // 影响后续创建的控件
			applyFontToAllChildren(this, font);
		}
		});

	connect(ui->actionfontc, &QAction::triggered, this, [=]() {
		QColor color = QColorDialog::getColor(Qt::black, this, "选择字体颜色");
		if (color.isValid()) {
			applyFontColorToAllChildren(this, color);
		}
		});
}

void MainWindow::init_status()
{
	QToolButton* btn1 = new QToolButton(this);
	btn1->setDefaultAction(ui->actionversion);
	ui->statusbar->addPermanentWidget(btn1);  // 添加到右侧
	connect(ui->actionversion, &QAction::triggered, this, [=]() {
		QMessageBox::information(this, "versoin", "current versoin:v1.0.0");
		});

	QToolButton* btn2 = new QToolButton(this);
	btn2->setDefaultAction(ui->actiontutorial);
	ui->statusbar->addPermanentWidget(btn2);
	connect(ui->actiontutorial, &QAction::triggered, this, [=]() {
		QString url = "https://blog.csdn.net/wwl412095144/category_12969818.html?spm=1001.2014.3001.5482";

		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(
			this, "doc",
			"You're going to the website: csdn ?\n",
			QMessageBox::Yes | QMessageBox::No
		);

		if (reply == QMessageBox::Yes) {
			QDesktopServices::openUrl(QUrl(url));
		}
		});

	QToolButton* btn3 = new QToolButton(this);
	btn3->setDefaultAction(ui->actioncontact);
	ui->statusbar->addPermanentWidget(btn3);
	connect(ui->actioncontact, &QAction::triggered, this, [=]() {
		QString url = "https://github.com/wwl-ww/BioChaInsight";

		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(
			this, "doc",
			"You're going to the website: ?\n" + url,
			QMessageBox::Yes | QMessageBox::No
		);

		if (reply == QMessageBox::Yes) {
			QDesktopServices::openUrl(QUrl(url));
		}
		});
}

void MainWindow::init_tool()
{
	ui->toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon); // 图片在上,文本在下
	ui->toolBar->setIconSize(QSize(48, 48));  // 图标大小
	// 遍历所有 Action 并设置按钮固定大小
	for (QAction* action : ui->toolBar->actions()) {
		QToolButton* button = qobject_cast<QToolButton*>(ui->toolBar->widgetForAction(action));
		if (button) {
			button->setFixedSize(120, 80);  // 按钮尺寸
			button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);  // 图标在上,文字在下
		}
	}
	// 只允许左和上
	ui->toolBar->setAllowedAreas(Qt::LeftToolBarArea | Qt::TopToolBarArea);
	// 默认放左侧
	this->addToolBar(Qt::TopToolBarArea, ui->toolBar);

	ui->menu11_2->setIcon(QIcon("C:\\Users\\wwl\\source\\repos\\BioChaInsight\\Image\\1.png"));
	ui->menu12_2->setIcon(QIcon("C:\\Users\\wwl\\source\\repos\\BioChaInsight\\Image\\2.png"));
	ui->menu13_2->setIcon(QIcon("C:\\Users\\wwl\\source\\repos\\BioChaInsight\\Image\\3.png"));
	ui->menu14_2->setIcon(QIcon("C:\\Users\\wwl\\source\\repos\\BioChaInsight\\Image\\4.png"));
	ui->menu15_2->setIcon(QIcon("C:\\Users\\wwl\\source\\repos\\BioChaInsight\\Image\\5.png"));
	ui->menu16_2->setIcon(QIcon("C:\\Users\\wwl\\source\\repos\\BioChaInsight\\Image\\6.png"));
	ui->menu17_2->setIcon(QIcon("C:\\Users\\wwl\\source\\repos\\BioChaInsight\\Image\\7.png"));
}

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindowClass())
{
	init_python();

	// 主窗口
	init_main();

	// 菜单栏
	init_menu();

	// 状态栏
	init_status();

	// 工具栏
	init_tool();

	// 中心区域
	setCentralWidget(ui->viewStack);
	init_readme();

	// 栏目1
	alignment_init();

	// 栏目2
	pca_init();

	// 栏目3
	km_init();

	// 栏目4
	exp_init();

	// 栏目5
	array_init();

	// 栏目6
	find_init();

	// 栏目7
	pro_init();

	// connect ui->actionai 和 initAssistant
	connect(ui->actionai, &QAction::triggered, this, [this]() {
		if (!assistant) {
			initAssistant();
		}
		else {
			assistant->show();
			assistant->syncPositionWith(this);  // 窗口对齐
		}
		});
	// connect ui->actionreader 和 initReader
	connect(ui->actiondoc, &QAction::triggered, this, [this]() {
		if (!mdReader) {
			initReader();
		}
		else {
			mdReader->show();
		}
		});
}

MainWindow::~MainWindow()
{
	delete ui;
}
