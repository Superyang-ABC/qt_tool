#pragma once
#include <QDir>
#include <QString>
class Filetool
{
public:
	static bool deleteFile(const QString &path);
	static bool deleteDir(const QString &path);
	static bool createDir(const QString &path);
	static bool copyFile(QString srcPath, QString dstPath, bool coverFileIfExist);
	static bool copyDirectory(const QString& srcPath, const QString& dstPath, const QStringList &filters, bool coverFileIfExist = true);
};

