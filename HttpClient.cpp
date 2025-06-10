#include "stdafx.h"

#include "HttpClient.h"
#include <QJsonDocument>
#include <QDebug>
#include <QUrl>
#include <sstream>

HttpClient::HttpClient(QObject* parent)
	: QObject(parent), m_timeout(30000) {
}

HttpClient::~HttpClient() {
}

void HttpClient::setTimeout(int timeoutMs) {
	m_timeout = timeoutMs;
}

QString HttpClient::getLastError() const {
	return m_lastError;
}

QString HttpClient::post(const QString& url, const QJsonObject& data, const QString& apiKey) {
	QJsonDocument doc(data);
	QString jsonData = doc.toJson(QJsonDocument::Compact);
	return makeRequest(url, "POST", jsonData, apiKey);
}

QString HttpClient::get(const QString& url, const QString& apiKey) {
	return makeRequest(url, "GET", "", apiKey);
}

QString HttpClient::makeRequest(const QString& url, const QString& method,
	const QString& data, const QString& apiKey) {
	m_lastError.clear();

	// 解析URL
	QString host, path;
	bool useHttps;
	int port;
	if (!parseUrl(url, host, path, useHttps, port)) {
		m_lastError = "Invalid URL format";
		return QString();
	}

	qDebug() << "发起请求:" << method << url;
	qDebug() << "主机:" << host << "端口:" << port << (useHttps ? "(HTTPS)" : "(HTTP)");

	// 初始化WinHTTP
	HINTERNET hSession = WinHttpOpen(L"AI-Assistant/1.0",
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS,
		0);

	if (!hSession) {
		m_lastError = QString("WinHttpOpen failed: %1").arg(GetLastError());
		return QString();
	}

	// 连接到服务器
	std::wstring wHost = qStringToWString(host);
	HINTERNET hConnect = WinHttpConnect(hSession,
		wHost.c_str(),
		port,
		0);

	if (!hConnect) {
		m_lastError = QString("WinHttpConnect failed: %1").arg(GetLastError());
		WinHttpCloseHandle(hSession);
		return QString();
	}

	// 创建请求
	std::wstring wMethod = qStringToWString(method);
	std::wstring wPath = qStringToWString(path);

	DWORD flags = 0;
	if (useHttps) {
		flags = WINHTTP_FLAG_SECURE;
		qDebug() << "使用HTTPS连接";
	}
	else {
		qDebug() << "使用HTTP连接";
	}

	HINTERNET hRequest = WinHttpOpenRequest(hConnect,
		wMethod.c_str(),
		wPath.c_str(),
		NULL,
		WINHTTP_NO_REFERER,
		WINHTTP_DEFAULT_ACCEPT_TYPES,
		flags);

	if (!hRequest) {
		m_lastError = QString("WinHttpOpenRequest failed: %1").arg(GetLastError());
		WinHttpCloseHandle(hConnect);
		WinHttpCloseHandle(hSession);
		return QString();
	}

	// 设置超时
	WinHttpSetTimeouts(hRequest, m_timeout, m_timeout, m_timeout, m_timeout);

	// 如果是HTTPS，禁用证书检查（解决SSL问题）
	if (useHttps) {
		DWORD flags = SECURITY_FLAG_IGNORE_CERT_CN_INVALID |
			SECURITY_FLAG_IGNORE_CERT_DATE_INVALID |
			SECURITY_FLAG_IGNORE_UNKNOWN_CA |
			SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE;

		WinHttpSetOption(hRequest,
			WINHTTP_OPTION_SECURITY_FLAGS,
			&flags,
			sizeof(flags));

		qDebug() << "已禁用HTTPS证书验证";
	}

	// 设置请求头
	QString headers = "Content-Type: application/json\r\n";
	if (!apiKey.isEmpty()) {
		headers += QString("Authorization: Bearer %1\r\n").arg(apiKey);
	}
	headers += "User-Agent: AI-Assistant-WinHTTP/1.0\r\n";

	std::wstring wHeaders = qStringToWString(headers);

	// 发送请求 - 修复JSON编码问题
	QByteArray utf8Data = data.toUtf8();  // 转为UTF-8字节
	BOOL result = WinHttpSendRequest(hRequest,
		wHeaders.c_str(),
		-1,
		(LPVOID)utf8Data.constData(),
		utf8Data.length(),
		utf8Data.length(),
		0);

	if (!result) {
		DWORD error = GetLastError();
		m_lastError = QString("WinHttpSendRequest failed: %1").arg(error);

		// 提供更友好的错误信息
		switch (error) {
		case ERROR_WINHTTP_TIMEOUT:
			m_lastError = "请求超时";
			break;
		case ERROR_WINHTTP_NAME_NOT_RESOLVED:
			m_lastError = "无法解析主机名";
			break;
		case ERROR_WINHTTP_CANNOT_CONNECT:
			m_lastError = "无法连接到服务器";
			break;
		case ERROR_WINHTTP_SECURE_FAILURE:
			m_lastError = "HTTPS安全连接失败";
			break;
		default:
			m_lastError = QString("网络请求失败，错误代码: %1").arg(error);
		}

		WinHttpCloseHandle(hRequest);
		WinHttpCloseHandle(hConnect);
		WinHttpCloseHandle(hSession);
		return QString();
	}

	// 接收响应
	result = WinHttpReceiveResponse(hRequest, NULL);
	if (!result) {
		m_lastError = QString("WinHttpReceiveResponse failed: %1").arg(GetLastError());
		WinHttpCloseHandle(hRequest);
		WinHttpCloseHandle(hConnect);
		WinHttpCloseHandle(hSession);
		return QString();
	}

	// 读取响应数据
	QString response;
	DWORD bytesAvailable = 0;

	do {
		bytesAvailable = 0;
		if (!WinHttpQueryDataAvailable(hRequest, &bytesAvailable)) {
			m_lastError = QString("WinHttpQueryDataAvailable failed: %1").arg(GetLastError());
			break;
		}

		if (bytesAvailable > 0) {
			char* buffer = new char[bytesAvailable + 1];
			DWORD bytesRead = 0;

			if (WinHttpReadData(hRequest, buffer, bytesAvailable, &bytesRead)) {
				buffer[bytesRead] = '\0';
				response += QString::fromUtf8(buffer);
			}

			delete[] buffer;
		}

	} while (bytesAvailable > 0);

	// 清理资源
	WinHttpCloseHandle(hRequest);
	WinHttpCloseHandle(hConnect);
	WinHttpCloseHandle(hSession);

	qDebug() << "请求完成，响应长度:" << response.length();

	return response;
}

bool HttpClient::parseUrl(const QString& url, QString& host, QString& path, bool& useHttps, int& port) {
	QUrl qurl(url);
	if (!qurl.isValid()) {
		return false;
	}

	host = qurl.host();
	path = qurl.path();
	if (path.isEmpty()) {
		path = "/";
	}
	if (!qurl.query().isEmpty()) {
		path += "?" + qurl.query();
	}

	QString scheme = qurl.scheme().toLower();
	useHttps = (scheme == "https");

	port = qurl.port();
	if (port == -1) {
		port = useHttps ? 443 : 80;
	}

	return !host.isEmpty();
}

QString HttpClient::wcharToQString(const wchar_t* wstr) {
	return QString::fromWCharArray(wstr);
}

std::wstring HttpClient::qStringToWString(const QString& str) {
	return str.toStdWString();
}