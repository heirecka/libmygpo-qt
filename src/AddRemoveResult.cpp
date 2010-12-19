/***************************************************************************
* This file is part of libmygpo-qt                                         *
* Copyright (c) 2010 Stefan Derkits <stefan@derkits.at>                    *
* Copyright (c) 2010 Christian Wagner <christian.wagner86@gmx.at>          *
* Copyright (c) 2010 Felix Winter <ixos01@gmail.com>                       *
*                                                                          *
* This library is free software; you can redistribute it and/or            *
* modify it under the terms of the GNU Lesser General Public               *
* License as published by the Free Software Foundation; either             *
* version 2.1 of the License, or (at your option) any later version.       *
*                                                                          *
* This library is distributed in the hope that it will be useful,          *
* but WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU        *
* Lesser General Public License for more details.                          *
*                                                                          *
* You should have received a copy of the GNU Lesser General Public         *
* License along with this library; if not, write to the Free Software      *
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 *
* USA                                                                      *
***************************************************************************/

#include <parser.h>

#include "AddRemoveResult.h"

#include <QDebug>
#include <QSharedPointer>

namespace mygpo {

class AddRemoveResultPrivate : public QObject
{
    Q_OBJECT
public:
    AddRemoveResultPrivate ( AddRemoveResult* qq, QNetworkReply* reply );
    ~AddRemoveResultPrivate ( );
    QVariant updateUrls() const;
    qulonglong timestamp() const;
    QList<QPair<QUrl, QUrl> > updateUrlsList() const;
private:
    AddRemoveResult* const q;
    qulonglong m_timestamp;
    QVariant m_updateUrls;

    QNetworkReply* m_reply;
    QNetworkReply::NetworkError m_error;

    bool parse ( const QVariant& data );
    bool parse ( const QByteArray& data );
private slots:
    void parseData();
    void error ( QNetworkReply::NetworkError error );
};


AddRemoveResultPrivate::AddRemoveResultPrivate ( AddRemoveResult* qq, QNetworkReply* reply ) : q(qq), m_reply ( reply )
{
    QObject::connect ( m_reply,SIGNAL ( finished() ), this, SLOT ( parseData() ) );
    QObject::connect ( m_reply,SIGNAL ( error ( QNetworkReply::NetworkError ) ), this,SLOT ( error ( QNetworkReply::NetworkError ) ) );
}

AddRemoveResultPrivate::~AddRemoveResultPrivate()
{
    if (m_reply)
        delete m_reply;
}


qulonglong AddRemoveResultPrivate::timestamp() const
{
    return m_timestamp;
}

QVariant AddRemoveResultPrivate::updateUrls() const
{
    return m_updateUrls;
}

QList< QPair< QUrl, QUrl > > AddRemoveResultPrivate::updateUrlsList() const
{
    QVariantList updateVarList = updateUrls().toList();
    QList<QPair<QUrl, QUrl > > updateUrls;
    foreach ( const QVariant& url, updateVarList )
    {
        QVariantList urlList = url.toList();
        QUrl first = QUrl ( urlList.at ( 0 ).toString() );
        QUrl second = QUrl ( urlList.at ( 1 ).toString() );
        updateUrls.append ( qMakePair ( first,second ) );
    }
    return updateUrls;
}

bool AddRemoveResultPrivate::parse ( const QVariant& data )
{
    QJson::Parser parser;
    if (!data.canConvert(QVariant::Map))
        return false;
    QVariantMap resultMap = data.toMap();
    QVariant v = resultMap.value ( QLatin1String ( "timestamp" ) );
    if (!v.canConvert(QVariant::ULongLong))
        return false;
    m_timestamp = v.toULongLong();
    m_updateUrls = resultMap.value ( QLatin1String ( "update_urls" ) );
    return true;
}

bool AddRemoveResultPrivate::parse ( const QByteArray& data )
{
    QJson::Parser parser;
    bool ok;
    QVariant variant = parser.parse ( data, &ok );
    if ( ok )
    {
        ok = ( parse ( variant ) );
    }
    return ok;
}


void AddRemoveResultPrivate::parseData()
{
    QJson::Parser parser;
    if ( parse ( m_reply->readAll( ) ) )
    {
        emit q->finished();
    }
    else
    {
        emit q->parseError();
    }
}

void AddRemoveResultPrivate::error ( QNetworkReply::NetworkError error )
{
    this->m_error = error;
    emit q->requestError ( error );
}

AddRemoveResult::AddRemoveResult( QNetworkReply* reply ,QObject* parent ) : QObject(parent), d(new AddRemoveResultPrivate(this, reply))
{

}

AddRemoveResult::~AddRemoveResult ()
{
	delete d;
}

QVariant AddRemoveResult::updateUrls() const
{
	return d->updateUrls();
}

qulonglong AddRemoveResult::timestamp() const
{
	return d->timestamp();
}

QList<QPair<QUrl, QUrl> > AddRemoveResult::updateUrlsList() const
{
	return d->updateUrlsList();
}

}

#include "AddRemoveResult.moc"
