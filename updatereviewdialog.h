#ifndef UPDATEREVIEWDIALOG_H
#define UPDATEREVIEWDIALOG_H

#include <QDialog>
#include <QSettings>

namespace ReviewBoard {
namespace Internal {

namespace Ui {
    class UpdateReviewDialog;
}
class ReviewInfo;

class UpdateReviewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateReviewDialog(QWidget *parent = 0);
    ~UpdateReviewDialog();

	void	SetFileList(const QStringList& files);
	void	SetReviewList(const QList<ReviewInfo>& reviews);

	QStringList	selectedFiles() const;

	QString comment() const;

	int		selectedReviewId() const;
	bool	doPublish() const;
	bool	doOpenReview() const;
	bool	canPublish() const;

	void	toSettings(QSettings* s) const;
	void	fromSettings(QSettings* s);

public slots:

	void	fieldsUpdated();

private:

    Ui::UpdateReviewDialog *ui;
};


} // namespace Internal
} // namespace ReviewBoard
#endif // UPDATEREVIEWDIALOG_H
