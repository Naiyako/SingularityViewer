/** 
 * @file llhudview.cpp
 * @brief 2D HUD overlay
 *
 * $LicenseInfo:firstyear=2003&license=viewergpl$
 * 
 * Copyright (c) 2003-2009, Linden Research, Inc.
 * 
 * Second Life Viewer Source Code
 * The source code in this file ("Source Code") is provided by Linden Lab
 * to you under the terms of the GNU General Public License, version 2.0
 * ("GPL"), unless you have obtained a separate licensing agreement
 * ("Other License"), formally executed by you and Linden Lab.  Terms of
 * the GPL can be found in doc/GPL-license.txt in this distribution, or
 * online at http://secondlifegrid.net/programs/open_source/licensing/gplv2
 * 
 * There are special exceptions to the terms and conditions of the GPL as
 * it is applied to this Source Code. View the full text of the exception
 * in the file doc/FLOSS-exception.txt in this software distribution, or
 * online at
 * http://secondlifegrid.net/programs/open_source/licensing/flossexception
 * 
 * By copying, modifying or distributing this software, you acknowledge
 * that you have read and understood your obligations described above,
 * and agree to abide by those obligations.
 * 
 * ALL LINDEN LAB SOURCE CODE IS PROVIDED "AS IS." LINDEN LAB MAKES NO
 * WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
 * COMPLETENESS OR PERFORMANCE.
 * $/LicenseInfo$
 */

#include "llviewerprecompiledheaders.h"

#include "llhudview.h"

#include "lltracker.h"
#include "lluictrlfactory.h"

LLHUDView *gHUDView = nullptr;

LLHUDView::LLHUDView(const LLRect& r)
{
	LLUICtrlFactory::getInstance()->buildPanel(this, "panel_hud.xml");
	setShape(r,true);
}

// virtual
void LLHUDView::draw()
{
	LLTracker::instance().drawHUDArrow();
	LLView::draw();
}

/*virtual*/
BOOL LLHUDView::handleMouseDown(S32 x, S32 y, MASK mask)
{
	return LLTracker::instance().handleMouseDown(x, y) || LLView::handleMouseDown(x, y, mask);
}
