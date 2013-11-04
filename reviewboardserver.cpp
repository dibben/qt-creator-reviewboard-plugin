#include "reviewboardserver.h"
#include "repository.h"

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QApplication>
#include <QDateTime>
#include <QAuthenticator>


#include <QScriptEngine>
#include <QScriptValueIterator>

#include <vcsbase/vcsbaseoutputwindow.h>

using namespace ReviewBoard;
using namespace ReviewBoard::Internal;

ReviewBoardServer::ReviewBoardServer()
{
	m_Debug = false;
	m_Old = false;

	connect(&qnam, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
	        this, SLOT(authentication(QNetworkReply*, QAuthenticator*)));
}

void ReviewBoardServer::setUrl(const QString& data)
{
	m_Url = data.trimmed();
    if (!m_Url.endsWith(QString::fromLatin1("/"))) {
        m_Url += QString::fromLatin1("/");
	}
}

void ReviewBoardServer::setDebug(bool debugOut)
{
	m_Debug = debugOut;
}

void ReviewBoardServer::login(const QString& username, const QString& password)
{
    m_User = username;
    m_Password = password;

    if (hasValidCookie()) return;

    if (m_Old) {

        QHash<QString, QVariant> files;
        QHash<QString, QVariant> values;
        values[QString::fromLatin1("username")] = m_User;
        values[QString::fromLatin1("password")] = password;

        QScriptValue resp = apiPost(QString::fromLatin1("api/json/accounts/login/"), values, files);

        if (resp.property(QString::fromLatin1("stat")).toString() == QString::fromLatin1("ok")) {
            Display(QString::fromLatin1(" Login OK - Yeah!"));
        } else {
            QString error = resp.property(QString::fromLatin1("err")).property(QString::fromLatin1("msg")).toString();
            Display(error);
        }
    }
}

void ReviewBoardServer::clearLogin()
{
	qnam.setCookieJar(new QNetworkCookieJar);
}


void ReviewBoardServer::authentication(QNetworkReply *reply, QAuthenticator *authenticator)
{
    Q_UNUSED(reply);
	authenticator->setUser(m_User);
	authenticator->setPassword(m_Password);
}


bool ReviewBoardServer::hasValidCookie() const
{
    if (!m_Old) return true;

	QNetworkCookieJar* jar = qnam.cookieJar();

	QList<QNetworkCookie> cookieList = jar->cookiesForUrl( QUrl(m_Url));

	if (cookieList.count() == 0) return false;

	foreach (const QNetworkCookie& cookie, cookieList) {
		if (cookie.name() == "rbsessionid") {
			return (cookie.expirationDate() > QDateTime::currentDateTime());
		}
	}

	return false;
}

//{"stat": "ok", "repositories": [{"path": "file:///d:/svn", "tool": "Subversion", "id": 1, "name": "Main"}]}
void ReviewBoardServer::listRepositories()
{
    QScriptValue rsp;
    if (m_Old) {
        rsp = apiGet(QString::fromLatin1("api/json/repositories/"));
    } else {
        rsp = apiGet(QString::fromLatin1("api/repositories/"));
    }

    QScriptValue repositories = rsp.property(QString::fromLatin1("repositories"));

	QScriptValueIterator it(repositories);
	while (it.hasNext()) {
		it.next();

        QString name = it.value().property(QString::fromLatin1("name")).toString();
        QString tool = it.value().property(QString::fromLatin1("tool")).toString();
        int rid = it.value().property(QString::fromLatin1("path")).toInteger();

		Repository rep(name, rid, tool);
		rep.getInfo(this);
	}

}

QScriptValue ReviewBoardServer::repositoryInfo(int rid)
{
    Display( QString::fromLatin1("Checking info for rid %1").arg(rid));
    QScriptValue rsp;
    if (m_Old) {
        rsp = apiGet( QString::fromLatin1("api/json/repositories/%1/info/").arg(rid));
    } else {
        rsp = apiGet( QString::fromLatin1("api/repositories/%1/").arg(rid));
    }
	//TODO error checking
    return rsp.property(QString::fromLatin1("info"));
}

