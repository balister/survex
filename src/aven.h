//
//  aven.h
//
//  Main class for Aven.
//
//  Copyright (C) 2001, Mark R. Shinwell.
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#ifndef aven_h
#define aven_h

#include <stdarg.h>

extern
#ifdef __cplusplus
 "C"
#endif
void aven_v_report(int severity, const char *fnm, int line, int en,
		   va_list ap);

#ifdef __cplusplus

#include "wx.h"

class MainFrm;

class Aven : public wxApp {
    MainFrm* m_Frame;
    wxBitmap m_AboutBitmap;

public:
    Aven();

    virtual bool OnInit();

    void ReportError(const wxString&);
    wxBitmap& GetAboutBitmap() { return m_AboutBitmap; }
    const wxBitmap LoadPreferencesIcon(const wxString& icon) const;
};

DECLARE_APP(Aven)

#endif

#endif
