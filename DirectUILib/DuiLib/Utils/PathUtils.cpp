#include "StdAfx.h"
#include "PathUtils.h"

PathFinder::PathFinder()
    :_recordNewlyReceived(false)
    ,_recorder(nullptr)
    ,_recursive(true)
    ,_stateSize(false)
{

}

PathFinder::~PathFinder()
{

}

bool PathFinder::PathMatch(const vector<QkString> & Patterns, const QkString & path, LPCTSTR name)
{
    for (size_t i = 0, length=Patterns.size(); i < length; i++)
    {
        if (PathMatchSpec(path, Patterns[i]))
            return true;
        if (Patterns[i].Find(L'\\')==-1)
        {
            if (!name)
            {
                int pos = path.ReverseFind(L'\\');
                if (pos==path.GetLength()) pos = path.ReverseFind(L'\\');
                if (pos!=-1)
                {
                    Pathbuffer = path.Mid(pos);
                    Pathbuffer.Replace(L"\\", L"");
                    name = Pathbuffer;
                }
            }
            if (name && PathMatchSpec(name, Patterns[i]))
                return true;
        }
    }
    return false;
}

void PathFinder::SearchFiles(QkString path
    , vector<QkString> * results, vector<FileInfo> * resultInfos
    , const bool Recursive)
{
    path.Replace(L"/", L"\\");
    path.Trim();
    QkString folderpath(path);
    WIN32_FIND_DATA findData;
    FileInfo fileInfo;
    if (path.GetLength()>256)
    {
        path.Prepend(L"\\\\?\\");
    }
    if (PathFileExists(path))
    {
        if (PathIsDirectory(path))
        { // is path
            if (folderpath[folderpath.GetLength()-1]!='\\')
            {
                path+=L'\\';
                folderpath+=L'\\';
            }
            //path = path.Mid(0, path.GetLength()-1);
            path+=L"*.*";
        }
        else 
        { // is file
            if(PathMatch(_patterns, folderpath, 0))
            {
                if (results)
                {
                    results->push_back(folderpath);
                }
                if (resultInfos)
                {
                    HANDLE hFind = ::FindFirstFile(path, &findData);
                    if (hFind != INVALID_HANDLE_VALUE)
                    {
                        ::PathRemoveFileSpec((LPWSTR)folderpath.GetData()); //todo
                        folderpath.RecalcSize();
                        FindDataIntoFindInfo(fileInfo, findData, folderpath);
                        resultInfos->push_back(fileInfo);
                        ::FindClose(hFind);
                    }
                }
            }
            return;
        }
    }
    else
    {   // is filter
        // fetch real path from wildcard expression.
        // wildcards in the middle part is not supported.
        int pos = path.ReverseFind(L'\\');
        if (pos==path.GetLength()) pos = path.ReverseFind(L'\\');
        if (pos==-1) return;
        QkString path_checker(path, pos);
        if (PathIsDirectory(path_checker))
        {
            if (path_checker[path_checker.GetLength()-1]!='\\')
            {
                folderpath+='\\';
            }
        }
        else return;
    }
    HANDLE hFind = ::FindFirstFile(path, &findData);
    QkString foundfullpath;
    //TCHAR buffer[100]={0};
    //wsprintf(buffer,TEXT("!!!  FindFirstFile   =%s"), path);
    //::MessageBox(NULL, buffer, TEXT(""), MB_OK);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        BOOL findSucessful = true;
        while(findSucessful)
        {
            foundfullpath = folderpath+findData.cFileName;
            if (_stateSize && (findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0)
            {
                _totalSize += (findData.nFileSizeHigh * (MAXDWORD+1)) + findData.nFileSizeLow;
            }
            if (!PathMatch(_exludes, foundfullpath, findData.cFileName) && (!_recorder || _recorder->find(foundfullpath)==_recorder->end()))
            {
                if (findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) 
                {
                    //TCHAR buffer[100]={0};
                    //wsprintf(buffer,TEXT("!!!  FindFirstFile   =%s =%d=%d"), foundfullpath.GetData(), foundfullpath.EndWith(L"夹"), foundfullpath.StartWith(L"新建"));
                    //::MessageBox(NULL, buffer, TEXT(""), MB_OK);
                    // 简单处理 ： 跳过..结尾的搜寻结果。
                    if (Recursive && !foundfullpath.EndWith(L"."))
                        SearchFiles(foundfullpath, results, resultInfos, Recursive);
                }
                else if (PathMatch(_patterns, foundfullpath, findData.cFileName))
                {
                    if (results)
                    {
                        results->push_back(foundfullpath);
                    }
                    if(_recordNewlyReceived && _recorder) 
                    {
                        _recorder->insert(foundfullpath);
                    }
                    if (resultInfos)
                    {
                        ::PathRemoveFileSpec((LPWSTR)foundfullpath.GetData()); //todo
                        foundfullpath.RecalcSize();
                        FindDataIntoFindInfo(fileInfo, findData, foundfullpath);
                        resultInfos->push_back(fileInfo);
                    }
                    if (_stateSize)
                    {
                        _totalFilteredSize += (findData.nFileSizeHigh * (MAXDWORD+1)) + findData.nFileSizeLow;
                    }
                }
            }
            findSucessful = ::FindNextFile(hFind, &findData);
        }
        ::FindClose(hFind);
    }
}

void PathFinder::FindDataIntoFindInfo(FileInfo& fileInfo, WIN32_FIND_DATA& findData, QkString& fullpath)
{
    if (_parents)
    {
        //fileInfo.parnet = _parents->find(fullpath);
        //if (fileInfo.parnet == _parents->end())
        //{
        //    fileInfo.parnet = _parents->insert(fullpath).first;
        //}
        //else Logger::WriteMessage(L"matched!!!");

        auto debug = _parents->find(fullpath);
        if (debug == _parents->end())
        {
            debug = _parents->insert(fullpath).first;
        }
        fileInfo.parnet = debug; // in debug mode the time*=10 after assignment (三万文件，1s vs 10s)
         
        // 较慢
        //auto debug = _parents_debug.find(fullpath.GetData());
        //if (debug == _parents_debug.end())
        //{
        //    debug = _parents_debug.insert(fullpath.GetData()).first;
        //}
        //fileInfo.parnet_debug = debug;

        fileInfo.path = findData.cFileName;
    }
    else
    {
        fileInfo.parnet._Ptr = NULL;
        fullpath += L"\\";
        fullpath += findData.cFileName;
        fileInfo.path = fullpath;
    }
    fileInfo.dwFileAttributes = findData.dwFileAttributes;
    fileInfo.ftCreationTime = findData.ftCreationTime;
    fileInfo.ftLastAccessTime = findData.ftLastAccessTime;
    fileInfo.ftLastWriteTime = findData.ftLastWriteTime;
}