QString ReviewBoardServer::newReviewRequest(const QString& repositoryUrl)
{
	QHash<QString, QVariant> files;
	QHash<QString, QVariant> values;
	//TODO need better URL matching - remove lower

	QScriptValue rsp;

	if (m_Old) {
        values[QString::fromLatin1("repository_path")] = repositoryUrl.toLower(); //"file:///d:/svn";
        rsp = apiPost(QString::fromLatin1("api/json/reviewrequests/new/"), values, files);
	} else {
        values[QString::fromLatin1("repository")] = repositoryUrl.toLower(); //"file:///d:/svn";
        rsp = apiPost(QString::fromLatin1("api/review-requests/"), values, files);
	}

	//TODO error checking
    return rsp.property(QString::fromLatin1("review_request")).property(QString::fromLatin1("id")).toString();
}


void ReviewBoardServer::saveDraft(const QString& id)
{
    Display(QString::fromLatin1("saving draft, id = %1").arg(id));

	QHash<QString, QVariant> values;
	QHash<QString, QVariant> files;
	if (m_Old) {
        apiPost( QString::fromLatin1("api/json/reviewrequests/%1/draft/save/").arg(id), values, files);
	} else {
        apiPut( QString::fromLatin1("api/review-requests/%1/draft/").arg(id), values);
	}

}

void ReviewBoardServer::uploadDiff(const QString &id, const QString& path, const QString &diffContent)
{
    Display(QString::fromLatin1("uploading diff, id = %1").arg(id));

	QHash<QString, QVariant> values;
	QHash<QString, QVariant> files;

    values[QString::fromLatin1("basedir")] = path;
    files[QString::fromLatin1("diff")] = diffContent;

	QScriptValue rsp;
	if (m_Old) {
        rsp = apiPost( QString::fromLatin1("api/json/reviewrequests/%1/diff/new/").arg(id), values, files);
	} else {
        rsp = apiPost( QString::fromLatin1("api/review-requests/%1/diffs/").arg(id), values, files);
	}


}


void  ReviewBoardServer::publish(const QString& id)
{
    Display(QString::fromLatin1("publishing, id = %1").arg(id));

    QHash<QString, QVariant> values;
    QHash<QString, QVariant> files;
    if (m_Old) {
        apiPost( QString::fromLatin1("api/json/reviewrequests/%1/publish/").arg(id), values, files);
    } else {
        values[QString::fromLatin1("public")] = 1;
        apiPut( QString::fromLatin1("api/review-requests/%1/draft/?public=1").arg(id), values);
    }
}

void ReviewBoardServer::setField(const QString& id, const QString& field, const QString& value)
{
    Display(QString::fromLatin1("setting field, id = ") + id);

	QHash<QString, QVariant> files;
	QHash<QString, QVariant> values;
	values[field] = value;
	if (m_Old) {
        apiPost( QString::fromLatin1("api/json/reviewrequests/%1/draft/set/").arg(id), values, files);
	} else {
        apiPut( QString::fromLatin1("api/review-requests/%1/draft/").arg(id), values);
	}
}


QString ReviewBoardServer::url(const QString &id) const
{
    return m_Url + QString::fromLatin1("r/%1/").arg(id);
}

QList<ReviewInfo> ReviewBoardServer::getReviews(const QString& user)
{
	QList<ReviewInfo> result;

    QScriptValue resp;
    if (m_Old) {
        resp = apiGet( QString::fromLatin1("api/json/reviewrequests/from/user/%1/?status=pending").arg(user));
    } else {
        resp = apiGet( QString::fromLatin1("api/review-requests/?from-user=%1&status=pending").arg(user));
    }

    QScriptValue reviews = resp.property(QString::fromLatin1("review_requests"));

	QScriptValueIterator it(reviews);
	while (it.hasNext()) {
		 it.next();
		 ReviewInfo info(it.value());

		 result.append(info);
	 }

	return result;
}

QScriptValue ReviewBoardServer::apiGet(const QString& path)
{
	QNetworkRequest request;
	request.setUrl(QUrl(m_Url + path));
	request.setRawHeader("User-Agent", "QtCreator 1.0");

	QNetworkReply *reply = qnam.get(request);

	while (!reply->isFinished()) {
		qApp->processEvents();
	}

	//error check
    QString result = QString::fromLatin1(reply->readAll());
	Display(result);

	reply->deleteLater();

    QScriptValue sc = engine.evaluate(QString::fromLatin1("rsp = ") + result);

	if (engine.hasUncaughtException()) {
        Display(QString::fromLatin1("problem parsing string"));

		Display(engine.uncaughtException().toString());
	}

	return sc;
}

