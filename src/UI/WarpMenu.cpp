#include <imgui.h>
#include "WarpMenu.h"

// clang-format off
const WarpArea AREAS_TEMPLE_GROUNDS[] = {
    // {.name="00_scandummy", .area=0x131C3388},
    {.name="Agon Transport Access", .area=0xA391C9AB},
    {.name="Collapsed Tunnel", .area=0x5B92D146},
    {.name="Command Chamber", .area=0xD627000B},
    {.name="Communication Area", .area=0x635E8DA1},
    {.name="Dynamo Chamber", .area=0xB4C067A7},
    {.name="Fortress Transport Access", .area=0x70E5E6DD},
    {.name="GFMC Compound", .area=0x57757AD5},
    {.name="Grand Windchamber", .area=0xAD70917D},
    {.name="Hall of Eyes", .area=0x476BA84C},
    {.name="Hall of Honored Dead", .area=0xB8B34634},
    {.name="Hive Access Tunnel", .area=0xB1140C24},
    {.name="Hive Chamber A", .area=0x50B372A5},
    {.name="Hive Chamber B", .area=0x1D7BD3AE},
    {.name="Hive Chamber C", .area=0xFB843824},
    {.name="Hive Save Station", .area=0x1461DC2F},
    {.name="Hive Storage", .area=0x6367D577},
    {.name="Hive Transport Area", .area=0x941F873A},
    {.name="Hive Tunnel", .area=0x1582EC4A},
    {.name="Industrial Site", .area=0x62AC8AC4},
    {.name="Landing Site", .area=0x62B0D67D},
    {.name="Meeting Grounds", .area=0x9B5EFA75},
    {.name="Path of Eyes", .area=0xEE4732BE},
    {.name="Path of Honor", .area=0x7B4D31DB},
    {.name="Sacred Bridge", .area=0xBABCDB22},
    {.name="Sacred Path", .area=0xC23CFFBD},
    {.name="Service Access", .area=0x9FB7503C},
    {.name="Storage Cavern A", .area=0x3800CF82},
    {.name="Storage Cavern B", .area=0xBE94BD2C},
    {.name="Temple Assembly Site", .area=0x4B4E5911},
    {.name="Temple Transport A", .area=0x503A0640},
    {.name="Temple Transport B", .area=0x4CC37F4A},
    {.name="Temple Transport C", .area=0xADED752E},
    {.name="Torvus Transport Access", .area=0xA8391EAD},
    {.name="Transport to Agon Wastes", .area=0x62FF94EE},
    {.name="Transport to Sanctuary Fortress", .area=0xCDF7686B},
    {.name="Transport to Torvus Bog", .area=0xAC32F338},
    {.name="Trooper Security Station", .area=0x55EE6DC3},
    {.name="Windchamber Gateway", .area=0x75318D5E},
    {.name="Windchamber Tunnel", .area=0x9A633BBF},
    // {.name="game_end_part1", .area=0x1ED8D8B1},
    // {.name="game_end_part2", .area=0x984CAA1F},
    // {.name="game_end_part3", .area=0x531079BA},
    // {.name="game_end_part4", .area=0x4E154902},
    // {.name="game_end_part5", .area=0x85499AA7},
};
const WarpWorld WARP_WORLD_TEMPLE_GROUNDS{
  .name="Temple Grounds",
  .world=0x3BFA3EFF,
  .areas=AREAS_TEMPLE_GROUNDS,
  .areaCount=sizeof(AREAS_TEMPLE_GROUNDS) / sizeof(AREAS_TEMPLE_GROUNDS[0])
};

