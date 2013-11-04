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
