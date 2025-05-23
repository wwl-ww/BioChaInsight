#include "stdafx.h"
#include "table.h"

#include <QTableView>
#include <QHeaderView>
#include <QFile>
#include <QTextStream>
#include <QVBoxLayout>

CsvTableModel::CsvTableModel(QObject* parent)
	: QAbstractTableModel(parent) {
}

bool CsvTableModel::loadFromFile(const QString& path, QChar sep)
{
	QFile f(path);
	if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
		return false;

	QTextStream ts(&f);
	ts.setCodec("UTF-8");

	beginResetModel();
	m_rows.clear();
	while (!ts.atEnd())
		m_rows << ts.readLine().split(sep, Qt::KeepEmptyParts);
	endResetModel();
	return true;
}

bool CsvTableModel::saveToFile(const QString& path, QChar sep) const
{
	QFile f(path);
	if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
		return false;

	QTextStream ts(&f);
	ts.setCodec("UTF-8");

	for (const auto& row : m_rows)
		ts << row.join(sep) << '\n';
	return true;
}

int CsvTableModel::rowCount(const QModelIndex&) const
{
	return m_rows.size();
}

int CsvTableModel::columnCount(const QModelIndex&) const
{
	return m_rows.isEmpty() ? 0 : m_rows[0].size();
}

QVariant CsvTableModel::data(const QModelIndex& idx, int role) const
{
	if (!idx.isValid() || (role != Qt::DisplayRole && role != Qt::EditRole))
		return {};
	return m_rows[idx.row()][idx.column()];
}

QVariant CsvTableModel::headerData(int sec, Qt::Orientation ori, int role) const
{
	if (role != Qt::DisplayRole) return {};
	return (ori == Qt::Horizontal)
		? QStringLiteral("Col %1").arg(sec + 1)
		: QString::number(sec + 1);
}

Qt::ItemFlags CsvTableModel::flags(const QModelIndex& idx) const
{
	if (!idx.isValid()) return Qt::NoItemFlags;
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

bool CsvTableModel::setData(const QModelIndex& idx, const QVariant& v, int role)
{
	if (role != Qt::EditRole || !idx.isValid()) return false;
	m_rows[idx.row()][idx.column()] = v.toString();
	emit dataChanged(idx, idx, { Qt::DisplayRole, Qt::EditRole });
	return true;
}

TableWidget::TableWidget(QWidget* parent)
	: QWidget(parent),
	m_model(new CsvTableModel(this)),
	m_view(new QTableView(this))
{
	setupView();

	auto* lay = new QVBoxLayout(this);
	lay->setContentsMargins(0, 0, 0, 0);
	lay->addWidget(m_view);
}

void TableWidget::setupView()
{
	m_view->setModel(m_model);
	m_view->setAlternatingRowColors(true);
	m_view->setSortingEnabled(true);
	m_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_view->setSelectionBehavior(QAbstractItemView::SelectItems);
	m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	m_view->horizontalHeader()->setStretchLastSection(true);
	m_view->verticalHeader()->setVisible(false);
}

bool TableWidget::loadCSV(const QString& path) { return m_model->loadFromFile(path); }
bool TableWidget::saveCSV(const QString& path) { return m_model->saveToFile(path); }
