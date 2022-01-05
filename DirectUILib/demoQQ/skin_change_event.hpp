#ifndef SKIN_CHANGE_EVENT_HPP
#define SKIN_CHANGE_EVENT_HPP

struct SkinChangedParam
{
	DWORD bkcolor;
	QkString bgimage;
};

typedef class ObserverImpl<BOOL, SkinChangedParam> SkinChangedObserver;
typedef class ReceiverImpl<BOOL, SkinChangedParam> SkinChangedReceiver;


#endif // SKIN_CHANGE_EVENT_HPP