const WarpArea AREAS_SKY_TEMPLE_GROUNDS[] = {
    {.name="Abandoned Base", .area=0xC6F4E0C2},
    {.name="Accursed Lake", .area=0x9FD196DF},
    {.name="Base Access", .area=0x686F5A1B},
    {.name="Defiled Shrine", .area=0x16E116B4},
    {.name="Gateway Access", .area=0x84324C98},
    {.name="Ing Reliquary", .area=0x749E82E3},
    {.name="Ing Windchamber", .area=0xCDEF2BF1},
    {.name="Lake Access", .area=0x688AB8AE},
    {.name="Phazon Grounds", .area=0x645EF243},
    {.name="Phazon Pit", .area=0x1E332000},
    {.name="Plain of Dark Worship", .area=0x0405EBA4},
    // ILS cutscene
    // {.name="Portal Site", .area=0xDA37FF97},
    {.name="Profane Path", .area=0x64E640D6},
    {.name="Reliquary Access", .area=0x8540CAD4},
    {.name="Reliquary Grounds", .area=0xFC8FC0A4},
    {.name="Shrine Access", .area=0x1F1D07DF},
    {.name="Sky Temple Gateway", .area=0x87D35EE4},
    {.name="War Ritual Grounds", .area=0xAA77EAAD},

};
const WarpWorld WARP_WORLD_SKY_TEMPLE_GROUNDS{
  .name="Sky Temple Grounds",
  .world=0x3BFA3EFF,
  .areas=AREAS_SKY_TEMPLE_GROUNDS,
  .areaCount=sizeof(AREAS_SKY_TEMPLE_GROUNDS) / sizeof(AREAS_SKY_TEMPLE_GROUNDS[0])
};

const WarpArea AREAS_GREAT_TEMPLE[] = {
  {.name="Controller Transport", .area=0x11E50BC6},
  {.name="Main Energy Controller", .area=0x02A01334},
  {.name="Temple Sanctuary", .area=0xA2CB7E32},
  {.name="Temple Transport A", .area=0x185B40F0},
  {.name="Temple Transport B", .area=0x8F01B104},
  {.name="Temple Transport C", .area=0x9860CBB0},
  {.name="Transport A Access", .area=0xE79AAFAE},
  {.name="Transport B Access", .area=0xFEB7BD27},
  {.name="Transport C Access", .area=0x1A4DDCC0},
};
const WarpWorld WARP_WORLD_GREAT_TEMPLE{
  .name="Great Temple",
  .world=0x863FCD72,
  .areas=AREAS_GREAT_TEMPLE,
  .areaCount=sizeof(AREAS_GREAT_TEMPLE) / sizeof(AREAS_GREAT_TEMPLE[0])
};

const WarpArea AREAS_SKY_TEMPLE[] = {
  {.name="Sanctum", .area=0xD7C3B839},
  {.name="Sanctum Access", .area=0xCBAD889B},
  {.name="Sky Temple Energy Controller", .area=0x7B4AFA6F},

};
const WarpWorld WARP_WORLD_SKY_TEMPLE{
  .name="Sky Temple",
  .world=0x863FCD72,
  .areas=AREAS_SKY_TEMPLE,
  .areaCount=sizeof(AREAS_SKY_TEMPLE) / sizeof(AREAS_SKY_TEMPLE[0])
};

const WarpArea AREAS_AGON[] = {
    {.name="Agon Energy Controller", .area=0x02FC3717},
    {.name="Agon Map Station", .area=0x845C85A7},
    {.name="Agon Temple", .area=0x75FC9AAE},
    {.name="Bioenergy Production", .area=0x13470DA8},
    {.name="Biostorage Access", .area=0xDEEB1668},
    {.name="Biostorage Station", .area=0x80896CE9},
    {.name="Central Mining Station", .area=0xF5A6D972},
    {.name="Central Station Access", .area=0xFE69E0C3},
    {.name="Command Center", .area=0x31C25557},
    {.name="Command Center Access", .area=0x708387F9},
    {.name="Controller Access", .area=0xBF5399C0},
    // {.name="Main Energy Controller", .area=0xD92890C3}, // cutscene
    {.name="Main Reactor", .area=0xCCD9F39E},
    {.name="Mine Shaft", .area=0xD7A15431},
    {.name="Mining Plaza", .area=0x427FAD9A},
    {.name="Mining Station A", .area=0xE3BEF27F},
    {.name="Mining Station Access", .area=0x8D7A7F8C},
    {.name="Mining Station B", .area=0xDB7B2CED},
    {.name="Plaza Access", .area=0xADF88927},
    {.name="Portal Access A", .area=0x2312EE1D},
    {.name="Portal Terminal", .area=0x2BCD44A7},
    {.name="Sand Cache", .area=0xF4B1F727},
    {.name="Sand Processing", .area=0xA4B2CB7E},
    {.name="Sandcanyon", .area=0xE5B72628},
    {.name="Save Station A", .area=0x50017152},
    {.name="Save Station C", .area=0xB979562C},
    {.name="Security Station A", .area=0xF723A29A},
    {.name="Security Station B", .area=0x0ADA5AEF},
    {.name="Storage A", .area=0x10323944},
    {.name="Storage B", .area=0x96A64BEA},
    {.name="Storage C", .area=0x5DFA984F},
    {.name="Storage D", .area=0x40FFA8F7},
    {.name="Temple Access", .area=0x2A58AC44},
    {.name="Transit Station", .area=0x79C9C5A0},
    {.name="Transport Center", .area=0x4BDFB7B9},
    {.name="Transport to Sanctuary Fortress", .area=0xC68B5B51},
    {.name="Transport to Temple Grounds", .area=0x57CE3A52},
    {.name="Transport to Torvus Bog", .area=0xA74EC002},
    {.name="Ventilation Area A", .area=0x84303DD5},
    {.name="Ventilation Area B", .area=0x594B330B},
};
const WarpWorld WARP_WORLD_AGON{
  .name="Agon Wastes",
  .world=0x42B935E4,
  .areas=AREAS_AGON,
  .areaCount=sizeof(AREAS_AGON) / sizeof(AREAS_AGON[0])
};

