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

	dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //���ù���

	QFileInfoList fileList = dir.entryInfoList(); // ��ȡ���е��ļ���Ϣ

	//�����ļ���Ϣ
	foreach(QFileInfo file, fileList)
	{
		// ���ļ���ɾ��
		if (file.isFile())
		{
			file.dir().remove(file.fileName());
		}
		else // �ݹ�ɾ��
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
//sourceԴ�ļ�Ŀ¼·����destinationĿ���ļ�Ŀ¼��override�ļ������Ƿ񸲸�
bool copyDir(const QString &source, const QString &destination, bool override)
{
	QDir directory(source);
	if (!directory.exists())
	{
		qDebug() << "������";
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
	if (!dstDir.exists()) { //Ŀ���ļ�Ŀ¼�������򴴽��ļ�Ŀ¼
		if (!dstDir.mkdir(dstDir.absolutePath()))
			return false;
	}
	srcDir.setNameFilters(filters);
	QFileInfoList fileInfoList = srcDir.entryInfoList();
	foreach(QFileInfo fileInfo, fileInfoList) {
		if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")
			continue;

		if (fileInfo.isFile())  // ��ΪĿ¼ʱ���ݹ�Ľ���copy 
		{
			if (coverFileIfExist && dstDir.exists(fileInfo.fileName())) {
				dstDir.remove(fileInfo.fileName());
			}
			/// �����ļ�copy
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

		if (fileInfo.isDir()) {    // ��ΪĿ¼ʱ���ݹ�Ľ���copy 
			if (!copyDirectory(fileInfo.filePath(), dstDir.filePath(fileInfo.fileName()), filters, coverFileIfExist))
				return false;
		}
	}
	return true;
}
/*
//(Դ�ļ�Ŀ¼·����Ŀ���ļ�Ŀ¼���ļ������Ƿ񸲸�)
bool ConfigDialog::copyDirectory(const QString& srcPath, const QString& dstPath, const QStringList &filters, bool coverFileIfExist)
{
	QDir srcDir(srcPath);
	QDir dstDir(dstPath);
	if (!dstDir.exists()) { //Ŀ���ļ�Ŀ¼�������򴴽��ļ�Ŀ¼
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

		if (fileInfo.isFile())  // ��ΪĿ¼ʱ���ݹ�Ľ���copy
		{
			if (coverFileIfExist && dstDir.exists(fileInfo.fileName())) {
				dstDir.remove(fileInfo.fileName());
			}
			/// �����ļ�copy
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

		if (fileInfo.isDir()) {    // ��ΪĿ¼ʱ���ݹ�Ľ���copy
			if (!copyDirectory(fileInfo.filePath(), QDir(dstDir.filePath(srcDir.dirName())).absolutePath(), filters, coverFileIfExist))
				return false;
		}
	}
	return true;
}
*/

//(Դ�ļ��ļ�·����Ŀ���ļ�·�����ļ������Ƿ񸲸�)
bool Filetool::copyFile(QString srcPath, QString dstPath, bool coverFileIfExist)
{
	srcPath.replace("\\", "/");
	dstPath.replace("\\", "/");
	if (srcPath == dstPath) {
		return true;
	}

	if (!QFile::exists(srcPath)) {  //Դ�ļ�������
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