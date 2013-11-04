#include "updatereviewdialog.h"
#include "reviewboardconstants.h"
#include "reviewinfo.h"

#include "ui_updatereviewdialog.h"

using namespace ReviewBoard;
using namespace ReviewBoard::Internal;


UpdateReviewDialog::UpdateReviewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpdateReviewDialog)
{
    ui->setupUi(this);

	connect(ui->comment, SIGNAL(textChanged()), this, SLOT(fieldsUpdated()));

	fieldsUpdated();
}


UpdateReviewDialog::~UpdateReviewDialog()
{
    delete ui;
}



QString UpdateReviewDialog::comment() const
{
	return ui->comment->toPlainText().trimmed();
}


bool UpdateReviewDialog::doPublish() const
{
	return (ui->publish->isChecked() && canPublish());
}

bool UpdateReviewDialog::doOpenReview() const
{
	return ui->display->isChecked();
}


bool UpdateReviewDialog::canPublish() const
{
	if (comment().trimmed().isEmpty()) return false;

	return true;
}

void UpdateReviewDialog::fieldsUpdated()
{
	ui->publish->setEnabled(canPublish());
}

QStringList UpdateReviewDialog::selectedFiles() const
{
	QStringList result;

	int rows = ui->fileList->count();
	for (int i = 0; i < rows; i++) {
		QListWidgetItem* item = ui->fileList->item(i);
		if (item == 0) continue;

		if (item->data(Qt::CheckStateRole).toBool()) {
			result += item->text();
		}
	}

	return result;
}

void UpdateReviewDialog::SetFileList(const QStringList& files)
{
	ui->fileList->clear();

	foreach (const QString& file, files) {
		QListWidgetItem* item = new QListWidgetItem(file, ui->fileList);
		item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		item->setCheckState(Qt::Checked);
	}
}

static const char* publishKey = "Publish";
static const char* displayKey = "Display";


void UpdateReviewDialog::toSettings(QSettings* s) const
{
	s->beginGroup(QLatin1String(Constants::REVIEWBOARD_SETTINGSGROUP));
	s->setValue(QLatin1String(publishKey), ui->publish->isChecked());
	s->setValue(QLatin1String(displayKey), doOpenReview());
	s->endGroup();
}

void UpdateReviewDialog::fromSettings(QSettings* s)
{
	s->beginGroup(QLatin1String(Constants::REVIEWBOARD_SETTINGSGROUP));
	ui->publish->setChecked( s->value(QLatin1String(publishKey), false).toBool());
	ui->display->setChecked( s->value(QLatin1String(displayKey), true).toBool());
	s->endGroup();

	fieldsUpdated();
}

void UpdateReviewDialog::SetReviewList(const QList<ReviewInfo> &reviews)
{
	ui->reviewlist->clear();

	foreach (const ReviewInfo& item, reviews) {
		if (item.valid()) {
			ui->reviewlist->addItem(item.displayString(), item.id());
		}
	}
}

int UpdateReviewDialog::selectedReviewId() const
{
	int index = ui->reviewlist->currentIndex();
	if (index < 0) return -1;

	return ui->reviewlist->itemData(index).toInt();
}
