//
//  mainfrm.h
//
//  Main frame handling for Aven.
//
//  Copyright (C) 2000-2001, Mark R. Shinwell.
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

#ifndef mainfrm_h
#define mainfrm_h

#include "wx.h"
#include "gfxcore.h"
#include <list>

enum {
    menu_FILE_OPEN = 1000,
    menu_FILE_QUIT,
    menu_ROTATION_START,
    menu_ROTATION_STOP,
    menu_ROTATION_SPEED_UP,
    menu_ROTATION_SLOW_DOWN,
    menu_ROTATION_REVERSE,
    menu_ROTATION_STEP_CCW,
    menu_ROTATION_STEP_CW,
    menu_ORIENT_MOVE_NORTH,
    menu_ORIENT_MOVE_EAST,
    menu_ORIENT_MOVE_SOUTH,
    menu_ORIENT_MOVE_WEST,
    menu_ORIENT_SHIFT_LEFT,
    menu_ORIENT_SHIFT_RIGHT,
    menu_ORIENT_SHIFT_UP,
    menu_ORIENT_SHIFT_DOWN,
    menu_ORIENT_PLAN,
    menu_ORIENT_ELEVATION,
    menu_ORIENT_HIGHER_VP,
    menu_ORIENT_LOWER_VP,
    menu_ORIENT_ZOOM_IN,
    menu_ORIENT_ZOOM_OUT,
    menu_ORIENT_DEFAULTS,
    menu_VIEW_SHOW_LEGS,
    menu_VIEW_SHOW_CROSSES,
    menu_VIEW_SHOW_NAMES,
    menu_VIEW_SHOW_OVERLAPPING_NAMES,
    menu_VIEW_COMPASS,
    menu_VIEW_DEPTH_BAR,
    menu_VIEW_SCALE_BAR,
    menu_VIEW_STATUS_BAR,
    menu_CTL_REVERSE,
    menu_CTL_CAVEROT_MID,
    menu_HELP_ABOUT
};

class PointInfo {
    friend class MainFrm;
    float x, y, z;
    bool isLine; // false => move, true => draw line
};

class LabelInfo {
    friend class MainFrm;
    float x, y, z;
    wxString text;
};

class MainFrm : public wxFrame {
    list<PointInfo*>* m_Points;
    list<LabelInfo*> m_Labels;
    float m_XExt;
    float m_YExt;
    float m_ZExt;
    float m_ZMin;
    int m_NumLegs;
    int m_NumPoints;
    int m_NumCrosses;
    int m_NumExtraLegs;
    GfxCore* m_Gfx;
    wxPen* m_Pens;
    wxBrush* m_Brushes;
    wxStatusBar* m_StatusBar; // NULL if status bar shown, otherwise pointer to hidden bar.

