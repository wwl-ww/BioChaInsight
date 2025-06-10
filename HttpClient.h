#pragma once

#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <QString>
#include <QObject>
#include <QJsonObject>
#include <windows.h>
#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")

class HttpClient : public QObject {
	Q_OBJECT

public:
	explicit HttpClient(QObject* parent = nullptr);
	~HttpClient();

	// HTTP/HTTPS请求方法
	QString post(const QString& url, const QJsonObject& data, const QString& apiKey);
	QString get(const QString& url, const QString& apiKey = "");

	// 设置超时时间
	void setTimeout(int timeoutMs);

	// 获取最后的错误信息
	QString getLastError() const;

private:
	QString makeRequest(const QString& url, const QString& method,
		const QString& data = "", const QString& apiKey = "");

	bool parseUrl(const QString& url, QString& host, QString& path, bool& useHttps, int& port);
	QString wcharToQString(const wchar_t* wstr);
	std::wstring qStringToWString(const QString& str);

	int m_timeout;
	QString m_lastError;
};

#endif // HTTPCLIENT_H 