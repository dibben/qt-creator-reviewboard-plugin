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

#include "subversionclient.h"
#include "repository.h"

#include <coreplugin/coreconstants.h>
#include <coreplugin/modemanager.h>

#include <coreplugin/icore.h>
#include <coreplugin/vcsmanager.h>

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/project.h>
#include <utils/qtcassert.h>

#include <vcsbase/basevcseditorfactory.h>
#include <vcsbase/vcsbaseeditor.h>
#include <vcsbase/basevcssubmiteditorfactory.h>
#include <vcsbase/vcsbaseoutputwindow.h>
#include <utils/synchronousprocess.h>
#include <utils/parameteraction.h>

#include <QTextStream>

static const char *groupC = "Subversion";
static const char *commandKeyC = "Command";
static const char *userKeyC = "User";
static const char *passwordKeyC = "Password";
static const char *authenticationKeyC = "Authentication";

static const char *userNameOptionC = "--username";
static const char *passwordOptionC = "--password";
static const char *timeOutKeyC = "TimeOut";


enum { defaultTimeOutS = 30, defaultLogCount = 1000 };

using namespace ReviewBoard;
using namespace ReviewBoard::Internal;

static QString defaultCommand()
{
	QString rc;
	rc = QLatin1String("svn");
#if defined(Q_OS_WIN32)
	rc.append(QLatin1String(".exe"));
#endif
	return rc;
}

SubversionSettings::SubversionSettings()
{

	QSettings* settings = Core::ICore::instance()->settings();

	settings->beginGroup(QLatin1String(groupC));
	svnCommand = settings->value(QLatin1String(commandKeyC), defaultCommand()).toString();
	useAuthentication = settings->value(QLatin1String(authenticationKeyC), QVariant(false)).toBool();
	user = settings->value(QLatin1String(userKeyC), QString()).toString();
	password =  settings->value(QLatin1String(passwordKeyC), QString()).toString();
	timeOutS = settings->value(QLatin1String(timeOutKeyC), defaultTimeOutS).toInt();
	settings->endGroup();
}

QStringList SubversionSettings::addOptions(const QStringList &args) const
{
	if (!useAuthentication || user.isEmpty())
		return args;
	// SVN pre 1.5 does not accept "--userName" for "add", which is most likely
	// an oversight. As no password is needed for the option, generally omit it.
	if (!args.empty() && args.front() == QLatin1String("add"))
		return args;

	QStringList rc;
	rc.push_back(QLatin1String(userNameOptionC));
	rc.push_back(user);
	if (!password.isEmpty()) {
		rc.push_back(QLatin1String(passwordOptionC));
		rc.push_back(password);
	}
	rc.append(args);
	return rc;
}

QString SubversionSettings::formatArguments(const QStringList &args)
{
	QString rc;
	QTextStream str(&rc);
	const int size = args.size();
	// Skip authentication options
	for (int i = 0; i < size; i++) {
		const QString &arg = args.at(i);
		if (i)
			str << ' ';
		str << arg;
		if (arg == QLatin1String(userNameOptionC) || arg == QLatin1String(passwordOptionC)) {
			str << " ********";
			i++;
		}
	}
	return rc;

}


SubversionClient::SubversionClient(bool debugOut)
{
	m_Debug = debugOut;
}


Repository SubversionClient::repositoryInfo(const QString &workingDir, const QStringList &relativePaths)
{
	QStringList args(QLatin1String("info"));

	if (!relativePaths.isEmpty()) {
		args.append(relativePaths);
	}

    VcsBase::VcsBaseOutputWindow *outwin = VcsBase::VcsBaseOutputWindow::instance();
	outwin->setRepository(workingDir);
	SubversionResponse response = runSvn(workingDir, args, m_Settings.timeOutMS(), true);
	outwin->clearRepository();

    QStringList data = response.stdOut.split(QChar::fromLatin1('\n'));

    QRegExp rootre(QString::fromLatin1("Repository Root: (.+)$"));
	if (data.indexOf(rootre) == -1) {
        outwin->append(QString::fromLatin1("Did not find root"));
		//return error
	}
	QString root = rootre.cap(1);
	if (m_Debug) outwin->append(root);

    QRegExp urlre(QString::fromLatin1("URL: (.+)$"));
	if (data.indexOf(urlre) == -1) {
        outwin->append(QString::fromLatin1("Did not find url"));
		//return error
	}
	QString basePath = urlre.cap(1);
	if (basePath.length() > root.length()) {
		basePath = basePath.right(basePath.length() - root.length());
	} else {
        basePath = QString::fromLatin1("/");
	}
	if (m_Debug) outwin->append(basePath);

    QRegExp uuidre(QString::fromLatin1("Repository UUID: (.+)$"));
	if (data.indexOf(uuidre) == -1) {
        outwin->append(QString::fromLatin1("Did not find UUID"));
		//return error
	}
	QString uuid = uuidre.cap(1);
	if (m_Debug) outwin->append(uuid);

	//should return heap object here?
	return Repository(root, basePath, uuid);
}

