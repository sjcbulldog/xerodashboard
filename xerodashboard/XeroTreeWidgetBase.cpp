#include "XeroTreeWidgetBase.h"


XeroTreeWidgetBase::XeroTreeWidgetBase(QWidget *parent)	: QTreeWidget(parent)
{
}

XeroTreeWidgetBase::~XeroTreeWidgetBase()
{
}

QTreeWidgetItem* XeroTreeWidgetBase::findTopLevelItem(const QString& name)
{
	for (int i = 0; i < topLevelItemCount(); i++)
	{
		auto item = topLevelItem(i);
		if (item->text(0) == name)
			return item;
	}

	return nullptr;
}

QTreeWidgetItem* XeroTreeWidgetBase::findChild(QTreeWidgetItem* parent, const QString& name)
{
	for (int i = 0; i < parent->childCount(); i++)
	{
		auto child = parent->child(i);
		if (child->text(0) == name)
			return child;
	}

	return nullptr;
}

QTreeWidgetItem* XeroTreeWidgetBase::createItem(QTreeWidgetItem* parent, QStringList& path)
{
	QTreeWidgetItem* ret = nullptr;

	if (path.size() == 0)
	{
		ret = parent;
	}
	else
	{
		QString here = path.at(0);
		path.removeAt(0);

		ret = findChild(parent, here);
		if (ret == nullptr)
		{
			ret = new QTreeWidgetItem();
			ret->setText(0, here);
			parent->addChild(ret);
		}

		//
		// Recursivelly continue to create the needed nodes
		//
		ret = createItem(ret, path);
	}
	return ret;
}

QTreeWidgetItem* XeroTreeWidgetBase::createItem(const QString& path)
{
	QStringList elements = path.split('/');
	if (elements.size() == 0)
		return nullptr;

	// The name generally starts with a '/' character which results in an empty
	// entry as the first thing in elements.  We remove it as it is not important to
	// the hierarchy of the network table entries we are displaying
	if (elements[0].length() == 0)
		elements.removeAt(0);

	//
	// The first elements of the path will be a top level item, see if it exists
	//
	QTreeWidgetItem* top = findTopLevelItem(elements[0]);
	if (top == nullptr)
	{
		top = new QTreeWidgetItem();
		top->setText(0, elements[0]);
		addTopLevelItem(top);
	}

	elements.removeAt(0);
	return createItem(top, elements);
}
