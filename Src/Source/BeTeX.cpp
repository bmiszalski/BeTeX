/*****************************************************************
 * Copyright (c) 2005 Tim de Jong, Brent Miszalski				 *
 *							       								 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
#ifndef BETEX_H
#include "BeTeX.h"
#endif

#include <be/storage/Mime.h>
#include <be/storage/FindDirectory.h>
#include <be/interface/Bitmap.h>
#include <be/support/String.h>
#include "TeXDocIcons.h"
#include "constants.h"
#include "Preferences.h"
#include "MessageFields.h"

BeTeX::BeTeX() 
		:	BApplication(APP_SIG)
{	
	//construct user settings dir
	find_directory(B_USER_SETTINGS_DIRECTORY,&m_prefsPath);
	m_prefsPath.Append("settings.betex");		
	//load app's preferences
	LoadPreferences(m_prefsPath.Path());
	
	BMimeType mime(TEX_FILETYPE);
	BMessage ex_msg;
	bool install = true;
	mime.GetFileExtensions(&ex_msg);
	const char* ext;
	if (ex_msg.FindString("extensions",&ext)==B_OK)
	{
		install = (strcmp(ext,"tex") != 0);
	}
			
	if (mime.InitCheck() == B_OK && install)// && !mime.IsInstalled())
	{
		BMessage msg;
		if(msg.AddString("extensions","tex") == B_OK)
		{
			mime.SetFileExtensions(&msg);
		}
		
		BBitmap* large_icon = new BBitmap(BRect(0,0,31,31),B_CMAP8);
		large_icon->SetBits(TeXIcon,3072,0,B_CMAP8);
		mime.SetIcon(large_icon,B_LARGE_ICON);
		
		BBitmap* small_icon = new BBitmap(BRect(0,0,15,15),B_CMAP8);
		small_icon->SetBits(SmallTeXIcon,768,0,B_CMAP8);
		mime.SetIcon(small_icon,B_MINI_ICON);
		
		mime.SetLongDescription("TeX/LaTeX Document");
		mime.SetShortDescription("TeX");
		
		mime.SetPreferredApp(APP_SIG);
		
		//Create some attributes......"
		BMessage attr;
		attr.AddString("attr:name","Author");
		attr.AddString("attr:public_name","Author");
		attr.AddInt32("attr:type",B_STRING_TYPE);
		attr.AddBool("attr:public",true);
		attr.AddBool("attr:editable",true);
		
		//Want more attributes for:
		// 1) Document status (draft, incomplete, complete) etc
		// 2) Document Priority (low, medium, high)
		// 3) Something else?
		
		/*attr.AddString("attr:name","Author");
		attr.AddString("attr:public_name","Author");
		attr.AddInt32("attr:type",B_STRING_TYPE);
		attr.AddBool("attr:public",true);
		attr.AddBool("attr:editable",true);
		*/		
		mime.Install();
	}
	//I could also add some attributes to the tex file
	//such as author, etc.....
	BRect frame;
	prefsLock.Lock();
	if (preferences.FindRect(K_MAIN_WINDOW_RECT,&frame) != B_OK)
	{
		frame = BRect(100,100,500,500);	
	}	
	prefsLock.Unlock();	
	
	m_mainWindow = new MainWindow(frame);
	m_mainWindow->Show();
}

BeTeX::~BeTeX()
{	
}

void BeTeX::RefsReceived(BMessage* message)
{
	m_mainWindow->PostMessage(message);
	BApplication::RefsReceived(message);
}

bool BeTeX::QuitRequested()
{
	if(m_mainWindow->PromptToQuit())
	{
		BString text;
		text << "Are you sure want to quit?";
			
		BAlert* alert = new BAlert("savealert",text.String(),"Cancel","Force Quit",NULL, B_WIDTH_AS_USUAL,B_WARNING_ALERT);
		alert->SetShortcut(0,B_ESCAPE);
		
		int32 button_index = alert->Go();
		switch(button_index)
		{
			case 0:
			{	
				return false;
			}			
			case 1:
			{
				SavePreferences(m_prefsPath.Path());
				return true;
			}
		}
		delete alert;
	}
	return true;
}

void BeTeX::AboutRequested()
{
	m_mainWindow->PostMessage(new BMessage(AboutMessages::K_ABOUT_WINDOW_LAUNCH));	
}

int main()
{
	BeTeX app;
	app.Run();
	return 0;
}