QStringList SubversionClient::svnStatus(const QString &workingDir, const QStringList &relativePaths)
{
	QStringList args(QLatin1String("status"));

	if (!relativePaths.isEmpty()) {
		args.append(relativePaths);
	}

    VcsBase::VcsBaseOutputWindow *outwin = VcsBase::VcsBaseOutputWindow::instance();
	outwin->setRepository(workingDir);
	SubversionResponse response = runSvn(workingDir, args, m_Settings.timeOutMS(), true);
	outwin->clearRepository();

	return ProcessSvnOutput(response.stdOut);
}

QStringList SubversionClient::ProcessSvnOutput(const QString& status) const
{
	QStringList added;
	QStringList modified;

    QStringList lines = status.split(QString::fromLatin1("\n"), QString::SkipEmptyParts);

	foreach (QString line, lines) {
		line = line.simplified();
		QString file = line.right(line.length() - 2);
        if (line.startsWith(QString::fromLatin1("M")) || line.startsWith(QString::fromLatin1("C"))) {
			modified += file;
        } else if (line.startsWith(QString::fromLatin1("A"))) {
			added += file;
		}
	}

	modified += added;

	return modified;
}

// Format log entry for command
static inline QString msgExecutionLogEntry(const QString &workingDir, const QString &executable, const QStringList &arguments)
{
	const QString argsS = SubversionSettings::formatArguments(arguments);
	if (workingDir.isEmpty())
		return SubversionClient::tr("Executing: %1 %2\n").arg(executable, argsS);
	return SubversionClient::tr("Executing in %1: %2 %3\n").
			arg(QDir::toNativeSeparators(workingDir), executable, argsS);
}


QString SubversionClient::svnDiff(const QString &workingDir, const QStringList& files)
{
	QStringList args(QLatin1String("diff"));

	args += files;

    VcsBase::VcsBaseOutputWindow *outwin = VcsBase::VcsBaseOutputWindow::instance();
	outwin->setRepository(workingDir);
	SubversionResponse response = runSvn(workingDir, args, m_Settings.timeOutMS(), true);
	outwin->clearRepository();

	return response.stdOut;
}


SubversionResponse SubversionClient::runSvn(const QString &workingDir,
											const QStringList &arguments,
											int timeOut,
											bool showStdOutInOutputWindow,
											QTextCodec *outputCodec)
{
	const QString executable = m_Settings.command();
	SubversionResponse response;
	if (executable.isEmpty()) {
		response.error = true;
		response.message =tr("No subversion executable specified!");
		return response;
	}
	const QStringList allArgs = m_Settings.addOptions(arguments);

    VcsBase::VcsBaseOutputWindow *outputWindow = VcsBase::VcsBaseOutputWindow::instance();
	// Hide passwords, etc in the log window
	//: Executing: <executable> <arguments>
	const QString outputText = msgExecutionLogEntry(workingDir, executable, allArgs);
	if (m_Debug) outputWindow->appendCommand(outputText);

	//if (Subversion::Constants::debug)
//		qDebug() << "runSvn" << timeOut << outputText;

	// Run, connect stderr to the output window
	Utils::SynchronousProcess process;
	if (!workingDir.isEmpty())
		process.setWorkingDirectory(workingDir);
	QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
	env.insert(QLatin1String("LANG"), QString(QLatin1Char('C')));
	process.setProcessEnvironment(env);
	process.setTimeout(timeOut);
	process.setCodec(outputCodec);

	process.setStdErrBufferedSignalsEnabled(true);
	connect(&process, SIGNAL(stdErrBuffered(QString,bool)), outputWindow, SLOT(append(QString)));

	// connect stdout to the output window if desired
	if (m_Debug && showStdOutInOutputWindow) {
		process.setStdOutBufferedSignalsEnabled(true);
		connect(&process, SIGNAL(stdOutBuffered(QString,bool)), outputWindow, SLOT(append(QString)));
	}

	const Utils::SynchronousProcessResponse sp_resp = process.run(executable, allArgs);
	response.error = true;
	response.stdErr = sp_resp.stdErr;
	response.stdOut = sp_resp.stdOut;
	switch (sp_resp.result) {
	case Utils::SynchronousProcessResponse::Finished:
		response.error = false;
		break;
	case Utils::SynchronousProcessResponse::FinishedError:
		response.message = tr("The process terminated with exit code %1.").arg(sp_resp.exitCode);
		break;
	case Utils::SynchronousProcessResponse::TerminatedAbnormally:
		response.message = tr("The process terminated abnormally.");
		break;
	case Utils::SynchronousProcessResponse::StartFailed:
		response.message = tr("Could not start subversion '%1'. Please check your settings in the preferences.").arg(executable);
		break;
	case Utils::SynchronousProcessResponse::Hang:
		response.message = tr("Subversion did not respond within timeout limit (%1 ms).").arg(timeOut);
		break;
	}
	if (response.error)
		outputWindow->appendError(response.message);

	return response;
}
