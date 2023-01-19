#include <InputServerFilter.h>
#include <Message.h>
#include <Messenger.h>
#include <String.h>
#include <Window.h>

class ScrollInputFilter : public BInputServerFilter
{
public:
							ScrollInputFilter();
	virtual 				~ScrollInputFilter();
    virtual status_t 		InitCheck();
    virtual filter_result 	Filter(BMessage *message, BList *outList);
private:
	bool					fInitScrolling;
	BPoint					fCurrentMousePosition;
	
#ifdef DCLOG
	void 					SendDebugString(BString& str);
#endif

};

ScrollInputFilter::ScrollInputFilter()
{
#ifdef DCLOG
	BString log("ScrollInputFilter()");
	SendDebugString(log);
#endif

	fInitScrolling = false;
}

ScrollInputFilter::~ScrollInputFilter()
{
}

status_t ScrollInputFilter::InitCheck()
{
	
#ifdef DCLOG
	BString log("InitCheck()");
	SendDebugString(log);
#endif

    return B_OK;
}

filter_result 
ScrollInputFilter::Filter(BMessage *message, BList *outList)
{
	BString log;
	if (message->what==B_MOUSE_DOWN) {

#ifdef DCLOG
		log = "B_MOUSE_DOWN: start";
		SendDebugString(log);
#endif

		fInitScrolling = false;
		int32 buttons;
		if (message->FindInt32("buttons", &buttons) != B_OK)
			return B_DISPATCH_MESSAGE;

		if (buttons != B_PRIMARY_MOUSE_BUTTON)
			return B_DISPATCH_MESSAGE;

		if (message->FindPoint("where", &fCurrentMousePosition) != B_OK)
			return B_DISPATCH_MESSAGE;

#ifdef DCLOG
		log <<  "current_mouse_position (" << current_mouse_position.x << "," 
			<< current_mouse_position.y << ")";
		SendDebugString(log);
#endif

		int32 modifiers;
		if (message->FindInt32("modifiers", &modifiers) != B_OK)
			return B_DISPATCH_MESSAGE;

		if (!(modifiers & (B_COMMAND_KEY | B_SHIFT_KEY)))
			return B_DISPATCH_MESSAGE;

		fInitScrolling = true;
		
#ifdef DCLOG
		log = "B_MOUSE_DOWN: init_scrolling = true";
		SendDebugString(log);
#endif
		
		return B_SKIP_MESSAGE;
	}
	if (message->what==B_MOUSE_UP) {
		
		fInitScrolling = false;
		
#ifdef DCLOG
		log = "B_MOUSE_UP: start_scrolling_combo = false";
		SendDebugString(log);
#endif
		
		return B_DISPATCH_MESSAGE;
	}
	
	if (message->what==B_MOUSE_MOVED && fInitScrolling==true) {
		
#ifdef DCLOG
		log = "B_MOUSE_MOVED: scrolling simulation initiated";
		SendDebugString(log);
#endif	
		
		BPoint newMousePosition;
		float deltay;
		if (message->FindPoint("where", &newMousePosition) != B_OK)
			return B_DISPATCH_MESSAGE;
			
		deltay = newMousePosition.y - fCurrentMousePosition.y;
		
#ifdef DCLOG
		log = "B_MOUSE_MOVED: ";
		log << "fCurrentMousePosition (" << fCurrentMousePosition.y << " - "	
			<< "newMousePosition (" << newMousePosition.y;
		log << " - FindPoint(where): " << deltay;
		SendDebugString(log);
#endif
		
		// TODO: adding the message to outList fires a kernel panic
		// investigate further and do it properly
		// BMessage wheel_message(B_MOUSE_WHEEL_CHANGED);
		// wheel_message.AddFloat("be:wheel_delta_y",deltay);
		// outList->AddItem(&wheel_message);
		message->what = B_MOUSE_WHEEL_CHANGED;
		message->AddFloat("be:wheel_delta_y",deltay);

#ifdef DCLOG
		log = "B_MOUSE_MOVED: outList->AddItem(&wheel_message)";
		SendDebugString(log);
#endif
		
		return B_DISPATCH_MESSAGE;
	}
	return B_DISPATCH_MESSAGE;
}

#ifdef DCLOG
void
ScrollInputFilter::SendDebugString(BString& str) {
	BMessenger messenger("application/x-vnd.Genio-DebugConsole");
	BMessage message('dbug');
	message.AddString("dbug_text", str);
	message.AddString("dbug_source", "scroll_ifilter");
	if (messenger.IsValid()) 
		messenger.SendMessage(&message);
}
#endif

extern "C" BInputServerFilter* instantiate_input_filter() {
	return new(std::nothrow) ScrollInputFilter();
}
