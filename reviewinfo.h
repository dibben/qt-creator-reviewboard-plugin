#ifndef REVIEWINFO_H
#define REVIEWINFO_H

#include <QObject>
#include <QScriptValue>
#include <QDateTime>

namespace ReviewBoard {
namespace Internal {

class ReviewInfo : public QObject
{
    Q_OBJECT
public:

		ReviewInfo();
		ReviewInfo(const QScriptValue& json);
		ReviewInfo(const ReviewInfo& rval);

		bool		valid() const;
		int			id() const;
		QString		summary() const;

		QString		displayString() const;

private:

		int			m_Id;
		QString		m_Description;
		QString		m_Summary;
		QDateTime	m_LastUpdated;
		QDateTime	m_Created;

};

} // namespace Internal
} // namespace ReviewBoard

#endif // REVIEWINFO_H

/*
branch: "master",
	  bugs_closed: ["12", "34"],
	  changenum: null,
	  description: "This is the description of the review request.",
	  id: 12321,
	  last_updated: "2009-12-05 19:03:00",
	  public: 1,
	  status: "pending",
	  submitter: {
		username: "joe",
		first_name: "Joe",
		last_name: "User",
		url: "/users/joe/",
		email: "joe@example.com",
		fullname: "Joe User",
		id: 1
	  },
	  testing_done: "Ran unit tests",
	  time_added: "2009-12-05 19:00:42",
	  summary: "This is the review request's summary",
	  repository: {
		path: "git://github.com/reviewboard/reviewboard.git",
		tool: "Git",
		id: 1,
		name: "Review Board"
	  },
	  target_groups: [
		{
		  url: "/groups/reviewboard/",
		  display_name: "Review Board",
		  id: 1,
		  mailing_list: "reviewboard@example.com",
		  name: "reviewboard"
		}
	  ],
	  target_people: []

 */
