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

#ifndef REVIEWBOARDSERVER_H
#define REVIEWBOARDSERVER_H

#include "reviewinfo.h"

#include <QObject>
#include <QHash>
#include <QVariant>
#include <QScriptEngine>


#include <QtNetwork/QNetworkAccessManager>

namespace ReviewBoard {
namespace Internal {

class ReviewBoardServer : public QObject
{
    Q_OBJECT
public:

	ReviewBoardServer();

	bool			hasValidCookie() const;

	void			setUrl(const QString& data);

	void			login(const QString& username, const QString& password);
	void			setDebug(bool debugOut);

	void			listRepositories();
	QScriptValue	repositoryInfo(int rid);

	QList<ReviewInfo>	getReviews(const QString& user);

	QString			newReviewRequest(const QString& repositoryUrl);
	void			saveDraft(const QString& id);
	void			publish(const QString& id);
	void			setField(const QString& id, const QString& field, const QString& value);
	void			uploadDiff(const QString& id, const QString& path, const QString& diffContent);

	QString			url(const QString& id) const;

	void			displayMsg(const QString& text);

public slots:

	void			clearLogin();
	void			authentication(QNetworkReply* reply, QAuthenticator* authenticator);

private:

	QScriptValue	apiPost(const QString& path, const QHash<QString, QVariant>& values, const QHash<QString, QVariant>& files);
	QScriptValue	apiPut(const QString& path, const QHash<QString, QVariant>& values);
	QScriptValue	apiGet(const QString& path);
	QString			encodeData(const QHash<QString, QVariant>& values, const QHash<QString, QVariant>& files);



	QNetworkAccessManager qnam;
	QScriptEngine	engine;
	QString			m_Url;
	QString			m_User;
	QString			m_Password;
	bool			m_Debug;
	bool			m_Old;

};

} // namespace Internal
} // namespace ReviewBoard

#endif // REVIEWBOARDSERVER_H
