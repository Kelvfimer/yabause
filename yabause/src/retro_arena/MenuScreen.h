﻿/*
        Copyright 2019 devMiyax(smiyaxdev@gmail.com)

This file is part of YabaSanshiro.

        YabaSanshiro is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

YabaSanshiro is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

        You should have received a copy of the GNU General Public License
along with YabaSanshiro; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include <SDL2/SDL.h>
#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL_opengles2.h>
#undef Success 
#include "nanogui/screen.h"
#include "nanovg.h"
#include <string.h>
#include <stack>
#include <vector>

using std::shared_ptr;

#if defined(_MSC_VER) 
#undef snprintf
#endif
#include <nlohmann/json.hpp>
using json = nlohmann::json;


#include "InputConfig.h"

using namespace nanogui;

class InputManager;
class Preference;
class GameInfo;
class GameInfoManager;

struct PreMenuInfo {
    Widget* window = nullptr;
    Widget* button = nullptr;
};

struct PlayerConfig {
    ComboBox * cb = nullptr;
    PopupButton *player = nullptr;
};

#include <iostream>
#include <list>
#include <unordered_map>

class LRU_Cache {
public:
  // キャッシュのサイズ
  const size_t cache_size;

  // コンストラクタ
  // size: キャッシュのサイズ
  LRU_Cache(size_t size) : cache_size(size) {}

  // キャッシュに値をセットする
  // key: キー
  // value: 値
  int set(const int& key, const int& value) {
    // キャッシュが満杯の場合、最も古いデータを削除する
    int last = -1;
    if (cache.size() >= cache_size) {
      last = cache[used.back()];
      cache.erase(used.back());
      used.pop_back();
    }

    // キャッシュに値をセットし、参照順序を更新する
    used.push_front(key);
    cache[key] = value;
    return last;
  }

  // キャッシュから値を取得する
  // key: キー
  // 戻り値: 値
  int get(const int& key) {
    // キャッシュに存在しない場合は、例外を発生させる
    if (cache.find(key) == cache.end()) {
      return -1;
    }

    // 参照順序を更新する
    used.remove(key);
    used.push_front(key);

    // 値を返す
    return cache[key];
  }

private:
  // キャッシュのデータ
  std::unordered_map<int, int> cache;

  // キャッシュのデータの参照順序
  std::list<int> used;
};



class MenuScreen : public nanogui::Screen
{
public:
    //Widget *activeWindow = nullptr;
    Widget *tools = nullptr;
    std::vector<PlayerConfig> player_configs_;
/*    
    PopupButton *player1;
    PopupButton *player1;
    ComboBox * p1cb = nullptr;
    PopupButton *player2 = nullptr;
    ComboBox * p2cb = nullptr;
*/    
    Button *bAnalog = nullptr;
    Button *bCdTray = nullptr;
    bool is_cdtray_open_ = false;

    std::map<SDL_JoystickID, SDL_Joystick*> joysticks_;

    int imageid_ = 0;
    int imageid_tmp_ = 0;
    int imgw_ = 0;
    int imgh_ = 0;
    NVGpaint imgPaint_ = {};     
    void setBackGroundImage( const std::string & fname );
    void setTmpBackGroundImage( const std::string & fname );
    void setDefalutBackGroundImage();

    nanogui::Window *window;
    nanogui::Window *swindow;
    nanogui::Window *imageWindow;
    nanogui::Window *dirSelectWindow;

    LRU_Cache * imageCache = new LRU_Cache(32);
    
    
    MenuScreen( SDL_Window* pwindow, int rwidth, int rheight, const std::string & fname, const std::string & game  );
    virtual bool keyboardEvent( std::string & keycode , int scancode, int action, int modifiers);
    virtual void draw(NVGcontext *ctx);

	uint32_t reset_ = 0;
	void setResetMenuEventCode( uint32_t type ){ reset_ = type; }

	uint32_t pad_ = 0;
	void setTogglePadModeMenuEventCode( uint32_t type ){ pad_ = type; }

	uint32_t toggile_fps_ = 0;
	void setToggleFpsCode( uint32_t type ){ toggile_fps_ = type; }

	uint32_t toggile_frame_skip_ = 0;
	void setToggleFrameSkip( uint32_t type ){ toggile_frame_skip_ = type; }

	uint32_t update_config_ = 0;
	void setUpdateConfig( uint32_t type ){ update_config_ = type; }

	uint32_t open_tray_ = 0;
	void setOpenTrayMenuEventCode( uint32_t type ){ open_tray_ = type; }

	uint32_t close_tray_ = 0;
	void setCloseTrayMenuEventCode( uint32_t type ){ close_tray_ = type; }

    std::string current_game_path_;
    void setCurrentGamePath( const char * path ){ current_game_path_ = path; }

    uint32_t save_state_ = 0;
    void setSaveStateEventCode( uint32_t code ){ save_state_ = code; }

    uint32_t load_state_ = 0;
    void setLoadStateEventCode( uint32_t code ){ load_state_ = code; }

    void showInputCheckDialog( const std::string & key );

    void showFileSelectDialog( Widget * parent, Widget * toback, const std::string & base_path );

    void setupPlayerPsuhButton( int user_index, PopupButton *player, const std::string & label, ComboBox **cb );

    int onRawInputEvent( InputManager & imp, const std::string & deviceguid, const std::string & type, int id, int value );

    void setCurrentInputDevices( std::map<SDL_JoystickID, SDL_Joystick*> & joysticks );

    std::stack<PreMenuInfo> menu_stack;
    void pushActiveMenu( Widget *active,  Widget * button );
    void popActiveMenu();
    Widget * getActiveMenu();

    void setConfigFile( const std::string & fname ){
        config_file_ = fname;
    }
    void getSelectedGUID( int user_index, std::string & selguid );


    //void showSaveStateDialog( Widget * parent, Widget * toback );
    void setCurrentGameId( const std::string & id ){ cuurent_game_id_ = id; }
    void showSaveStateDialog( Popup *popup );
    void showLoadStateDialog( Popup *popup );
    void showConfigDialog( PopupButton *popup );

    void setupBiosMenu(PopupButton *parent, shared_ptr<Preference> preference);
    void listdir(const string & dirname, int indent, vector<shared_ptr<GameInfo>> & files);
    void checkdir(const string & dirname, int indent, vector<string> & files);

    void setupGameDirsMenu(PopupButton *parent, std::shared_ptr<Preference> preference);


public:  // events
    int onBackButtonPressed();    
    int onShow();    

protected:
    std::string cuurent_deviceguid_;
    std::string current_user_;    
    std::string current_key_; 
    std::string config_file_;
    std::string cuurent_game_id_;

    GameInfoManager * gameInfoManager;

    vector<shared_ptr<GameInfo>> games;

    std::atomic<bool> bFileSearchCancled;

    void refreshGameListAsync(const vector<string> & base_path_array);

};
