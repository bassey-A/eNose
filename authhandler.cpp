#include "authhandler.h"

AuthHandler::AuthHandler(QString key, QObject *parent)
    : QObject(parent)
    , m_apiKey(QString())
{
    m_networkAccessManager = new QNetworkAccessManager( this );
    m_apiKey = key;
}

AuthHandler::~AuthHandler()
{
    m_networkAccessManager->deleteLater();
}

void AuthHandler::signUserUp(const QString &emailAddress, const QString &password)
{
    QString signUpEndpoint = "https://identitytoolkit.googleapis.com/v1/accounts:signUp?key=" + m_apiKey;

    QVariantMap variantPayload;
    variantPayload["email"] = emailAddress;
    variantPayload["password"] = password;
    variantPayload["returnSecureToken"] = true;

    QJsonDocument jsonPayload = QJsonDocument::fromVariant( variantPayload );
    performPOST( signUpEndpoint, jsonPayload );
}

void AuthHandler::signUserIn(const QString &emailAddress, const QString &password)
{
    QString signInEndpoint = "https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key=" + m_apiKey;

    QVariantMap variantPayload;
    variantPayload["email"] = emailAddress;
    variantPayload["password"] = password;
    variantPayload["returnSecureToken"] = true;

    QJsonDocument jsonPayload = QJsonDocument::fromVariant(variantPayload);

    performPOST( signInEndpoint, jsonPayload );
}

void AuthHandler::networkReplyReadyRead()
{
    QByteArray response = m_networkReply->readAll();
    //qDebug() << response;
    m_networkReply->deleteLater();

    parseResponse( response );
}

void AuthHandler::performAuthenticatedDatabaseCall()
{
    QString endPoint = "https://enose-hkr24-default-rtdb.europe-west1.firebasedatabase.app/SensorReadings.json?auth=" + m_idToken;
    //QString endPoint = "https://enose-hkr24-default-rtdb.europe-west1.firebasedatabase.app/Dogs.json";
    m_networkReply = m_networkAccessManager->get( QNetworkRequest(QUrl(endPoint)));
    connect( m_networkReply, &QNetworkReply::readyRead, this, &AuthHandler::networkReplyReadyRead );
}

void AuthHandler::performPOST(const QString &url, const QJsonDocument &payload)
{
    QNetworkRequest newRequest( (QUrl( url )) );
    newRequest.setHeader( QNetworkRequest::ContentTypeHeader, QString( "application/json"));
    m_networkReply = m_networkAccessManager->post(newRequest, payload.toJson());
    connect( m_networkReply, &QNetworkReply::readyRead, this, &AuthHandler::networkReplyReadyRead );
}

void AuthHandler::parseResponse(const QByteArray &response)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(response);
    if ( jsonDocument.object().contains("error"))
    {
        QString message = jsonDocument.object().value("error").toString();
        qDebug() << "Error occured!" << response << "\n\n" << message;
    }
    else if ( jsonDocument.object().contains("kind"))
    {
        QString idToken = jsonDocument.object().value("idToken").toString();
        qDebug() << "Success!";
        m_idToken = idToken;
        emit userSignedIn();
    }
    else
        qDebug() << "The response was: " << response;
}

void AuthHandler::add_to_db(QVariantMap readings)
{
    QJsonDocument jsonDoc = QJsonDocument::fromVariant(readings);
    QNetworkRequest add_to_db_req (QUrl("https://enose-hkr24-default-rtdb.europe-west1.firebasedatabase.app/SensorReadings.json?auth=" + m_idToken));
    add_to_db_req.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));
    m_networkAccessManager->put(add_to_db_req, jsonDoc.toJson()); // use put to update, post to write
}