const WarpArea AREAS_DARK_AGON[] = {
    {.name="Battleground", .area=0xEA79522C},
    {.name="Bitter Well", .area=0x04F1C1D8},
    {.name="Crossroads", .area=0x8E23DB3F},
    {.name="Dark Agon Energy Controller", .area=0xB7DD4006},
    {.name="Dark Agon Temple", .area=0x7575048A},
    {.name="Dark Agon Temple Access", .area=0x2103C8AE},
    {.name="Dark Controller Access", .area=0xA2F347D1},
    {.name="Dark Oasis", .area=0x99FB957F},
    {.name="Dark Transit Station", .area=0xD73ECD68},
    {.name="Doomed Entry", .area=0xA49F8A6D},
    {.name="Double Path", .area=0x7FEF433B},
    {.name="Duelling Range", .area=0x3F5CDF39},
    {.name="Feeding Pit", .area=0x075A76EB},
    {.name="Feeding Pit Access", .area=0xE035C0A5},
    {.name="Hall of Stairs", .area=0x6D164780},
    {.name="Ing Cache 1", .area=0x52962FB6},
    {.name="Ing Cache 2", .area=0x9318F076},
    {.name="Ing Cache 3", .area=0xBA23E5F5},
    {.name="Ing Cache 4", .area=0xC5C7C9D3},
    {.name="Judgment Pit", .area=0x6B77F9BD},
    {.name="Junction Site", .area=0x1608C563},
    {.name="Oasis Access", .area=0xF2CCC41E},
    {.name="Phazon Site", .area=0x200EF5E7},
    {.name="Portal Access", .area=0x89D246FD},
    {.name="Portal Site", .area=0xBF7F572C},
    {.name="Save Station 1", .area=0x5A154E2E},
    {.name="Save Station 2", .area=0xAC204F8B},
    {.name="Save Station 3", .area=0x0279F7EF},
    {.name="Trial Grounds", .area=0x86C1AB37},
    {.name="Trial Tunnel", .area=0x48E44EF6},
    {.name="Warrior's Walk", .area=0x33FACC15},
    {.name="Watering Hole", .area=0x9B2B4246},

};
const WarpWorld WARP_WORLD_DARK_AGON{
  .name="Dark Agon",
  .world=0x42B935E4,
  .areas=AREAS_DARK_AGON,
  .areaCount=sizeof(AREAS_DARK_AGON) / sizeof(AREAS_DARK_AGON[0])
};

