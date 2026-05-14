#include "utils/ReplaceFunction.hpp"
#include "prime/CMapWorldInfo.hpp"
#include "prime/CMappableObj.hpp"
#include "prime/CGraphics.hpp"
#include "kyoto/CSimplePool.hpp"
#include "prime/CStateManager.hpp"

// global storage for the CToken so it doesn't unload
CToken g_pickupMapIconToken = CToken();

// guards to let us reinitialize after the CTexture is deleted
bool g_pickupMapIconToken_wasEverInitialized = false;
bool g_pickupMapIconToken_isInitialized = false;

DECLARE_FUNCTION_REPLACEMENT(CStateManager_CStateManager) {
  static void Callback(
    CStateManager* self,
    rstl::rc_ptr<void>* relayTracker,
    rstl::rc_ptr<void>* areaAssetIdA,
    int param4,
    rstl::rc_ptr<void>* areaAssetIdB,
    rstl::rc_ptr<void>* worldState0x1c
  ) {
    // reset the CToken every time a new CStateManager is constructed
    if (g_pickupMapIconToken_isInitialized) {
      g_pickupMapIconToken_isInitialized = false;
    }
    Orig(self, relayTracker, areaAssetIdA, param4, areaAssetIdB, worldState0x1c);
  }
};

DECLARE_FUNCTION_REPLACEMENT(CMappableObject_Draw) {
  static void Callback(CMappableObject* self, u32 curArea, const CMapWorldInfo& mwInfo, float alpha, bool needsVtxLoad) {
    if (!g_pickupMapIconToken_isInitialized) {
      // get a new CToken for this lifetime
      const CAssetId PICKUP_MAP_ICON_TEXTURE = 0x3dcbaca5u;
      static auto iconTag = SObjectTag('TXTR', PICKUP_MAP_ICON_TEXTURE);
      auto token = gpSimplePool->GetObj(iconTag);

      if (!g_pickupMapIconToken_wasEverInitialized) {
        // on first init, the global token's object reference is a null pointer.
        // since operator=() tries to decrement the refcount, it crashes here.
        // InitializeFromOther() skips the decrement, but otherwise does the same
        // things as operator=() would
        g_pickupMapIconToken.InitializeFromOther(token);
        g_pickupMapIconToken_wasEverInitialized = true;
      } else {
        g_pickupMapIconToken = token;
      }

      token.CToken_dtor(0);
    }

    if (*(self->getMappableObjectType()) == EMappableObjectType_Unused_0x12) {
      // render custom pickup icon
      if (!mwInfo.IsObjectUnmapped(self->getEditorId())) {
        auto iconTexture = (CTexture*) g_pickupMapIconToken.GetObj()->GetContents();
        iconTexture->LoadMipLevel(0, 1);
        CGraphics::SetTevOp(GxTevOp_ADD, &g_tevPass_80416c2c);
        CGraphics::StreamBegin(ERglPrimitive_TRIANGLESTRIP);
        CGraphics::StreamColor(0xFFFFFFFF);
        CGraphics::StreamTexcoord(0.0, 1.0);
        CGraphics::StreamVertex(-2.6, 0.0, 2.6);
        CGraphics::StreamTexcoord(0.0, 0.0);
        CGraphics::StreamVertex(-2.6, 0.0, -2.6);
        CGraphics::StreamTexcoord(1.0, 1.0);
        CGraphics::StreamVertex(2.6, 0.0, 2.6);
        CGraphics::StreamTexcoord(1.0, 0.0);
        CGraphics::StreamVertex(2.6, 0.0, -2.6);
        CGraphics::StreamEnd();
      }
    } else {
      Orig(self, curArea, mwInfo, alpha, needsVtxLoad);
    }  
  }
};

void InstallRandomizerHooks() {
  CStateManager_CStateManager::InstallAtFuncPtr(&CStateManager::CStateManager_ctor);
  CMappableObject_Draw::InstallAtFuncPtr(&CMappableObject::Draw);
};

