#include "StdAfx.h"
extern int tickSetAttr;
namespace Tests
{
	const std::int64_t num_keys = 10000;
	const std::int64_t value = 1;

#define INSERT_INT_INTO_HASH(key, value) hash.Insert(key, value)

	TEST_CLASS(Tests)
	{
	public: 
		CPaintManagerUI _manager;
		CDialogBuilder _builder;
		bool Initialized = false;
		vector<QkString> xmls;
		PathFinder _pathFinder;
		vector<QkString> FilePaths;
		set<QkString, strPtrCmp> FileRecords;
		set<QkString, strPtrCmp> ParentFilePaths;
		vector<FileInfo> FileInfos;
		void Init(bool readxmls=false)
		{
			if (!Initialized)
			{
				Initialized = true;
				// 不用手动调用
				//CControlFactory::GetInstance(); 
				// 指定测试模块
				CPaintManagerUI::SetInstance(::GetModuleHandle(L"Tests.dll"));
				// 使用相对于测试模块的资源路径，
				//Logger::WriteMessage(CPaintManagerUI::GetInstancePath());
				// 也可以是绝对路径。
				CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath()+L"..\\..\\bin");
			}
			FilePaths.empty();
			ParentFilePaths.empty();
			// 扫描xml文件
			if (readxmls)
			{
				if (!xmls.size())
				{
					_pathFinder.StatTotalSize(true);
					_pathFinder.SetRecorder(FileRecords);
					_pathFinder.GetPatterns().push_back(L"*.xml");
					_pathFinder.SearchFiles(CPaintManagerUI::GetResourcePath(), &FilePaths, NULL);
				}
				Assert::IsTrue(FilePaths.size()>0);
			}
		}

		TEST_METHOD(测试大批量扫描文件)
		{
			measurements m;

			_pathFinder.GetPatterns().push_back(L"*.*");
			_pathFinder.GetExcludePatterns().push_back(L".git");
			_pathFinder.GetExcludePatterns().push_back(L".vs");
			_pathFinder.StatTotalSize(true);

			_pathFinder.SetParentRecorder(ParentFilePaths);

			//_pathFinder.SearchFiles("D:\\Code\\FigureOut\\Textrument\\plugins\\DirectUILib\\bin\\TestSkimGui.exe", &FilePaths, &FileInfos);
			_pathFinder.SearchFiles("D:\\Code\\FigureOut\\Textrument\\秘籍", &FilePaths, &FileInfos);
			
			if (false)
			{
				for (size_t i = 0; i < FilePaths.size(); i++)
				{
					//Logger::WriteMessage(FilePaths[i]);
				}
				QkString path;
				for (size_t i = 0; i < FileInfos.size(); i++)
				{
					if (FileInfos[i].parnet._Ptr)
					{
						path = *FileInfos[i].parnet;
						path += L"\\";
						path += FileInfos[i].path;
						//Logger::WriteMessage(path);
					}
					else
					{
						//Logger::WriteMessage(FileInfos[i].path);
					}
				}
			}
			_buffer.Format(L"总计 %d 个文件, 大小 %.2f MB / %.2f MB ", FilePaths.size(), _pathFinder.GetFilteredSizeMb()/1000, _pathFinder.GetSearchedSizeMb()/1000);
			Logger::WriteMessage(_buffer);
		}

		TEST_METHOD(LayoutInf_1)
		{
			Init();
			CControlUI* root = _builder.Create(L"TextrumentDemo.xml", NULL, NULL, &_manager);

			Logger::WriteMessage(_builder.GetLastErrorMessage());
			Logger::WriteMessage(_builder.GetLastErrorLocation());

			_buffer.Format(L"root=%d", root);
			Logger::WriteMessage(_buffer);

			Assert::IsTrue(root!=NULL);
		}

		TEST_METHOD(LayoutInf_扫描xml文件)
		{
			Init(true);
			measurements m;
			for (size_t i = 0; i < FilePaths.size(); i++)
			{
				Logger::WriteMessage(FilePaths[i]);
			}
			_buffer.Format(L"总计 %d 个文件", FilePaths.size());
			Logger::WriteMessage(_buffer);
		}

