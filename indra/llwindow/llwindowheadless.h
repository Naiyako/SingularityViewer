/** 
 * @file llwindowheadless.h
 * @brief Headless definition of LLWindow class
 *
 * $LicenseInfo:firstyear=2001&license=viewerlgpl$
 * Second Life Viewer Source Code
 * Copyright (C) 2010, Linden Research, Inc.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * version 2.1 of the License only.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 * Linden Research, Inc., 945 Battery Street, San Francisco, CA  94111  USA
 * $/LicenseInfo$
 */

#ifndef LL_LLWINDOWHEADLESS_H
#define LL_LLWINDOWHEADLESS_H

#include "llwindow.h"

class LLWindowHeadless : public LLWindow
{
public:
	/*virtual*/ void show(bool focus = true) override {};
	/*virtual*/ void hide() override {};
	/*virtual*/ void close() override {};
	/*virtual*/ BOOL getVisible() override {return FALSE;};
	/*virtual*/ BOOL getMinimized() override {return FALSE;};
	/*virtual*/ BOOL getMaximized() override {return FALSE;};
	/*virtual*/ BOOL maximize() override {return FALSE;};
	/*virtual*/ void minimize() override {};
	/*virtual*/ void restore() override {};
	/*virtual*/ BOOL getFullscreen() override {return FALSE;};
	/*virtual*/ BOOL getPosition(LLCoordScreen *position) override {return FALSE;};
	/*virtual*/ BOOL getSize(LLCoordScreen *size) override {return FALSE;};
	/*virtual*/ BOOL getSize(LLCoordWindow *size) override {return FALSE;};
	/*virtual*/ BOOL setPosition(LLCoordScreen position) override {return FALSE;};
	/*virtual*/ BOOL setSizeImpl(LLCoordScreen size) override {return FALSE;};
	/*virtual*/ BOOL setSizeImpl(LLCoordWindow size) override {return FALSE;};
	/*virtual*/ BOOL switchContext(BOOL fullscreen, const LLCoordScreen &size, const S32 vsync_mode, const LLCoordScreen * const posp = nullptr) override {return FALSE;};
	/*virtual*/ BOOL setCursorPosition(LLCoordWindow position) override {return FALSE;};
	/*virtual*/ BOOL getCursorPosition(LLCoordWindow *position) override {return FALSE;};
	/*virtual*/ void showCursor() override {};
	/*virtual*/ void hideCursor() override {};
	/*virtual*/ void showCursorFromMouseMove() override {};
	/*virtual*/ void hideCursorUntilMouseMove() override {};
	/*virtual*/ BOOL isCursorHidden() override {return FALSE;};
	/*virtual*/ void updateCursor() override {};
	//virtual ECursorType getCursor() { return mCurrentCursor; };
	/*virtual*/ void captureMouse() override {};
	/*virtual*/ void releaseMouse() override {};
	/*virtual*/ void setMouseClipping( BOOL b ) override {};
	/*virtual*/ BOOL isClipboardTextAvailable() override {return FALSE; };
	/*virtual*/ BOOL pasteTextFromClipboard(LLWString &dst) override {return FALSE; };
	/*virtual*/ void setWindowTitle(const std::string& title) override {}
	/*virtual*/ BOOL copyTextToClipboard(const LLWString &src) override {return FALSE; };
	/*virtual*/ void flashIcon(F32 seconds) override {};
	/*virtual*/ F32 getGamma() override {return 1.0f; };
	/*virtual*/ BOOL setGamma(const F32 gamma) override {return FALSE; }; // Set the gamma
	/*virtual*/ void setFSAASamples(const U32 fsaa_samples) override { }
	/*virtual*/ U32 getFSAASamples() override { return 0; }
	/*virtual*/ void setVsyncMode(const S32 vsync_mode) {}
	/*virtual*/ S32	 getVsyncMode() { return 0; }
	/*virtual*/ BOOL restoreGamma() override {return FALSE; };	// Restore original gamma table (before updating gamma)
	//virtual ESwapMethod getSwapMethod() { return mSwapMethod; }
	/*virtual*/ void gatherInput() override {};
	/*virtual*/ void delayInputProcessing() override {};
	/*virtual*/ void swapBuffers() override;


	// handy coordinate space conversion routines
	/*virtual*/ BOOL convertCoords(LLCoordScreen from, LLCoordWindow *to) override { return FALSE; };
	/*virtual*/ BOOL convertCoords(LLCoordWindow from, LLCoordScreen *to) override { return FALSE; };
	/*virtual*/ BOOL convertCoords(LLCoordWindow from, LLCoordGL *to) override { return FALSE; };
	/*virtual*/ BOOL convertCoords(LLCoordGL from, LLCoordWindow *to) override { return FALSE; };
	/*virtual*/ BOOL convertCoords(LLCoordScreen from, LLCoordGL *to) override { return FALSE; };
	/*virtual*/ BOOL convertCoords(LLCoordGL from, LLCoordScreen *to) override { return FALSE; };

	/*virtual*/ LLWindowResolution* getSupportedResolutions(S32 &num_resolutions) override { return nullptr; };
	/*virtual*/ F32	getNativeAspectRatio() override { return 1.0f; };
	/*virtual*/ F32 getPixelAspectRatio() override { return 1.0f; };
	/*virtual*/ void setNativeAspectRatio(F32 ratio) override {}

	/*virtual*/ void *getPlatformWindow() override { return nullptr; };
	/*virtual*/ void bringToFront() override {};
	
	LLWindowHeadless(LLWindowCallbacks* callbacks,
		const std::string& title, const std::string& name,
		S32 x, S32 y, 
		S32 width, S32 height,
		U32 flags,  BOOL fullscreen, BOOL clear_background,
		const S32 vsync_mode, BOOL ignore_pixel_depth);
	virtual ~LLWindowHeadless();

private:
};

class LLSplashScreenHeadless : public LLSplashScreen
{
public:
	LLSplashScreenHeadless() {};
	virtual ~LLSplashScreenHeadless() {};

	/*virtual*/ void showImpl() override {};
	/*virtual*/ void updateImpl(const std::string& mesg) override {};
	/*virtual*/ void hideImpl() override {};

};

#endif //LL_LLWINDOWHEADLESS_H