const WarpArea AREAS_TORVUS_BOG[] = {
    {.name="Abandoned Worksite", .area=0x54DAEF57},
    {.name="Catacombs", .area=0xFB628DCB},
    {.name="Catacombs Access", .area=0xB2CAA0C9},
    {.name="Controller Access", .area=0x2AF65B49},
    {.name="Forgotten Bridge", .area=0x09DECF21},
    {.name="Fortress Transport Access", .area=0x63190A61},
    {.name="Gathering Access", .area=0x530DFF81},
    {.name="Gathering Hall", .area=0x3501473A},
    {.name="Great Bridge", .area=0xE3D5A55E},
    {.name="Grove Access", .area=0x46644C45},
    {.name="Hydrochamber Storage", .area=0x96A6E17E},
    {.name="Hydrodynamo Shaft", .area=0x308351E5},
    {.name="Hydrodynamo Station", .area=0xD5B5B066},
    // {.name="Main Energy Controller", .area=0xA6803305}, // cutscene
    {.name="Main Hydrochamber", .area=0xCEBAC0BD},
    {.name="Meditation Vista", .area=0x5DFA32DB},
    {.name="Path of Roots", .area=0x7448931C},
    {.name="Plaza Access", .area=0xC1C28706},
    {.name="Portal Chamber (Light)", .area=0xF3EE585F},
    {.name="Ruined Alcove", .area=0xD37C2414},
    {.name="Save Station A", .area=0xDB6E4075},
    {.name="Save Station B", .area=0xCB165BD8},
    {.name="Temple Access", .area=0x0740B4D3},
    {.name="Temple Transport Access", .area=0x27D2C898},
    {.name="Torvus Energy Controller", .area=0x133BF5B8},
    {.name="Torvus Grove", .area=0xE838FA18},
    {.name="Torvus Lagoon", .area=0x7E0BE699},
    {.name="Torvus Map Station", .area=0x282B587F},
    {.name="Torvus Plaza", .area=0x75F158FF},
    {.name="Torvus Temple", .area=0xE640CE96},
    {.name="Training Access", .area=0x77AF9FD1},
    {.name="Training Chamber", .area=0x4BB5AE60},
    {.name="Transit Tunnel East", .area=0x66F6300E},
    {.name="Transit Tunnel South", .area=0xB7259AA6},
    {.name="Transit Tunnel West", .area=0xE150FB4D},
    {.name="Transport to Agon Wastes", .area=0xCF659F4E},
    {.name="Transport to Sanctuary Fortress", .area=0xBF0EE428},
    {.name="Transport to Temple Grounds", .area=0x6F6515F2},
    {.name="Underground Transport", .area=0x9258DC82},
    {.name="Underground Tunnel", .area=0x80E67F90},
};
const WarpWorld WARP_WORLD_TORVUS_BOG{
  .name="Torvus Bog",
  .world=0x3DFD2249,
  .areas=AREAS_TORVUS_BOG,
  .areaCount=sizeof(AREAS_TORVUS_BOG) / sizeof(AREAS_TORVUS_BOG[0])
};

const WarpArea AREAS_DARK_TORVUS[] = {
    {.name="Ammo Station", .area=0x8697F62A},
    {.name="Brooding Ground", .area=0xDCEF49B9},
    {.name="Cache A", .area=0x0CAAEEC6},
    {.name="Cache B", .area=0x007551F9},
    {.name="Crypt", .area=0xC8A84B98},
    {.name="Crypt Tunnel", .area=0x4DC421AD},
    {.name="Dark Arena Tunnel", .area=0x390DFCB9},
    {.name="Dark Controller Access", .area=0x11C4526B},
    {.name="Dark Falls", .area=0x2E259879},
    {.name="Dark Forgotten Bridge", .area=0x0253782D},
    {.name="Dark Torvus Arena", .area=0xA49A572F},
    {.name="Dark Torvus Energy Controller", .area=0x5ED7CD19},
    {.name="Dark Torvus Temple", .area=0x3663B195},
    {.name="Dark Torvus Temple Access", .area=0xE597E4F9},
    {.name="Dungeon", .area=0xBBE4B3AE},
    {.name="Gloom Vista", .area=0x954888C7},
    {.name="Poisoned Bog", .area=0x14E2C3A6},
    {.name="Polluted Mire", .area=0x839DC4F6},
    {.name="Portal Chamber (Dark)", .area=0xAE1E1339},
    {.name="Putrid Alcove", .area=0xCBC72D79},
    {.name="Sacrificial Chamber", .area=0x629790F4},
    {.name="Sacrificial Chamber Tunnel", .area=0x65E571CD},
    {.name="Save Station 1", .area=0x9A777442},
    {.name="Save Station 2", .area=0x13254EC6},
    {.name="Undertemple", .area=0x85A3801F},
    {.name="Undertemple Access", .area=0x29B990F8},
    {.name="Undertemple Shaft", .area=0xB2D3E26C},
    {.name="Undertransit One", .area=0x5CD4C2F9},
    {.name="Undertransit Two", .area=0x4BFCA639},
    {.name="Venomous Pond", .area=0x858EECD3},

};
const WarpWorld WARP_WORLD_DARK_TORVUS{
  .name="Dark Torvus",
  .world=0x3DFD2249,
  .areas=AREAS_DARK_TORVUS,
  .areaCount=sizeof(AREAS_DARK_TORVUS) / sizeof(AREAS_DARK_TORVUS[0])
};

