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

#ifndef REVIEWBOARDSETTINGSPAGE_H
#define REVIEWBOARDSETTINGSPAGE_H

#include <coreplugin/dialogs/ioptionspage.h>
#include <QPointer>
#include <QSharedPointer>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE


namespace ReviewBoard {
namespace Internal {

namespace Ui {
	class ReviewBoardSettingsPage;
}

struct ReviewBoardSettings
{
	ReviewBoardSettings();

	void toSettings(QSettings *) const;
	void fromSettings(QSettings *);

	bool equals(const ReviewBoardSettings &rhs) const;

	QString serverUrl;
	QString user;
	QString password;
	bool	debug;
};

inline bool operator==(const ReviewBoardSettings &s1, const ReviewBoardSettings &s2) { return s1.equals(s2); }
inline bool operator!=(const ReviewBoardSettings &s1, const ReviewBoardSettings &s2) { return !s1.equals(s2); }

class ReviewBoardSettingsWidget : public QWidget {
	Q_OBJECT
public:
	explicit ReviewBoardSettingsWidget(QWidget *parent = 0);
	virtual ~ReviewBoardSettingsWidget();

	ReviewBoardSettings settings() const;
	void setSettings(const ReviewBoardSettings &s);


private:

	Ui::ReviewBoardSettingsPage *m_ui;
};


class ReviewBoardSettingsPage : public Core::IOptionsPage
{
	Q_OBJECT
	Q_DISABLE_COPY(ReviewBoardSettingsPage)
public:

	ReviewBoardSettingsPage(QSharedPointer<ReviewBoardSettings> &settings,
								 QObject *parent = 0);
	virtual ~ReviewBoardSettingsPage();

	virtual	QWidget*	widget();
	virtual QWidget*	createPage(QWidget *parent);
	virtual void apply();
	virtual void finish() { }



signals:

	void	settingsChanged();

private:
	const QSharedPointer<ReviewBoardSettings> m_settings;
	QPointer<ReviewBoardSettingsWidget> m_widget;
};

} // namespace Internal
} // namespace ReviewBoard

#endif // REVIEWBOARDSETTINGSPAGE_H


