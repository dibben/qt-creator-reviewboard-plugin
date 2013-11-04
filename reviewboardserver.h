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

	void		Display(const QString& text);

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
