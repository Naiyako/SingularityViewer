/** 
 * @file llthread.cpp
 *
 * $LicenseInfo:firstyear=2004&license=viewergpl$
 * 
 * Copyright (c) 2004-2009, Linden Research, Inc.
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

#include "linden_common.h"
#include "llapr.h"

#include "apr_portable.h"

#include "llthread.h"

#include "lltimer.h"

#if LL_LINUX || LL_SOLARIS
#include <sched.h>
#endif

//----------------------------------------------------------------------------
// Usage:
// void run_func(LLThread* thread)
// {
// }
// LLThread* thread = new LLThread();
// thread->run(run_func);
// ...
// thread->setQuitting();
// while(!timeout)
// {
//   if (thread->isStopped())
//   {
//     delete thread;
//     break;
//   }
// }
// 
//----------------------------------------------------------------------------

#if !LL_DARWIN
U32 ll_thread_local local_thread_ID = 0;
#endif 

U32 LLThread::sIDIter = 0;
LLAtomicS32	LLThread::sCount = 0;
LLAtomicS32	LLThread::sRunning = 0;

LL_COMMON_API void assert_main_thread()
{
	static U32 s_thread_id = LLThread::currentID();
	if (LLThread::currentID() != s_thread_id)
	{
		llerrs << "Illegal execution outside main thread." << llendl;
	}
}

//
// Handed to the APR thread creation function
//
void *APR_THREAD_FUNC LLThread::staticRun(apr_thread_t *apr_threadp, void *datap)
{
#ifdef CWDEBUG
  	debug::init_thread();
#endif

	LLThread *threadp = (LLThread *)datap;

#if !LL_DARWIN
	local_thread_ID = threadp->mID;
#endif

	// Create a thread local data.
	LLThreadLocalData::create(threadp);

	// Run the user supplied function
	threadp->run();

	// Setting mStatus to STOPPED is done non-thread-safe, so it's
	// possible that the thread is deleted by another thread at
	// the moment it happens... therefore make a copy here.
	char const* volatile name = threadp->mName.c_str();
	
	// Always make sure that sRunning <= number of threads with status RUNNING,
	// so do this before changing mStatus (meaning that once we see that we
	// are STOPPED, then sRunning is also up to date).
	--sRunning;

	// We're done with the run function, this thread is done executing now.
	threadp->mStatus = STOPPED;

	// Only now print this info [doing that before setting mStatus
	// to STOPPED makes it much more likely that another thread runs
	// after the LLCurl::Multi::run() function exits and we actually
	// change this variable (which really SHOULD have been inside
	// the critical area of the mSignal lock)].
	lldebugs << "LLThread::staticRun() Exiting: " << name << llendl;

	--sRunning;		// Would be better to do this after joining with the thread, but we don't join :/
	return NULL;
}


LLThread::LLThread(std::string const& name) :
	mPaused(false),
	mName(name),
	mAPRThreadp(NULL),
	mStatus(STOPPED),
	mThreadLocalData(NULL)
{
	mID = ++sIDIter;
	sCount++;
	llassert(sCount <= 50);
	mRunCondition = new LLCondition;
}


LLThread::~LLThread()
{
	shutdown();
}

void LLThread::shutdown()
{
	// Warning!  If you somehow call the thread destructor from itself,
	// the thread will die in an unclean fashion!
	if (mAPRThreadp)
	{
		if (!isStopped())
		{
			// The thread isn't already stopped
			// First, set the flag that indicates that we're ready to die
			setQuitting();

			llinfos << "LLThread::shutdown() Killing thread " << mName << " Status: " << mStatus << llendl;
			// Now wait a bit for the thread to exit
			// It's unclear whether I should even bother doing this - this destructor
			// should netver get called unless we're already stopped, really...
			S32 counter = 0;
			const S32 MAX_WAIT = 600;
			while (counter < MAX_WAIT)
			{
				if (isStopped())
				{
					break;
				}
				// Sleep for a tenth of a second
				ms_sleep(100);
				yield();
				counter++;
			}
		}

		if (!isStopped())
		{
			// This thread just wouldn't stop, even though we gave it time
			llwarns << "LLThread::shutdown() exiting thread before clean exit!" << llendl;
			// Put a stake in its heart.
			apr_thread_exit(mAPRThreadp, -1);
			return;
		}
		mAPRThreadp = NULL;
	}
	--sCount;
	delete mRunCondition;
	mRunCondition = 0;
}

void LLThread::start()
{
	llassert(isStopped());
	
	// Set thread state to running
	mStatus = RUNNING;
	sRunning++;

	apr_status_t status =
		apr_thread_create(&mAPRThreadp, NULL, staticRun, (void *)this, tldata().mRootPool());

	if(status == APR_SUCCESS)
	{	
		// We won't bother joining
		apr_thread_detach(mAPRThreadp);
	}
	else
	{
		--sRunning;
		mStatus = STOPPED;
		llwarns << "failed to start thread " << mName << llendl;
		ll_apr_warn_status(status);
	}
}

//============================================================================
// Called from MAIN THREAD.

// Request that the thread pause/resume.
// The thread will pause when (and if) it calls checkPause()
void LLThread::pause()
{
	if (!mPaused)
	{
		// this will cause the thread to stop execution as soon as checkPause() is called
		mPaused = true;		// Does not need to be atomic since this is only set/unset from the main thread
	}	
}

void LLThread::unpause()
{
	if (mPaused)
	{
		mPaused = false;
	}

	wake(); // wake up the thread if necessary
}

// virtual predicate function -- returns true if the thread should wake up, false if it should sleep.
bool LLThread::runCondition(void)
{
	// by default, always run.  Handling of pause/unpause is done regardless of this function's result.
	return true;
}

//============================================================================
// Called from run() (CHILD THREAD).
// Stop thread execution if requested until unpaused.
void LLThread::checkPause()
{
	mRunCondition->lock();

	// This is in a while loop because the pthread API allows for spurious wakeups.
	while(shouldSleep())
	{
		mRunCondition->wait(); // unlocks mRunCondition
		// mRunCondition is locked when the thread wakes up
	}
	
 	mRunCondition->unlock();
}

//============================================================================

void LLThread::setQuitting()
{
	mRunCondition->lock();
	if (mStatus == RUNNING)
	{
		mStatus = QUITTING;
	}
	mRunCondition->unlock();
	wake();
}

// static
U32 LLThread::currentID()
{
	return (U32)apr_os_thread_current();
}

// static
void LLThread::yield()
{
#if LL_LINUX || LL_SOLARIS
	sched_yield(); // annoyingly, apr_thread_yield  is a noop on linux...
#else
	apr_thread_yield();
#endif
}

void LLThread::wake()
{
	mRunCondition->lock();
	if(!shouldSleep())
	{
		mRunCondition->signal();
	}
	mRunCondition->unlock();
}

void LLThread::wakeLocked()
{
	if(!shouldSleep())
	{
		mRunCondition->signal();
	}
}

//static
apr_os_thread_t LLThread::sMainThreadID;

void LLThread::set_main_thread_id(void)
{
	sMainThreadID = apr_os_thread_current();
}

// The thread private handle to access the LLThreadLocalData instance.
apr_threadkey_t* LLThreadLocalData::sThreadLocalDataKey;

LLThreadLocalData::LLThreadLocalData(char const* name) : mCurlMultiHandle(NULL), mCurlErrorBuffer(NULL), mName(name)
{
}

LLThreadLocalData::~LLThreadLocalData()
{
  delete mCurlMultiHandle;
  delete [] mCurlErrorBuffer;
}

//static
void LLThreadLocalData::init(void)
{
	// Only do this once.
	if (sThreadLocalDataKey)
	{
		return;
	}

	apr_status_t status = apr_threadkey_private_create(&sThreadLocalDataKey, &LLThreadLocalData::destroy, LLAPRRootPool::get()());
	ll_apr_assert_status(status);   // Or out of memory, or system-imposed limit on the
									// total number of keys per process {PTHREAD_KEYS_MAX}
									// has been exceeded.

	// Create the thread-local data for the main thread (this function is called by the main thread).
	LLThreadLocalData::create(NULL);

	// This function is called by the main thread.
	LLThread::set_main_thread_id();
}

// This is called once for every thread when the thread is destructed.
//static
void LLThreadLocalData::destroy(void* thread_local_data)
{
	delete static_cast<LLThreadLocalData*>(thread_local_data);
}

//static
void LLThreadLocalData::create(LLThread* threadp)
{
	LLThreadLocalData* new_tld = new LLThreadLocalData(threadp ? threadp->mName.c_str() : "main thread");
	if (threadp)
	{
		threadp->mThreadLocalData = new_tld;
	}
	apr_status_t status = apr_threadkey_private_set(new_tld, sThreadLocalDataKey);
	llassert_always(status == APR_SUCCESS);
}

//static
LLThreadLocalData& LLThreadLocalData::tldata(void)
{
	if (!sThreadLocalDataKey)
	{
		LLThreadLocalData::init();
	}

	void* data;
	apr_status_t status = apr_threadkey_private_get(&data, sThreadLocalDataKey);
	llassert_always(status == APR_SUCCESS);
	return *static_cast<LLThreadLocalData*>(data);
}

//============================================================================

LLCondition::LLCondition(LLAPRPool& parent) : LLMutex(parent)
{
	apr_thread_cond_create(&mAPRCondp, mPool());
}


LLCondition::~LLCondition()
{
	apr_thread_cond_destroy(mAPRCondp);
	mAPRCondp = NULL;
}

void LLCondition::wait()
{
	apr_thread_cond_wait(mAPRCondp, mAPRMutexp);
}

void LLCondition::signal()
{
	apr_thread_cond_signal(mAPRCondp);
}

void LLCondition::broadcast()
{
	apr_thread_cond_broadcast(mAPRCondp);
}

//============================================================================
LLMutexBase::LLMutexBase() :
	mLockingThread(NO_THREAD),
	mCount(0)
{
}

bool LLMutexBase::isSelfLocked() const
{
#if LL_DARWIN
	return mLockingThread == LLThread::currentID();
#else
	return mLockingThread == local_thread_ID;
#endif
}

void LLMutexBase::lock() 
{ 
#if LL_DARWIN
	if (mLockingThread == LLThread::currentID())
#else
	if (mLockingThread == local_thread_ID)
#endif
	{ //redundant lock
		mCount++;
		return;
	}

	apr_thread_mutex_lock(mAPRMutexp);
	
#if LL_DARWIN
	mLockingThread = LLThread::currentID();
#else
	mLockingThread = local_thread_ID;
#endif
}

void LLMutexBase::unlock()
{
	if (mCount > 0)
	{ //not the root unlock
		mCount--;
		return;
	}
	mLockingThread = NO_THREAD;

	apr_thread_mutex_unlock(mAPRMutexp);
}

//----------------------------------------------------------------------------

LLThreadSafeRefCount::LLThreadSafeRefCount() :
	mRef(0)
{
}

LLThreadSafeRefCount::~LLThreadSafeRefCount()
{ 
	if (mRef != 0)
	{
		llerrs << "deleting non-zero reference" << llendl;
	}
}

//============================================================================

LLResponder::~LLResponder()
{
}

//============================================================================
