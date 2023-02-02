/*
* Android-Like ListView Demo By KnIFER
* 
*/
#include "stdafx.h"


using namespace DuiLib;

class ListMainForm : public WindowImplBase, public INotifyUI
{
public:
    ListMainForm() { };     

    LPCTSTR GetWindowClassName() const override
    { 
        return _T("ListMainForm"); 
    }

    UINT GetClassStyle() const override
    { 
        return CS_DBLCLKS; 
    }

    void OnFinalMessage(HWND hWnd) override
    { 
        __super::OnFinalMessage(hWnd);
        delete this;
    }

    LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) override
    {
        PostQuitMessage(0);
        bHandled = TRUE;
        return 0;
    }

    void InitWindow() override
    {
        m_pCloseBtn = static_cast<Button*>(m_pm.FindControl(_T("closebtn")));
        m_pMaxBtn = static_cast<Button*>(m_pm.FindControl(_T("maxbtn")));
        m_pRestoreBtn = static_cast<Button*>(m_pm.FindControl(_T("restorebtn")));
        m_pMinBtn = static_cast<Button*>(m_pm.FindControl(_T("minbtn")));
        m_pSearch = static_cast<Button*>(m_pm.FindControl(_T("btnSearch")));
        if (m_pSearch)
        {
            m_pSearch->Activate();
        }
    }

    class ListViewBaseAdapter : public ListViewAdapter
    {
        size_t GetItemCount()
        {
            return 100;
        }
        CControlUI* CreateItemView()
        {
            Button* btn = new Button;
            btn->SetText(L"Test");
            btn->SetTextColor(0xff0000ff);
            return btn;

        }
        void OnBindItemView(CControlUI* view, size_t index)
        {
            Button* btn = dynamic_cast<Button*>(view);

            if (btn)
            {
                QkString label;
                label.Format(L"#%d", index);
                btn->SetText(label);
            }
        }
    };

    QkString GetSkinFile() override
    {
        return _T("testScroll.xml");
        //return _T("skin\\ListRes\\Main.xml");
        //return _T("VSMake.xml");
        //return _T("settings.xml");
    }

    void Notify( TNotifyUI &msg ) override
    {
        //LogIs(L"Notify %s %s", (LPCWSTR)msg.sType, msg.pSender->GetClass());
        if (msg.sType==L"setfocus")
        {
            if(lstrcmp(msg.pSender->GetClass(), L"InputBox")==0) {
                msg.pSender->GetParent()->SetBorderEnhanced(true);
                msg.pSender->GetParent()->SetBorderColor(0xff039ed3);
            }
        }
        if (msg.sType==L"killfocus")
        {
            if(lstrcmp(msg.pSender->GetClass(), L"InputBox")==0) {
                msg.pSender->GetParent()->SetBorderEnhanced(false);
                msg.pSender->GetParent()->SetBorderColor(0x9f454545);
            }
        }
        if (msg.sType==L"click")
        {
            m_pm.SetDPI(125);
            if( msg.pSender == m_pCloseBtn ) 
            { 
                PostQuitMessage(0);
                return; 
            }
            else if( msg.pSender == m_pMaxBtn ) 
            { 
                SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); return; 
            }
            else if( msg.pSender == m_pRestoreBtn ) 
            { 
                SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); return; 
            }
            else if(msg.pSender == m_pSearch)
            {
                ListView* pList = static_cast<ListView*>(m_pm.FindControl(_T("vList")));
                if (pList)
                {
                    m_pSearch->SetTextColor(0xff0000ff);

                    //TCHAR buffer[100]={0};
                    //wsprintf(buffer,TEXT("position=%s"), pList->GetClass());
                    //::MessageBox(NULL, buffer, TEXT(""), MB_OK);

                    ListViewAdapter* ada = new ListViewBaseAdapter();

                    Button* refer = new Button;

                    refer->SetFixedWidth(-1);
                    refer->SetFixedHeight(50);

                    pList->SetReferenceItemView(refer);

                    pList->SetAdapter(ada);
                }
            }
            else if( msg.sType == _T("itemclick") ) 
            {
            }
        }
        // WindowImplBase::Notify(msg);
    }

private:
    Button* m_pCloseBtn;
    Button* m_pMaxBtn;
    Button* m_pRestoreBtn;
    Button* m_pMinBtn;
    Button* m_pSearch;
};


#include <fstream>
#include <regex>
#include <set>

string& replace_str(string& str, const string& to_replaced, const string& newchars)
{
    for(string::size_type pos(0); pos != string::npos; pos += newchars.length())
    {
        pos = str.find(to_replaced,pos);
        if(pos!=string::npos)
            str.replace(pos,to_replaced.length(),newchars);
        else
            break;
    }
    return str;
}

//QkString& replace_str(QkString& str, const QkString& to_replaced, const QkString& newchars)
//{
//    for(int pos(0); pos != -1; pos += newchars.length())
//    {
//        pos = str.Find(to_replaced, pos);
//        if(pos!=-1)
//            str.Replace(pos,to_replaced.length(),newchars);
//        else
//            break;
//    }
//    return str;
//}

