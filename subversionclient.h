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

#ifndef SUBVERSIONCLIENT_H
#define SUBVERSIONCLIENT_H

#include <QObject>
#include <QString>

namespace ReviewBoard {
namespace Internal {

class Repository;

struct SubversionResponse
{
	SubversionResponse() : error(false) {}
	bool error;
	QString stdOut;
	QString stdErr;
	QString message;
};

class SubversionSettings
{
public:
	SubversionSettings();

inline	int			timeOutMS() const     { return timeOutS * 1000;  }

		QStringList addOptions(const QStringList &args) const;
static	QString		formatArguments(const QStringList &args);

QString		command() const {return svnCommand;}

private:

	QString svnCommand;
	bool useAuthentication;
	QString user;
	QString password;
	int timeOutS;
};


class SubversionClient : public QObject
{
    Q_OBJECT
public:
	SubversionClient(bool debugOut = false);

	Repository	repositoryInfo(const QString &workingDir, const QStringList &relativePaths);
	QStringList	svnStatus(const QString &workingDir, const QStringList &relativePaths);
	QString		svnDiff(const QString &workingDir, const QStringList& files);

private:

	QStringList			ProcessSvnOutput(const QString& status) const;

	SubversionResponse	runSvn(const QString &workingDir,
												const QStringList &arguments,
												int timeOut,
												bool showStdOutInOutputWindow,
												QTextCodec *outputCodec = 0);

	SubversionSettings	m_Settings;
	bool				m_Debug;
};

} // namespace Internal
} // namespace ReviewBoard

#endif // SUBVERSIONCLIENT_H
