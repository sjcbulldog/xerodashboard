#pragma once

#include "DSFile.h"
#include <QtCore/QString>

class DSEventFile : public DSFile
{
public:
	DSEventFile() {
	}

	virtual ~DSEventFile() = default;

	bool load(const QString& filename) {
		return true;
	}
};

