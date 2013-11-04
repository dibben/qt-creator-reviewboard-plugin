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

#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <QString>

namespace ReviewBoard {
namespace Internal {

class ReviewBoardServer;



class Repository
{
public:
	Repository(const QString& name, int rid, const QString& tool);
	Repository(const QString& path, const QString& basePath, const QString& uuid);

	QString		url() const;
	QString		basePath() const;

	void		getInfo(ReviewBoardServer* server);

private:

	QString	m_url;
	QString	m_rootUrl;
	QString m_uuid;
	int		m_rid;
	QString	m_name;
	QString m_tool;

};

} // namespace Internal
} // namespace ReviewBoard

#endif // REPOSITORY_H
