#pragma once

#ifndef TABLE_H
#define TABLE_H

#include <QWidget>
#include <QAbstractTableModel>
#include <QVector>
#include <QStringList>

class QTableView;
class CsvTableModel;

class CsvTableModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	explicit CsvTableModel(QObject* parent = nullptr);

	bool loadFromFile(const QString& path, QChar sep = ',');
	bool saveToFile(const QString& path, QChar sep = ',') const;

	int rowCount(const QModelIndex & = {}) const override;
	int columnCount(const QModelIndex & = {}) const override;
	QVariant data(const QModelIndex&, int role = Qt::DisplayRole) const override;
	QVariant headerData(int, Qt::Orientation, int role = Qt::DisplayRole) const override;
	Qt::ItemFlags flags(const QModelIndex&) const override;
	bool setData(const QModelIndex&, const QVariant&, int role = Qt::EditRole) override;

private:
	QVector<QStringList> m_rows;
};

class TableWidget : public QWidget
{
	Q_OBJECT
public:
	explicit TableWidget(QWidget* parent = nullptr);
	bool loadCSV(const QString& path);
	bool saveCSV(const QString& path);

private:
	void setupView();

	CsvTableModel* m_model;
	QTableView* m_view;
};


#endif