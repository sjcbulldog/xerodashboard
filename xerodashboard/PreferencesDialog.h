#pragma once

#include <QDialog>
#include "ui_PreferencesDialog.h"

class PreferencesDialog : public QDialog
{
	Q_OBJECT

public:
	PreferencesDialog(QWidget *parent = Q_NULLPTR);
	~PreferencesDialog();

	QString ipaddr() const {
		return ui.ipaddr_->text();
	}

	void setIPAddress(const QString& ipaddr) {
		ui.ipaddr_->setText(ipaddr);
	}

	QString plotKey() const {
		return ui.plotkey_->text();
	}

	void setPlotKey(const QString& key) {
		ui.plotkey_->setText(key);
	}

	int tileMargin() const {
		return ui.margin_->value();
	}

	void setTileMargin(int margin) {
		ui.margin_->setValue(margin);
	}

private:
	Ui_PreferencesDialog ui;
};
