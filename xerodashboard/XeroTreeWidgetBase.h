#pragma once

#include <QtCore/QString>
#include <QtWidgets/QTreeWidget>

class XeroTreeWidgetBase : public QTreeWidget
{
	Q_OBJECT

public:
	XeroTreeWidgetBase(QWidget *parent = Q_NULLPTR);
	~XeroTreeWidgetBase();

protected:
	QTreeWidgetItem* createItem(const QString& path);
	QTreeWidgetItem* createItem(QTreeWidgetItem* top, QStringList& path);

	QTreeWidgetItem* findTopLevelItem(const QString& name);
	QTreeWidgetItem* findChild(QTreeWidgetItem* parent, const QString& name);
};
