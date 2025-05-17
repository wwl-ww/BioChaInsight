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
#include "AlignmentWindow.h"
#include "Alignment.h"
#include "FASTA.h"
#include "matplotlibcpp.h"
#include <qdebug.h>

void applyFontToAllChildren(QWidget *widget, const QFont &font) {
    widget->setFont(font);
    for (QObject *child : widget->children()) {
        QWidget *childWidget = qobject_cast<QWidget *>(child);
        if (childWidget) {
            applyFontToAllChildren(childWidget, font);
        }
    }
}

void applyFontColorToAllChildren(QWidget *widget, const QColor &color) {
    QString style = QString("color: %1;").arg(color.name());
    widget->setStyleSheet(style);

    for (QObject *child : widget->children()) {
        QWidget *childWidget = qobject_cast<QWidget *>(child);
        if (childWidget) {
            applyFontColorToAllChildren(childWidget, color);
        }
    }
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
    connect(ui->rbSeq1Manual, &QRadioButton::toggled, this, [this](bool checked){
        if (checked) ui->swSeq1Input->setCurrentIndex(2);  // 手动 2
    });
    connect(ui->rbSeq1File, &QRadioButton::toggled, this, [this](bool checked){
        if (checked) ui->swSeq1Input->setCurrentIndex(0);  // 文件 0
    });

    // Seq2 手动 / 文件 模式切换
    connect(ui->rbSeq2Manual, &QRadioButton::toggled, this, [this](bool checked){
        if (checked) ui->swSeq2Input->setCurrentIndex(2);  // 手动 2
    });
    connect(ui->rbSeq2File, &QRadioButton::toggled, this, [this](bool checked){
        if (checked) ui->swSeq2Input->setCurrentIndex(0);  // 文件 0
    });

    // 文件选择按钮
    connect(ui->pushButton_1, &QPushButton::clicked, this, [this]() {
        QString filePath = QFileDialog::getOpenFileName(
            this,
            tr("选择 FASTA 文件"),
            QString(),
            tr("*")
            );
        if (!filePath.isEmpty()) {
            ui->lineEdit_1->setText(filePath);
        }
    });

    connect(ui->pushButton_2, &QPushButton::clicked, this, [this]() {
        QString filePath = QFileDialog::getOpenFileName(
            this,
            tr("选择 FASTA 文件"),
            QString(),
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
    if(ui->swSeq1Input->currentIndex()==0){
        QString path1 = ui->lineEdit_1->text();
        if (path1.isEmpty()) {
            qDebug()<<"xixi";
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
    else{
        std::string s1 = ui->leSeq1Manual->text().toStdString();
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
        int idx2 = ui->spinBox_2->value();  // 注意替换为你实际的spinbox对象名
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
        std::string s2 = ui->leSeq2Manual->text().toStdString();
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
    else{
        ui->teResult->append("Error: 未知的对齐算法");
        return;
    }

    // 执行算法
    alg->align();

    // 显示逻辑
    {
        const std::string& s1 = alg->getAlignedSeq1();
        const std::string& s2 = alg->getAlignedSeq2();
        const std::vector<int>& st1 = alg->getSeq1State();
        const std::vector<int>& st2 = alg->getSeq2State();

        // 生成 HTML
        QString html;
        html += "<p><b>Alignment Result:</b></p>";
        // 这里在 pre 上统一设置等宽字体和更大的字号
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
        auto mat = alg->getMatrix();
        // TODO: 在这里把 mat 转化成直方图或热图展示
    }

    // 可选：轨迹可视化
    if (ui->chkVisualizeTrace->isChecked()) {
        auto path = alg->getAlignmentPath();
        // TODO: 在这里把 path 绘制到子窗口或 Canvas 上
    }

    delete alg;
}

// readme栏目 "D:\\Documents\\Desktop\\BioChaInsight\\doc\\main_ui.html"
void MainWindow::init_readme()
{
    // 创建临时容器
    QWidget *readmePanel = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(readmePanel);

    // 将容器添加到中心区域
    ui->viewStack->addWidget(readmePanel);
    ui->viewStack->setCurrentWidget(readmePanel);

    // 创建 QTextBrowser
    QTextBrowser *browser = new QTextBrowser(readmePanel);
    browser->setFont(QFont("等线", 9));
    browser->setOpenExternalLinks(true);

    // 读取 README
    QFile file("D:\\Documents\\Desktop\\BioChaInsight\\doc\\main_ui.html");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        in.setCodec("UTF-8");
        browser->setHtml(in.readAll());
        file.close();
    }

    // 创建右上角关闭按钮
    QPushButton *closeButton = new QPushButton("✖", readmePanel);
    closeButton->setFixedSize(30, 15);
    closeButton->setStyleSheet("border: none; font-size: 16px;");
    closeButton->setStyleSheet(R"(
            QPushButton {
                border: none;
                color: red;
                font-size: 16px;
            }
            QPushButton:hover {
                background-color: #ffdddd;
            }
        )");
    connect(closeButton, &QPushButton::clicked, [=]() {
        // 关闭后释放面板，并恢复主界面
        readmePanel->deleteLater();
        // ui->setupUi(this);  // 恢复原有UI
    });

    // 布局：关闭按钮 + 阅读器
    QHBoxLayout *headerLayout = new QHBoxLayout();
    headerLayout->addStretch();
    headerLayout->addWidget(closeButton);

    layout->addLayout(headerLayout);
    layout->addWidget(browser);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

}

void MainWindow::init_main()
{
    // 设置窗口图标
    this->setWindowIcon(QIcon("D:\\Documents\\Desktop\\BioChaInsight\\Image\\基因-copy.png"));
    // 设置窗口标题
    this->setWindowTitle("BioChaInsight");
    ui->setupUi(this);
    this->setFixedSize(1000, 650);
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

            // 递归应用到现有控件
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
    QToolButton *btn1 = new QToolButton(this);
    btn1->setDefaultAction(ui->actionversion);
    ui->statusbar->addPermanentWidget(btn1);  // 添加到右侧
    connect(ui->actionversion, &QAction::triggered, this, [=]() {
        QMessageBox::information(this, "版本信息", "当前软件版本：v1.0.0");
    });

    QToolButton *btn2 = new QToolButton(this);
    btn2->setDefaultAction(ui->actiontutorial);
    ui->statusbar->addPermanentWidget(btn2);
    connect(ui->actiontutorial, &QAction::triggered, this, [=]() {
        QString url = "https://教程网站";

        // 弹出确认对话框
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(
            this, "打开网站",
            "是否要打开以下网站？\n" + url,
            QMessageBox::Yes | QMessageBox::No
            );

        // 用户点击“是”
        if (reply == QMessageBox::Yes) {
            QDesktopServices::openUrl(QUrl(url));
        }
    });

    QToolButton *btn3 = new QToolButton(this);
    btn3->setDefaultAction(ui->actioncontact);
    ui->statusbar->addPermanentWidget(btn3);
    connect(ui->actioncontact, &QAction::triggered, this, [=]() {
        QString url = "https://github.com/wwl-ww/BioChaInsight";

        // 弹出确认对话框
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(
            this, "打开网站",
            "是否要打开以下网站？\n" + url,
            QMessageBox::Yes | QMessageBox::No
            );

        // 用户点击“是”
        if (reply == QMessageBox::Yes) {
            QDesktopServices::openUrl(QUrl(url));
        }
    });
}

void MainWindow::init_tool()
{
    ui->toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon); // 图片在上 文本在下
    ui->toolBar->setIconSize(QSize(48, 48));  // 图标大小
    // 遍历所有 Action 并设置按钮固定大小
    for (QAction *action : ui->toolBar->actions()) {
        QToolButton *button = qobject_cast<QToolButton *>(ui->toolBar->widgetForAction(action));
        if (button) {
            button->setFixedSize(120, 80);  // 按钮尺寸
            button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);  // 图标在上文字在下
        }
    }
    // 只允许左和上
    ui->toolBar->setAllowedAreas(Qt::LeftToolBarArea | Qt::TopToolBarArea);
    // 默认放左侧
    this->addToolBar(Qt::TopToolBarArea, ui->toolBar);

    ui->menu11_2->setIcon(QIcon("D:\\Documents\\Desktop\\BioChaInsight\\Image\\报告基因相关.png"));
    ui->menu12_2->setIcon(QIcon("D:\\Documents\\Desktop\\BioChaInsight\\Image\\数据矩阵.png"));
    ui->menu13_2->setIcon(QIcon("D:\\Documents\\Desktop\\BioChaInsight\\Image\\癌遗传基因.png"));
    ui->menu14_2->setIcon(QIcon("D:\\Documents\\Desktop\\BioChaInsight\\Image\\基因查询.png"));
    ui->menu15_2->setIcon(QIcon("D:\\Documents\\Desktop\\BioChaInsight\\Image\\24聚类主题.png"));
    ui->menu16_2->setIcon(QIcon("D:\\Documents\\Desktop\\BioChaInsight\\Image\\降维.png"));
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
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
}

MainWindow::~MainWindow()
{
    delete ui;
}
