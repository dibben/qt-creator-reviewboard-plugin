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

#include "reviewboardsettingspage.h"

#include "reviewboardconstants.h"
#include "ui_reviewboardsettingspage.h"

#include <coreplugin/icore.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/mimedatabase.h>
#include <cppeditor/cppeditorconstants.h>

#include <extensionsystem/pluginmanager.h>

#include <utils/pathchooser.h>

#include <QSettings>
#include <QDebug>
#include <QFile>
#include <QCoreApplication>
#include <QDate>
#include <QLocale>

#include <QFileDialog>
#include <QMessageBox>


using namespace ReviewBoard;
using namespace ReviewBoard::Internal;
using namespace Utils;

static const char *serverUrlC = "ServerUrl";
static const char *userKeyC = "User";
static const char *passwordKeyC = "Password";
static const char *debugKeyC = "Debug";


ReviewBoardSettings::ReviewBoardSettings()
{
	debug = false;
}

void ReviewBoardSettings::toSettings(QSettings *s) const
{
	s->beginGroup(QLatin1String(Constants::REVIEWBOARD_SETTINGSGROUP));
	s->setValue(QLatin1String(serverUrlC), serverUrl);
	s->setValue(QLatin1String(userKeyC), user);
	s->setValue(QLatin1String(passwordKeyC), password);
	s->setValue(QLatin1String(debugKeyC), debug);
	s->endGroup();
}

void ReviewBoardSettings::fromSettings(QSettings *s)
{
	s->beginGroup(QLatin1String(Constants::REVIEWBOARD_SETTINGSGROUP));
	serverUrl = s->value(QLatin1String(serverUrlC), QString()).toString();
	user = s->value(QLatin1String(userKeyC), QString()).toString();
	password =  s->value(QLatin1String(passwordKeyC), QString()).toString();
	debug =  s->value(QLatin1String(debugKeyC), false).toBool();
	s->endGroup();

}


bool ReviewBoardSettings::equals(const ReviewBoardSettings &rhs) const
{
	return serverUrl      == rhs.serverUrl
		&& user           == rhs.user
		&& password       == rhs.password
		&& debug          == rhs.debug;
}



// ------------------ ReviewBoardSettingsWidget

ReviewBoardSettingsWidget::ReviewBoardSettingsWidget(QWidget *parent) :
	QWidget(parent),
	m_ui(new Ui::ReviewBoardSettingsPage)
{
	m_ui->setupUi(this);
}

ReviewBoardSettingsWidget::~ReviewBoardSettingsWidget()
{
	delete m_ui;
}

ReviewBoardSettings ReviewBoardSettingsWidget::settings() const
{
	ReviewBoardSettings rc;

	rc.serverUrl = m_ui->urlLineEdit->text();
	rc.user =  m_ui->usernameLineEdit->text();
	rc.password = m_ui->passwordLineEdit->text();
	rc.debug = m_ui->debugCheckBox->isChecked();
	return rc;
}


void ReviewBoardSettingsWidget::setSettings(const ReviewBoardSettings &s)
{
	m_ui->urlLineEdit->setText(s.serverUrl);
	m_ui->usernameLineEdit->setText(s.user);
	m_ui->passwordLineEdit->setText(s.password);
	m_ui->debugCheckBox->setChecked(s.debug);
}


// --------------- ReviewBoardSettingsPage

ReviewBoardSettingsPage::ReviewBoardSettingsPage(QSharedPointer<ReviewBoardSettings> &settings,
										 QObject *parent) :
	Core::IOptionsPage(parent),
	m_settings(settings)
{
    setId(Constants::REVIEWBOARD_SETTINGS_ID);
    setDisplayName( QCoreApplication::translate("ReviewBoard", Constants::REVIEWBOARD_SETTINGS_ID));
    setCategory( Constants::REVIEWBOARD_SETTINGS_CATEGORY);
    setDisplayCategory( QCoreApplication::translate("ReviewBoard", Constants::REVIEWBOARD_SETTINGS_CATEGORY));
    setCategoryIcon(QLatin1String(":/reviewboard/images/review_board_logo.png"));
}

ReviewBoardSettingsPage::~ReviewBoardSettingsPage()
{
}


QWidget *ReviewBoardSettingsPage::createPage(QWidget *parent)
{
	m_widget = new ReviewBoardSettingsWidget(parent);
	m_widget->setSettings(*m_settings);
	return m_widget;
}

void ReviewBoardSettingsPage::apply()
{
	if (m_widget) {
		const ReviewBoardSettings newSettings = m_widget->settings();
		if (newSettings != *m_settings) {
			*m_settings = newSettings;
			m_settings->toSettings(Core::ICore::instance()->settings());
			emit settingsChanged();
		}
	}
}
