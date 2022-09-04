#pragma once

#include <QtCore/QObject>
#include <QtCore/QFile>

class DSFile : public QObject
{
	Q_OBJECT

public:
protected:
	int get1(QFile& file);
	int get2(QFile& file);
	int get4(QFile& file);

private:
};