const WarpArea AREAS_SANCTUARY[] = {
    {.name="Aerie", .area=0x5D3A0001},
    {.name="Aerie Access", .area=0xDF073157},
    {.name="Aerie Transport Station", .area=0xBAF94A13},
    {.name="Agon Transport Access", .area=0x750BBFEF},
    {.name="Central Area Transport East", .area=0x1152015D},
    {.name="Central Area Transport West", .area=0x70E48580},
    {.name="Checkpoint Station", .area=0x844A690C},
    {.name="Controller Access", .area=0xE89BDC92},
    {.name="Dynamo Access", .area=0x42C85AD9},
    {.name="Dynamo Storage", .area=0x1F7728F8},
    {.name="Dynamo Works", .area=0xC0113CE8},
    {.name="Grand Abyss", .area=0x237EDE04},
    {.name="Hall of Combat Mastery", .area=0x5571E89E},

    // {.name="Main Energy Controller", .area=0x57134DC2}, // cutscene
    {.name="Main Gyro Chamber", .area=0x73342A54},
    {.name="Main Research", .area=0x914F1381},
    {.name="Minigyro Chamber", .area=0x96F4CA1E},
    {.name="Power Junction", .area=0x31C07D16},
    {.name="Reactor Access", .area=0xB666B655},
    {.name="Reactor Core", .area=0x4A6CF295},
    {.name="Sanctuary Energy Controller", .area=0x0D032A6A},
    {.name="Sanctuary Entrance", .area=0x47265C0B},
    {.name="Sanctuary Map Station", .area=0x52BF89F3},
    {.name="Sanctuary Temple", .area=0x6C363CD4},
    {.name="Save Station A", .area=0x65B801CE},
    {.name="Save Station B", .area=0xE32C7360},
    {.name="Sentinel's Path", .area=0xA4D81547},
    {.name="Temple Access", .area=0xC56E919A},
    {.name="Temple Transport Access", .area=0x625A2692},
    {.name="Torvus Transport Access", .area=0xF2AD74AC},
    {.name="Transit Station", .area=0xF7424EC3},
    {.name="Transport to Agon Wastes", .area=0x35A94603},
    {.name="Transport to Temple Grounds", .area=0xD24B673D},
    {.name="Transport to Torvus Bog", .area=0xBB77569E},
    {.name="Vault", .area=0x52254361},
    {.name="Watch Station", .area=0xA2406387},
    {.name="Watch Station Access", .area=0x5076F9CB},
    {.name="Workers Path", .area=0x03ECA24F},
};
const WarpWorld WARP_WORLD_SANCTUARY{
  .name="Sanctuary Fortress",
  .world=0x1BAA96C2,
  .areas=AREAS_SANCTUARY,
  .areaCount=sizeof(AREAS_SANCTUARY) / sizeof(AREAS_SANCTUARY[0])
};

