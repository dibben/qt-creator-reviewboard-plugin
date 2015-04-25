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


#include "reviewboardsettingspage.h"

#include "reviewboardconstants.h"
#include "ui_reviewboardsettingspage.h"

#include <coreplugin/icore.h>
#include <coreplugin/editormanager/editormanager.h>
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

QWidget* ReviewBoardSettingsPage::widget()
{
	return createPage(0);
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

