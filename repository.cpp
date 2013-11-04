#include "repository.h"
#include "reviewboardserver.h"


using namespace ReviewBoard;
using namespace ReviewBoard::Internal;

Repository::Repository(const QString& name, int rid, const QString& tool) :
			 m_rid(rid), m_name(name), m_tool(tool)
{
}

Repository::Repository(const QString& path, const QString& basePath, const QString& uuid)
{
	m_name = path;
	m_url = basePath; //this is confusing
	m_rootUrl = path;
	m_uuid = uuid;
	m_rid = -1;
}

/*
	{"url": "file:///D:/svn",
	 "root_url": "file:///D:/svn",
	 "uuid": "718c6328-c3a0-b44e-8826-3b4ba7b52296"},
*/

void Repository::getInfo(ReviewBoardServer *server)
{
	QScriptValue info = server->repositoryInfo(m_rid);

    m_url = info.property(QString::fromLatin1("url")) .toString();
    m_rootUrl= info.property(QString::fromLatin1("root_url")) .toString();
    m_uuid = info.property(QString::fromLatin1("uuid")) .toString();

}

QString Repository::basePath() const
{
	return m_url;
}

QString Repository::url() const
{
	return m_rootUrl;
}

