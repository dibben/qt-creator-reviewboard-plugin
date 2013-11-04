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
