#pragma once
struct LibraryGame {wchar_t name[260],target[1024],lastAccount[32];int source;}; // 0 shortcut, 1 Steam, 2 Epic
struct GameList {LibraryGame* games;int count,capacity;};
constexpr unsigned libraryLoaded=0x8052;
extern "C" bool scanGames(void* window);
extern "C" void freeGames(GameList* list);
extern "C" bool storeGame(const wchar_t* path,bool remove);
extern "C" bool launchGame(const LibraryGame* game,void* window);

constexpr unsigned steamLaunchFinished=0x8053;
extern "C" const wchar_t* requestLibraryLaunch(const LibraryGame*,void*);
extern "C" bool assignActiveAccount(const LibraryGame*);
