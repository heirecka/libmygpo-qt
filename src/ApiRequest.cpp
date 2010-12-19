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


#include "ApiRequest.h"
#include "UrlBuilder.h"
#include "JsonParser.h"
//#include "RequestExceptions.h"
#include "RequestHandler.h"

#include <QDebug>

#include <QLatin1String>

namespace mygpo
{

class ApiRequestPrivate
{
public:
    //Constructors
    ApiRequestPrivate( const QString& username, const QString& password, QNetworkAccessManager* nam );
    ApiRequestPrivate( QNetworkAccessManager* nam);
    //Member Functions
    QNetworkReply* toplistOpml ( uint count );
    QNetworkReply* searchOpml ( const QString& query );
    QNetworkReply* suggestionsOpml ( uint count );
    PodcastListPtr toplist ( uint count );
    PodcastListPtr search ( const QString& query );
    PodcastListPtr suggestions ( uint count );
    PodcastListPtr podcastsOfTag ( uint count, const QString& tag );
    PodcastPtr podcastData ( const QUrl& podcasturl );
    EpisodePtr episodeData ( const QUrl& podcasturl, const QUrl& episodeurl );
    EpisodeListPtr favoriteEpisodes ( const QString& username );
    TagListPtr topTags ( uint count );
    AddRemoveResultPtr addRemoveSubscriptions ( const QString& username, const QString& device, const QList< QUrl >& add, const QList< QUrl >& remove );
    SettingsPtr accountSettings( const QString& username );
    SettingsPtr deviceSettings ( const QString& username, const QString& device );
    SettingsPtr podcastSettings ( const QString& username, const QString& podcastUrl );
    SettingsPtr episodeSettings ( const QString& username, const QString& podcastUrl, const QString& episodeUrl );
private:
    RequestHandler m_requestHandler;
};

}

using namespace mygpo;

ApiRequestPrivate::ApiRequestPrivate(const QString& username, const QString& password, QNetworkAccessManager* nam) : m_requestHandler(username,password,nam)
{

}

ApiRequestPrivate::ApiRequestPrivate(QNetworkAccessManager* nam) : m_requestHandler(nam)
{

}

QNetworkReply* ApiRequestPrivate::toplistOpml ( uint count )
{
    QUrl requestUrl = UrlBuilder::getToplistUrl ( count, UrlBuilder::OPML );
    return m_requestHandler.getRequest ( requestUrl );
}

QNetworkReply* ApiRequestPrivate::searchOpml ( const QString& query )
{
    QUrl requestUrl = UrlBuilder::getPodcastSearchUrl ( query, UrlBuilder::OPML );
    return m_requestHandler.getRequest ( requestUrl );
}


QNetworkReply* ApiRequestPrivate::suggestionsOpml ( uint count )
{
    QUrl requestUrl = UrlBuilder::getSuggestionsUrl ( count , UrlBuilder::OPML );
    return m_requestHandler.getRequest ( requestUrl );
}

PodcastListPtr ApiRequestPrivate::toplist ( uint count )
{
    QUrl requestUrl = UrlBuilder::getToplistUrl ( count );
    QNetworkReply *reply;
    reply = m_requestHandler.getRequest ( requestUrl );
    PodcastListPtr podcastList ( new PodcastList( reply ) );
    return podcastList;
}

PodcastListPtr ApiRequestPrivate::search ( const QString& query )
{
    QUrl requestUrl = UrlBuilder::getPodcastSearchUrl ( query );
    QNetworkReply *reply;
    reply = m_requestHandler.getRequest ( requestUrl );

    PodcastListPtr podcastList (new PodcastList( reply ) );
    return podcastList;
}

EpisodePtr ApiRequestPrivate::episodeData ( const QUrl& podcasturl, const QUrl& episodeurl )
{
    QUrl requestUrl = UrlBuilder::getEpisodeDataUrl ( podcasturl.toString(),episodeurl.toString() );
    QNetworkReply *reply;
    reply = m_requestHandler.getRequest ( requestUrl );

    EpisodePtr episode ( new Episode( reply ) );
    return episode;
}

EpisodeListPtr ApiRequestPrivate::favoriteEpisodes ( const QString& username )
{
    QUrl requestUrl = UrlBuilder::getFavEpisodesUrl ( username );
    QNetworkReply *reply;
    reply = m_requestHandler.getRequest ( requestUrl );
    EpisodeListPtr episodeList ( new EpisodeList (reply) );
    return episodeList;
}

PodcastPtr ApiRequestPrivate::podcastData ( const QUrl& podcasturl )
{
    QUrl requestUrl = UrlBuilder::getPodcastDataUrl ( podcasturl.toString() );
    QNetworkReply *reply;
    reply = m_requestHandler.getRequest ( requestUrl );

    PodcastPtr podcast ( new Podcast( reply ) );
    return podcast;
}

PodcastListPtr ApiRequestPrivate::podcastsOfTag ( uint count, const QString& tag )
{
    QUrl requestUrl = UrlBuilder::getPodcastsOfTagUrl ( tag,count );
    QNetworkReply *reply;
    reply = m_requestHandler.getRequest ( requestUrl );

    PodcastListPtr podcastList ( new PodcastList( reply ) );
    return podcastList;
}