std::string& trim_str(std::string &s)   
{  
    if (s.empty())   
    {  
        return s;  
    }  

    s.erase(0,s.find_first_not_of(" "));  
    s.erase(s.find_last_not_of(" ") + 1);  
    return s;  
}

void split_str(std::string& s, const std::string& delim,std::vector< std::string >* ret)  
{  
    size_t last = 0;  
    size_t index=s.find_first_of(delim,last);  
    while (index!=std::string::npos)  
    {  
        ret->push_back(s.substr(last,index-last));  
        last=index+1;  
        index=s.find_first_of(delim,last);  
    }  
    if (index-last>0)  
    {  
        ret->push_back(s.substr(last,index-last));  
    }  
}

PathFinder _pathFinder;
vector<QkString> _paths;
set<QkString, strPtrCmp> _recorder;
set<QkString, strPtrCmp> _parents;
bool _recursive = true;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
    int buildType = 0;
    //////////////////////////////////////

    QkString SolutionPath = "D:\\Godot\\godot\\godot_build.sln";

    QkString ProjectSaveFolder = L"D:\\Godot\\godot\\thirdparty\\";

    QkString ProjectName = L"zlib";

    //////////////////////////////////////
    
    //////////////////////////////////////
    SolutionPath = "D:\\Code\\FigureOut\\magnifier-master\\magnifier.sln";
    ProjectSaveFolder = L"D:\\Code\\FigureOut\\magnifier-master\\";
    ProjectName = L"magnifier";
    buildType = 2;
    //////////////////////////////////////

    int prefixLength = ProjectSaveFolder.GetLength();
    QkString ProjectPath = ProjectSaveFolder+ProjectName+L"."+L"vcxproj";

    std::string SrcFilterRoot;

    ProjectSaveFolder.GetData(SrcFilterRoot);

    // path, filepath, !path, !filepath. could have wildcards in the name section
    QkString SrcFolders = //L"D:\\Godot\\godot\\thirdparty\\zlib"
        L"D:\\Code\\FigureOut\\magnifier-master"
        ;

    vector<QkString> folders;
    SrcFolders.Split(L"\n", folders);

    auto & Patterns = _pathFinder.GetPatterns();
    auto & exludes = _pathFinder.GetExcludePatterns();
    auto & Pathbuffer = _pathFinder.GetPathBufferW();
    auto & pathbuffer = _pathFinder.GetPathBuffer();

    for (int i = folders.size()-1; i >= 0; i--)
    {
        QkString & rule = folders[i];
        bool b2=rule[0]=='!' && rule.GetLength()>2;
        if (!rule.GetLength() || b2)
        {
            folders.erase(folders.begin()+i);
            if (b2)
            {
                exludes.push_back(rule.Mid(1));
            }
        }
    }

    if (true)
    {
        exludes.push_back(L".git");
        exludes.push_back(L".vs");
        exludes.push_back(L"Debug*");
        exludes.push_back(L"Release*");
        exludes.push_back(L"build");
        exludes.push_back(L"bin");
    }

    Patterns.push_back(L"*.cpp");
    Patterns.push_back(L"*.c");
    Patterns.push_back(L"*.h");

    _pathFinder.SetRecorder(_recorder);

    {
        ifstream ifs("D:\\Code\\FigureOut\\Textrument\\plugins\\DirectUILib\\bin\\VSMakeTemplate.vcxproj");
        string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        tinyxml2::XMLDocument doc;
        bool is_long_path = ProjectPath.GetLength()>256;
        is_long_path = 1;
        if (is_long_path)
            ProjectPath.Prepend(L"\\\\?\\");
        bool loadPresetTemplate = true;
        if (PathFileExists(ProjectPath))
        {
            auto error = doc.LoadFile(ProjectPath.GetData(pathbuffer));

            if (!error)
            {
                loadPresetTemplate = false;
            }
        }
        if(loadPresetTemplate)
        {
            replace_str(str, "__Application"
                , buildType==0?"StaticLibrary":buildType==1?"DynamicLibrary":"Application");

            doc.Parse(str.c_str(), str.length());
        }

        tinyxml2::XMLElement* project_node = doc.FirstChildElement("Project");
        tinyxml2::XMLElement* insert_node = NULL;
        tinyxml2::XMLElement* source_node = NULL;
        tinyxml2::XMLElement* include_node = NULL;
        if (!project_node)
        {
            project_node = doc.NewElement("Project");
            doc.InsertEndChild(project_node);
        }
        if (!loadPresetTemplate)
        {
            // 寻找已经存在的源码，避免重复加入。
            auto group_node = project_node->FirstChildElement("ItemGroup");
            QkString file;
            short search = 0x3;
            while (group_node)
            {
                auto cv = group_node->FirstChildElement();
                while (cv)
                {
                    bool b1 = strcmp(cv->Name(), "ClCompile")==0;
                    bool b2 = strcmp(cv->Name(), "ClInclude")==0;
                    if(b1 || b2)
                    {
                        const char* path = cv->Attribute("Include");
                        if (path)
                        {
                            file = path;
                            if (file.Find(L":")==-1)
                                file.Prepend(ProjectSaveFolder, prefixLength);
                            // 规整化路径
                            if (Pathbuffer.EnsureCapacity(2*file.GetLength()))
                            {
                                TCHAR* data = (TCHAR*)Pathbuffer.GetData();
                                PathCanonicalize(data, file);
                                Pathbuffer.RecalcSize();
                                _recorder.insert(Pathbuffer);
                            }
                            else
                            {
                                _recorder.insert(file);
                            }
                        }
                        if (search)
                        {
                            if (search&0x1 && b1)
                            {
                                source_node = group_node;
                                search &= ~0x1;
                            }
                            if (search&0x2 && b2)
                            {
                                include_node = group_node;
                                search &= ~0x2;
                            }
                        }
                    }
                    cv = cv->NextSiblingElement();
                }
                group_node = group_node->NextSiblingElement("ItemGroup");
            }
        }
        if (!source_node)
        {
            source_node = doc.NewElement("ItemGroup");
            project_node->InsertEndChild(source_node);
        }
        if (!include_node)
        {
            include_node = doc.NewElement("ItemGroup");
            project_node->InsertEndChild(include_node);
        }
        if (is_long_path)
            ProjectPath = ProjectPath.Mid(4);

        for (size_t i = 0; i < folders.size(); i++)
        {
            _pathFinder.SearchFiles(folders[i], &_paths, NULL, _recursive);
        }

        tinyxml2::XMLElement* node;
        for (size_t i = 0; i < _paths.size(); i++)
        {
            if (_paths[i].EndWith(L".h", true))
            {
                insert_node = include_node;
                node = include_node->InsertNewChildElement("ClInclude");
            }
            else
            {
                insert_node = source_node;
                node = source_node->InsertNewChildElement("ClCompile");
            }
            Pathbuffer.EnsureCapacity((std::max)(MAX_PATH, (int)_paths[i].GetLength()*2));
            TCHAR* data = (TCHAR*)Pathbuffer.GetData();
            PathRelativePathTo(data, ProjectSaveFolder, 0, _paths[i], 0);
            Pathbuffer.RecalcSize();
            const char* result = Pathbuffer.GetData(pathbuffer);
            node->SetAttribute("Include", result);
        }
        if (!source_node->FirstChildElement())
            project_node->DeleteChild(source_node);
        if (!include_node->FirstChildElement())
            project_node->DeleteChild(include_node);
        //doc.SaveFile(ProjectPath.GetData(pathbuffer), false);
    }



    //Project("{}") = "zlib", "zlib.vcxproj", "{}"


    // 构造解决方案

    {
        ifstream ifs("D:\\Code\\FigureOut\\Textrument\\plugins\\DirectUILib\\bin\\VSMakeTemplate.sln");

        string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

        size_t pos = 0;
        size_t index;
        while((index = str.find("EndProject", pos+10))!=string::npos)
        {
            pos = index;
        }

        // find existing projects

        // 简单处理：禁止加入同名项目
        set<string> projNameSet;

        // 正则搜索
        const regex pattern("Project.*?\"(.*?)\",.*?\"(.*?)\"");
        smatch result;
        string::const_iterator start = str.begin();
        string::const_iterator end = start+pos;
        string temp;
        string vcxporj;
        int projects = 0;
        while (regex_search(start, end, result, pattern))
        {
            start = result[0].second;
            //temp+=vcxporj+" ";
            vcxporj = result[2];
            trim_str(vcxporj);
            replace_str(vcxporj, ".vcxporj", "");
            replace_str(vcxporj, ".vcporj", "");
            projNameSet.insert(vcxporj);
            projects++;
        }
        if (projects==0)
        {
            str.erase(pos, 10);
        }

        //  在字符串 pos 之后插入新的项目
        Pathbuffer.EnsureCapacity((std::max)(MAX_PATH, (int)ProjectPath.GetLength()*2));
        TCHAR* data = (TCHAR*)Pathbuffer.GetData();
        PathRelativePathTo(data, SolutionPath, 0, ProjectPath, 0);
        Pathbuffer.RecalcSize();

        QkString tmp;
        tmp.Format(L"Project(\"{}\") = \"%s\", \"%s\", \"{}\"\r\nEndProject\r\n", (LPCTSTR)ProjectName, (LPCTSTR)Pathbuffer);
        str.insert(pos,  tmp.GetData(temp));

        // 保存解决方案
       // ofstream write;
       // write.open(SolutionPath.GetData(temp), ios::out | ios::binary );
       // write.write(str.c_str(), str.size());
       // write.flush();
       // write.close();

    }

    //if(1) return 1;



    REGIST_QKCONTROL(SciEditText);

    CPaintManagerUI::SetInstance(hInstance);
    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());

    ListMainForm* pFrame = new ListMainForm;



    if( pFrame == NULL ) return 0;
    
    pFrame->Create(NULL, _T("ListDemo"), UI_WNDSTYLE_FRAME, WS_EX_APPWINDOW , 0, 0, 800, 600);
    
    pFrame->CenterWindow();
    
    pFrame->ShowModal();
    
    
    CPaintManagerUI::MessageLoop();



    return 0;
}