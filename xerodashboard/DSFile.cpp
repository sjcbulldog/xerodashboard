#include "DSFile.h"

int DSFile::get1(QFile& file)
{
	QByteArray d = file.read(1);
	return d[0];
}

int DSFile::get2(QFile& file)
{
	QByteArray d = file.read(2);
	return d.toInt();
}

int DSFile::get4(QFile& file)
{
	QByteArray d = file.read(4);
	return d.toInt();
}
