cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)
include("PrimeAPI2/PrimeAPI.cmake")
project(prime2-practice-mod C CXX ASM)

#include_directories(PrimeAPI/include)
include_directories(src/include)
include_directories(src)
include_directories(ext/imgui)
include_directories(ext/QRCode/src)

set(IMGUI_H_DEFINE [=[-DIMGUI_USER_CONFIG="\"pracc_imconfig.h\""]=])
set(QR_DEFINE "-DLOCK_VERSION=4")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${IMGUI_H_DEFINE} ${QR_DEFINE}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${IMGUI_H_DEFINE} ${QR_DEFINE}")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEBUG")
else ()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os")
endif ()


message(${CMAKE_CXX_FLAGS})

set(SOURCE_FILES
        src/include/PrimeAPI.h
        src/include/rstl/string.h
        src/include/rstl/rstl.h
        src/include/rstl/vector.h
        src/include/rstl/auto_ptr.h
        src/include/rstl/list.h
        src/include/rstl/pair.h
        src/include/dvd.h
        src/include/types.h
        src/include/os.h
        src/include/pad.h
        src/include/utils.hpp
        src/include/MathyTypes.hpp

        src/include/prime/CMemory.hpp
        src/include/prime/CStateManager.hpp
        src/include/prime/CGraphics.hpp
        src/include/prime/CRandom16.hpp
        src/include/prime/CFinalInput.hpp
        src/include/prime/CGameState.hpp
        src/include/prime/CMain.hpp
        src/include/prime/CMainFlow.hpp
        src/include/prime/CGameGlobalObjects.hpp
        src/include/prime/CArchitectureMessage.hpp
        src/include/prime/CArchitectureQueue.hpp
        src/include/prime/CIOWin.hpp
        src/include/prime/CIOWinManager.hpp
        src/include/prime/CPlayer.hpp
        src/include/prime/CPhysicsActor.hpp
        src/include/prime/CActor.hpp
        src/include/prime/CPauseScreen.hpp
        src/include/prime/CHealthInfo.hpp
        src/include/prime/CWorldState.hpp
        src/include/prime/CWorld.hpp
        src/include/prime/CGameArea.hpp
        src/include/prime/CPlayMovie.hpp
        src/include/prime/CScriptSpawnPoint.hpp

        src/PracticeMod.hpp
        src/include/GetField.hpp
        src/include/settings.hpp
        src/include/GX.hpp

        ext/imgui/imgui.cpp
        #        ext/imgui/imgui_demo.cpp
        ext/imgui/imgui_draw.cpp
        ext/imgui/imgui_tables.cpp
        ext/imgui/imgui_widgets.cpp
        #        ext/imgui/imgui_demo.cpp
        ext/imgui/imgui.h
        ext/imgui/imgui_internal.h
        #        imgui/imgui_draw.cpp

        ext/QRCode/src/qrcode.c

        src/include/pracc_imconfig.h

        src/system/CrashAssert.h
        src/system/CrashAssert.cpp
        src/system/sscanf.c
        src/system/new.cpp
        src/system/malloc_wrappers.h
        src/system/malloc_wrappers.cpp
        src/system/font_atlas.hpp
        src/system/ImGuiEngine.cpp
        src/system/ImGuiEngine.hpp
        src/system/libc.c

        src/utils/settings.cpp
        src/utils/ReplaceFunction.hpp
        src/utils/ReplaceFunction.cpp
        src/utils/DataByteStream.hpp
        src/utils/DataBitStream.cpp

        src/UI/ImHelpers.hpp
        src/UI/ImHelpers.cpp
        src/UI/WarpMenu.h
        src/UI/WarpMenu.cpp
        src/UI/PlayerMenu.hpp
        src/UI/PlayerMenu.cpp
        src/UI/InventoryMenu.hpp
        src/UI/InventoryMenu.cpp
        src/UI/MonitorWindow.hpp
        src/UI/MonitorWindow.cpp
        src/UI/SettingsMenu.cpp
        src/UI/SettingsMenu.hpp
        src/UI/QR.cpp
        src/UI/QR.hpp
        src/UI/DumpMemoryUI.cpp
        src/UI/DumpMemoryUI.hpp

        src/PracticeMod.cpp
        src/prime-practice.cpp
        src/hooks.cpp
)

add_gc_static_binary(prime2-practice src/prime-practice.lst
        src/patcher_config.toml
        src/opening_practice.bnr
        ${SOURCE_FILES})

if(CREATE_PATCHED_DOL)
    patch_dol(prime2-practice default.dol "${CREATE_PATCHED_DOL}")
endif()