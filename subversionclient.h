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