const WarpArea AREAS_ING_HIVE[] = {
    {.name="Aerial Training Site", .area=0x40F7447E},
    {.name="Central Hive East Transport", .area=0xC2D7966B},
    {.name="Central Hive West Transport", .area=0x6CBCD4AB},
    {.name="Culling Chamber", .area=0x690B786A},
    {.name="Entrance Defense Hall", .area=0xA70EA82B},
    {.name="Hazing Cliff", .area=0x37814F2A},
    {.name="Hive Ammo Station", .area=0xF1DF78DC},
    {.name="Hive Cache 1", .area=0x5F43767D},
    {.name="Hive Cache 3", .area=0x072FCFBC},
    {.name="Hive Controller Access", .area=0x26AC7F1E},
    {.name="Hive Dynamo Access", .area=0xFBAF3B2B},
    {.name="Hive Dynamo Works", .area=0xF24EF6B4},
    {.name="Hive Energy Controller", .area=0x2DECC318},
    {.name="Hive Entrance", .area=0xF3FA4121},
    {.name="Hive Gyro Access", .area=0x2735236B},
    {.name="Hive Gyro Chamber", .area=0xA3656292},
    {.name="Hive Portal Chamber", .area=0x7B94B06B},
    {.name="Hive Reactor", .area=0xF8DBC03D},
    {.name="Hive Reactor Access", .area=0xB026CCEB},
    {.name="Hive Save Station 1", .area=0xBB57222F},
    {.name="Hive Save Station 2", .area=0x22B5442E},
    {.name="Hive Summit", .area=0x9D804955},
    {.name="Hive Temple", .area=0xA92F00B3},
    {.name="Hive Temple Access", .area=0xEC875FEB},
    {.name="Judgment Drop", .area=0x55C479EB},
    {.name="Staging Area", .area=0x5BBF334F},
    {.name="Temple Security Access", .area=0x42EC1D2B},
    {.name="Unseen Way", .area=0xD5FFF2AB},
    {.name="Vault Attack Portal", .area=0x1DF13149},
};
const WarpWorld WARP_WORLD_ING_HIVE{
  .name="Ing Hive",
  .world=0x1BAA96C2,
  .areas=AREAS_ING_HIVE,
  .areaCount=sizeof(AREAS_ING_HIVE) / sizeof(AREAS_ING_HIVE[0])
};

// clang-format on

const WarpWorld WARP_WORLDS[] = {
  WARP_WORLD_TEMPLE_GROUNDS,
  WARP_WORLD_SKY_TEMPLE_GROUNDS,
  WARP_WORLD_GREAT_TEMPLE,
  WARP_WORLD_SKY_TEMPLE,
  WARP_WORLD_AGON,
  WARP_WORLD_DARK_AGON,
  WARP_WORLD_TORVUS_BOG,
  WARP_WORLD_DARK_TORVUS,
  WARP_WORLD_SANCTUARY,
  WARP_WORLD_ING_HIVE,
};

// OOB spawns:
//Undertemple Access 00350007

namespace GUI {
  void drawWarpMenu() {
    if (ImGui::TreeNode("Warps")) {
      // ImGui::Text("You can also warp by pressing 'x' on the map");
      for (auto world : WARP_WORLDS) {
        if (ImGui::TreeNode(world.name)) {
          for (int i = 0; i < world.areaCount; i++) {
            ImGuiTreeNodeFlags node_flags =  ImGuiTreeNodeFlags_OpenOnArrow
                | ImGuiTreeNodeFlags_OpenOnDoubleClick
                | ImGuiTreeNodeFlags_SpanAvailWidth
                | ImGuiTreeNodeFlags_Leaf
                | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            const WarpArea &area = world.areas[i];
            ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "%s", area.name);
            if (ImGui::IsItemActivated()) {
              warp(world.world, area.area);
            }
          }
          ImGui::TreePop();
        }
      }
      ImGui::TreePop();
    }
  }
}

const char* getNameForWorldAsset(u32 world) {
  for (auto &warpWorld : WARP_WORLDS) {
    if (warpWorld.world == world) {
      return warpWorld.name;
    }
  }
  return "Unknown";
}

const char* getNameForAreaAsset(u32 world, u32 area) {
  for (auto &warpWorld : WARP_WORLDS) {
    if (warpWorld.world == world) {
      for (int i = 0; i < warpWorld.areaCount; i++) {
        auto &warpArea = warpWorld.areas[i];
        if (warpArea.area == area) {
          return warpArea.name;
        }
      }
    }
  }
  return "Unknown";
}
