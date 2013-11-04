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