    void ClearPointLists();
    bool LoadData(const wxString& file);
    void SortIntoDepthBands(list<PointInfo*>& points);
    void IntersectLineWithPlane(float x0, float y0, float z0,
				float x1, float y1, float z1,
				float z, float& x, float& y);
    float GetDepthBoundaryBetweenBands(int a, int b);
    int GetDepthColour(float z);
    void CentreDataset(float xmin, float ymin, float zmin);

public:
    MainFrm(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~MainFrm();

    void OnOpen(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnPaint(wxPaintEvent&);

    // temporary bodges until event handling problem is sorted out:
    void OnDefaultsUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnDefaultsUpdate(event); }
    void OnPlanUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnPlanUpdate(event); }
    void OnElevationUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnElevationUpdate(event); }
    void OnDisplayOverlappingNamesUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnDisplayOverlappingNamesUpdate(event); }
    void OnShowCrossesUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnShowCrossesUpdate(event); }
    void OnShowStationNamesUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnShowStationNamesUpdate(event); }
    void OnShowSurveyLegsUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnShowSurveyLegsUpdate(event); }
    void OnMoveEastUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnMoveEastUpdate(event); }
    void OnMoveNorthUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnMoveNorthUpdate(event); }
    void OnMoveSouthUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnMoveSouthUpdate(event); }
    void OnMoveWestUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnMoveWestUpdate(event); }
    void OnStartRotationUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnStartRotationUpdate(event); }
    void OnStopRotationUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnStopRotationUpdate(event); }
    void OnOriginalCaverotMouseUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnOriginalCaverotMouseUpdate(event); }
    void OnReverseControlsUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnReverseControlsUpdate(event); }
    void OnSlowDownUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnSlowDownUpdate(event); }
    void OnSpeedUpUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnSpeedUpUpdate(event); }
    void OnStepOnceAnticlockwiseUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnStepOnceAnticlockwiseUpdate(event); }
    void OnStepOnceClockwiseUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnStepOnceClockwiseUpdate(event); }
    void OnHigherViewpointUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnHigherViewpointUpdate(event); }
    void OnLowerViewpointUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnLowerViewpointUpdate(event); }
    void OnShiftDisplayDownUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnShiftDisplayDownUpdate(event); }
    void OnShiftDisplayLeftUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnShiftDisplayLeftUpdate(event); }
    void OnShiftDisplayRightUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnShiftDisplayRightUpdate(event); }
    void OnShiftDisplayUpUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnShiftDisplayUpUpdate(event); }
    void OnZoomInUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnZoomInUpdate(event); }
    void OnZoomOutUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnZoomOutUpdate(event); }
    void OnToggleScalebarUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnToggleScalebarUpdate(event); }
    void OnToggleDepthbarUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnToggleDepthbarUpdate(event); }
    void OnViewCompassUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnViewCompassUpdate(event); }
    void OnReverseDirectionOfRotationUpdate(wxUpdateUIEvent& event) { assert(m_Gfx); m_Gfx->OnReverseDirectionOfRotationUpdate(event); }

    void OnDefaults(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnDefaults(event); }
    void OnPlan(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnPlan(event); }
    void OnElevation(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnElevation(event); }
    void OnDisplayOverlappingNames(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnDisplayOverlappingNames(event); }
    void OnShowCrosses(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnShowCrosses(event); }
    void OnShowStationNames(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnShowStationNames(event); }
    void OnShowSurveyLegs(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnShowSurveyLegs(event); }
    void OnMoveEast(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnMoveEast(event); }
    void OnMoveNorth(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnMoveNorth(event); }
    void OnMoveSouth(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnMoveSouth(event); }
    void OnMoveWest(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnMoveWest(event); }
    void OnStartRotation(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnStartRotation(event); }
    void OnStopRotation(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnStopRotation(event); }
    void OnOriginalCaverotMouse(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnOriginalCaverotMouse(event); }
    void OnReverseControls(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnReverseControls(event); }
    void OnSlowDown(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnSlowDown(event); }
    void OnSpeedUp(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnSpeedUp(event); }
    void OnStepOnceAnticlockwise(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnStepOnceAnticlockwise(event); }
    void OnStepOnceClockwise(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnStepOnceClockwise(event); }
    void OnHigherViewpoint(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnHigherViewpoint(event); }
    void OnLowerViewpoint(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnLowerViewpoint(event); }
    void OnShiftDisplayDown(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnShiftDisplayDown(event); }
    void OnShiftDisplayLeft(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnShiftDisplayLeft(event); }
    void OnShiftDisplayRight(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnShiftDisplayRight(event); }
    void OnShiftDisplayUp(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnShiftDisplayUp(event); }
    void OnZoomIn(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnZoomIn(event); }
    void OnZoomOut(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnZoomOut(event); }
    void OnToggleScalebar(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnToggleScalebar(event); }
    void OnToggleDepthbar(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnToggleDepthbar(event); }
    void OnViewCompass(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnViewCompass(event); }
    void OnReverseDirectionOfRotation(wxCommandEvent& event) { assert(m_Gfx); m_Gfx->OnReverseDirectionOfRotation(event); }
    // end of horrible bodges

    void OnToggleStatusbar(wxCommandEvent& event);
    void OnToggleStatusbarUpdate(wxUpdateUIEvent& event);

    float GetXExtent() { return m_XExt; }
    float GetYExtent() { return m_YExt; }
    float GetZExtent() { return m_ZExt; }
    float GetZMin()    { return m_ZMin; }

    int GetNumLegs()   { return m_NumLegs; }
    int GetNumPoints() { return m_NumPoints; }
    int GetNumCrosses() { return m_NumCrosses; }

    int GetNumDepthBands();

    wxPen GetPen(int band);
    wxBrush GetBrush(int band);

    list<PointInfo*>::iterator GetFirstPoint(int band);
    bool GetNextPoint(int band, list<PointInfo*>::iterator& pos, float& x, float& y, float& z,
		      bool& isLine);

    list<LabelInfo*>::iterator GetFirstLabel();
    bool GetNextLabel(list<LabelInfo*>::iterator& pos, float& x, float& y, float& z, wxString& text);

    //bool ProcessEvent(wxEvent&);

private:
    DECLARE_EVENT_TABLE()
};

#endif
