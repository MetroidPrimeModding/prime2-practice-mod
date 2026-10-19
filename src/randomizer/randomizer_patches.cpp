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
  static CStateManager *Callback(
    CStateManager* self,
    const rstl::ncrc_ptr<CScriptMailbox> &mailbox,
    const rstl::ncrc_ptr<CMapWorldInfo> &mapInfo,
    const rstl::reserved_vector<rstl::ncrc_ptr<CPlayerState>, 4> &playerStates,
    const rstl::ncrc_ptr<CWorldTransManager> &worldTrans,
    const rstl::ncrc_ptr<CWorldLayerState> &worldLayers
  ) {
    // reset the CToken every time a new CStateManager is constructed
    if (g_pickupMapIconToken_isInitialized) {
      g_pickupMapIconToken_isInitialized = false;
    }
    return Orig(self, mailbox, mapInfo, playerStates, worldTrans, worldLayers);
  }
};

DECLARE_FUNCTION_REPLACEMENT(CMappableObject_Draw) {
  static void Callback(const CMappableObject* self, int curArea, const CMapWorldInfo& mwInfo, float alpha, bool needsVtxLoad) {
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
      
      g_pickupMapIconToken_isInitialized = true;
      token.CToken_dtor(0);
    }

    if (*(self->getMappableObjectType()) == EMappableObjectType_Unused_0x12) {
      // render custom pickup icon
      if (!mwInfo.IsObjectUnmapped(self->getEditorId())) {
        auto iconTexture = (CTexture*) g_pickupMapIconToken.GetObj()->GetContents();
        iconTexture->Load(GXTexMap0, CTexture::kCM_Repeat);
        CGraphics::SetTevOp(kTS_Stage0, g_tevPass_80416c2c);
        CGraphics::StreamBegin(ERglPrimitive_TRIANGLESTRIP);
        CGraphics::StreamColor(0xFFFFFF00 | int(255.0f * alpha));
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

