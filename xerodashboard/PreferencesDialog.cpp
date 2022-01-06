#include "PreferencesDialog.h"

PreferencesDialog::PreferencesDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	connect(ui.button_box_, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(ui.button_box_, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

PreferencesDialog::~PreferencesDialog()
{
}
