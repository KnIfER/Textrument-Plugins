



#if 0
class MyWindow : public sk_app::Window
{
public:
    MyWindow():sk_app::Window(){};

    ~MyWindow(){};

    virtual void setTitle(const char*) 
    {

    }


    virtual void show()
    {


    }


    // Schedules an invalidation event for window if one is not currently pending.
    // Make sure that either onPaint or markInvalReceived is called when the client window consumes
    // the the inval event. They unset fIsContentInvalided which allow future onInval.
    void inval()
    {

    }


    virtual void onInval()
    {

    }

    virtual bool attach(BackendType)
    {

    }

};

//MyWindow* win = new MyWindow();


#endif


