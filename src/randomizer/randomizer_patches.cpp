#include "utils/ReplaceFunction.hpp"
#include "prime/CMapWorldInfo.hpp"
#include "prime/CMappableObj.hpp"
#include "prime/CGraphics.hpp"
#include "kyoto/CSimplePool.hpp"

DECLARE_FUNCTION_REPLACEMENT(CMappableObject_Draw) {
  static void Callback(CMappableObject* self, u32 curArea, const CMapWorldInfo& mwInfo, float alpha, bool needsVtxLoad) {
    if (*(self->getMappableObjectType()) == EMappableObjectType_Unused_0x12) {
      const FourCC TXTR_FOURCC = 0x54585452u;
      const CAssetId PICKUP_MAP_ICON_TEXTURE = 0x3dcbaca5u;
      static auto iconTexture = (CTexture*) gpSimplePool->GetObj(
        SObjectTag(TXTR_FOURCC, PICKUP_MAP_ICON_TEXTURE)
      ).GetObj()->GetContents();

      // render custom pickup icon
      if (!mwInfo.IsObjectUnmapped(self->getEditorId())) {
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
  CMappableObject_Draw::InstallAtFuncPtr(&CMappableObject::Draw);
};