TagListPtr ApiRequestPrivate::topTags ( uint count )
{
    QUrl requestUrl = UrlBuilder::getTopTagsUrl ( count );
    QNetworkReply *reply;
    reply = m_requestHandler.getRequest ( requestUrl );

    TagListPtr tagList ( new TagList( reply ) );
    return tagList;
}

PodcastListPtr ApiRequestPrivate::suggestions ( uint count )
{
    QUrl requestUrl = UrlBuilder::getSuggestionsUrl ( count );
    QNetworkReply *reply;
    reply = m_requestHandler.getRequest ( requestUrl );

    PodcastListPtr podcastList ( new PodcastList( reply ) );
    return podcastList;
}

AddRemoveResultPtr ApiRequestPrivate::addRemoveSubscriptions ( const QString& username, const QString& device,const QList< QUrl >& add, const QList< QUrl >& remove )
{
    QUrl requesturl = UrlBuilder::getAddRemoveSubUrl ( username,device );
    QByteArray data = JsonParser::addRemoveSubsToJSON ( add,remove );
    //TODO: Check if no URL is contained in both Lists
    QNetworkReply *reply;
    reply = m_requestHandler.postRequest ( data, requesturl );
    AddRemoveResultPtr addRemoveResult(new AddRemoveResult ( reply ));
    return addRemoveResult;
}

SettingsPtr ApiRequestPrivate::accountSettings(const QString& username)
{
    QUrl requesturl = UrlBuilder::getAccountSettingsUrl( username );
    QNetworkReply *reply;
    reply = m_requestHandler.getRequest( requesturl );
    SettingsPtr settings(new Settings ( reply ));
    return settings;
}

SettingsPtr ApiRequestPrivate::deviceSettings(const QString& username, const QString& device)
{
    QUrl requesturl = UrlBuilder::getDeviceSettingsUrl( username, device );
    qDebug() << requesturl;
    QNetworkReply *reply;
    reply = m_requestHandler.getRequest( requesturl );
    SettingsPtr settings(new Settings ( reply ));
    return settings;
}

SettingsPtr ApiRequestPrivate::podcastSettings(const QString& username, const QString& podcastUrl)
{
    QUrl requesturl = UrlBuilder::getPodcastSettingsUrl( username, podcastUrl );
    QNetworkReply *reply;
    reply = m_requestHandler.getRequest( requesturl );
    SettingsPtr settings(new Settings ( reply ));
    return settings;
}

SettingsPtr ApiRequestPrivate::episodeSettings(const QString& username, const QString& podcastUrl, const QString& episodeUrl)
{
    QUrl requesturl = UrlBuilder::getEpisodeSettingsUrl( username, podcastUrl, episodeUrl );
    QNetworkReply *reply;
    reply = m_requestHandler.getRequest( requesturl );
    SettingsPtr settings(new Settings ( reply ));
    return settings;
}

ApiRequest::ApiRequest ( const QString& username, const QString& password, QNetworkAccessManager* nam ) : d(new ApiRequestPrivate( username, password, nam ))
{
}

ApiRequest::ApiRequest ( QNetworkAccessManager* nam ) : d(new ApiRequestPrivate(nam))
{
}

ApiRequest::~ApiRequest()
{
    delete d;
}

QNetworkReply* ApiRequest::toplistOpml(uint count)
{
    return d->toplistOpml(count);
}

QNetworkReply* ApiRequest::searchOpml(const QString& query)
{
    return d->searchOpml(query);
}

QNetworkReply* ApiRequest::suggestionsOpml(uint count)
{
    return d->suggestionsOpml(count);
}

PodcastListPtr ApiRequest::toplist(uint count)
{
    return d->toplist(count);
}

PodcastListPtr ApiRequest::search(const QString& query)
{
    return d->search(query);
}

PodcastListPtr ApiRequest::suggestions(uint count)
{
    return d->suggestions(count);
}

PodcastListPtr ApiRequest::podcastsOfTag(uint count, const QString& tag)
{
    return d->podcastsOfTag(count,tag);
}

PodcastPtr ApiRequest::podcastData(const QUrl& podcasturl)
{
    return d->podcastData(podcasturl);
}

EpisodePtr ApiRequest::episodeData(const QUrl& podcasturl, const QUrl& episodeurl)
{
    return d->episodeData(podcasturl,episodeurl);
}

EpisodeListPtr ApiRequest::favoriteEpisodes(const QString& username)
{
    return d->favoriteEpisodes(username);
}

TagListPtr ApiRequest::topTags(uint count)
{
    return d->topTags(count);
}

AddRemoveResultPtr ApiRequest::addRemoveSubscriptions(const QString& username, const QString& device, const QList< QUrl >& add, const QList< QUrl >& remove)
{
    return d->addRemoveSubscriptions(username,device,add,remove);
}

SettingsPtr ApiRequest::accountSettings(const QString& username)
{
    return d->accountSettings( username );
}

SettingsPtr ApiRequest::deviceSettings(const QString& username, const QString& device)
{
    return d->deviceSettings( username, device );
}

SettingsPtr ApiRequest::podcastSettings(const QString& username, const QString& podcastUrl)
{
    return d->podcastSettings( username, podcastUrl );
}

SettingsPtr ApiRequest::episodeSettings(const QString& username, const QString& podcastUrl, const QString& episodeUrl)
{
    return d->episodeSettings( username, podcastUrl, episodeUrl );
}
