/**************************************************************************
**
** This file is part of ReviewBoard plugin for Qt Creator
**
** Copyright (c) 2010 David Dibben
**
** This plugin is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as
** published by the Free Software Foundation, either version 2.1
** of the License, or (at your option) any later version.
**
** This plugin is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with REVIEWBOARD.  If not, see <http://www.gnu.org/licenses/>.
**/

#ifndef SUBMITREVIEWDIALOG_H
#define SUBMITREVIEWDIALOG_H

#include <QDialog>
#include <QSettings>

namespace ReviewBoard {
namespace Internal {

namespace Ui {
    class SubmitReviewDialog;
}

class SubmitReviewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SubmitReviewDialog(QWidget *parent = 0);
    ~SubmitReviewDialog();

	void	SetFileList(const QStringList& files);

	QStringList	selectedFiles() const;

	QString summary() const;
	QString description() const;
	QString bugsClosed() const;
	QString reviewers() const;
	QString groups() const;

	bool	doPublish() const;
	bool	doOpenReview() const;
	bool	canPublish() const;

	void	toSettings(QSettings* s) const;
	void	fromSettings(QSettings* s);

public slots:

	void	fieldsUpdated();

private:

	Ui::SubmitReviewDialog* ui;

};


} // namespace Internal
} // namespace ReviewBoard
#endif // SUBMITREVIEWDIALOG_H
