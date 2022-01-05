#pragma once

using namespace DuiLib;

typedef struct _FileInfo 
{
    QkString path;
    size_t length;
    set<QkString>::iterator parnet;
    set<wstring>::iterator parnet_debug;
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
} FileInfo;

typedef struct
{
    bool operator()( const QkString & s1, const QkString & s2 ) const
    {
        return s1.CompareNoCase( s2 ) < 0;
    }
} strPtrCmp;

class PathFinder
{
public:
    PathFinder();
    ~PathFinder();
    bool PathMatch(const vector<QkString> & Patterns, const QkString & path, LPCTSTR name=0);
    void SearchFiles(QkString path, vector<QkString> * results, vector<FileInfo> * resultInfos=NULL, const bool Recursive=true);

    vector<QkString> & GetPatterns(){
        return _patterns;
    };
    vector<QkString> & GetExcludePatterns(){
        return _exludes;
    };
    // To See Total File Size.
    void StatTotalSize(bool val) {
        if (_stateSize = val) 
            _totalSize = _totalFilteredSize = 0;
    };
    // To Reduce Path Buffer Size ( use relative path ).
    void SetParentRecorder(set<QkString, strPtrCmp> & parents) {
        _parents = &parents;
    };
    // To Avoid Duplication.
    void SetRecorder(set<QkString, strPtrCmp> & recorder, bool recordNewlyReceived=false) {
        _recorder = &recorder;
        _recordNewlyReceived = recordNewlyReceived;
    };
    double GetFilteredSizeMb() {
        return _totalFilteredSize / 1024.f / 1024.f;
    };
    double GetSearchedSizeMb() {
        return _totalSize / 1024.f / 1024.f;
    };
    string & GetPathBuffer() {
        return pathbuffer;
    };
    QkString & GetPathBufferW() {
        return Pathbuffer;
    };
protected:
    void FindDataIntoFindInfo(FileInfo & fileInfo, WIN32_FIND_DATA & findData, QkString & fullpath);

    BOOL _recursive;
    vector<QkString> _patterns;
    vector<QkString> _exludes;
    set<QkString, strPtrCmp> * _parents;
    set<wstring> _parents_debug;

    set<QkString, strPtrCmp> * _recorder;
    bool _recordNewlyReceived;
    bool _stateSize;

    long long _totalSize;
    long long _totalFilteredSize;

    string pathbuffer;
    QkString Pathbuffer;
};