		TEST_METHOD(LayoutInf_性能测试)
		{
			Init(true);
			int cc=0;
			size_t i = 0, length=FilePaths.size();
			measurements m;
			m.SetTaskCnt(length);
			for (; i < length; i++)
			{
				CPaintManagerUI manager;
				CControlUI* root = _builder.Create(FilePaths[i].GetData(), NULL, NULL, &manager);
				if (!root)
				{
					Logger::WriteMessage(FilePaths[i]);
					Logger::WriteMessage(_builder.GetLastErrorMessage());
					Logger::WriteMessage(_builder.GetLastErrorLocation());
				}
				else 
				{
					delete root;
					cc++;
				}
			}
			_buffer.Format(L"总计 %d 个文件, 成功创建 %d 个布局, 总文件大小 %.3f MB ", length, cc, _pathFinder.GetFilteredSizeMb());
			Logger::WriteMessage(_buffer);
			_buffer.Format(L"tickSetAttr = %d ", tickSetAttr);
			Logger::WriteMessage(_buffer);
		}

		LPCTSTR STR2ARGBMy(LPCTSTR STR, DWORD & ARGB)
		{
			bool intOpened=false;
			int index=0;
			while(index<16) {
				const char & intVal = *(STR++);
				//if (intVal>'\0' && intVal<=' ') continue;
				if (intVal>='0'&&intVal<='9')
				{
					ARGB = ARGB*16+(intVal - '0');
				}
				else if (intVal>='A' && intVal<='F')
				{
					ARGB = ARGB*16+(10 + intVal - 'A');
				}
				else if (intVal>='a' && intVal<='f')
				{
					ARGB = ARGB*16+(10 + intVal - 'a');
				}
				else if (intOpened || intVal=='\0')
				{
					break;
				}
				if(!intOpened && intVal)
				{
					intOpened=true;
				}
				index++;
			}
			if (index==3)
			{
				ARGB = (ARGB&0xF)<<8 | (ARGB&0xF0)<<16 | (ARGB&0xF00)<<24;
			}
			//DWORD val = _tcstoul(STR, 0, 16);
			//assert(ARGB==val);

			//ARGB = _tcstoul(STR, 0, 16);

			return STR;
		}
		
		#define testBits 1000000

		TEST_METHOD(LayoutInf_16Bit_tcstoul)
		{
			LPCTSTR text = L"FF12abCF";
			DWORD ret = 0;
			measurements m;
			m.SetTaskCnt(testBits);
			for (int i=0; i < testBits; i++)
			{
				ret = _tcstoul(text, 0, 16);
			}
			_buffer.Format(L"转换_tcstoul");
			Logger::WriteMessage(_buffer);
		}

		TEST_METHOD(LayoutInf_16Bit_STR2ARGB)
		{
			QkString text = L"#FF12abCF";
			measurements m;
			m.SetTaskCnt(testBits);
			for (int i=0; i < testBits; i++)
			{
				DWORD ret = 0;
				STR2ARGB((LPCTSTR)text+1, ret);
			}
			_buffer.Format(L"转换_STR2ARGB");
			Logger::WriteMessage(_buffer);
		}

		TEST_METHOD(LayoutInf_10Bit_tcstol)
		{
			LPCTSTR text = L"-123456";
			DWORD ret = 0;
			measurements m;
			m.SetTaskCnt(testBits);
			for (int i=0; i < testBits; i++)
			{
				ret = _tcstol(text, 0, 10);
			}
			_buffer.Format(L"转换_10Bit_tcstol");
			Logger::WriteMessage(_buffer);
		}

		TEST_METHOD(LayoutInf_10Bit_STR2Decimal)
		{
			QkString text = L"-123456";
			measurements m;
			m.SetTaskCnt(testBits);
			for (int i=0; i < testBits; i++)
			{
				long ret = 0;
				STR2Decimal((LPCTSTR)text, ret);
			}
			_buffer.Format(L"转换_10Bit_STR2Decimal");
			Logger::WriteMessage(_buffer);
		}

		TEST_METHOD(LayoutInf_10Bit_STR2Decimal_1)
		{
			QkString text = L"4";
			measurements m;
			m.SetTaskCnt(testBits);
			for (int i=0; i < testBits; i++)
			{
				long ret = 0;
				STR2Decimal((LPCTSTR)text, ret);
			}
			_buffer.Format(L"转换_10Bit_STR2Decimal");
			Logger::WriteMessage(_buffer);
		}

		TEST_METHOD(LayoutInf_16Bit_STR2ARGB_1)
		{
			measurements m;
			DWORD ret = 0;
			STR2ARGB(L"#FFF", ret);

		}
		



	};


}
