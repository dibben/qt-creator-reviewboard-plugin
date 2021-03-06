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

#include "reviewboardplugin.h"
#include "reviewboardconstants.h"
#include "reviewboardsettingspage.h"
#include "submitreviewdialog.h"
#include "updatereviewdialog.h"
#include "reviewboardserver.h"
#include "subversionclient.h"
#include "repository.h"

#include <coreplugin/coreconstants.h>
#include <coreplugin/modemanager.h>

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/icore.h>
#include <coreplugin/vcsmanager.h>
#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/editormanager/editormanager.h>

#include <cppeditor/cppeditorconstants.h>

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/project.h>
#include <projectexplorer/projecttree.h>
#include <utils/qtcassert.h>

#include <vcsbase/basevcseditorfactory.h>
#include <vcsbase/vcsbaseeditor.h>
#include <vcsbase/basevcssubmiteditorfactory.h>
//#include <vcsbase/vcsbaseoutputwindow.h>
#include <utils/synchronousprocess.h>
#include <utils/parameteraction.h>

#include <QtPlugin>
#include <QAction>
#include <QKeySequence>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QDesktopServices>
#include <QUrl>
#include <QMenu>
#include <QProgressDialog>
#include <QMessageBox>

using namespace ReviewBoard;
using namespace ReviewBoard::Internal;



ReviewBoardPlugin::ReviewBoardPlugin() :
		m_Settings(new ReviewBoardSettings),
		m_Server(new ReviewBoardServer)
{
}

ReviewBoardPlugin::~ReviewBoardPlugin()
{
}

bool ReviewBoardPlugin::initialize(const QStringList &arguments, QString *error_message)
{
	Q_UNUSED(arguments);
	Q_UNUSED(error_message);

	Core::ActionManager *am = Core::ActionManager::instance();

	ReviewBoardSettingsPage* settingsPage = new ReviewBoardSettingsPage(m_Settings);
	connect(settingsPage, SIGNAL(settingsChanged()), this, SLOT(updateSettings()));
	addAutoReleasedObject(settingsPage);

	// Menus
	Core::ActionContainer *mtools = am->actionContainer(Core::Constants::M_TOOLS);
	Core::ActionContainer *mreviewtools = am->createMenu(Constants::M_TOOLS_REVIEWBOARD);
	QMenu *menu = mreviewtools->menu();
	menu->setTitle(tr("Code &Review"));
	menu->setEnabled(true);
    menu->setIcon(QIcon(QString::fromLatin1(":/reviewboard/images/logo16.ico")));
	mtools->addMenu(mreviewtools);

	Core::Context context(CppEditor::Constants::CPPEDITOR_ID);

	QAction *submitReview = new QAction(tr("Submit Review"),  this);
	Core::Command *submitCmd = am->registerAction(submitReview, Constants::CREATE_REVIEW, context);

	connect(submitReview, SIGNAL(triggered()), this, SLOT(projectStatus()));
	mreviewtools->addAction(submitCmd);

	QAction* updateReview = new QAction(tr("Update Review"),  this);
	Core::Command* updateCmd = am->registerAction(updateReview, Constants::UPDATE_REVIEW, context);
	connect(updateReview, SIGNAL(triggered()), this, SLOT(updateReview()));
	mreviewtools->addAction(updateCmd);

	return true;
}

void ReviewBoardPlugin::extensionsInitialized()
{
	m_Settings->fromSettings(Core::ICore::instance()->settings());

	updateSettings();
}

void ReviewBoardPlugin::shutdown()
{
}


void ReviewBoardPlugin::updateSettings()
{
	m_Server->clearLogin();
	m_Server->setUrl( m_Settings->serverUrl );
}


void ReviewBoardPlugin::projectStatus()
{
	QString currentProjectTopLevel = projectTopLevel();
	QStringList relativeCurrentProject;
	svnStatus(currentProjectTopLevel, relativeCurrentProject);

}

QString ReviewBoardPlugin::projectTopLevel()
{
	//current project or file



	//Core::IVersionControl *projectControl = 0;
	const ProjectExplorer::Project *currentProject = ProjectExplorer::ProjectTree::currentProject();

	QString currentProjectTopLevel;

	if (currentProject) {

		QString currentProjectPath = currentProject->projectDirectory().toString();
		//QString  currentProjectName = currentProject->displayName();

		currentProjectTopLevel = currentProjectPath;

		//Core::IVersionControl *vc = vcsManager->findVersionControlForDirectory(currentProjectPath,
		//															&currentProjectTopLevel);


		//if (vc) {
			//if (data->m_state.currentProjectTopLevel != data->m_state.currentProjectPath)
			//	rc.append(QDir(data->m_state.currentProjectTopLevel).relativeFilePath(data->m_state.currentProjectPath));
			//ToDo - add relative paths
		//}
	}


	return currentProjectTopLevel;

}

