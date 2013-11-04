/**************************************************************************
**
** Copyright (c) 2013 David Dibben (http://www.codegardening.com/)
**
** Permission is hereby granted, free of charge, to any person obtaining a copy of
** this software and associated documentation files (the "Software"), to deal in
** the Software without restriction, including without limitation the rights to
** use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
** the Software, and to permit persons to whom the Software is furnished to do so,
** subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all
** copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
** FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
** COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
** IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
** CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
****************************************************************************/

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
