#include "submitreviewdialog.h"
#include "reviewboardconstants.h"

#include "ui_submitreviewdialog.h"

using namespace ReviewBoard;
using namespace ReviewBoard::Internal;

SubmitReviewDialog::SubmitReviewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SubmitReviewDialog)
{
    ui->setupUi(this);

	connect(ui->people, SIGNAL(textChanged(QString)), this, SLOT(fieldsUpdated()));
	connect(ui->groups, SIGNAL(textChanged(QString)), this, SLOT(fieldsUpdated()));
	connect(ui->summary, SIGNAL(textChanged(QString)), this, SLOT(fieldsUpdated()));
	connect(ui->description, SIGNAL(textChanged()), this, SLOT(fieldsUpdated()));

	fieldsUpdated();
}

SubmitReviewDialog::~SubmitReviewDialog()
{
    delete ui;
}

QString SubmitReviewDialog::summary() const
{
	return ui->summary->text().trimmed();
}

QString SubmitReviewDialog::description() const
{
	return ui->description->toPlainText().trimmed();
}

QString SubmitReviewDialog::bugsClosed() const
{
	return ui->bugsClosed->text().trimmed();
}

QString SubmitReviewDialog::reviewers() const
{
	return ui->people->text().trimmed();
}

QString SubmitReviewDialog::groups() const
{
	return ui->groups->text().trimmed();
}

bool SubmitReviewDialog::doPublish() const
{
	return (ui->publish->isChecked() && canPublish());
}

bool SubmitReviewDialog::doOpenReview() const
{
	return ui->display->isChecked();
}


bool SubmitReviewDialog::canPublish() const
{
	if ( reviewers().isEmpty() && groups().isEmpty()) return false;

	if (summary().trimmed().isEmpty()) return false;
	if (description().isEmpty()) return false;

	return true;
}

void SubmitReviewDialog::fieldsUpdated()
{
	ui->publish->setEnabled(canPublish());
}

QStringList SubmitReviewDialog::selectedFiles() const
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

void SubmitReviewDialog::SetFileList(const QStringList& files)
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
static const char* peopleKey = "People";
static const char* groupsKey = "Groups";


void SubmitReviewDialog::toSettings(QSettings* s) const
{
	s->beginGroup(QLatin1String(Constants::REVIEWBOARD_SETTINGSGROUP));
	s->setValue(QLatin1String(publishKey), ui->publish->isChecked());
	s->setValue(QLatin1String(displayKey), doOpenReview());
	s->setValue(QLatin1String(peopleKey), reviewers());
	s->setValue(QLatin1String(groupsKey), groups());
	s->endGroup();
}

void SubmitReviewDialog::fromSettings(QSettings* s)
{
	s->beginGroup(QLatin1String(Constants::REVIEWBOARD_SETTINGSGROUP));
	ui->publish->setChecked( s->value(QLatin1String(publishKey), false).toBool());
	ui->display->setChecked( s->value(QLatin1String(displayKey), true).toBool());
	ui->people->setText( s->value(QLatin1String(peopleKey), QString()).toString() );
	ui->groups->setText( s->value(QLatin1String(groupsKey), QString()).toString() );
	s->endGroup();

	fieldsUpdated();
}