void ReviewBoardPlugin::svnStatus(const QString &workingDir, const QStringList& relativePaths)
{
	QProgressDialog progress(tr("Getting changed files..."), QString(), 0, 0);
	progress.setWindowModality(Qt::WindowModal);
	progress.setMinimumDuration(0);
	progress.setLabelText(tr("Logging in to ReviewBoard..."));
	progress.show();

	m_Server->setDebug(m_Settings->debug);
	m_Server->login(m_Settings->user, m_Settings->password);

	if (!m_Server->hasValidCookie()) {
		progress.hide();
		QMessageBox::warning(0, tr("ReviewBoard Server Error"),
							 tr("Can not log into server. Please check user ID and password."));
		// can't log in
		return;
	}

	progress.setLabelText(tr("Getting changed files..."));

	SubversionClient svn(m_Settings->debug);
	Repository rep = svn.repositoryInfo(workingDir, relativePaths);

	QStringList files = svn.svnStatus(workingDir, relativePaths);

	progress.hide();

	submitReview(workingDir, rep, files);
}



void ReviewBoardPlugin::submitReview(const QString &workingDir, const Repository& rep, const QStringList& files)
{
	SubmitReviewDialog dialog;

	dialog.SetFileList(files);

	dialog.fromSettings( Core::ICore::instance()->settings() );

	if (dialog.exec() == QDialog::Accepted) {
		dialog.toSettings( Core::ICore::instance()->settings() );


		QString request = m_Server->newReviewRequest(rep.url());

		QString summary = dialog.summary();
		QString description = dialog.description();
		QString reviewers = dialog.reviewers();
		QString groups = dialog.groups();

        if (!summary.isEmpty()) m_Server->setField(request, QString::fromLatin1("summary"), summary);
        if (!description.isEmpty()) m_Server->setField(request, QString::fromLatin1("description"), description);
        if (!reviewers.isEmpty()) m_Server->setField(request, QString::fromLatin1("target_people"), reviewers);
        if (!groups.isEmpty()) m_Server->setField(request, QString::fromLatin1("target_groups"), groups);

		m_Server->saveDraft(request);

		QStringList selectedFiles = dialog.selectedFiles();

		if (selectedFiles.count() > 0) {
			QString basepath = rep.basePath();
			SubversionClient svn(m_Settings->debug);
			QString diff = svn.svnDiff(workingDir, selectedFiles);

			m_Server->uploadDiff(request, basepath, diff);
		}

		if (dialog.doPublish()) {
			m_Server->publish(request);
		}

		if (dialog.doOpenReview()) {
			openUrl( m_Server->url(request) );
		}

	}

}

void ReviewBoardPlugin::openUrl(const QString &data)
{
	QDesktopServices::openUrl(QUrl(data));
}

void ReviewBoardPlugin::updateReview()
{
	QProgressDialog progress(tr("Getting changed files..."), QString(), 0, 0);
	progress.setWindowModality(Qt::WindowModal);
	progress.setMinimumDuration(0);
	progress.setLabelText(tr("Logging in to ReviewBoard..."));
	progress.show();

	m_Server->setDebug(m_Settings->debug);
	m_Server->login(m_Settings->user, m_Settings->password);

	if (!m_Server->hasValidCookie()) {
		progress.hide();
		QMessageBox::warning(0, tr("ReviewBoard Server Error"),
							 tr("Can not log into server. Please check user ID and password."));
		return;
	}

	QString workingDir = projectTopLevel();
	QStringList relativePaths;

	SubversionClient svn(m_Settings->debug);
	Repository rep = svn.repositoryInfo(workingDir, relativePaths);

	QStringList files = svn.svnStatus(workingDir, relativePaths);

	QList<ReviewInfo> reviews = m_Server->getReviews(m_Settings->user);


	progress.hide();

	UpdateReviewDialog dialog;

	dialog.SetFileList(files);
	dialog.SetReviewList(reviews);
	dialog.fromSettings( Core::ICore::instance()->settings() );

	if (dialog.exec() == QDialog::Accepted) {
		dialog.toSettings( Core::ICore::instance()->settings() );


		//QString request = m_Server->newReviewRequest(rep.url());
		QString request = QString::number(dialog.selectedReviewId());



		QStringList selectedFiles = dialog.selectedFiles();

		if (selectedFiles.count() > 0) {
			QString basepath = rep.basePath();
			SubversionClient svn(m_Settings->debug);
			QString diff = svn.svnDiff(workingDir, selectedFiles);

			m_Server->uploadDiff(request, basepath, diff);
		}

		QString comment = dialog.comment();

        /*if (!comment.isEmpty()) */m_Server->setField(request, QString::fromLatin1("changedescription"), QString::fromLatin1("change description"));

		//m_Server->saveDraft(request);

		if (dialog.doPublish()) {
			m_Server->publish(request);
		}

		if (dialog.doOpenReview()) {
			openUrl( m_Server->url(request) );
		}

	}
}



//Q_EXPORT_PLUGIN(ReviewBoardPlugin);





