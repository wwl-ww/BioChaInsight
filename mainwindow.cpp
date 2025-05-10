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
#include <QFile>
#include <QTextStream>
#include <QDockWidget>

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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // 主窗口
    {
        // 设置窗口图标
        this->setWindowIcon(QIcon("D:\\Documents\\Desktop\\BioChaInsight\\Image\\基因-copy.png"));
        // 设置窗口标题
        this->setWindowTitle("BioChaInsight");
        ui->setupUi(this);
        this->setFixedSize(1000, 650);
    }

    // 菜单栏
    {
        ui->menubar->setStyleSheet(R"(
            QMenuBar {
                background-color: #ffffff;
                color: #333333;
                font: 18px;
            }
            QMenuBar::item {
                background-color: transparent;
                padding: 5px 15px;
                margin: 2px;
            }
            QMenuBar::item:selected {
                background-color: #e6f7ff;
                color: #1890ff;
            }
            QMenuBar::item:pressed {
                background-color: #bae7ff;
            }
        )");
        ui->menubar->setStyleSheet(ui->menubar->styleSheet() + R"(
            QMenu {
                background-color: #ffffff;
                border: 1px solid #cccccc;
            }
            QMenu::item {
                padding: 5px 20px;
            }
            QMenu::item:selected {
                background-color: #e6f7ff;
                color: #1890ff;
            }
        )");

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

    // 状态栏
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

        ui->statusbar->setStyleSheet(R"(
            QToolButton {
                border: none;
                background: transparent;
            }
            QMenuBar::item:selected {
                background-color: #e6f7ff;
                color: #1890ff;
            }
            QMenuBar::item:pressed {
                background-color: #bae7ff;
            }
        )");
    }

    // 工具栏
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
        ui->toolBar->setStyleSheet("QToolBar { background: transparent; }");
        ui->toolBar->setStyleSheet(R"(
            QToolButton {
                background-color: #ffffff;
                border: 1px solid #cccccc;
                border-radius: 8px;
                padding: 4px;
                font: 14px;
            }
            QToolButton:hover {
                background-color: #e6f7ff;
                border: 1px solid #1890ff;
            }
        )");
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

    // 中心栏目 "D:\\Documents\\Desktop\\BioChaInsight\\doc\\main_ui.html"
    {
        // 创建临时容器
        QWidget *readmePanel = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(readmePanel);

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

        // 替换 centralWidget
        this->setCentralWidget(readmePanel);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
