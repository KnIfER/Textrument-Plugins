#include "DuiLibTranslator.h"

namespace DuiLib {

    bool TranslateUI(CPaintManagerUI & m_pm, std::map<std::string, std::string> & localizefile) {
        //if(1) return false;
        if(localizefile.size()==0) return false;
        CControlUI * root = m_pm.GetRoot();
        CContainerUI * thisVG = dynamic_cast<CContainerUI*>(root);
        std::vector<CContainerUI *> vgs;
        TCHAR text[MAX_PATH];
        char keyTmp[MAX_PATH/2];
        keyTmp[0] = '_';
        if(thisVG){
            // traverse
            vgs.push_back(thisVG);
            while(vgs.size()) {
                thisVG = vgs[vgs.size()-1];
                vgs.pop_back();
                int cc = thisVG->GetCount();
                for(int i=0;i<cc;i++)
                {
                    CControlUI * item = thisVG->GetItemAt(i);
                    auto & name = item->GetName();
                    if(!name.IsEmpty()&&!item->GetText().IsEmpty())
                    {
                        //item->SetText(name);
                        int len = WideCharToMultiByte(CP_ACP, 0, name.GetData(), name.GetLength()
                            , keyTmp+1, MAX_PATH/2-2, 0, 0);
                        keyTmp[len+1]='\0';
                        auto idx = localizefile.find(keyTmp);
                        if(idx!=localizefile.end())
                        {
                            auto & value =  (*idx).second;
                            int len = MultiByteToWideChar(CP_ACP, 0, value.c_str(), value.size(), text, MAX_PATH-1);
                            text[len]='\0';
                            item->SetText(text);
                        }
                        if (item->m_sUserDataTally)
                        {
                            idx = localizefile.find(item->m_sUserDataTally);
                            if(idx!=localizefile.end())
                            {
                                auto & value =  (*idx).second;
                                int len = MultiByteToWideChar(CP_ACP, 0, value.c_str(), value.size(), text, MAX_PATH-1);
                                text[len]='\0';
                                item->SetUserData(text);
                            }
                        }
                    }
                    CContainerUI * vgTest = dynamic_cast<CContainerUI*>(item);
                    if(vgTest)
                    {
                        vgs.push_back(vgTest);
                    }
                }
            }
        }
        return true;
    }
}