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

#ifndef IRCNETWORK_H
#define IRCNETWORK_H

#include <IrcGlobal>
#include <QtCore/qobject.h>
#include <QtCore/qmetatype.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qscopedpointer.h>

IRC_BEGIN_NAMESPACE

class IrcConnection;
class IrcNetworkPrivate;

class IRC_CORE_EXPORT IrcNetwork : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool initialized READ isInitialized NOTIFY initialized)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QStringList modes READ modes NOTIFY modesChanged)
    Q_PROPERTY(QStringList prefixes READ prefixes NOTIFY prefixesChanged)
    Q_PROPERTY(QStringList channelTypes READ channelTypes NOTIFY channelTypesChanged)
    Q_PROPERTY(QStringList availableCapabilities READ availableCapabilities NOTIFY availableCapabilitiesChanged)
    Q_PROPERTY(QStringList requestedCapabilities READ requestedCapabilities WRITE setRequestedCapabilities NOTIFY requestedCapabilitiesChanged)
    Q_PROPERTY(QStringList activeCapabilities READ activeCapabilities NOTIFY activeCapabilitiesChanged)
    Q_ENUMS(Limit ModeType)
    Q_FLAGS(ModeTypes)

public:
    virtual ~IrcNetwork();

    bool isInitialized();

    QString name() const;

    QStringList modes() const;
    QStringList prefixes() const;

    Q_INVOKABLE QString modeToPrefix(const QString& mode) const;
    Q_INVOKABLE QString prefixToMode(const QString& prefix) const;

    QStringList channelTypes() const;

    Q_INVOKABLE bool isChannel(const QString& name) const;

    enum ModeType {
        TypeA    = 0x1,
        TypeB    = 0x2,
        TypeC    = 0x4,
        TypeD    = 0x8,
        AllTypes = 0xf
    };
    Q_DECLARE_FLAGS(ModeTypes, ModeType)

    Q_INVOKABLE QStringList channelModes(IrcNetwork::ModeTypes types) const;

    enum Limit {
        NickLength,
        ChannelLength,
        TopicLength,
        MessageLength,
        KickReasonLength,
        AwayReasonLength,
        ModeCount
    };

    Q_INVOKABLE int numericLimit(IrcNetwork::Limit limit) const;

    Q_INVOKABLE int modeLimit(const QString& mode) const;
    Q_INVOKABLE int channelLimit(const QString& type) const;
    Q_INVOKABLE int targetLimit(const QString& command) const;

    QStringList availableCapabilities() const;
    QStringList requestedCapabilities() const;
    QStringList activeCapabilities() const;

    Q_INVOKABLE bool hasCapability(const QString& capability) const;
    Q_INVOKABLE bool isCapable(const QString& capability) const;

public Q_SLOTS:
    bool requestCapability(const QString& capability);
    bool requestCapabilities(const QStringList& capabilities);
    void setRequestedCapabilities(const QStringList& capabilities);

Q_SIGNALS:
    void initialized();
    void nameChanged(const QString& name);
    void modesChanged(const QStringList& modes);
    void prefixesChanged(const QStringList& prefixes);
    void channelTypesChanged(const QStringList& types);
    void availableCapabilitiesChanged(const QStringList& capabilities);
    void requestedCapabilitiesChanged(const QStringList& capabilities);
    void activeCapabilitiesChanged(const QStringList& capabilities);
    void requestingCapabilities();

private:
    friend class IrcProtocol;
    friend class IrcConnection;
    explicit IrcNetwork(IrcConnection* connection);

    QScopedPointer<IrcNetworkPrivate> d_ptr;
    Q_DECLARE_PRIVATE(IrcNetwork)
    Q_DISABLE_COPY(IrcNetwork)
};

#ifndef QT_NO_DEBUG_STREAM
IRC_CORE_EXPORT QDebug operator<<(QDebug debug, IrcNetwork::Limit limit);
IRC_CORE_EXPORT QDebug operator<<(QDebug debug, IrcNetwork::ModeType type);
IRC_CORE_EXPORT QDebug operator<<(QDebug debug, IrcNetwork::ModeTypes types);
IRC_CORE_EXPORT QDebug operator<<(QDebug debug, const IrcNetwork* network);
#endif // QT_NO_DEBUG_STREAM

Q_DECLARE_OPERATORS_FOR_FLAGS(IrcNetwork::ModeTypes)

IRC_END_NAMESPACE

Q_DECLARE_METATYPE(IRC_PREPEND_NAMESPACE(IrcNetwork*))

#endif // IRCNETWORK_H
