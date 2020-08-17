#ifndef LUAAPPLICATION_H
#define LUAAPPLICATION_H

//extern "C"
//{
//#include "lua.h"
//#include "lauxlib.h"
//#include "lualib.h"
//}
#include "lua.hpp"

class Application;
class Ticker;

#include <exception>
#include <string>
#include <vector>
#include <set>
#include <gideros_p.h>
#include <deque>
#include <map>

#include <gglobal.h>

struct Touch;

#include "orientation.h"

class Event;

#include <gstatus.h>

/*
class LuaException : public std::exception
{
public:
	enum Type
	{
		eLoadError,
		eRuntimeErrorAfterLoad,
		eRuntimeError
	};
	
	LuaException(Type type, const char* message) : type_(type), message_(message) {}
	~LuaException() throw() {}

	virtual const char* what() const throw()
	{
		return message_.c_str();
	}

	Type type() const
	{
		return type_;
	}

private:
	Type type_;
	std::string message_;
};
*/

class LuaApplication : public LuaApplicationBase
{
public:
	LuaApplication(void);
	~LuaApplication(void);

	void enableExceptions();
	void disableExceptions();
	
	lua_PrintFunc getPrintFunc(void);
    void setPrintFunc(lua_PrintFunc printFunc, void *data = NULL);

	virtual void initialize();

    void loadFile(const char* filename, GStatus *status);

    void tick(GStatus *status);
    void enterFrame(GStatus *status);
	void clearBuffers();
	void renderScene(int deltaFrameCount = -1);
	
    virtual void deinitialize();

	bool isInitialized() const;

    void setPlayerMode(bool isPlayer);
    bool isPlayerMode();
    bool isPlayer_;

	void setHardwareOrientation(Orientation orientation);
	void setResolution(int width, int height);

//	void broadcastApplicationDidFinishLaunching();
//	void broadcastApplicationWillTerminate();
//  void broadcastMemoryWarning();

    void broadcastEvent(Event* event, GStatus *status);

	lua_State* luaState() const
	{
		return L;
	}

//	void orientationChange(Orientation orientation);

	Application* getApplication() const
	{
		return application_;
	}

	float getPhysicsScale() const
	{
		return physicsScale_;
	}

	void setPhysicsScale(float physicsScale)
	{
		physicsScale_ = physicsScale;
	}

    void setScale(float scale);

	void setLogicalDimensions(int width, int height);
	void setLogicalScaleMode(LogicalScaleMode mode);

	int getLogicalWidth() const;
	int getLogicalHeight() const;
	int getHardwareWidth() const;
	int getHardwareHeight() const;

	void setImageScales(const std::vector<std::pair<std::string, float> >& imageScales);
	const std::vector<std::pair<std::string, float> >& getImageScales() const;

	void setOrientation(Orientation orientation);
	Orientation orientation() const;
	Orientation hardwareOrientation() const
	{
	     return orientation_;
	}

	virtual void addTicker(Ticker* ticker); //Virtual calls are accessible to .DLL plugins
	virtual void removeTicker(Ticker* ticker);

	float getLogicalTranslateX() const;
	float getLogicalTranslateY() const;
	float getLogicalScaleX() const;
	float getLogicalScaleY() const;

    virtual void setError(const char* error);
    virtual bool isErrorSet() const;
    virtual const char* getError() const;
    virtual void clearError();

    lua_State *getLuaState() const;

    struct AsyncLuaTask {
    	lua_State *L;
    	int taskRef;
    	double sleepTime;
    	bool skipFrame;
    	bool autoYield;
    	int nargs;
    };
    static std::deque<AsyncLuaTask> tasks_;
    static double meanFrameTime_; //Average frame duration
    static double meanFreeTime_; //Average time available for async tasks
    static unsigned long frameCounter_; //Global frame counter
    static int Core_asyncCall(lua_State *L);
    static int Core_yield(lua_State *L);
    static int Core_frameStatistics(lua_State *L);
    static int Core_profilerStart(lua_State *L);
    static int Core_profilerStop(lua_State *L);
    static int Core_profilerReset(lua_State *L);
    static int Core_profilerReport(lua_State *L);
    static int Core_random(lua_State *L);
    static int Core_randomSeed(lua_State *L);
    //Debugger support
#define DBG_MASKBREAK	(1<<7) //Check breakpoints: Not a lua debug flag
#define DBG_MASKSUB		(1<<6) //Ignore subcalls: Not a lua debug flag
#define DBG_MASKLUA		0x1F
    static int debuggerBreak;
    static std::map<int,bool> breakpoints;
    static void (*debuggerHook)(void *context,lua_State *L,lua_Debug *ar);
    static void *debuggerContext;
private:
	float physicsScale_;

	static const char* fileNameFunc_s(const char* filename, void* data);
	const char* fileNameFunc(const char* filename);

	lua_State* L;
	Application* application_;
	lua_PrintFunc printFunc_;
    void *printData_;
	bool exceptionsEnabled_;
	Orientation orientation_;
	int width_, height_;

    float scale_;

    std::string error_;

    static void callback_s(int type, void *event, void *udata);
    void callback(int type, void *event);

    double frameStartTime_; //Time at which that frame processing started
    double lastFrameTime_; //Total duration of last frame
    double taskFrameTime_; //Total time consumed by async tasks
};


#endif
