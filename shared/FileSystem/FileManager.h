//  ***************************************************************
//  FileManager - Creation date: 03/24/2009
//  -------------------------------------------------------------
//  Robinson Technologies Copyright (C) 2009 - All Rights Reserved
//
//  ***************************************************************
//  Programmer(s):  Seth A. Robinson (seth@rtsoft.com)
//  ***************************************************************

#ifndef FileManager_h__
#define FileManager_h__

//a simple way to load and access a file, example:
//FileInstance f("crap.txt");
class FileSystem;
class StreamingInstance;

#include "FileSystem/FileSystem.h"

class FileInstance
{
public:
	
	FileInstance(const std::string &fileName);
	FileInstance(const std::string &fileName, bool bAddBasePath);
	FileInstance() { m_size = 0; m_pData = NULL;}

	~FileInstance();

	bool IsLoaded() {return m_pData != NULL;}
	byte * GetAsBytes() {return m_pData;}
	char * GetAsChars() {return (char*)m_pData;}
	int GetSize() {return m_size;}
	bool Load(std::string fileName, bool bAddBasePath = true);

private:
	
	void Kill();

	byte *m_pData;
	int m_size;

};

//the real workhorse that should handle zips/disk/mounting

class FileManager
{
public:
	FileManager();
	virtual ~FileManager();
	byte * Get(std::string fileName, int *pSizeOut, bool bAddBasePath = true); //it news' it, up to you to SAFE_DELETE_ARRAY()
	StreamingInstance * GetStreaming(std::string fileName, int *pSizeOut, bool bAddBasePath = true); //pSizeOut currently always set to 0.  Returns null on fail. You must DELETE !
	void MountFileSystem(FileSystem* pFileSystem);
	bool FileExists(std::string fileName, bool bAddBasePath = true);
	int GetFileSize(std::string fileName, bool bAddBasePath = true);
	FileSystem * GetFileSystem(int index); //returns null if invalid index
	bool Copy(std::string srcFile, std::string dstFile, bool bAddBasePath = true); //copy a file to a file.  Destination can't be inside a zip, but src can be.

private:

	std::list<FileSystem*> m_fileSystems;
};

#endif // FileManager_h__