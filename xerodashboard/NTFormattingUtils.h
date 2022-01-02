#pragma once

#include <QtCore/QString>
#include <networktables/NetworkTableEntry.h>

class NTFormattingUtils
{
public:
	NTFormattingUtils() = delete;

	static QString toString(const nt::NetworkTableEntry& entry, int length = 256);
};

