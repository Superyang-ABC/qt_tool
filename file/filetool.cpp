#include "filetool.h"
#include "QDebug"

bool Filetool::deleteFile(const QString & path)
{
	QFile file(path);
	if (!file.remove())
	{
		qDebug() << "remove " << path << " error:" << file.errorString();
		return false;
	}
	return true;
}

bool Filetool::deleteDir(const QString & path)
{
	
	if (path.isEmpty())
	{
		return false;
	}

	QDir dir(path);
	if (!dir.exists())
	{
		return false;
	}

	dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤

	QFileInfoList fileList = dir.entryInfoList(); // 获取所有的文件信息

	//遍历文件信息
	foreach(QFileInfo file, fileList)
	{
		// 是文件，删除
		if (file.isFile())
		{
			file.dir().remove(file.fileName());
		}
		else // 递归删除
		{
			deleteDir(file.absoluteFilePath());
			file.dir().rmdir(file.absoluteFilePath());
		}
	}

	return true;
	
}
bool Filetool::createDir(const QString &path)
{
	QDir dir;
	return dir.mkpath(path);
}

/*
//source源文件目录路径，destination目的文件目录，override文件存在是否覆盖
bool copyDir(const QString &source, const QString &destination, bool override)
{
	QDir directory(source);
	if (!directory.exists())
	{
		qDebug() << "不存在";
		return false;
	}


	QString srcPath = QDir::toNativeSeparators(source);
	if (!srcPath.endsWith(QDir::separator()))
		srcPath += QDir::separator();
	QString dstPath = QDir::toNativeSeparators(destination);
	if (!dstPath.endsWith(QDir::separator()))
		dstPath += QDir::separator();


	bool error = false;
	QStringList fileNames = directory.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
	for (QStringList::size_type i = 0; i != fileNames.size(); ++i)
	{
		QString fileName = fileNames.at(i);
		QString srcFilePath = srcPath + fileName;
		QString dstFilePath = dstPath + fileName;
		QFileInfo fileInfo(srcFilePath);
		if (fileInfo.isFile() || fileInfo.isSymLink())
		{
			if (override)
			{
				QFile::setPermissions(dstFilePath, QFile::WriteOwner);
			}
			QFile::copy(srcFilePath, dstFilePath);
		}
		else if (fileInfo.isDir())
		{
			QDir dstDir(dstFilePath);
			dstDir.mkpath(dstFilePath);
			if (!copyDir(srcFilePath, dstFilePath, override))
			{
				error = true;
			}
		}
	}
	return !error;
}
*/

bool Filetool::copyDirectory(const QString& srcPath, const QString& dstPath, const QStringList &filters, bool coverFileIfExist)
{
	QDir srcDir(srcPath);
	QDir dstDir(dstPath);
	if (!dstDir.exists()) { //目的文件目录不存在则创建文件目录
		if (!dstDir.mkdir(dstDir.absolutePath()))
			return false;
	}
	srcDir.setNameFilters(filters);
	QFileInfoList fileInfoList = srcDir.entryInfoList();
	foreach(QFileInfo fileInfo, fileInfoList) {
		if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")
			continue;

		if (fileInfo.isFile())  // 当为目录时，递归的进行copy 
		{
			if (coverFileIfExist && dstDir.exists(fileInfo.fileName())) {
				dstDir.remove(fileInfo.fileName());
			}
			/// 进行文件copy
			if (!QFile::copy(fileInfo.filePath(), dstDir.filePath(fileInfo.fileName()))) {
				return false;
			}
		}
	}
	srcDir.setNameFilters(QStringList());
	fileInfoList = srcDir.entryInfoList();
	foreach(QFileInfo fileInfo, fileInfoList) {
		if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")
			continue;

		if (fileInfo.isDir()) {    // 当为目录时，递归的进行copy 
			if (!copyDirectory(fileInfo.filePath(), dstDir.filePath(fileInfo.fileName()), filters, coverFileIfExist))
				return false;
		}
	}
	return true;
}
/*
//(源文件目录路劲，目的文件目录，文件存在是否覆盖)
bool ConfigDialog::copyDirectory(const QString& srcPath, const QString& dstPath, const QStringList &filters, bool coverFileIfExist)
{
	QDir srcDir(srcPath);
	QDir dstDir(dstPath);
	if (!dstDir.exists()) { //目的文件目录不存在则创建文件目录
		if (!dstDir.mkpath(dstDir.absolutePath()))
			return false;
	}
	QString temp = dstDir.filePath(srcDir.dirName());
	if(!dstDir.mkpath(QDir(dstDir.filePath(srcDir.dirName())).absolutePath()))
	{
		return false;
	}

	if (!srcDir.exists())
	{
		return false;
	}

	srcDir.setNameFilters(filters);
	QFileInfoList fileInfoList = srcDir.entryInfoList();
	foreach(QFileInfo fileInfo, fileInfoList) {
		if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")
			continue;

		if (fileInfo.isFile())  // 当为目录时，递归的进行copy
		{
			if (coverFileIfExist && dstDir.exists(fileInfo.fileName())) {
				dstDir.remove(fileInfo.fileName());
			}
			/// 进行文件copy
			if (!QFile::copy(fileInfo.filePath(), QFileInfo(dstDir.filePath(srcDir.dirName() + "/" + fileInfo.fileName())).absoluteFilePath())) {
				return false;
			}
		}
	}
	srcDir.setNameFilters(QStringList());
	fileInfoList = srcDir.entryInfoList();
	foreach(QFileInfo fileInfo, fileInfoList) {
		if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")
			continue;

		if (fileInfo.isDir()) {    // 当为目录时，递归的进行copy
			if (!copyDirectory(fileInfo.filePath(), QDir(dstDir.filePath(srcDir.dirName())).absolutePath(), filters, coverFileIfExist))
				return false;
		}
	}
	return true;
}
*/

//(源文件文件路径，目的文件路径，文件存在是否覆盖)
bool Filetool::copyFile(QString srcPath, QString dstPath, bool coverFileIfExist)
{
	srcPath.replace("\\", "/");
	dstPath.replace("\\", "/");
	if (srcPath == dstPath) {
		return true;
	}

	if (!QFile::exists(srcPath)) {  //源文件不存在
		return false;
	}

	if (QFile::exists(dstPath)) {
		if (coverFileIfExist) {
			QFile::remove(dstPath);
		}
	}

	if (!QFile::copy(srcPath, dstPath)) {
		return false;
	}
	return true;
}