QScriptValue ReviewBoardServer::apiPost(const QString& path, const QHash<QString, QVariant>& values, const QHash<QString, QVariant>& files)
{
	//TODO need better boundary string
    QString BOUNDARY = QString::fromLatin1("12345678");
    QString content_type = QString::fromLatin1("multipart/form-data; boundary=%1").arg(BOUNDARY);

	QNetworkRequest request;
	request.setUrl(QUrl(m_Url + path));
    request.setRawHeader("User-Agent", "QtCreator 1.0");
	request.setHeader(QNetworkRequest::ContentTypeHeader, content_type);

	QNetworkReply* reply = qnam.post(request, encodeData(values, files).toUtf8());


    Display(QString::fromLatin1("Posting: ") + path);

	while (!reply->isFinished()) {
		qApp->processEvents();
	}

    Display(QString::fromLatin1("Network error = %1").arg(reply->error()));

	//TODO: error check here

    QString result = QString::fromLatin1(reply->readAll());
	Display(result);

	reply->deleteLater();

    QScriptValue sc = engine.evaluate(QString::fromLatin1("rsp = ") + result);

	if (engine.hasUncaughtException()) {
        Display(QString::fromLatin1("problem parsing string"));

		Display(engine.uncaughtException().toString());
	} else {

		QScriptValueIterator it(sc);
		 while (it.hasNext()) {
			 it.next();
             Display(QString::fromLatin1("data: ") + it.name() + QString::fromLatin1(" : ") + it.value().toString());
		 }
	}

	return sc;
}

QScriptValue ReviewBoardServer::apiPut(const QString &path, const QHash<QString, QVariant> &values)
{
	//TODO need better boundary string
    QString BOUNDARY = QString::fromLatin1("12345678");
    QString content_type = QString::fromLatin1("multipart/form-data; boundary=%1").arg(BOUNDARY);

	QNetworkRequest request;
	request.setUrl(QUrl(m_Url + path));
	request.setRawHeader("User-Agent", "QtCreator 1.0");
	request.setHeader(QNetworkRequest::ContentTypeHeader, content_type);

	const QHash<QString, QVariant> files;
	QNetworkReply* reply = qnam.put(request, encodeData(values, files).toUtf8());


    Display(QString::fromLatin1("Posting: ") + path);

	while (!reply->isFinished()) {
		qApp->processEvents();
	}

    Display(QString::fromLatin1("Network error = %1").arg(reply->error()));

	//TODO: error check here

    QString result = QString::fromLatin1(reply->readAll());
	Display(result);

	reply->deleteLater();

    QScriptValue sc = engine.evaluate(QString::fromLatin1("rsp = ") + result);

	if (engine.hasUncaughtException()) {
        Display(QString::fromLatin1("problem parsing string"));

		Display(engine.uncaughtException().toString());
	} else {

		QScriptValueIterator it(sc);
		 while (it.hasNext()) {
			 it.next();
             Display(QString::fromLatin1("data: ") + it.name() + QString::fromLatin1(" : ") + it.value().toString());
		 }
	}

	return sc;
}

QString ReviewBoardServer::encodeData(const QHash<QString, QVariant>& values, const QHash<QString, QVariant>& files)
{
	//TODO need better boundary string
    QString BOUNDARY = QString::fromLatin1("12345678");
	QString content;


	QHashIterator<QString, QVariant> it(values);
	while (it.hasNext()) {
		it.next();

        content += QString::fromLatin1("--") + BOUNDARY + QString::fromLatin1("\r\n");
        content += QString::fromLatin1("Content-Disposition: form-data; name=\"%1\"\r\n").arg(it.key());
        content += QString::fromLatin1("\r\n");
        content +=  it.value().toString() + QString::fromLatin1("\r\n");
	}

	QHashIterator<QString, QVariant> fit(files);
	while (fit.hasNext()) {
		fit.next();

        content += QString::fromLatin1("--") + BOUNDARY + QString::fromLatin1("\r\n");
        content += QString::fromLatin1("Content-Disposition: form-data; name=\"path\"; filename=\"%1\"\r\n").arg(fit.key());

        content += QString::fromLatin1("\r\n");
        content +=  fit.value().toString() + QString::fromLatin1("\r\n");
	}

    content += QString::fromLatin1("--") + BOUNDARY + QString::fromLatin1("--\r\n");
    content += QString::fromLatin1("\r\n");

	return content;
}


void ReviewBoardServer::Display(const QString& text)
{
	if (!m_Debug) return;

	VcsBase::VcsBaseOutputWindow *outputWindow = VcsBase::VcsBaseOutputWindow::instance();
	outputWindow->append(text);
}







