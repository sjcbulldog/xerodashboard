#include "NTFormattingUtils.h"


QString NTFormattingUtils::toString(const nt::NetworkTableEntry& entry, int length)
{
	QString ret;

	assert(length >= 3);

	auto value = entry.GetValue();
	if (value->IsValid())
	{
		if (value->IsBoolean())
		{
			ret = value->GetBoolean() ? "true" : "false";
		}
		else if (value->IsBooleanArray())
		{
			ret = "[";
			auto v = value->GetBooleanArray();
			for (int i = 0; i < v.size(); i++)
			{
				if (ret.length() > 1)
					ret += ",";
				ret += v[i] ? "true " : "false";
				if (ret.length() > length)
					break;
			}
			ret += "]";
		}
		else if (value->IsDouble())
		{
			ret = QString::number(value->GetDouble());
		}
		else if (value->IsDoubleArray())
		{
			ret = "[";
			auto v = value->GetDoubleArray();
			for (int i = 0; i < v.size(); i++)
			{
				if (ret.length() > 1)
					ret += ",";
				ret += QString::number(v[i]);
				if (ret.length() > length)
					break;
			}
			ret += "]";
		}
		else if (value->IsString())
		{
			ret = QString::fromStdString(value->GetString());
		}
		else if (value->IsStringArray())
		{
			ret = "[";
			auto v = value->GetStringArray();
			for (int i = 0; i < v.size(); i++)
			{
				if (ret.length() > 1)
					ret += ",";
				ret += QString::fromStdString(v[i]);
				if (ret.length() > length)
					break;
			}
			ret += "]";
		}
		else
		{
			assert(false);
		}

		if (ret.length() > length)
		{
			ret = ret.mid(0, length - 3) + "...";
		}
	}

	return ret;
}

