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

