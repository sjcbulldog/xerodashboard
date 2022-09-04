#pragma once

#include "DSFile.h"
#include <QtCore/QString>

class DSLogFile : public DSFile
{
public:
	class DSLogFileEntry
	{
	public:
		DSLogFileEntry() {
		}
		virtual ~DSLogFileEntry() = default;

		void setTripTime(int tt) {
			trip_time_ = tt;
		}

		void setPacketLoss(int pl) {
			packet_loss_ = pl;
		}

		void setBatteryLevel(int bl) {
			battery_level_ = bl;
		}

		void setCPUPercent(int cpup) {
			cpu_percent_ = cpup;
		}

		void setTrace(int t) {
			trace_ = t;
		}

		void setCANUtilization(int cu) {
			can_utilization_ = cu;
		}

		void setWifiDb(int db) {
			wifi_db_ = db;
		}

		void setWiFiMb(int mb) {
			wifi_mb_ = mb;
		}

		bool isDisabled() const {
			return (trace_ & DisabledMask) != 0;
		}

		bool isAuto() const {
			return (trace_ & AutoMask) != 0;
		}

		bool isTeleop() const {
			return (trace_ & TeleopMask) != 0;
		}

		bool isDSDisabled() const {
			return (trace_ & DSDisabledMask) != 0;
		}

		bool isDSTeleop() const {
			return (trace_ & DSTeleopMask) != 0;
		}

		bool isWatchdog() const {
			return (trace_ & WatchdogMask) != 0;
		}

		bool isBrownOut() const {
			return (trace_ & BrownOutMask) != 0;
		}

	private:
		static const uint8_t DisabledMask = (0x01 << 0);
		static const uint8_t AutoMask = (0x01 << 1);
		static const uint8_t TeleopMask = (0x01 << 2);
		static const uint8_t DSDisabledMask = (0x01 << 3);
		static const uint8_t UnusedMask = (0x01 << 4);
		static const uint8_t DSTeleopMask = (0x01 << 5);
		static const uint8_t WatchdogMask = (0x01 << 6);
		static const uint8_t BrownOutMask = (0x01 << 7);

	private:
		int trip_time_;
		int packet_loss_;
		int battery_level_;
		int cpu_percent_;
		int trace_;
		int can_utilization_;
		int wifi_db_;
		int wifi_mb_;
	};

public:
	DSLogFile();

	virtual ~DSLogFile() = default;

	bool load(const QString& filename);

private:
	QString filename_;
	int version_;
	int timestamp_;
	int fractime_;
};
