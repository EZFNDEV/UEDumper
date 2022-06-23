# UEDumper
This can probably dump the SDK for any Unreal Engine Game (Educational Purpose Only)
Note: We released this early, for now it's working perfect for fortnite (lower than u4.25?), but we will add more support later!

# Usage

### Set the build settings  
Example of DUMP_OBJECT_NAMES  
Output path: D:\5.41-CL-4363240\FortniteGame\Binaries\Win64\SDK\  
Dumped 99158 objects in 636.12 ms  
  
Example of SEARCH_OFFSETS and PRINT_OFFSETS  
Memory base: 00007FF77AA20000  
ProcessEvent: 00007FF77C24D530  
StaticFindObject: 00007FF77C274780  
GObjects: 00007FF77FE45940  
....
  
1. Inject the dll into your game
2. If all offsets are found automatically you will need to do nothing, otherwise you will need to get them manually
3. Go to the game path and copy your SDK
4. To use it in a project check out the examples, not all packages are included, just add the ones you need to Core.hpp

# Features
✔️ Dump all Object names  
✔️ Create a SDK  
  
# Details  
✔️ Getting the real UPROPERTY Specifiers  
✔️ Automatically find most (often all) offsets

# TODO:
Check all functions if they contain the offset for a property
Add support for newer ue4 versions
 
More coming soon
