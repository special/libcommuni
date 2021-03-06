/*
* Copyright (C) 2008-2013 The Communi Project
*
* This library is free software; you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
* License for more details.
*/

#include "ircmessage_p.h"
#include "ircmessagedecoder_p.h"

IRC_BEGIN_NAMESPACE

#ifndef IRC_DOXYGEN
IrcMessagePrivate::IrcMessagePrivate() :
    connection(0), type(IrcMessage::Unknown), timeStamp(QDateTime::currentDateTime()), encoding("ISO-8859-15"), flags(-1)
{
}

QString IrcMessagePrivate::prefix() const
{
    if (!m_prefix.isExplicit() && m_prefix.isNull() && !data.prefix.isNull()) {
        if (data.prefix.startsWith(':')) {
            if (data.prefix.length() > 1)
                m_prefix = decode(data.prefix.mid(1), encoding);
        } else {
            // empty (not null)
            m_prefix = QString("");
        }
    }
    return m_prefix.value();
}

void IrcMessagePrivate::setPrefix(const QString& prefix)
{
    m_prefix.setValue(prefix);
    m_nick.clear();
    m_ident.clear();
    m_host.clear();
}

QString IrcMessagePrivate::nick() const
{
    if (m_nick.isNull())
        parsePrefix(prefix(), &m_nick, &m_ident, &m_host);
    return m_nick;
}

QString IrcMessagePrivate::ident() const
{
    if (m_ident.isNull())
        parsePrefix(prefix(), &m_nick, &m_ident, &m_host);
    return m_ident;
}

QString IrcMessagePrivate::host() const
{
    if (m_host.isNull())
        parsePrefix(prefix(), &m_nick, &m_ident, &m_host);
    return m_host;
}

QString IrcMessagePrivate::command() const
{
    if (!m_command.isExplicit() && m_command.isNull() && !data.command.isNull())
        m_command = decode(data.command, encoding);
    return m_command.value();
}

void IrcMessagePrivate::setCommand(const QString& command)
{
    m_command.setValue(command);
}

QStringList IrcMessagePrivate::params() const
{
    if (!m_params.isExplicit() && m_params.isNull() && !data.params.isEmpty()) {
        QStringList params;
        foreach (const QByteArray& param, data.params)
            params += decode(param, encoding);
        m_params = params;
    }
    return m_params.value();
}

QString IrcMessagePrivate::param(int index) const
{
    return params().value(index);
}

void IrcMessagePrivate::setParams(const QStringList& params)
{
    m_params.setValue(params);
}

void IrcMessagePrivate::invalidate()
{
    m_nick.clear();
    m_ident.clear();
    m_host.clear();

    m_prefix.clear();
    m_command.clear();
    m_params.clear();
}

IrcMessageData IrcMessageData::fromData(const QByteArray& data)
{
    IrcMessageData message;
    message.content = data;

    // From RFC 1459:
    //  <message>  ::= [':' <prefix> <SPACE> ] <command> <params> <crlf>
    //  <prefix>   ::= <servername> | <nick> [ '!' <user> ] [ '@' <host> ]
    //  <command>  ::= <letter> { <letter> } | <number> <number> <number>
    //  <SPACE>    ::= ' ' { ' ' }
    //  <params>   ::= <SPACE> [ ':' <trailing> | <middle> <params> ]
    //  <middle>   ::= <Any *non-empty* sequence of octets not including SPACE
    //                 or NUL or CR or LF, the first of which may not be ':'>
    //  <trailing> ::= <Any, possibly *empty*, sequence of octets not including
    //                   NUL or CR or LF>
    QByteArray process = data;

    // parse <prefix>
    if (process.startsWith(':')) {
        message.prefix = process.left(process.indexOf(' '));
        process.remove(0, message.prefix.length() + 1);
    } else {
        // empty (not null)
        message.prefix = QByteArray("");
    }

    // parse <command>
    message.command = process.mid(0, process.indexOf(' '));
    process.remove(0, message.command.length() + 1);

    // parse <params>
    while (!process.isEmpty()) {
        if (process.startsWith(':')) {
            process.remove(0, 1);
            message.params += process;
            process.clear();
        } else {
            QByteArray param = process.mid(0, process.indexOf(' '));
            process.remove(0, param.length() + 1);
            message.params += param;
        }
    }

    return message;
}

QString IrcMessagePrivate::decode(const QByteArray& data, const QByteArray& encoding)
{
    // TODO: not thread safe
    static IrcMessageDecoder decoder;
    return decoder.decode(data, encoding);
}

bool IrcMessagePrivate::parsePrefix(const QString& prefix, QString* nick, QString* ident, QString* host)
{
    const QString trimmed = prefix.trimmed();
    if (trimmed.contains(QLatin1Char(' ')))
        return false;

    const int len = trimmed.length();
    const int ex = trimmed.indexOf(QLatin1Char('!'));
    const int at = trimmed.indexOf(QLatin1Char('@'));

    if (ex == -1 && at == -1) {
        if (nick) *nick = trimmed;
    } else if (ex > 0 && at > 0 && ex + 1 < at && at < len - 1) {
        if (nick) *nick = trimmed.mid(0, ex);
        if (ident) *ident = trimmed.mid(ex + 1, at - ex - 1);
        if (host) *host = trimmed.mid(at + 1);
    } else if (ex > 0 && ex < len - 1 && at == -1) {
        if (nick) *nick = trimmed.mid(0, ex);
        if (ident) *ident = trimmed.mid(ex + 1);
    } else if (at > 0 && at < len - 1 && ex == -1) {
        if (nick) *nick = trimmed.mid(0, at);
        if (host) *host = trimmed.mid(at + 1);
    } else {
        return false;
    }
    return true;
}
#endif // IRC_DOXYGEN

IRC_END_NAMESPACE
