/**************************************************************************
**
** This file is part of ReviewBoard plugin for Qt Creator
**
** Copyright (c) 2010 David Dibben.
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
** along with ReviewBoard.  If not, see <http://www.gnu.org/licenses/>.
**/

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

	virtual QWidget *createPage(QWidget *parent);
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


