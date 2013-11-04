#include "reviewinfo.h"

using namespace ReviewBoard;
using namespace ReviewBoard::Internal;

ReviewInfo::ReviewInfo()
{
	m_Id = -1;
}

ReviewInfo::ReviewInfo(const QScriptValue& json)
{

    m_Summary = json.property(QString::fromLatin1("summary")).toString();
    m_Description = json.property(QString::fromLatin1("description")).toString();
    m_Id  = json.property(QString::fromLatin1("id")).toInteger();
    m_LastUpdated  = json.property(QString::fromLatin1("last_updated")).toDateTime();
    m_Created = json.property(QString::fromLatin1("time_added")).toDateTime();

}

ReviewInfo::ReviewInfo(const ReviewInfo& rval)
{
	m_Id  = rval.m_Id;
	m_Description  = rval.m_Description;
	m_Summary  = rval.m_Summary;
	m_LastUpdated  = rval.m_LastUpdated;
	m_Created  = rval.m_Created;
}

bool ReviewInfo::valid() const
{
	return m_Id > 0;
}


int ReviewInfo::id() const
{
	return m_Id;
}

QString ReviewInfo::summary() const
{
	return m_Summary;

}

QString ReviewInfo::displayString() const
{
	if (!valid()) return tr("invalid");
	return tr("%1: %2").arg(id()).arg(summary());
}


