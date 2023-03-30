#include "StdAfx.h"

bool showYou=false;

namespace Tests
{
	TEST_CLASS(Tests)
	{
		//QkString _buffer;
		//Tests(){
		//	Logger::WriteMessage(L"Tests！"); 
		//}
	public:

		class FooPar
		{
		public:
			FooPar(){
				Logger::WriteMessage(L"FooPar！"); 
			};
		};

		class FooPtr : public FooPar
		{
		public:
			FooPtr(){ Logger::WriteMessage(L"已经删除！");  };
			~FooPtr()
			{
				Logger::WriteMessage(L"FooPtr！"); 
			}
			QkString _buffer;
			RECT rect;
		};

		TEST_METHOD(测试无类型指针)
		{
			showYou = true;

			FooPtr bar1;

			Logger::WriteMessage(L"已经删除???"); 
			FooPtr* bar = new FooPtr;
			LPVOID pBar = bar;
			//delete pBar; // 无法调用解构函数
			delete bar;

			RECT & rect = bar->rect;
			_buffer.Format(L"rect:: %ld %ld %ld %ld", rect.left, rect.top, rect.right, rect.bottom);
			Logger::WriteMessage(_buffer); 


		}

		void qstr_ref(const QkString & val)
		{
		}
		void qstr_vref(const QkString val)
		{
		}
		QkString& qstr_ref_ret(QkString & val)
		{
			return *(&val);
		}
		QkString qstr_vref_ret(QkString & val)
		{
			return *(&val);
		}
		QkString* qstr_ptr_ret(QkString & val)
		{
			return &val;
		}
#define test_count 1000000

		TEST_METHOD(TestMethod1)
		{
			Logger::WriteMessage("Test initialized.\n"); 

			auto tm = GetTickCount64();

			QkString test = L"123";

			for (size_t i = 0; i < test_count; i++)
			{
				qstr_ptr_ret(test);
			}

			test.Format(L"TestMethod1::时间:: %d", GetTickCount64()-tm);

			Logger::WriteMessage(test); 



			QkString projectName = L"zlib";
			QkString projectFilterPath = L"";
			QkString tmp;
			tmp.Format(L"Project(\"{}\") = \"%s\", \"%s\", \"{}\"\r\nEndProject\r\n", (LPCTSTR)projectName, (LPCTSTR)(projectFilterPath+projectName));

			Logger::WriteMessage(tmp);

		}

		TEST_METHOD(TestMethod12)
		{
			Logger::WriteMessage("Test initialized.\n"); 

			auto tm = GetTickCount64();

			TCHAR* test = L"123";
			QkString test1 = L"123";

			for (size_t i = 0; i < test_count; i++)
			{
				qstr_vref_ret(test1);
			}

			test1.Format(L"TestMethod1::时间:: %d", GetTickCount64()-tm);

			Logger::WriteMessage(test); 

		}

		TEST_METHOD(Test2)
		{
			Logger::WriteMessage("Test initialized.\n"); 

			auto tm = GetTickCount64();

			QkString test = L"123";
			//QkString test = L"CVerticalLayoutUI";
			//auto factory = CControlFactory::GetInstance();
			//factory->RegistControl(test, 0);
			//Assert::IsTrue(test==L"verticallayout");
			//Assert::IsTrue(test.GetLength()==14);

			Logger::WriteMessage(test); 

			for (size_t i = 0; i < test_count; i++)
			{
				qstr_ref_ret(test);
			}

			test.Format(L"TestMethod1::时间:: %d", GetTickCount64()-tm);

			Logger::WriteMessage(test); 

		}
		

		TEST_METHOD(TestPackWord)
		{
			INT x = -59;
			INT y = 2;

			LRESULT pack = MAKELONG((short)x, (short)y);

			_buffer.Format(L"TestPackWord::%d, %d", (short)LOWORD(pack), (short)HIWORD(pack));
			Logger::WriteMessage(_buffer); 

		}

		class Foo
		{
		public:
			QkString text;
		};

		TEST_METHOD(QkStringCpyClass)
		{
			const WCHAR* t1 = L"123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
			const WCHAR* t2 = L"456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123";
			Foo foo;
			Foo bar;
			foo.text = t1;
			bar = foo;

			Assert::IsTrue(foo.text==bar.text);
			Assert::IsTrue(foo.text==t1);

			bar.text = t2;

			Assert::IsTrue(foo.text==t1);
			Assert::IsTrue(bar.text==t2);
		}

		TEST_METHOD(SanmuYunSuan)
		{
			Assert::IsTrue(10==(false?30/2:20/2));
		}

		TEST_METHOD(Map)
		{
			QkStringPtrMap months;

			//months["january"] = 31;
			//months["february"] = 28;
			//months["march"] = 31;
			//months["april"] = 30;
			//months["may"] = 31;
			//months["june"] = 30;
			//months["july"] = 31;
			//months["august"] = 31;
			//months["september"] = 30;
			//months["october"] = 31;
			//months["november"] = 30;
			//months["december"] = 31;

		}

		TEST_METHOD(Numbers)
		{
			Assert::IsTrue(0xFF123456>0x00FFFFFF);
			DWORD dwNumber = 0xFF123456;
			Assert::IsTrue(dwNumber>0x00FFFFFF);
		}
		
#include <string.h>
#include <locale.h>

		TEST_METHOD(sort_strcoll)
		{
			setlocale(LC_COLLATE, "zh_CN.utf8"); // Set the locale to US English
			const char* s1 = "我";
			const char* s2 = "们";
			int result = strcoll(s1, s2);
			if (result < 0) {
				_buffer.Format(L"%s comes before %s\n", STRW(s1), STRW(s2));
				lpp(_buffer);
			} else if (result > 0) {
				_buffer.Format(L"%s comes after %s\n", STRW(s1), STRW(s2));
				lpp(_buffer);
			} else {
				_buffer.Format(L"%s equal %s\n", STRW(s1), STRW(s2));
				lpp(_buffer);
			}
		}


	};


}
